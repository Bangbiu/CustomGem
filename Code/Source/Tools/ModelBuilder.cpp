#include "ModelBuilder.h"

#include <AzCore/Asset/AssetManager.h>
#include <AzCore/Math/Vector3.h>

#include <Atom/RHI.Reflect/BufferPoolDescriptor.h>
#include <Atom/RPI.Reflect/Buffer/BufferAsset.h>
#include <Atom/RPI.Reflect/Buffer/BufferAssetCreator.h>
#include <Atom/RPI.Reflect/Model/ModelLodAsset.h>
#include <Atom/RPI.Reflect/Model/ModelLodAssetCreator.h>
#include <Atom/RPI.Reflect/Model/ModelAssetCreator.h>
#include <Atom/RPI.Reflect/ResourcePoolAsset.h>
#include <Atom/RPI.Reflect/ResourcePoolAssetCreator.h>

namespace CustomGem
{
    using namespace AZ;
    using namespace AZ::RPI;

    Data::Asset<BufferAsset> ModelBuilder::MakeBufferAsset(
        const void* data, uint32_t elementCount, uint32_t elementSize)
    {
        // 1) Create a simple host-visible InputAssembly buffer pool
        Data::Asset<ResourcePoolAsset> bufferPoolAsset;
        {
            const Data::AssetId poolId = Uuid::CreateRandom();
            bufferPoolAsset = Data::AssetManager::Instance().CreateAsset(
                poolId, azrtti_typeid<ResourcePoolAsset>(), Data::AssetLoadBehavior::PreLoad);

            auto poolDesc = AZStd::make_unique<RHI::BufferPoolDescriptor>();
            poolDesc->m_bindFlags = RHI::BufferBindFlags::InputAssembly;
            poolDesc->m_heapMemoryLevel = RHI::HeapMemoryLevel::Host;

            ResourcePoolAssetCreator poolCreator;
            poolCreator.Begin(poolId);
            poolCreator.SetPoolDescriptor(AZStd::move(poolDesc));
            poolCreator.SetPoolName("ModelBuilderBufferPool");
            poolCreator.End(bufferPoolAsset);
        }

        // 2) Create the buffer asset with a copy of the provided data
        Data::Asset<BufferAsset> bufferAsset;
        {
            const Data::AssetId bufId = Uuid::CreateRandom();
            bufferAsset = Data::AssetManager::Instance().CreateAsset(
                bufId, azrtti_typeid<BufferAsset>(), Data::AssetLoadBehavior::PreLoad);

            RHI::BufferDescriptor desc;
            desc.m_bindFlags = RHI::BufferBindFlags::InputAssembly;
            desc.m_byteCount = elementCount * elementSize;

            BufferAssetCreator creator;
            creator.Begin(bufId);
            creator.SetPoolAsset(bufferPoolAsset);
            creator.SetBuffer(data, desc.m_byteCount, desc);
            // Use a structured view (count + stride); callers pick the typed format on the mesh side.
            creator.SetBufferViewDescriptor(RHI::BufferViewDescriptor::CreateStructured(0, elementCount, elementSize));
            creator.End(bufferAsset);
        }

        return bufferAsset;
    }

    AZ::Data::Asset<AZ::RPI::ModelAsset> ModelBuilder::CreateModel(
        const Name& name,
        AZStd::span<const uint32_t> indices,
        AZStd::span<const float> positions,
        AZStd::span<const float> normals,
        AZStd::span<const float> tangents,
        AZStd::span<const float> bitangents,
        AZStd::span<const float> uvs)
    {

        // ---- Build one LOD with one mesh ----
        const Data::AssetId lodId = Uuid::CreateRandom();
        Data::Asset<ModelLodAsset> lodAsset =
            Data::AssetManager::Instance().CreateAsset(lodId, azrtti_typeid<ModelLodAsset>(), Data::AssetLoadBehavior::PreLoad);

        ModelLodAssetCreator lodCreator;
        lodCreator.Begin(lodId);

        // AABB from positions (float3)
        Aabb aabb = Aabb::CreateNull();
        for (size_t i = 0; i + 2 < positions.size(); i += 3)
        {
            aabb.AddPoint(Vector3(positions[i + 0], positions[i + 1], positions[i + 2]));
        }

        const uint32_t indexCount     = static_cast<uint32_t>(indices.size());
        const uint32_t positionCount  = static_cast<uint32_t>(positions.size() / 3);
        const uint32_t normalCount    = static_cast<uint32_t>(normals.size() / 3);
        const uint32_t tangentCount   = static_cast<uint32_t>(tangents.size() / 4);
        const uint32_t bitangentCount = static_cast<uint32_t>(bitangents.size() / 3);
        const uint32_t uvCount        = static_cast<uint32_t>(uvs.size() / 2);

        lodCreator.BeginMesh();
        lodCreator.SetMeshAabb(aabb);
        lodCreator.SetMeshMaterialSlot(0); // keep it valid; no real material bound

        // Indices (uint32)
        lodCreator.SetMeshIndexBuffer(
            {
                MakeBufferAsset(indices.data(), indexCount, sizeof(uint32_t)),
                RHI::BufferViewDescriptor::CreateTyped(0, indexCount, RHI::Format::R32_UINT)
            });

        // POSITION
        lodCreator.AddMeshStreamBuffer(
            RHI::ShaderSemantic(Name("POSITION")), Name(),
            {
                MakeBufferAsset(positions.data(), positionCount, sizeof(float) * 3),
                RHI::BufferViewDescriptor::CreateTyped(0, positionCount, RHI::Format::R32G32B32_FLOAT)
            });

        // NORMAL (optional)
        if (normalCount)
        {
            lodCreator.AddMeshStreamBuffer(
                RHI::ShaderSemantic(Name("NORMAL")), Name(),
                {
                    MakeBufferAsset(normals.data(), normalCount, sizeof(float) * 3),
                    RHI::BufferViewDescriptor::CreateTyped(0, normalCount, RHI::Format::R32G32B32_FLOAT)
                });
        }

        // TANGENT (optional)
        if (tangentCount)
        {
            lodCreator.AddMeshStreamBuffer(
                RHI::ShaderSemantic(Name("TANGENT")), Name(),
                {
                    MakeBufferAsset(tangents.data(), tangentCount, sizeof(float) * 4),
                    RHI::BufferViewDescriptor::CreateTyped(0, tangentCount, RHI::Format::R32G32B32A32_FLOAT)
                });
        }

        // BITANGENT (optional)
        if (bitangentCount)
        {
            lodCreator.AddMeshStreamBuffer(
                RHI::ShaderSemantic(Name("BITANGENT")), Name(),
                {
                    MakeBufferAsset(bitangents.data(), bitangentCount, sizeof(float) * 3),
                    RHI::BufferViewDescriptor::CreateTyped(0, bitangentCount, RHI::Format::R32G32B32_FLOAT)
                });
        }

        // UV (optional)
        if (uvCount)
        {
            lodCreator.AddMeshStreamBuffer(
                RHI::ShaderSemantic(Name("UV")), Name(),
                {
                    MakeBufferAsset(uvs.data(), uvCount, sizeof(float) * 2),
                    RHI::BufferViewDescriptor::CreateTyped(0, uvCount, RHI::Format::R32G32_FLOAT)
                });
        }

        lodCreator.EndMesh();
        lodCreator.End(lodAsset);

        
        // ---- Final ModelAsset via ModelAssetCreator (public) ----
        const Data::AssetId modelId = Uuid::CreateRandom();
        Data::Asset<ModelAsset> result =
            Data::AssetManager::Instance()
            .CreateAsset(modelId, azrtti_typeid<ModelAsset>(), 
            Data::AssetLoadBehavior::PreLoad);

        AZ::RPI::ModelAssetCreator modelCreator;
        modelCreator.Begin(modelId);
        modelCreator.SetName(name.GetStringView());

        // Define a single material slot with stable id 0 and a readable name.
        // DrawListTag can be left default ({}); engine will resolve a default tag per pass.
        AZ::RPI::ModelMaterialSlot slot(
            AZ::RPI::ModelMaterialSlot::StableId{0},
            AZ::Name("Default"));

        modelCreator.AddMaterialSlot(slot);

        modelCreator.AddLodAsset(AZStd::move(lodAsset));
        modelCreator.End(result);

        return result;
    }


    AZ::Data::Asset<AZ::RPI::ModelAsset> ModelBuilder::CreateModel(
        const AZ::Name& name,
        const MeshData& mesh)
    {
        // AZ_Printf("CustomGem", "Creating model '%s' with %zu vertices and %zu indices.\n",
        //     name.c_str(), meshData.positions.size() / 3, meshData.indices.size());
        return CreateModel(
            name,
            AZStd::span<const uint32_t>(mesh.indices.data(),    mesh.indices.size()),
            AZStd::span<const float>(   mesh.positions.data(),  mesh.positions.size()),
            AZStd::span<const float>(   mesh.normals.data(),    mesh.normals.size()),
            AZStd::span<const float>(   mesh.tangents.data(),   mesh.tangents.size()),
            AZStd::span<const float>(   mesh.bitangents.data(), mesh.bitangents.size()),
            AZStd::span<const float>(   mesh.uvs.data(),        mesh.uvs.size())
        );
    }

    AZ::Data::Asset<AZ::RPI::ModelAsset> ModelBuilder::BuildPlane() {
        using namespace CustomGem;

        // 1) Stage geometry into MeshData via the utility
        MeshData mesh;
        // plane = 1 => XZ plane (Y=0), normal +Y; corner = (0.5, 0, 0.5) gives a 1x1 quad
        MeshUtils::FillQuad(mesh, AZ::Vector3(-0.5f, 0.0f, 0.5f), /*plane=*/1);

        // 2) Feed CreateModel with spans over the staged vectors
        return CreateModel(AZ::Name("ProceduralPlane"), mesh);
    }

    AZ::Data::Asset<AZ::RPI::ModelAsset> ModelBuilder::BuildCube() {
        constexpr int ValuesPerPositionEntry = 3;
        constexpr int ValuesPerUvEntry = 2;
        constexpr int ValuesPerNormalEntry = 3;
        constexpr int ValuesPerTangentEntry = 4;

        constexpr int VerticesPerFace = 6;
        constexpr int MeshFaces = 6;
        constexpr int ValuesPerFace = 4;

        // 6 vertices per face, 6 faces.
        constexpr AZStd::array<uint32_t, VerticesPerFace * MeshFaces> indices = {
            0,  1,  2,  0,  2,  3,   // front face
            4,  5,  6,  4,  6,  7,   // right face
            8,  9, 10,  8, 10, 11,   // back face
            12, 13, 14, 12, 14, 15,   // left face
            16, 17, 18, 16, 18, 19,   // top face
            20, 21, 22, 20, 22, 23    // bottom face
        };

        // 3 values per position, 4 positions per face, 6 faces
        constexpr AZStd::array<float, ValuesPerPositionEntry * ValuesPerFace * MeshFaces> positions = {
            -0.5f, -0.5f, -0.5f, +0.5f, -0.5f, -0.5f, +0.5f, -0.5f, +0.5f, -0.5f, -0.5f, +0.5f,     // front
            +0.5f, -0.5f, -0.5f, +0.5f, +0.5f, -0.5f, +0.5f, +0.5f, +0.5f, +0.5f, -0.5f, +0.5f,     // right
            +0.5f, +0.5f, -0.5f, -0.5f, +0.5f, -0.5f, -0.5f, +0.5f, +0.5f, +0.5f, +0.5f, +0.5f,     // back
            -0.5f, +0.5f, -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, +0.5f, -0.5f, +0.5f, +0.5f,     // left
            -0.5f, -0.5f, +0.5f, +0.5f, -0.5f, +0.5f, +0.5f, +0.5f, +0.5f, -0.5f, +0.5f, +0.5f,     // top
            -0.5f, +0.5f, -0.5f, +0.5f, +0.5f, -0.5f, +0.5f, -0.5f, -0.5f, -0.5f, -0.5f, -0.5f,     // bottom
        };

        // 2 values per position, 4 positions per face, 6 faces
        // This aribtrarily maps the UVs to use the full texture on each face.
        // This choice can be changed if a different mapping would be more usable.
        constexpr AZStd::array<float, ValuesPerUvEntry * ValuesPerFace * MeshFaces> uvs = {
            0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,     // front
            0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,     // right
            0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,     // back
            0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,     // left
            0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,     // top
            0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,     // bottom
        };

        // 3 values per position, 4 positions per face, 6 faces
        constexpr AZStd::array<float, ValuesPerNormalEntry * ValuesPerFace * MeshFaces> normals = {
            +0.0f, -1.0f, +0.0f, +0.0f, -1.0f, +0.0f, +0.0f, -1.0f, +0.0f, +0.0f, -1.0f, +0.0f,     // front (-Y)
            +1.0f, +0.0f, +0.0f, +1.0f, +0.0f, +0.0f, +1.0f, +0.0f, +0.0f, +1.0f, +0.0f, +0.0f,     // right (+X)
            +0.0f, +1.0f, +0.0f, +0.0f, +1.0f, +0.0f, +0.0f, +1.0f, +0.0f, +0.0f, +1.0f, +0.0f,     // back (+Y)
            -1.0f, +0.0f, +0.0f, -1.0f, +0.0f, +0.0f, -1.0f, +0.0f, +0.0f, -1.0f, +0.0f, +0.0f,     // left (-X)
            +0.0f, +0.0f, +1.0f, +0.0f, +0.0f, +1.0f, +0.0f, +0.0f, +1.0f, +0.0f, +0.0f, +1.0f,     // top (+Z)
            +0.0f, +0.0f, -1.0f, +0.0f, +0.0f, -1.0f, +0.0f, +0.0f, -1.0f, +0.0f, +0.0f, -1.0f,     // bottom (-Z)
        };

        // 4 values per position, 4 positions per face, 6 faces
        constexpr AZStd::array<float, ValuesPerTangentEntry * ValuesPerFace * MeshFaces> tangents = {
            0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // front (+Z)
            0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // right (+Z)
            0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // back (+Z)
            0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // left (+Z)
            0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, // top (+Y)
            0.0f, -1.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, // bottom (-Y)
        };

        //CreateModel(modelAsset, AZ::Name("UnitCube"), indices, positions, normals, tangents, bitangents, uvs);
        return CreateModel(
            AZ::Name("UnitCube")
            , indices, positions, normals, tangents, {}, uvs
        );
    }
} // namespace CustomGem
