#pragma once
#include "MeshUtils.h"

#include <AzCore/Asset/AssetCommon.h>
#include <AzCore/Math/Aabb.h>
#include <AzCore/std/containers/span.h>

#include <Atom/RHI.Reflect/Format.h>
#include <Atom/RHI.Reflect/BufferViewDescriptor.h>
#include <Atom/RPI.Reflect/Model/ModelAsset.h>

namespace CustomGem
{
    //! Minimal extraction of O3DE's ModelAssetHelpers "create model" logic.
    struct ModelBuilder
    {
        //! Build a single-LOD, single-mesh ModelAsset from raw arrays.
        //! All spans are tightly packed (no stride) and use the formats noted below.
        //! Formats:
        //!   indices:    uint32 (R32_UINT)
        //!   positions:  float3 (R32G32B32_FLOAT)
        //!   normals:    float3 (R32G32B32_FLOAT)
        //!   tangents:   float4 (R32G32B32A32_FLOAT)
        //!   bitangents: float3 (R32G32B32_FLOAT)
        //!   uvs:        float2 (R32G32_FLOAT)
        static AZ::Data::Asset<AZ::RPI::ModelAsset> CreateModel(
            const AZ::Name& name,
            AZStd::span<const uint32_t> indices,
            AZStd::span<const float> positions,
            AZStd::span<const float> normals,
            AZStd::span<const float> tangents,
            AZStd::span<const float> bitangents,
            AZStd::span<const float> uvs);

        static AZ::Data::Asset<AZ::RPI::ModelAsset> CreateModel(
            const AZ::Name& name,
            const MeshData& mesh);
        
        static AZ::Data::Asset<AZ::RPI::ModelAsset> BuildPlane();
        static AZ::Data::Asset<AZ::RPI::ModelAsset> BuildPlane(const AZ::Vector3& pos);
        static AZ::Data::Asset<AZ::RPI::ModelAsset> BuildPlane(const float x, const float y, const float z);
        static AZ::Data::Asset<AZ::RPI::ModelAsset> BuildCube();
    private:
        static AZ::Data::Asset<AZ::RPI::BufferAsset> MakeBufferAsset(
            const void* data, uint32_t elementCount, uint32_t elementSize);
    };
} // namespace CustomGem
