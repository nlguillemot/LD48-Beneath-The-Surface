#ifndef BILLBOARD_HPP
#define BILLBOARD_HPP

#include <GLplus.hpp>

#include <glm/glm.hpp>

class Billboard
{
    std::shared_ptr<GLplus::Texture2D> mpTexture;
    std::shared_ptr<GLplus::Buffer> mpPositions;
    std::shared_ptr<GLplus::Buffer> mpTexcoords;
    bool mIsDirty = true;

    glm::vec3 mCenterPosition;
    glm::vec2 mDimensions;

    glm::vec3 mCameraPosition;
    glm::vec3 mCameraView;
    glm::vec3 mCameraUp;

public:
    Billboard();

    void SetTexture(const std::shared_ptr<GLplus::Texture2D> pTexture);

    void SetCenterPosition(glm::vec3 centerPosition);
    void SetDimensions(glm::vec2 dimensions);

    void SetCameraPosition(glm::vec3 cameraPosition);
    void SetCameraViewDirection(glm::vec3 cameraView);
    void SetCameraUp(glm::vec3 cameraUp);

    void RebuildBuffers();

    void Render(GLplus::Program& program);
};

#endif // BILLBOARD_HPP
