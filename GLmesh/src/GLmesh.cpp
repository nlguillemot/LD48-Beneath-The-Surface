#include "GLmesh.hpp"

#include <tiny_obj_loader.h>

namespace GLmesh
{

void StaticMesh::LoadShape(const tinyobj::shape_t& shape)
{
    if (shape.mesh.indices.size() % 3 != 0)
    {
        throw std::runtime_error("Expected 3d vertices.");
    }

    std::shared_ptr<GLplus::Buffer> newIndices;
    std::shared_ptr<GLplus::Buffer> newPositions;
    std::shared_ptr<GLplus::Buffer> newNormals;
    std::shared_ptr<GLplus::Buffer> newTexcoords;
    std::shared_ptr<GLplus::Texture2D> newDiffuseTexture;

    newIndices.reset(new GLplus::Buffer());
    {
        GLplus::ScopedBufferBinding bufferBinding(*newIndices, GL_ELEMENT_ARRAY_BUFFER);
        bufferBinding.GetBinding().Upload(
                  shape.mesh.indices.size() * sizeof(shape.mesh.indices[0]),
                  shape.mesh.indices.data(), GL_STATIC_DRAW);
    }

    if (!shape.mesh.positions.empty())
    {
        newPositions.reset(new GLplus::Buffer());
        GLplus::ScopedBufferBinding bufferBinding(*newPositions, GL_ARRAY_BUFFER);
        bufferBinding.GetBinding().Upload(
                    shape.mesh.positions.size() * sizeof(shape.mesh.positions[0]),
                    shape.mesh.positions.data(), GL_STATIC_DRAW);
    }

    if (!shape.mesh.normals.empty())
    {
        newNormals.reset(new GLplus::Buffer());
        GLplus::ScopedBufferBinding bufferBinding(*newNormals, GL_ARRAY_BUFFER);
        bufferBinding.GetBinding().Upload(
                    shape.mesh.normals.size() * sizeof(shape.mesh.normals[0]),
                    shape.mesh.normals.data(), GL_STATIC_DRAW);
    }

    if (!shape.mesh.texcoords.empty())
    {
        newTexcoords.reset(new GLplus::Buffer());
        GLplus::ScopedBufferBinding bufferBinding(*newTexcoords, GL_ARRAY_BUFFER);
        bufferBinding.GetBinding().Upload(
                    shape.mesh.texcoords.size() * sizeof(shape.mesh.texcoords[0]),
                    shape.mesh.texcoords.data(), GL_STATIC_DRAW);
    }

    if (!shape.material.diffuse_texname.empty())
    {
        newDiffuseTexture.reset(new GLplus::Texture2D());
        GLplus::ScopedTexture2DBinding textureBinding(*newDiffuseTexture);
        textureBinding.GetBinding().LoadImage(shape.material.diffuse_texname.c_str(), GLplus::Texture2D::InvertY);
    }

    mVertexCount = shape.mesh.indices.size();

    mIndices = std::move(newIndices);
    mPositions = std::move(newPositions);
    mTexcoords = std::move(newTexcoords);
    mNormals = std::move(newNormals);
    mDiffuseTexture = std::move(newDiffuseTexture);
}

void StaticMesh::Render(GLplus::Program& program) const
{
    GLplus::VertexArray vertexArray;
    GLplus::ScopedVertexArrayBinding scopedVAO(vertexArray);
    GLplus::VertexArrayBinding& vaoBinding = scopedVAO.GetBinding();

    vaoBinding.SetIndexBuffer(mIndices, GL_UNSIGNED_INT);

    if (mPositions)
    {
        GLint positionLoc;
        if (program.TryGetAttributeLocation("position", positionLoc))
        {
            vaoBinding.SetAttribute(
                        positionLoc, mPositions,
                        3, GL_FLOAT, GL_FALSE, 0, 0);
        }
    }

    if (mNormals)
    {
        GLint normalLoc;
        if (program.TryGetAttributeLocation("normal", normalLoc))
        {
            vaoBinding.SetAttribute(
                        normalLoc, mNormals,
                        3, GL_FLOAT, GL_FALSE, 0, 0);
        }
    }

    if (mTexcoords)
    {
        GLint texcoord0Loc;
        if (program.TryGetAttributeLocation("texcoord0", texcoord0Loc))
        {
            vaoBinding.SetAttribute(
                        texcoord0Loc, mTexcoords,
                        2, GL_FLOAT, GL_FALSE, 0, 0);
        }
    }

    GLplus::ScopedProgramBinding programBinding(program);
    std::unique_ptr<GLplus::ScopedActiveTextureBinding> activeTextureBind;
    std::unique_ptr<GLplus::ScopedTexture2DBinding> diffuseBind;
    if (mDiffuseTexture)
    {
        activeTextureBind.reset(new GLplus::ScopedActiveTextureBinding(GL_TEXTURE0));
        diffuseBind.reset(new GLplus::ScopedTexture2DBinding(*mDiffuseTexture));
        programBinding.GetBinding().UploadInt("diffuseTexture", 0);
    }

    GLplus::DrawElements(GL_TRIANGLES, GL_UNSIGNED_INT, 0, mVertexCount);
}

} // end namespace GLmesh
