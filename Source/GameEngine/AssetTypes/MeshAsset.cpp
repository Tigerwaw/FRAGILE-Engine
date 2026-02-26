#include "Enginepch.h"
#include "MeshAsset.h"
#include "TGAFBX/TGAFbx.h"
#include "Objects/Vertices/Vertex.h"
#include "CommonUtilities/StringUtilities.hpp"

// If mesh only contains one element -> Only create a submesh with one lod
// If mesh has multiple elements -> Check for LOD namestandard
// If mesh submeshes have LOD name standard -> Sort each mesh into a LOD hierarchy by each name

static bool IsLOD(const std::string& aSubmeshName)
{
    std::string lowercaseSubmeshName = Utilities::ToLowerCopy(aSubmeshName);
    std::string lodSubstr = "lod";
    return lowercaseSubmeshName.find(lodSubstr) != std::string::npos;
}

static unsigned GetLODLevel(const std::string& aSubmeshName)
{
    return static_cast<unsigned>(stoi(aSubmeshName.substr(aSubmeshName.length() - 1, 1)));
}

static std::string GetTrimmedName(const std::string& aLODName)
{
    return aLODName.substr(0, aLODName.length() - 5);
}

static bool HasSubmeshes(const TGA::FBX::Mesh& aMesh)
{
    return aMesh.Elements.size() > 1;
}

bool MeshAsset::Load()
{
    TGA::FBX::Mesh tgaMesh;
    TGA::FBX::Importer::LoadMesh(GetPath(), tgaMesh);

    Math::Vector3f minBBPoint;
    Math::Vector3f maxBBPoint;

    std::unordered_map<std::string, std::map<unsigned, unsigned>> submeshToLODs;

    for (unsigned i = 0; i < tgaMesh.Elements.size(); i++)
    {
        std::string submeshName = tgaMesh.Elements[i].MeshName;
        unsigned lodLevel = 0;
        if (IsLOD(submeshName))
        {
            lodLevel = GetLODLevel(submeshName);
            submeshName = GetTrimmedName(submeshName);
        }

        submeshToLODs[submeshName][lodLevel] = i;
    }

    Mesh newMesh;

    for (auto& [submeshName, lodLevelToElementIndex] : submeshToLODs)
    {
        Mesh::Submesh newSubmesh;

        for (auto& [lodLevel, elementIndex] : lodLevelToElementIndex)
        {
            TGA::FBX::Mesh::Element& tgaElement = tgaMesh.Elements[elementIndex];
            newSubmesh.MaterialIndex = tgaElement.MaterialIndex;

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

            newSubmesh.AddLOD(lodLevel, std::move(vertices), std::move(indices));
        }

        newMesh.AddSubmesh(std::move(newSubmesh));
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
