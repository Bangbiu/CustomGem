#include "MeshUtils.h"
#include <algorithm>

namespace CustomGem
{
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
    

    void MeshUtils::FillQuad(MeshData& mesh, const AZ::Vector3& corner, int plane)
    {
        using AZ::Vector3;

        // Indices start
        const uint32_t base = static_cast<uint32_t>(mesh.positions.size() / 3);

        // Corner aliases (top-left provided), compute the other 3 corners per plane
        Vector3 tl, tr, br, bl;        // positions
        Vector3 n, t, b;               // normal, tangent (+U), bitangent (+V)

        switch (plane)
        {
        case 0: // XY plane, normal +Z
            // Positions
            tl = Vector3(corner.GetX(),         corner.GetY(),         corner.GetZ());
            tr = Vector3(corner.GetX() + 1.0f,  corner.GetY(),         corner.GetZ());
            br = Vector3(corner.GetX() + 1.0f,  corner.GetY() - 1.0f,  corner.GetZ());
            bl = Vector3(corner.GetX(),         corner.GetY() - 1.0f,  corner.GetZ());
            // Frame
            n = Vector3(0.0f, 0.0f, 1.0f);
            t = Vector3(1.0f, 0.0f, 0.0f);  // +X is +U
            b = Vector3(0.0f, 1.0f, 0.0f);  // +Y is +V
            break;

        case 1: // XZ plane, normal +Y
            tl = Vector3(corner.GetX(),         corner.GetY(),         corner.GetZ());
            tr = Vector3(corner.GetX() + 1.0f,  corner.GetY(),         corner.GetZ());
            br = Vector3(corner.GetX() + 1.0f,  corner.GetY(),         corner.GetZ() - 1.0f);
            bl = Vector3(corner.GetX(),         corner.GetY(),         corner.GetZ() - 1.0f);
            n = Vector3(0.0f, 1.0f, 0.0f);
            t = Vector3(1.0f, 0.0f, 0.0f);  // +X is +U
            b = Vector3(0.0f, 0.0f, 1.0f);  // +Z is +V
            break;

        case 2: // YZ plane, normal +X
        default:
            tl = Vector3(corner.GetX(),         corner.GetY(),         corner.GetZ());
            tr = Vector3(corner.GetX(),         corner.GetY(),         corner.GetZ() + 1.0f);
            br = Vector3(corner.GetX(),         corner.GetY() - 1.0f,  corner.GetZ() + 1.0f);
            bl = Vector3(corner.GetX(),         corner.GetY() - 1.0f,  corner.GetZ());
            n = Vector3(1.0f, 0.0f, 0.0f);
            t = Vector3(0.0f, 0.0f, 1.0f);  // +Z is +U
            b = Vector3(0.0f, 1.0f, 0.0f);  // +Y is +V
            break;
        }

        // Push vertices in CCW order w.r.t. the chosen normal: BL, BR, TR, TL
        // UVs: TL(0,1), TR(1,1), BR(1,0), BL(0,0)
        PushVertex(mesh, bl, n, t, b, 0.0f, 0.0f);
        PushVertex(mesh, br, n, t, b, 1.0f, 0.0f);
        PushVertex(mesh, tr, n, t, b, 1.0f, 1.0f);
        PushVertex(mesh, tl, n, t, b, 0.0f, 1.0f);

        // Two triangles
        mesh.indices.insert(mesh.indices.end(),
            { base + 0, base + 1, base + 2,  // BL, BR, TR
            base + 0, base + 2, base + 3 } // BL, TR, TL
        );
    }

}
