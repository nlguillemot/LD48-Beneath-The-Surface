#ifndef GLMESH_H
#define GLMESH_H

#include <GLplus.hpp>

namespace tinyobj
{
    struct shape_t;
} // end namespace tinyobj

namespace GLmesh
{

class StaticMesh
{
    std::shared_ptr<GLplus::Buffer> mpPositions;
    std::shared_ptr<GLplus::Buffer> mpTexcoords;
    std::shared_ptr<GLplus::Buffer> mpNormals;
    std::shared_ptr<GLplus::Buffer> mpIndices;

    size_t mVertexCount = 0;

    std::shared_ptr<GLplus::Texture2D> mpDiffuseTexture;

public:
    void LoadShape(const tinyobj::shape_t& shape);

    void Render(GLplus::Program& program) const;
};

} // end namespace GLmesh

#endif // GLMESH_H
