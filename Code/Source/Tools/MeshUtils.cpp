#include "MeshUtils.h"
#include <algorithm>

namespace CustomGem
{
    // Choose T (right) and B (up) so that:
    // 1) corner is the TOP-LEFT vertex on the face's plane,
    // 2) B points "up" within that plane by a consistent convention,
    // 3) T x B = N (right-handed).
    inline void ComputeFaceBasis(int orientation, AZ::Vector3& N, AZ::Vector3& T, AZ::Vector3& B)
    {
        switch (orientation)
        {
        // +Z (front): screen-up = +Y, right = +X
        case 0: N = { 0, 0,  1}; T = { 1, 0,  0}; B = { 0, 1,  0}; break;

        // -Z (back): screen-up = +Y, right = -X
        case 1: N = { 0, 0, -1}; T = {-1, 0,  0}; B = { 0, 1,  0}; break;

        // -X (left): screen-up = +Y, right = +Z
        case 2: N = {-1, 0,  0}; T = { 0, 0,  1}; B = { 0, 1,  0}; break;

        // +X (right): screen-up = +Y, right = -Z
        case 3: N = { 1, 0,  0}; T = { 0, 0, -1}; B = { 0, 1,  0}; break;

        // +Y (top): screen-up = +Z, right = +X  (so top is towards +Z)
        case 4: N = { 0, 1,  0}; T = { 1, 0,  0}; B = { 0, 0,  1}; break;

        // -Y (bottom): screen-up = -Z, right = +X (so top is towards -Z)
        case 5: N = { 0,-1,  0}; T = { 1, 0,  0}; B = { 0, 0, -1}; break;

        default:
            AZ_Assert(false, "FillQuad: orientation out of range [0..5]");
            N = {0,0,1}; T = {1,0,0}; B = {0,1,0};
            break;
        }

        // Sanity: enforce right-handedness (T x B == N) by flipping T if needed
        if (!AZ::IsClose((T.Cross(B)).Dot(N), 1.0f, 1e-3f))
        {
            T = -T;
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

        // Define per-orientation normal (N), tangent (T = +U direction), and bitangent (B = +V direction)
        // Chosen so that T x B = N (right-handed).
        AZ::Vector3 N, T, B;
        ComputeFaceBasis(orientation, N, T, B);

        const uint32_t base = static_cast<uint32_t>(mesh.positions.size() / 3);
        const AZ::Vector3 TL = corner;
        const AZ::Vector3 TR = corner + T;        // right
        const AZ::Vector3 BR = corner + T - B;    // right then down
        const AZ::Vector3 BL = corner - B;        // down

        float u0, v0, u1, v1;
        ComputeUvRect(uv, u0, v0, u1, v1);

        // TL -> (u0,v0), TR -> (u1,v0), BR -> (u1,v1), BL -> (u0,v1)
        PushVertex(mesh, TL, N, T, B, u0, v0);
        PushVertex(mesh, TR, N, T, B, u1, v0);
        PushVertex(mesh, BR, N, T, B, u1, v1);
        PushVertex(mesh, BL, N, T, B, u0, v1);

        mesh.indices.insert(mesh.indices.end(),
        {
            base + 0, base + 3, base + 2,
            base + 0, base + 2, base + 1
        });
    }

    void MeshUtils::PushQuad(MeshData& mesh, const AZ::Vector3& corner, int orientation) {
        PushQuad(mesh, corner, orientation, {1, 0});
    }

}
