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

    AZ::Data::Asset<AZ::RPI::ModelAsset> ModelBuilder::BuildPlane(const AZ::Vector3& pos) {
        MeshData mesh;
        MeshUtils::PushQuad(mesh, pos, 0);
        return CreateModel(AZ::Name("ProceduralPlane"), mesh);
    }

    AZ::Data::Asset<AZ::RPI::ModelAsset> ModelBuilder::BuildPlane(const float x, const float y, const float z) {
        return BuildPlane(AZ::Vector3(x, y, z));
    }

    AZ::Data::Asset<AZ::RPI::ModelAsset> ModelBuilder::BuildPlane() {
        return BuildPlane(-0.5f, 0.0f, 0.5f);
    }

    AZ::Data::Asset<AZ::RPI::ModelAsset> ModelBuilder::BuildCube()
    {
        MeshData mesh;

        // The cube extends from (0,0,0) to (1,1,1)
        // Each PushQuad builds one oriented face at the corresponding side.

        MeshUtils::PushQuad(mesh, AZ::Vector3(0.0f, 1.0f, 1.0f), 0, {3, 0}); // +Z (front)
        MeshUtils::PushQuad(mesh, AZ::Vector3(1.0f, 1.0f, 0.0f), 1, {3, 2}); // -Z (back)
        MeshUtils::PushQuad(mesh, AZ::Vector3(0.0f, 1.0f, 0.0f), 2, {3, 1}); // -X (left)
        MeshUtils::PushQuad(mesh, AZ::Vector3(1.0f, 1.0f, 1.0f), 3, {3, 1}); // +X (right)
        MeshUtils::PushQuad(mesh, AZ::Vector3(0.0f, 1.0f, 1.0f), 4, {3, 1}); // +Y (top)
        MeshUtils::PushQuad(mesh, AZ::Vector3(0.0f, 0.0f, 0.0f), 5, {3, 1}); // -Y (bottom)

        return CreateModel(AZ::Name("ProceduralCube"), mesh);
    }
} // namespace CustomGem
