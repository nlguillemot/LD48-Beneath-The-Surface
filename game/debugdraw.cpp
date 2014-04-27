#include "debugdraw.hpp"

void DebugDraw::SetLineWidth(float width)
{
    mLineWidth = width;
}

void DebugDraw::AddLine(glm::vec3 start, glm::vec3 end, glm::vec4 tint)
{
    std::array<glm::vec3,2> line;
    line[0] = start;
    line[1] = end;
    mLines.push_back(line);

    mTints.push_back(tint);
    mTints.push_back(tint);

    mIsDirty = true;
}

void DebugDraw::RebuildBuffers()
{
    std::shared_ptr<GLplus::Buffer> newPositions(new GLplus::Buffer());
    std::shared_ptr<GLplus::Buffer> newTints(new GLplus::Buffer());

    {
        GLplus::ScopedBufferBinding scopedPositions(*newPositions, GL_ARRAY_BUFFER);
        scopedPositions.GetBinding().Upload(mLines.size() * sizeof(mLines[0]), mLines.data(), GL_STATIC_DRAW);
    }

    {
        GLplus::ScopedBufferBinding scopedTints(*newTints, GL_ARRAY_BUFFER);
        scopedTints.GetBinding().Upload(mTints.size() * sizeof(mTints[0]), mTints.data(), GL_STATIC_DRAW);
    }

    mPositionBuffer = std::move(newPositions);
    mTintBuffer = std::move(newTints);

    mIsDirty = false;
}

void DebugDraw::Render(GLplus::Program& program)
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
        vaoBinding.SetAttribute(positionLoc, mPositionBuffer, 3, GL_FLOAT, GL_FALSE, 0, 0);
    }

    GLint tintLoc;
    if (program.TryGetAttributeLocation("tint", tintLoc))
    {
        vaoBinding.SetAttribute(tintLoc, mTintBuffer, 4, GL_FLOAT, GL_FALSE, 0, 0);
    }

    glLineWidth(mLineWidth);

    GLplus::ScopedProgramBinding scopedProgram(program);
    GLplus::DrawArrays(GL_LINES, 0, mLines.size() * 2);
}
