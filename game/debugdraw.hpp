#ifndef DEBUGDRAW_HPP
#define DEBUGDRAW_HPP

#include <GLplus.hpp>

#include <glm/glm.hpp>
#include <array>
#include <vector>

class DebugDraw
{
    std::vector<std::array<glm::vec3,2>> mLines;
    std::vector<glm::vec4> mTints;

    std::shared_ptr<GLplus::Buffer> mPositionBuffer;
    std::shared_ptr<GLplus::Buffer> mTintBuffer;

    float mLineWidth = 1.0f;

    bool mIsDirty = true;

public:
    void SetLineWidth(float width);

    void AddLine(glm::vec3 start, glm::vec3 end, glm::vec4 tint);

    void RebuildBuffers();

    void Render(GLplus::Program& program);
};

#endif // DEBUGDRAW_HPP
