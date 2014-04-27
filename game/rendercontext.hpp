#ifndef RENDERCONTEXT_HPP
#define RENDERCONTEXT_HPP

#include <GLplus.hpp>

#include <glm/glm.hpp>

struct Viewport
{
    Viewport(glm::ivec2 topLeft = glm::ivec2(), glm::ivec2 size = glm::ivec2())
        : TopLeft(topLeft), Size(size) { }

    glm::ivec2 TopLeft;
    glm::ivec2 Size;
};

class RenderContext
{
public:
    std::shared_ptr<GLplus::FrameBuffer> CurrentFrameBuffer;
    Viewport CurrentViewport;

    float GetAspectRatio() const
    {
        return (float) CurrentViewport.Size.x / CurrentViewport.Size.y;
    }
};

#endif // RENDERCONTEXT_HPP
