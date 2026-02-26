#include "Enginepch.h"

#include "ComponentSystem/GameObject.h"

#include "AnimatedModel.h"
#include "Objects/Mesh.h"
#include "Objects/Animation.h"
#include "Engine.h"
#include "Time/Timer.h"
#include "Math/Quaternion.hpp"
#include "GraphicsEngine.h"
#include "AssetTypes/MaterialAsset.h"
#include "AssetTypes/MeshAsset.h"
#include "AssetTypes/AnimationAsset.h"
#include "AssetManager.h"
#include "Camera.h"
#include "ComponentSystem/Components/Transform.h"

AnimatedModel::~AnimatedModel()
{
    myMesh = nullptr;
    myMaterials.clear();
    mySlotToIndex.clear();
    myAnimationLayers.clear();
}

AnimatedModel::AnimatedModel()
{
    SetMaterialOnSlot(0, GraphicsEngine::Get().GetDefaultMaterial());
}

AnimatedModel::AnimatedModel(std::shared_ptr<Mesh> aMesh)
{
    SetMesh(aMesh);
    for (auto& element : myMesh->GetSubmeshes())
    {
        SetMaterialOnSlot(element.MaterialIndex, GraphicsEngine::Get().GetDefaultMaterial());
    }
}

AnimatedModel::AnimatedModel(std::shared_ptr<Mesh> aMesh, std::shared_ptr<Material> aMaterial)
{
    SetMesh(aMesh);
    SetMaterialOnSlot(0, aMaterial);
}

void AnimatedModel::Start()
{
}

void AnimatedModel::Update()
{
    if (!ValidateMesh()) return;
    if (myAnimationLayers.empty()) return;

    PIXScopedEvent(PIX_COLOR_INDEX(2), "AnimatedModel Update");

    int updatedLayers = 0;

    for (auto& animationLayer : myAnimationLayers)
    {
        if (UpdateAnimationLayer(animationLayer))
            ++updatedLayers;
    }

    if (updatedLayers > 0)
    {
        for (auto& animationLayer : myAnimationLayers)
        {
            UpdateAnimationLayerPose(animationLayer);
        }
    }
}

void AnimatedModel::SetMesh(std::shared_ptr<Mesh> aMesh)
{
    myMesh = aMesh;
    AddAnimationLayer(0);
}

void AnimatedModel::SetMaterialOnSlot(unsigned aSlot, std::shared_ptr<Material> aMaterial)
{
    myMaterials.emplace_back(aMaterial);

    if (mySlotToIndex.find(aSlot) != mySlotToIndex.end())
    {
        myMaterials.erase(myMaterials.begin() + mySlotToIndex.at(aSlot));
        mySlotToIndex.erase(aSlot);
    }

    mySlotToIndex.emplace(aSlot, static_cast<unsigned>(myMaterials.size() - 1));
}

const Math::AABB3D<float> AnimatedModel::GetBoundingBox() const
{
    if (!ValidateMesh())
    {
        return Math::AABB3D<float>();
    }

    return myMesh->GetBoundingBox();
}

void AnimatedModel::SetAnimationHalfLODDistance(float aDistance)
{
    myHalfFramesLODDistance = aDistance * aDistance;
}

void AnimatedModel::SetAnimationQuarterLODDistance(float aDistance)
{
    myQuarterFramesLODDistance = aDistance * aDistance;
}

void AnimatedModel::AddAnimationLayer(unsigned aJointID)
{
    if (!ValidateMesh()) return;
    if (!ValidateJointIndex(aJointID)) return;

    AnimationLayer& newAnimLayer = myAnimationLayers.emplace_back(AnimationLayer());
    myJointNameToLayerIndex.emplace(myMesh->GetSkeleton().myJoints[aJointID].Name, static_cast<unsigned>(myAnimationLayers.size() - 1));
    newAnimLayer.startJointID = aJointID;
}

void AnimatedModel::AddAnimationLayer(const std::string& aStartJoint)
{
    if (!ValidateMesh()) return;
    if (!ValidateJointName(aStartJoint)) return;

    unsigned index = static_cast<unsigned>(myMesh->GetSkeleton().JointNameToIndex.at(aStartJoint));
    AddAnimationLayer(index);
}

void AnimatedModel::AddAnimationToLayer(const std::string& aAnimationName, std::shared_ptr<Animation> aNewAnimation, unsigned aLayerIndex, bool aShouldLoop)
{
    if (!ValidateLayerIndex(aLayerIndex)) return;

    std::shared_ptr<AnimationState> newState = std::make_shared<AnimationState>();
    newState->animation = aNewAnimation;
    newState->name = aAnimationName;
    newState->isLooping = aShouldLoop;
    newState->currentFrame = 0;
    newState->currentTime = 0;
    newState->frameTime = 1.0f / aNewAnimation->FramesPerSecond;
    myAnimationLayers[aLayerIndex].myAnimationStates.emplace(aAnimationName, newState);

    if (!myAnimationLayers[aLayerIndex].currentState)
    {
        myAnimationLayers[aLayerIndex].currentState = newState;
    }
}

void AnimatedModel::AddAnimationToLayer(const std::string& aAnimationName, std::shared_ptr<Animation> aNewAnimation, const std::string& aStartJoint, bool aShouldLoop)
{
    if (aStartJoint == "")
    {
        AddAnimationToLayer(aAnimationName, aNewAnimation, 0, aShouldLoop);
        return;
    }

    if (!ValidateLayerJointName(aStartJoint)) return;

    AddAnimationToLayer(aAnimationName, aNewAnimation, myJointNameToLayerIndex.at(aStartJoint), aShouldLoop);
}

void AnimatedModel::SetCurrentAnimationOnLayer(const std::string& aAnimationName, unsigned aLayerIndex, float aBlendTime, unsigned aStartingFrame)
{
    if (!ValidateLayerIndex(aLayerIndex)) return;
    if (!ValidateAnimationName(aLayerIndex, aAnimationName)) return;
 
    if (aBlendTime > 0)
    {
        myAnimationLayers[aLayerIndex].isBlending = true;
        myAnimationLayers[aLayerIndex].maxBlendTime = aBlendTime;
        myAnimationLayers[aLayerIndex].currentBlendTime = 0;

        myAnimationLayers[aLayerIndex].nextState = myAnimationLayers[aLayerIndex].myAnimationStates.at(aAnimationName);
        myAnimationLayers[aLayerIndex].nextState->currentFrame = aStartingFrame;
    }
    else
    {
        myAnimationLayers[aLayerIndex].currentState = myAnimationLayers[aLayerIndex].myAnimationStates.at(aAnimationName);
        myAnimationLayers[aLayerIndex].currentState->currentFrame = aStartingFrame;
    }
}

void AnimatedModel::SetCurrentAnimationOnLayer(const std::string& aAnimationName, const std::string& aStartJoint, float aBlendTime, unsigned aStartingFrame)
{
    if (aStartJoint == "")
    {
        SetCurrentAnimationOnLayer(aAnimationName, 0, aBlendTime, aStartingFrame);
        return;
    }

    if (!ValidateLayerJointName(aStartJoint)) return;

    SetCurrentAnimationOnLayer(aAnimationName, myJointNameToLayerIndex.at(aStartJoint), aBlendTime, aStartingFrame);
}

//void AnimatedModel::AddAnimationEvent(const std::string& aAnimationName, unsigned aEventFrame, GameObjectEventType aEventTypeToSend, unsigned aLayerIndex)
//{
//    if (!ValidateLayerIndex(aLayerIndex)) return;
//    if (!ValidateAnimationName(aLayerIndex, aAnimationName)) return;
//
//    if (aEventFrame < 0 || aEventFrame >= myAnimationLayers[aLayerIndex].myAnimationStates.at(aAnimationName)->animation->Frames.size())
//    {
//        LOG(LogComponentSystem, Warning, "Frame {} is out of the range of Animation {}! Event will not be added!", aEventFrame, aAnimationName);
//        return;
//    }
//
//    AnimationEvent newEvent;
//    newEvent.eventTypeToSend = aEventTypeToSend;
//    newEvent.frame = aEventFrame;
//    myAnimationLayers[aLayerIndex].myAnimationStates.at(aAnimationName)->events.emplace_back(newEvent);
//}

//void AnimatedModel::AddAnimationEvent(const std::string& aAnimationName, unsigned aEventFrame, GameObjectEventType aEventTypeToSend, const std::string& aStartJoint)
//{
//    if (aStartJoint == "")
//    {
//        AddAnimationEvent(aAnimationName, aEventFrame, aEventTypeToSend, 0);
//        return;
//    }
//
//    AddAnimationEvent(aAnimationName, aEventFrame, aEventTypeToSend, myJointNameToLayerIndex.at(aStartJoint));
//}

const std::string AnimatedModel::GetCurrentAnimationNameOnLayer(unsigned aLayerIndex) const
{
    if (!ValidateLayerIndex(aLayerIndex)) return "";
    if (!myAnimationLayers[aLayerIndex].currentState) return "";

    return myAnimationLayers[aLayerIndex].currentState->name;
}

const std::string AnimatedModel::GetCurrentAnimationNameOnLayer(const std::string& aStartJoint) const
{
    if (aStartJoint == "")
    {
        return GetCurrentAnimationNameOnLayer(0);
    }

    if (!ValidateLayerJointName(aStartJoint)) return "";

    return GetCurrentAnimationNameOnLayer(myJointNameToLayerIndex.at(aStartJoint));
}

std::shared_ptr<AnimatedModel::AnimationState> AnimatedModel::GetCurrentAnimationStateOnLayer(unsigned aLayerIndex)
{
    return myAnimationLayers[aLayerIndex].currentState;
}

std::vector<std::shared_ptr<AnimatedModel::AnimationState>> AnimatedModel::GetAnimationStatesOnLayer(unsigned aLayerIndex)
{
    std::vector<std::shared_ptr<AnimatedModel::AnimationState>> animationStates;
    if (!ValidateLayerIndex(aLayerIndex)) return animationStates;

    for (auto& animState : myAnimationLayers[aLayerIndex].myAnimationStates)
    {
        animationStates.emplace_back(animState.second);
    }

    return animationStates;
}

const bool AnimatedModel::IsLayerCurrentlyBlending(unsigned aLayerIndex) const
{
    if (!ValidateLayerIndex(aLayerIndex)) return false;

    return myAnimationLayers[aLayerIndex].isBlending;
}

const float AnimatedModel::GetCurrentBlendFactorOnLayer(unsigned aLayerIndex) const
{
    if (!ValidateLayerIndex(aLayerIndex)) return 0;
    float blendFactor = myAnimationLayers[aLayerIndex].currentBlendTime / myAnimationLayers[aLayerIndex].maxBlendTime;

    return blendFactor;
}

void AnimatedModel::PlayAnimation()
{
    for (auto& layer : myAnimationLayers)
    {
        layer.isPlaying = true;
    }
}

void AnimatedModel::StopAnimation()
{
    for (auto& layer : myAnimationLayers)
    {
        layer.isPlaying = false;
    }
}

void AnimatedModel::PlayAnimationOnLayer(unsigned aLayerIndex)
{
    if (!ValidateLayerIndex(aLayerIndex)) return;

    myAnimationLayers[aLayerIndex].isPlaying = true;
}

void AnimatedModel::PlayAnimationOnLayer(const std::string& aStartJoint)
{
    if (aStartJoint == "")
    {
        PlayAnimationOnLayer(0);
        return;
    }

    if (!ValidateLayerJointName(aStartJoint)) return;

    PlayAnimationOnLayer(myJointNameToLayerIndex.at(aStartJoint));
}

void AnimatedModel::StopAnimationOnLayer(unsigned aLayerIndex)
{
    if (!ValidateLayerIndex(aLayerIndex)) return;

    myAnimationLayers[aLayerIndex].isPlaying = true;
}

void AnimatedModel::StopAnimationOnLayer(const std::string& aStartJoint)
{
    if (aStartJoint == "")
    {
        StopAnimationOnLayer(0);
        return;
    }

    if (!ValidateLayerJointName(aStartJoint)) return;

    StopAnimationOnLayer(myJointNameToLayerIndex.at(aStartJoint));
}

bool AnimatedModel::Serialize(nl::json& outJsonObject)
{
    outJsonObject;
    return false;
}

bool AnimatedModel::Deserialize(nl::json& aJsonObject)
{
    if (!aJsonObject.contains("Model")) return false;

    if (aJsonObject.contains("ShouldViewcull"))
    {
        SetViewcull(aJsonObject["ShouldViewcull"].get<bool>());
    }

    if (aJsonObject.contains("CastShadows"))
    {
        SetCastShadows(aJsonObject["CastShadows"].get<bool>());
    }

    SetMesh(AssetManager::Get().GetAsset<MeshAsset>(aJsonObject["Model"].get<std::string>())->mesh);

    if (aJsonObject.contains("Materials"))
    {
        for (int i = 0; i < aJsonObject["Materials"].size(); i++)
        {
            SetMaterialOnSlot(i, AssetManager::Get().GetAsset<MaterialAsset>(aJsonObject["Materials"][i].get<std::string>())->material);
        }
    }

    if (aJsonObject.contains("AnimationLayers"))
    {
        for (auto& layer : aJsonObject["AnimationLayers"])
        {
            std::string startBone;

            if (layer.contains("StartBone"))
            {
                startBone = layer["StartBone"].get<std::string>();
            }

            if (startBone != "")
            {
                AddAnimationLayer(startBone);
            }

            if (layer.contains("Animations"))
            {
                for (auto& animation : layer["Animations"])
                {
                    std::string animationName;
                    std::shared_ptr<Animation> anim;
                    bool shouldLoop = false;

                    if (animation.contains("Name"))
                    {
                        animationName = animation["Name"].get<std::string>();
                    }

                    if (animation.contains("Path"))
                    {
                        anim = AssetManager::Get().GetAsset<AnimationAsset>(animation["Path"].get<std::string>())->animation;
                    }

                    if (animation.contains("Loop"))
                    {
                        shouldLoop = animation["Loop"].get<bool>();
                    }

                    AddAnimationToLayer(animationName, anim, startBone, shouldLoop);
                }
            }
        }
    }

    return true;
}

bool AnimatedModel::UpdateAnimationLayer(AnimationLayer& aAnimationLayer)
{
    if (!aAnimationLayer.isPlaying) return false;
    PIXScopedEvent(PIX_COLOR_INDEX(2), "AnimatedModel Update Animation Layer");

    bool updateAnimation = true;

    if (aAnimationLayer.isBlending)
    {
        bool updateCurrent = UpdateAnimationState(*aAnimationLayer.currentState);
        bool updateNext = UpdateAnimationState(*aAnimationLayer.nextState);
        updateAnimation = updateCurrent || updateNext;

        aAnimationLayer.currentBlendTime += Engine::Get().GetTimer().GetDeltaTime();
        if (aAnimationLayer.currentBlendTime >= aAnimationLayer.maxBlendTime)
        {
            aAnimationLayer.currentState = aAnimationLayer.nextState;
            aAnimationLayer.isBlending = false;
        }
    }
    else
    {
        updateAnimation = UpdateAnimationState(*aAnimationLayer.currentState);
    }

    return updateAnimation;
}

void AnimatedModel::UpdateAnimationLayerPose(AnimationLayer& aAnimationLayer)
{
    PIXScopedEvent(PIX_COLOR_INDEX(2), "AnimatedModel Update Animation Layer Pose");

    if (aAnimationLayer.isBlending)
    {
        float blendFactor = aAnimationLayer.currentBlendTime / aAnimationLayer.maxBlendTime;
        BlendPoses(aAnimationLayer, blendFactor);
    }
    else
    {
        UpdatePose(aAnimationLayer);
    }

    Math::Matrix4x4<float> parentBoneMatrix;
    if (aAnimationLayer.startJointID > 0)
    {
        int parentID = myMesh->GetSkeleton().myJoints[aAnimationLayer.startJointID].Parent;
        parentBoneMatrix = myMesh->GetSkeleton().myJoints[parentID].BindPoseInverse.GetFastInverse() * myJointTransforms[parentID];
    }

    UpdateAnimation(aAnimationLayer, aAnimationLayer.startJointID, parentBoneMatrix, myJointTransforms);
}

bool AnimatedModel::UpdateAnimationState(AnimationState& aAnimationState)
{
    PIXScopedEvent(PIX_COLOR_INDEX(2), "AnimatedModel Update Animation State");

    aAnimationState.currentTime += Engine::Get().GetTimer().GetDeltaTime() * aAnimationState.speedMultiplier;
    int lodLevel = static_cast<int>(GetAnimationLOD());

    if (aAnimationState.currentTime < aAnimationState.frameTime + (aAnimationState.frameTime * lodLevel)) return false;

    aAnimationState.currentTime = 0;
    aAnimationState.currentFrame += 1 + lodLevel;

    for (auto& event : aAnimationState.events)
    {
        if (event.frame < aAnimationState.currentFrame)
        {
            //gameObject->SendEvent(event.eventTypeToSend);
            event.hasBeenCalledThisLoop = true;
        }
    }

    if (aAnimationState.animation->Frames.size() <= aAnimationState.currentFrame)
    {
        if (aAnimationState.isLooping)
        {
            aAnimationState.currentFrame = 0;

            for (auto& event : aAnimationState.events)
            {
                event.hasBeenCalledThisLoop = false;
            }
        }
        else
        {
            // Freeze last frame
            // Maybe make it automatically return to its last state?
            aAnimationState.currentFrame = static_cast<unsigned>(aAnimationState.animation->Frames.size() - 1);
        }
    }

    return true;
}

void AnimatedModel::UpdateAnimation(AnimationLayer& aAnimLayer, unsigned aJointIdx, const Math::Matrix4x4f& aParentJointTransform, std::array<Math::Matrix4x4f, 128>& outTransforms)
{
    PIXScopedEvent(PIX_COLOR_INDEX(2), "AnimatedModel Update Animation");

    const Mesh::Skeleton::Joint& currentJoint = myMesh->GetSkeleton().myJoints[aJointIdx];
    Math::Matrix4x4f currentFrameJointTransform = aAnimLayer.currentPose[aJointIdx];
    currentFrameJointTransform = currentFrameJointTransform * aParentJointTransform;
    Math::Matrix4x4f result = currentJoint.BindPoseInverse * currentFrameJointTransform;

    outTransforms[aJointIdx] = result;

    for (auto& childIndex : currentJoint.Children)
    {
        UpdateAnimation(aAnimLayer, childIndex, currentFrameJointTransform, outTransforms);
    }
}

void AnimatedModel::UpdatePose(AnimationLayer& aAnimLayer)
{
    PIXScopedEvent(PIX_COLOR_INDEX(2), "AnimatedModel Update Pose");

    assert(aAnimLayer.currentState->animation->Frames.size() > 0 && "Current animation has no frames!");

    const Mesh::Skeleton& skeleton = myMesh->GetSkeleton();
    for (size_t i = aAnimLayer.startJointID; i < skeleton.myJoints.size(); i++)
    {
        const Mesh::Skeleton::Joint& currentJoint = skeleton.myJoints[i];
        aAnimLayer.currentPose[i] = aAnimLayer.currentState->animation->Frames[aAnimLayer.currentState->currentFrame].BoneTransforms[currentJoint.Name];
    }
}

void AnimatedModel::BlendPoses(AnimationLayer& aAnimLayer, float aBlendFactor)
{
    PIXScopedEvent(PIX_COLOR_INDEX(2), "AnimatedModel Blend Poses");

    const Mesh::Skeleton& skeleton = myMesh->GetSkeleton();
    for (size_t i = aAnimLayer.startJointID; i < skeleton.myJoints.size(); i++)
    {
        const Mesh::Skeleton::Joint& currentJoint = skeleton.myJoints[i];
        const Math::Matrix4x4f& currentStateJointTransform = aAnimLayer.currentState->animation->Frames[aAnimLayer.currentState->currentFrame].BoneTransforms[currentJoint.Name];
        const Math::Matrix4x4f& nextStateJointTransform = aAnimLayer.nextState->animation->Frames[aAnimLayer.nextState->currentFrame].BoneTransforms[currentJoint.Name];
         
        Math::Vector3f T = Math::Vector3f::Lerp(Math::Matrix4x4f::CreateTranslationVector(currentStateJointTransform), Math::Matrix4x4f::CreateTranslationVector(nextStateJointTransform), aBlendFactor);
        Math::Quatf R = Math::Quatf::Slerp(Math::Quatf(currentStateJointTransform), Math::Quatf(nextStateJointTransform), aBlendFactor);
        //Math::Vector3 currentS = Math::Matrix4x4f::CreateScaleVector(currentStateJointTransform);
        //Math::Vector3 nextS = Math::Matrix4x4f::CreateScaleVector(nextStateJointTransform);
        //Math::Vector3f S = Math::Vector3f::Lerp(currentS, nextS, aBlendFactor);
        //std::cout << S.x << ", " << S.y << ", " << S.z << std::endl;
        Math::Vector3f S = { 1.0f, 1.0f, 1.0f };

        aAnimLayer.currentPose[i] = Math::Matrix4x4f::CreateScaleMatrix(S) * R.GetRotationMatrix4x4f() * Math::Matrix4x4f::CreateTranslationMatrix(T);
    }
}

AnimatedModel::AnimLODLevel AnimatedModel::GetAnimationLOD() const
{
    if (myShouldLODAnimations)
    {
        if (auto cam = Camera::GetMainCamera())
        {
            Math::Vector3f diff = cam->gameObject->GetComponent<Transform>()->GetTranslation(true) - gameObject->GetComponent<Transform>()->GetTranslation(true);
            float diffLengthSqr = diff.LengthSqr();

            if (diffLengthSqr > myQuarterFramesLODDistance)
                return AnimLODLevel::LOD2;
            else if (diffLengthSqr > myHalfFramesLODDistance)
                return AnimLODLevel::LOD1;
            else
                return AnimLODLevel::LOD0;
        }
    }

    return AnimLODLevel::LOD0;
}

const bool AnimatedModel::ValidateMesh() const
{
    if (!myMesh.get())
    {
        LOG(LogComponentSystem, Error, "Could not find mesh!");
        return false;
    }

    return true;
}

const bool AnimatedModel::ValidateJointIndex(unsigned aStartJoint) const
{
    if (aStartJoint >= myMesh->GetSkeleton().myJoints.size())
    {
        LOG(LogComponentSystem, Error, "Joint index {} out of range!", aStartJoint);
        return false;
    }

    return true;
}

const bool AnimatedModel::ValidateJointName(const std::string& aStartJoint) const
{
    if (myMesh->GetSkeleton().JointNameToIndex.find(aStartJoint) == myMesh->GetSkeleton().JointNameToIndex.end())
    {
        LOG(LogComponentSystem, Error, "Could not find joint with name {}!", aStartJoint);
        return false;
    }

    return true;
}

const bool AnimatedModel::ValidateLayerIndex(unsigned aLayerIndex) const
{
    if (aLayerIndex < 0 || aLayerIndex >= myAnimationLayers.size())
    {
        LOG(LogComponentSystem, Error, "Layer index {} out of range!", aLayerIndex);
        return false;
    }

    return true;
}

const bool AnimatedModel::ValidateLayerJointName(const std::string& aStartJoint) const
{
    if (myJointNameToLayerIndex.find(aStartJoint) == myJointNameToLayerIndex.end())
    {
        LOG(LogComponentSystem, Error, "Could not find a layer by the name of {}!", aStartJoint);
        return false;
    }

    return true;
}

const bool AnimatedModel::ValidateAnimationName(unsigned aLayerIndex, const std::string& aAnimationName) const
{
    if (myAnimationLayers[aLayerIndex].myAnimationStates.find(aAnimationName) == myAnimationLayers[aLayerIndex].myAnimationStates.end())
    {
        LOG(LogComponentSystem, Error, "Could not find an animation by the name of {} in layer {}!", aAnimationName, aLayerIndex);
        return false;
    }

    return true;
}
