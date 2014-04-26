#ifndef SDL2PLUS_H
#define SDL2PLUS_H

#include <SDL.h>

#include <memory>
#include <type_traits>

namespace SDL2plus
{

class LibSDL
{
public:
    LibSDL(Uint32 flags);
    ~LibSDL();

    LibSDL(const LibSDL&) = delete;
    LibSDL& operator=(const LibSDL&) = delete;
    LibSDL(LibSDL&&) = delete;
    LibSDL& operator=(LibSDL&&) = delete;

    void InitSubSystem(Uint32 flags);
    void QuitSubSystem(Uint32 flags);

    void SetGLAttribute(SDL_GLattr attr, int value);
};

class Window
{
    SDL_Window* mHandle = nullptr;

public:
    Window(const char* title, int x, int y, int w, int h, Uint32 flags);
    ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;
    Window(Window&& other) noexcept;
    Window& operator=(Window&& other) noexcept;

    void swap(Window& other) noexcept;

    void SetPosition(int x, int y);

    int GetWidth() const;
    int GetHeight() const;

    SDL_Window* GetSDLHandle() const;
};

inline void swap(Window& a, Window& b) noexcept
{
    a.swap(b);
}

class GLContext
{
    SDL_GLContext mHandle = nullptr;

public:
    GLContext(Window& host);
    ~GLContext();

    GLContext(const GLContext&) = delete;
    GLContext& operator=(const GLContext&) = delete;
    GLContext(GLContext&& other) noexcept;
    GLContext& operator=(GLContext&& other) noexcept;

    void swap(GLContext& other) noexcept;

    SDL_GLContext GetSDLHandle() const;
};

inline void swap(GLContext& a, GLContext& b) noexcept
{
    a.swap(b);
}

class WindowGL
{
    Window mWindow;
    GLContext mGLContext;

public:
    WindowGL(const char* title, int x, int y, int w, int h, Uint32 flags);

    void SetPosition(int x, int y) { mWindow.SetPosition(x,y); }

    int GetWidth()  const { return mWindow.GetWidth();  }
    int GetHeight() const { return mWindow.GetHeight(); }

    void MakeCurrent();
    void SwapBuffers();

          Window& GetWindow()             { return mWindow;    }
    const Window& GetWindow() const       { return mWindow;    }
          GLContext& GetGLContext()       { return mGLContext; }
    const GLContext& GetGLContext() const { return mGLContext; }

};

} // end namespace SDL2plus

#endif // SDL2PLUS_H
