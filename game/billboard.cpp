#include "billboard.hpp"

Billboard::Billboard()
{
    mpPositions.reset(new GLplus::Buffer());
    {
        GLplus::ScopedBufferBinding scopedBind(*mpPositions, GL_ARRAY_BUFFER);
        scopedBind.GetBinding().Upload(sizeof(glm::vec3) * 4, NULL, GL_STREAM_DRAW);
    }

    mpTexcoords.reset(new GLplus::Buffer());
    {
        GLplus::ScopedBufferBinding scopedBind(*mpTexcoords, GL_ARRAY_BUFFER);
        scopedBind.GetBinding().Upload(sizeof(glm::vec2) * 4, NULL, GL_STATIC_DRAW);
    }
}

void Billboard::SetTexture(const std::shared_ptr<GLplus::Texture2D> pTexture)
{
    mpTexture = pTexture;
}

void Billboard::SetCenterPosition(glm::vec3 centerPosition)
{
    mCenterPosition = centerPosition;
    mIsDirty = true;
}

void Billboard::SetDimensions(glm::vec2 dimensions)
{
    mDimensions = dimensions;
}

void Billboard::SetCameraPosition(glm::vec3 cameraPosition)
{
    mCameraPosition = cameraPosition;
    mIsDirty = true;
}

void Billboard::SetCameraViewDirection(glm::vec3 cameraView)
{
    mCameraView = cameraView;
    mIsDirty = true;
}

void Billboard::SetCameraUp(glm::vec3 cameraUp)
{
    mCameraUp = cameraUp;
    mIsDirty = true;
}

void Billboard::RebuildBuffers()
{
    printf("mCenterPosition: { %f, %f, %f }\n", mCenterPosition.x, mCenterPosition.y, mCenterPosition.z);
    printf("mDimensions: { %f, %f }\n", mDimensions.x, mDimensions.y);
    printf("mCameraPosition: { %f, %f, %f }\n", mCameraPosition.x, mCameraPosition.y, mCameraPosition.z);
    printf("mCameraView: { %f, %f, %f }\n", mCameraView.x, mCameraView.y, mCameraView.z);
    printf("mCameraUp: { %f, %f, %f }\n", mCameraUp.x, mCameraUp.y, mCameraUp.z);

    glm::vec3 unitView = glm::normalize(mCameraView);
    glm::vec3 unitSide = glm::cross(unitView, glm::normalize(mCameraUp));
    glm::vec3 unitUp = glm::cross(unitSide, unitView);

    glm::vec3 bottomLeft = mCenterPosition - unitSide / 2.0f * mDimensions.x - unitUp / 2.0f * mDimensions.y;

    printf("unitView: { %f, %f, %f }\n", unitView.x, unitView.y, unitView.z);
    printf("unitSide: { %f, %f, %f }\n", unitSide.x, unitSide.y, unitSide.z);
    printf("unitUp: { %f, %f, %f }\n", unitUp.x, unitUp.y, unitUp.z);
    printf("bottomLeft: { %f, %f, %f }\n", bottomLeft.x, bottomLeft.y, bottomLeft.z);

    glm::vec3 positions[4] = {
        bottomLeft,
        bottomLeft + unitSide * mDimensions.x,
        bottomLeft + unitSide * mDimensions.x + unitUp * mDimensions.y,
        bottomLeft + unitUp * mDimensions.y
    };

    printf("positions:\n");
    for (glm::vec3 v : positions) {
        printf("{ %f, %f, %f }\n", v.x, v.y, v.z);
    }
    fflush(stdout);

    {
        GLplus::ScopedBufferBinding scopedBind(*mpPositions, GL_ARRAY_BUFFER);
        scopedBind.GetBinding().Patch(0, sizeof(positions), positions);
    }

    glm::vec2 texcoords[4] = {
        glm::vec2(0.0f, 0.0f),
        glm::vec2(1.0f, 0.0f),
        glm::vec2(1.0f, 1.0f),
        glm::vec2(0.0f, 1.0f)
    };

    {
        GLplus::ScopedBufferBinding scopedBind(*mpTexcoords, GL_ARRAY_BUFFER);
        scopedBind.GetBinding().Patch(0, sizeof(texcoords), texcoords);
    }

    mIsDirty = false;
}

void Billboard::Render(GLplus::Program& program)
{
    if (mIsDirty)
    {
        RebuildBuffers();
    }

    GLplus::VertexArray vao;
    GLplus::ScopedVertexArrayBinding scopedVAO(vao);
    GLplus::VertexArrayBinding& vaoBinding = scopedVAO.GetBinding();

    GLint positionLoc;
    if (program.TryGetAttributeLocation("position", positionLoc))
    {
        vaoBinding.SetAttribute(positionLoc, mpPositions, 3, GL_FLOAT, GL_FALSE, 0, 0);
    }

    GLint texcoordLoc;
    if (program.TryGetAttributeLocation("texcoord0", texcoordLoc))
    {
        vaoBinding.SetAttribute(texcoordLoc, mpTexcoords, 2, GL_FLOAT, GL_FALSE, 0, 0);
    }

    GLplus::ScopedProgramBinding programBinding(program);
    std::unique_ptr<GLplus::ScopedActiveTextureBinding> activeTextureBind;
    std::unique_ptr<GLplus::ScopedTexture2DBinding> diffuseBind;
    if (mpTexture)
    {
        activeTextureBind.reset(new GLplus::ScopedActiveTextureBinding(GL_TEXTURE0));
        diffuseBind.reset(new GLplus::ScopedTexture2DBinding(*mpTexture));
        programBinding.GetBinding().UploadInt("diffuseTexture", 0);
    }

    GLplus::DrawArrays(GL_TRIANGLE_FAN, 0, 4);
}
