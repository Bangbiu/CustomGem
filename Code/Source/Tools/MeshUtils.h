#pragma once

#include <AzCore/std/containers/vector.h>
#include <AzCore/Math/Vector2.h>
#include <AzCore/Math/Vector3.h>

namespace CustomGem
{
    struct UVIndex {
        int segment;
        int index;
    };

    struct MeshData
    {
        AZStd::vector<uint32_t> indices;
        AZStd::vector<float> positions;   // x, y, z
        AZStd::vector<float> normals;     // nx, ny, nz
        AZStd::vector<float> tangents;    // tx, ty, tz, tw
        AZStd::vector<float> bitangents;  // bx, by, bz
        AZStd::vector<float> uvs;         // u, v

        //! Convenience clear function
        void Clear()
        {
            indices.clear();
            positions.clear();
            normals.clear();
            tangents.clear();
            bitangents.clear();
            uvs.clear();
        }

        //! Returns whether tangent and bitangent data are available
        bool HasTangents() const { return !tangents.empty(); }
        bool HasBitangents() const { return !bitangents.empty(); }
        bool HasUVs() const { return !uvs.empty(); }
    };

    struct MeshUtils
    {

        //! Append a quad defined corner and plane
        //! @param mesh Target mesh data to append to
        //! @param corner The top left corner of the quad
        //! @param orientation 0..5 => (+Z), (-Z), (-X), (+X), (+Y), (-Y)
        //! @param uv segment and index to locate uv
        //! The quad will be on the plane always be 1x1
        static void PushQuad(MeshData& mesh, const AZ::Vector3& corner, int orientation, UVIndex uv);
        // Overloaded for 0.0-1.0 UV
        static void PushQuad(MeshData& mesh, const AZ::Vector3& corner, int orientation);

        static void PushVertex(MeshData& m,
                               const AZ::Vector3& p,
                               const AZ::Vector3& n,
                               const AZ::Vector3& t,
                               const AZ::Vector3& b,
                               float u, float v);

        static void ComputeUvRect(const CustomGem::UVIndex& uv, float& u0, float& v0, float& u1, float& v1);
    };
}