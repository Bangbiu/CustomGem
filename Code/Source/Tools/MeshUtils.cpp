#include "MeshUtils.h"
#include <algorithm>

namespace CustomGem
{
    inline void ComputeFaceBasisPositiveAxes(int orientation, AZ::Vector3& N, AZ::Vector3& T, AZ::Vector3& B)
    {
        switch (orientation)
        {
            case 0: N = { 0, 0,  1}; break; // +Z
            case 1: N = { 0, 0, -1}; break; // -Z
            case 2: N = {-1, 0,  0}; break; // -X
            case 3: N = { 1, 0,  0}; break; // +X
            case 4: N = { 0, 1,  0}; break; // +Y
            case 5: N = { 0,-1,  0}; break; // -Y
            default:
                AZ_Assert(false, "PushQuad: orientation out of range [0..5]");
                N = {0,0,1};
                break;
        }

        // Pick T,B by plane so they always point to positive world axes of that plane
        if (AZ::IsClose(AZStd::abs(N.GetZ()), 1.0f, 1e-6f))
        {   // XY plane
            T = { 1, 0, 0}; // +X
            B = { 0, 1, 0}; // +Y
        }
        else if (AZ::IsClose(AZStd::abs(N.GetX()), 1.0f, 1e-6f))
        {   // YZ plane
            T = { 0, 0, 1}; // +Z
            B = { 0, 1, 0}; // +Y
        }
        else
        {   // ZX plane (abs(N.y) == 1)
            T = { 1, 0, 0}; // +X
            B = { 0, 0, 1}; // +Z
        }
    }

    void MeshUtils::ComputeUvRect(const CustomGem::UVIndex& uv, float& u0, float& v0, float& u1, float& v1)
    {
        // Guard against invalid input; fall back to full range
        if (uv.segment <= 1)
        {
            u0 = 0.0f; v0 = 0.0f; u1 = 1.0f; v1 = 1.0f;
            return;
        }

        const int seg = uv.segment;
        const int tiles = seg * seg;
        int idx = uv.index;

        if (idx < 0) idx = 0;
        if (idx >= tiles) idx = tiles - 1; // clamp

        const int row = idx / seg; // 0 .. seg-1
        const int col = idx % seg; // 0 .. seg-1

        const float step = 1.0f / static_cast<float>(seg);
        u0 = col * step;
        v0 = row * step;
        u1 = u0 + step;
        v1 = v0 + step;
    }

    void MeshUtils::PushVertex(MeshData& m,
                        const AZ::Vector3& p,
                        const AZ::Vector3& n,
                        const AZ::Vector3& t,
                        const AZ::Vector3& b,
                        float u, float v)
    {
        m.positions.insert(m.positions.end(), { p.GetX(), p.GetY(), p.GetZ() });
        m.normals.insert(m.normals.end(),   { n.GetX(), n.GetY(), n.GetZ() });
        // Tangent with handedness w=1
        m.tangents.insert(m.tangents.end(), { t.GetX(), t.GetY(), t.GetZ(), 1.0f });
        m.bitangents.insert(m.bitangents.end(), { b.GetX(), b.GetY(), b.GetZ() });
        m.uvs.insert(m.uvs.end(), { u, v });
    }
    
    void MeshUtils::PushQuad(MeshData& mesh, const AZ::Vector3& corner, int orientation, UVIndex uv)
    {
        AZ_Assert(orientation >= 0 && orientation <= 5, "PushQuad: orientation out of range [0..5]");

        AZ::Vector3 N, T, B;
        ComputeFaceBasisPositiveAxes(orientation, N, T, B);

        // Build corners so geometry expands along +T and +B from 'corner'
        const uint32_t base = static_cast<uint32_t>(mesh.positions.size() / 3);
        const AZ::Vector3 LL = corner;          // lower-left (min along T & B)
        const AZ::Vector3 LR = corner + T;      // +T
        const AZ::Vector3 UR = corner + T + B;  // +T +B
        const AZ::Vector3 UL = corner + B;      // +B

        float u0, v0, u1, v1;
        ComputeUvRect(uv, u0, v0, u1, v1);

        // u along +T, v along +B
        PushVertex(mesh, LL, N, T, B, u0, v0);
        PushVertex(mesh, LR, N, T, B, u1, v0);
        PushVertex(mesh, UR, N, T, B, u1, v1);
        PushVertex(mesh, UL, N, T, B, u0, v1);

        // Maintain correct front-face winding: CCW when looking along +N.
        const bool needsFlip = ((T.Cross(B)).Dot(N) < 0.0f);

        if (!needsFlip)
        {
            // CCW: LL, LR, UR and LL, UR, UL
            mesh.indices.insert(mesh.indices.end(),
            {
                base + 0, base + 1, base + 2,
                base + 0, base + 2, base + 3
            });
        }
        else
        {
            // Flip winding to keep face front-facing relative to N
            mesh.indices.insert(mesh.indices.end(),
            {
                base + 0, base + 2, base + 1,
                base + 0, base + 3, base + 2
            });
        }
    }

    void MeshUtils::PushQuad(MeshData& mesh, const AZ::Vector3& corner, int orientation) {
        PushQuad(mesh, corner, orientation, {1, 0});
    }

}
