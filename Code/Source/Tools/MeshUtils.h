#pragma once

#include <AzCore/std/containers/vector.h>
#include <AzCore/Math/Vector2.h>
#include <AzCore/Math/Vector3.h>

namespace CustomGem
{
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
        //! Append a quad defined by two opposite corners to a MeshData.
        //! @param mesh Target mesh data to append to
        //! @param corner The corner of the quad
        //! @param plane x,y,z plane
        //! The quad will be on the plane
        static void FillQuad(MeshData& mesh, const AZ::Vector3& corner, int plane);
    };
}