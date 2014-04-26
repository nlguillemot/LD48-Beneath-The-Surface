#include "SDL2plus.hpp"
#include <GL/glew.h>

#include <stdexcept>

namespace SDL2plus
{

LibSDL::LibSDL(Uint32 flags)
{
    if (SDL_Init(flags))
    {
        throw std::runtime_error(SDL_GetError());
    }
}

LibSDL::~LibSDL()
{
    SDL_Quit();
}

void LibSDL::InitSubSystem(Uint32 flags)
{
    if (SDL_InitSubSystem(flags))
    {
        throw std::runtime_error(SDL_GetError());
    }
}

void LibSDL::QuitSubSystem(Uint32 flags)
{
    SDL_QuitSubSystem(flags);
}

void LibSDL::SetGLAttribute(SDL_GLattr attr, int value)
{
    if (SDL_GL_SetAttribute(attr, value))
    {
        throw std::runtime_error(SDL_GetError());
    }
}

Window::Window(const char* title, int x, int y, int w, int h, Uint32 flags)
{
    mHandle = SDL_CreateWindow(title, x, y, w, h, flags);

    if (!mHandle)
    {
        throw std::runtime_error(SDL_GetError());
    }
}

Window::~Window()
{
    SDL_DestroyWindow(mHandle);
}

Window::Window(Window&& other) noexcept
{
    swap(other);
}

Window& Window::operator=(Window&& other) noexcept
{
    swap(other);
}

void Window::swap(Window& other) noexcept
{
    using std::swap;
    swap(mHandle, other.mHandle);
}

void Window::SetPosition(int x, int y)
{
    SDL_SetWindowPosition(mHandle, x, y);
}

int Window::GetWidth() const
{
    int w;
    SDL_GetWindowSize(mHandle, &w, NULL);
    return w;
}

int Window::GetHeight() const
{
    int h;
    SDL_GetWindowSize(mHandle, NULL, &h);
    return h;
}

SDL_Window* Window::GetSDLHandle() const
{
    return mHandle;
}

GLContext::GLContext(Window& host)
{
    mHandle = SDL_GL_CreateContext(host.GetSDLHandle());
    if (!mHandle)
    {
        throw std::runtime_error(SDL_GetError());
    }
}

GLContext::~GLContext()
{
    SDL_GL_DeleteContext(mHandle);
}

GLContext::GLContext(GLContext&& other) noexcept
{
    swap(other);
}

GLContext& GLContext::operator=(GLContext&& other) noexcept
{
    swap(other);
}

void GLContext::swap(GLContext& other) noexcept
{
    using std::swap;
    swap(mHandle, other.mHandle);
}

SDL_GLContext GLContext::GetSDLHandle() const
{
    return mHandle;
}

WindowGL::WindowGL(const char* title, int x, int y, int w, int h, Uint32 flags)
    : mWindow(title, x, y, w, h,
              (flags & SDL_WINDOW_OPENGL) ? flags
                                          : throw std::logic_error(
                                                "Missing SDL_WINDOW_OPENGL flag"))
    , mGLContext(mWindow)
{
    // wrangle GL extensions
    glewExperimental = GL_TRUE;
    GLenum glewError = glewInit();
    if (glewError != GLEW_OK)
    {
        throw std::runtime_error((const char*) glewGetErrorString(glewError));
    }

    // flush errors. necessary because of http://stackoverflow.com/questions/19453439/solved-opengl-error-gl-invalid-enum-0x0500-while-glewinit
    while (glGetError() != GL_NO_ERROR);
}

void WindowGL::MakeCurrent()
{
    if (SDL_GL_MakeCurrent(mWindow.GetSDLHandle(), mGLContext.GetSDLHandle()))
    {
        throw std::runtime_error(SDL_GetError());
    }
}

void WindowGL::SwapBuffers()
{
    SDL_GL_SwapWindow(mWindow.GetSDLHandle());
}

} // end namespace SDL2plus
