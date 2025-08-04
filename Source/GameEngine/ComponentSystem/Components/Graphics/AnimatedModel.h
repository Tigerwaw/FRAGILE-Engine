#pragma once
#include "ComponentSystem/Component.h"
#include "Math/AABB3D.hpp"
#include "Math/Matrix4x4.hpp"

// The reason why this class doesn't inherit from the Model class is that it creates problems with type-casting.
// (Calling GetComponent<Model> would return a pointer to an instance of AnimatedModel, which may not be desired)

class Mesh;
class Material;
struct Animation;

class AnimatedModel : public Component
{
public:
    struct AnimationEvent
    {
        unsigned frame = 0;
        //GameObjectEventType eventTypeToSend = GameObjectEventType::Count;
        bool hasBeenCalledThisLoop = false;
    };

    struct AnimationState
    {
        std::shared_ptr<Animation> animation;
        std::string name;
        unsigned currentFrame = 0;
        float frameTime = 0;
        float currentTime = 0;
        float speedMultiplier = 1.0f;
        bool isLooping = false;

        std::vector<AnimationEvent> events;
    };

    struct AnimationLayer
    {
        std::shared_ptr<AnimationState> currentState;
        std::shared_ptr<AnimationState> nextState;
        std::unordered_map<std::string, std::shared_ptr<AnimationState>> myAnimationStates;
        std::array<Math::Matrix4x4f, 128> currentPose;
        unsigned startJointID = 0;
        bool isBlending = false;
        float currentBlendTime = 0;
        float maxBlendTime = 0;
        bool isPlaying = true;
    };

    ~AnimatedModel() override;
    AnimatedModel();
    AnimatedModel(std::shared_ptr<Mesh> aMesh);
    AnimatedModel(std::shared_ptr<Mesh> aMesh, std::shared_ptr<Material> aMaterial);

    void Start() override;
    void Update() override;

    void SetMesh(std::shared_ptr<Mesh> aMesh);
    std::shared_ptr<Mesh> GetMesh() { return myMesh; }

    void SetMaterialOnSlot(unsigned aSlot, std::shared_ptr<Material> aMaterial);
    std::shared_ptr<Material> GetMaterialOnSlot(unsigned aSlot) { return myMaterials[mySlotToIndex[aSlot]]; }
    const std::vector<std::shared_ptr<Material>>& GetMaterials() { return myMaterials; }
    const Math::AABB3D<float> GetBoundingBox() const;

    void PlayAnimation();
    void StopAnimation();
    void PlayAnimationOnLayer(unsigned aLayerIndex = 0);
    void PlayAnimationOnLayer(const std::string& aStartJoint = "");
    void StopAnimationOnLayer(unsigned aLayerIndex = 0);
    void StopAnimationOnLayer(const std::string& aStartJoint = "");

    void AddAnimationLayer(unsigned aJointID = 0);
    void AddAnimationLayer(const std::string& aStartJoint = "");
    void AddAnimationToLayer(const std::string& aAnimationName, std::shared_ptr<Animation> aNewAnimation, unsigned aLayerIndex = 0, bool aShouldLoop = false);
    void AddAnimationToLayer(const std::string& aAnimationName, std::shared_ptr<Animation> aNewAnimation, const std::string& aStartJoint = "", bool aShouldLoop = false);
    void SetCurrentAnimationOnLayer(const std::string& aAnimationName, unsigned aLayerIndex = 0, float aBlendTime = 0, unsigned aStartingFrame = 0);
    void SetCurrentAnimationOnLayer(const std::string& aAnimationName, const std::string& aStartJoint = "", float aBlendTime = 0, unsigned aStartingFrame = 0);
    //void AddAnimationEvent(const std::string& aAnimationName, unsigned aEventFrame, GameObjectEventType aEventTypeToSend, unsigned aLayerIndex = 0);
    //void AddAnimationEvent(const std::string& aAnimationName, unsigned aEventFrame, GameObjectEventType aEventTypeToSend, const std::string& aStartJoint = "");

    const std::string GetCurrentAnimationNameOnLayer(unsigned aLayerIndex = 0) const;
    const std::string GetCurrentAnimationNameOnLayer(const std::string& aStartJoint = "") const;

    std::shared_ptr<AnimatedModel::AnimationState> GetCurrentAnimationStateOnLayer(unsigned aLayerIndex);
    std::vector<std::shared_ptr<AnimatedModel::AnimationState>> GetAnimationStatesOnLayer(unsigned aLayerIndex = 0);

    const bool IsLayerCurrentlyBlending(unsigned aLayerIndex = 0) const;
    const float GetCurrentBlendFactorOnLayer(unsigned aLayerIndex = 0) const;

    const std::array<Math::Matrix4x4f, 128>& GetCurrentPose() const { return myJointTransforms; }

    void SetViewcull(bool aShouldViewcull) { myShouldViewcull = aShouldViewcull; }
    const bool GetShouldViewcull() const { return myShouldViewcull; }

    void SetCastShadows(bool aCastShadows) { myCastShadows = aCastShadows; }
    const bool GetCastShadows() const { return myCastShadows; }

    bool Serialize(nl::json& outJsonObject) override;
    bool Deserialize(nl::json& aJsonObject) override;

private:
    void UpdateAnimationLayer(AnimationLayer& aAnimationLayer);
    void UpdateAnimationState(AnimationState& aAnimationState);
    void UpdateAnimation(AnimationLayer& aAnimLayer, unsigned aJointIdx, const Math::Matrix4x4f& aParentJointTransform, std::array<Math::Matrix4x4f, 128>& outTransforms);
    void UpdatePose(AnimationLayer& aAnimLayer);
    void BlendPoses(AnimationLayer& aAnimLayer, float aBlendFactor);

    const bool ValidateMesh() const;
    const bool ValidateJointIndex(unsigned aStartJoint) const;
    const bool ValidateJointName(const std::string& aStartJoint) const;
    const bool ValidateLayerIndex(unsigned aLayerIndex) const;
    const bool ValidateLayerJointName(const std::string& aStartJoint) const;
    const bool ValidateAnimationName(unsigned aLayerIndex, const std::string& aAnimationName) const;

    std::shared_ptr<Mesh> myMesh = nullptr;
    std::vector<std::shared_ptr<Material>> myMaterials;
    std::unordered_map<unsigned, unsigned> mySlotToIndex;
    bool myShouldViewcull = true;
    bool myCastShadows = true;

    std::vector<AnimationLayer> myAnimationLayers;
    std::unordered_map<std::string, unsigned> myJointNameToLayerIndex;

    std::array<Math::Matrix4x4f, 128> myJointTransforms;
};

