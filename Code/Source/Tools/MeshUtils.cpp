#include "MeshUtils.h"
#include <algorithm>

namespace CustomGem
{
    namespace
    {
        // Push one vertex (pos, n, t, b, uv)
        inline void PushVertex(MeshData& m,
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
    }

    void MeshUtils::FillQuad(MeshData& mesh, const AZ::Vector3& corner, int plane)
    {
        // Base index before adding four new vertices
        const uint32_t base = static_cast<uint32_t>(mesh.positions.size() / 3);

        // Clamp/normalize plane
        plane = std::max(0, std::min(2, plane));

        // Build axis-aligned corners symmetric around the origin on the selected plane.
        // Also define normal/tangent/bitangent consistent with UVs:
        //   U goes along the 'first' axis of the plane, V along the 'second' axis.
        AZ::Vector3 n, t, b;
        AZ::Vector3 p0, p1, p2, p3; // CCW winding for a front-facing quad.

        switch (plane)
        {
        case 0: // XY plane (Z = 0), normal +Z
        default:
        {
            const float x = corner.GetX();
            const float y = corner.GetY();

            n = AZ::Vector3(0.0f, 0.0f, 1.0f);
            t = AZ::Vector3(1.0f, 0.0f, 0.0f); // +U along +X
            b = AZ::Vector3(0.0f, 1.0f, 0.0f); // +V along +Y

            // CCW in XY: (-x,-y) -> (x,-y) -> (x,y) -> (-x,y)
            p0 = AZ::Vector3(-x, -y, 0.0f);
            p1 = AZ::Vector3( +x, -y, 0.0f);
            p2 = AZ::Vector3( +x,  +y, 0.0f);
            p3 = AZ::Vector3(-x,  +y, 0.0f);

            // UVs map full [0,1]x[0,1]
            PushVertex(mesh, p0, n, t, b, 0.0f, 0.0f);
            PushVertex(mesh, p1, n, t, b, 1.0f, 0.0f);
            PushVertex(mesh, p2, n, t, b, 1.0f, 1.0f);
            PushVertex(mesh, p3, n, t, b, 0.0f, 1.0f);
            break;
        }
        case 1: // XZ plane (Y = 0), normal +Y
        {
            const float x = corner.GetX();
            const float z = corner.GetZ();

            n = AZ::Vector3(0.0f, 1.0f, 0.0f);
            t = AZ::Vector3(1.0f, 0.0f, 0.0f); // +U along +X
            b = AZ::Vector3(0.0f, 0.0f, 1.0f); // +V along +Z

            // CCW as seen from +Y: (-x,-z) -> (x,-z) -> (x,z) -> (-x,z)
            p0 = AZ::Vector3(-x, 0.0f, -z);
            p1 = AZ::Vector3( +x, 0.0f, -z);
            p2 = AZ::Vector3( +x, 0.0f,  +z);
            p3 = AZ::Vector3(-x, 0.0f,  +z);

            PushVertex(mesh, p0, n, t, b, 0.0f, 0.0f);
            PushVertex(mesh, p1, n, t, b, 1.0f, 0.0f);
            PushVertex(mesh, p2, n, t, b, 1.0f, 1.0f);
            PushVertex(mesh, p3, n, t, b, 0.0f, 1.0f);
            break;
        }
        case 2: // YZ plane (X = 0), normal +X
        {
            const float y = corner.GetY();
            const float z = corner.GetZ();

            n = AZ::Vector3(1.0f, 0.0f, 0.0f);
            t = AZ::Vector3(0.0f, 1.0f, 0.0f); // +U along +Y
            b = AZ::Vector3(0.0f, 0.0f, 1.0f); // +V along +Z

            // CCW as seen from +X: (-y,-z) -> (y,-z) -> (y,z) -> (-y,z) in (Y,Z)
            p0 = AZ::Vector3(0.0f, -y, -z);
            p1 = AZ::Vector3(0.0f,  +y, -z);
            p2 = AZ::Vector3(0.0f,  +y,  +z);
            p3 = AZ::Vector3(0.0f, -y,  +z);

            PushVertex(mesh, p0, n, t, b, 0.0f, 0.0f);
            PushVertex(mesh, p1, n, t, b, 1.0f, 0.0f);
            PushVertex(mesh, p2, n, t, b, 1.0f, 1.0f);
            PushVertex(mesh, p3, n, t, b, 0.0f, 1.0f);
            break;
        }
        }

        // Two triangles: (0,1,2) and (0,2,3) with base offset
        mesh.indices.insert(mesh.indices.end(),
        {
            base + 0, base + 1, base + 2,
            base + 0, base + 2, base + 3
        });
    }
}
