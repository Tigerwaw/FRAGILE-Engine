#include "Enginepch.h"
#include "MeshAsset.h"
#include "TGAFBX/TGAFbx.h"
#include "Objects/Vertices/Vertex.h"

bool MeshAsset::Load()
{
    TGA::FBX::Mesh tgaMesh;
    TGA::FBX::Importer::LoadMesh(GetPath(), tgaMesh);

    Math::Vector3f minBBPoint;
    Math::Vector3f maxBBPoint;

    Mesh newMesh;

    for (auto& tgaElement : tgaMesh.Elements)
    {
        std::vector<Vertex> vertices;
        vertices.reserve(tgaElement.Vertices.size());
        for (auto& v : tgaElement.Vertices)
        {
            vertices.emplace_back(v.Position, v.VertexColors, v.BoneIDs, v.BoneWeights, v.UVs, v.Normal, v.Tangent);

            minBBPoint.x = v.Position[0] < minBBPoint.x ? v.Position[0] : minBBPoint.x;
            minBBPoint.y = v.Position[1] < minBBPoint.y ? v.Position[1] : minBBPoint.y;
            minBBPoint.z = v.Position[2] < minBBPoint.z ? v.Position[2] : minBBPoint.z;

            maxBBPoint.x = v.Position[0] > maxBBPoint.x ? v.Position[0] : maxBBPoint.x;
            maxBBPoint.y = v.Position[1] > maxBBPoint.y ? v.Position[1] : maxBBPoint.y;
            maxBBPoint.z = v.Position[2] > maxBBPoint.z ? v.Position[2] : maxBBPoint.z;
        }

        std::vector<unsigned> indices;
        indices.reserve(tgaElement.Indices.size());
        for (auto& i : tgaElement.Indices)
        {
            indices.emplace_back(i);
        }

        newMesh.AddSubmesh(std::move(vertices), std::move(indices), tgaElement.MaterialIndex);
    }

    Mesh::Skeleton skeleton;
    if (tgaMesh.Skeleton.Bones.size() > 0)
    {
        skeleton.myJoints.reserve(tgaMesh.Skeleton.Bones.size());

        for (auto& tgaJoint : tgaMesh.Skeleton.Bones)
        {
            Mesh::Skeleton::Joint& joint = skeleton.myJoints.emplace_back();
            joint.Parent = tgaJoint.ParentIdx;
            joint.Children = tgaJoint.Children;
            joint.Name = tgaJoint.Name;
            auto& matrix = tgaJoint.BindPoseInverse.Data;
            joint.BindPoseInverse = { matrix[0], matrix[1], matrix[2], matrix[3],
                                      matrix[4], matrix[5], matrix[6], matrix[7],
                                      matrix[8], matrix[9], matrix[10], matrix[11],
                                      matrix[12], matrix[13], matrix[14], matrix[15] };

            joint.BindPoseInverse = joint.BindPoseInverse.GetTranspose();
        }

        skeleton.JointNameToIndex = tgaMesh.Skeleton.BoneNameToIndex;

        newMesh.SetSkeleton(std::move(skeleton));
    }

    newMesh.InitBoundingBox(minBBPoint, maxBBPoint);
    mesh = std::make_shared<Mesh>(std::move(newMesh));
    return true;
}

bool MeshAsset::Unload()
{
    mesh = nullptr;
    return true;
}
