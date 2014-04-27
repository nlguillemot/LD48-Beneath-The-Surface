#ifndef GLPLUS_H
#define GLPLUS_H

#include <GL/glew.h>

#include <memory>
#include <unordered_map>

namespace GLplus
{

void CheckGLErrors();

namespace detail
{
    // Gives proper copy/move constructors for handles to GL objects
    struct ObjectHandle
    {
        GLuint mHandle;
        ObjectHandle(){ mHandle = 0; }
        ObjectHandle(const ObjectHandle& other) = delete;
        ObjectHandle& operator=(const ObjectHandle& other) = delete;
        ObjectHandle(ObjectHandle&& other){ std::swap(mHandle, other.mHandle); }
        ObjectHandle& operator=(ObjectHandle&& other){ std::swap(mHandle, other.mHandle); }
    };
}

class Shader
{
    detail::ObjectHandle mHandle;
    GLenum mShaderType;

public:
    Shader(GLenum shaderType);
    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;
    Shader(Shader&&) = default;
    Shader& operator=(Shader&&) = default;
    ~Shader();

    void Compile(const GLchar* source);

    GLenum GetShaderType() const { return mShaderType; }

    GLuint GetGLHandle() const { return mHandle.mHandle; }
};

class Program
{
    detail::ObjectHandle mHandle;
    std::shared_ptr<Shader> mFragmentShader;
    std::shared_ptr<Shader> mVertexShader;

public:
    static Program FromFiles(const char* vShaderFile, const char* fShaderFile);

    Program();
    Program(const Program&) = delete;
    Program& operator=(const Program&) = delete;
    Program(Program&&) = default;
    Program& operator=(Program&&) = default;
    ~Program();

    void Attach(const std::shared_ptr<Shader>& shader);
    void Link();

    bool TryGetAttributeLocation(const GLchar* name, GLint& loc) const;
    GLint GetAttributeLocation(const GLchar* name) const;

    bool TryGetUniformLocation(const GLchar* name, GLint& loc) const;
    GLint GetUniformLocation(const GLchar* name) const;

    GLuint GetGLHandle() const { return mHandle.mHandle; }
};

class ProgramBinding
{
    Program& mProgram;

public:
    ProgramBinding(Program& program);

    void UploadInt(const GLchar* name, GLuint value) const;
    void UploadInt(GLint location, GLuint value) const;

    void UploadFloat(const GLchar* name, GLfloat value) const;
    void UploadFloat(GLint location, GLfloat value) const;

    void UploadVec2(const GLchar* name, GLfloat v0, GLfloat v1) const;
    void UploadVec2(GLint location, GLfloat v0, GLfloat v1) const;
    void UploadVec2(const GLchar* name, const GLfloat* const values) const;
    void UploadVec2(GLint location, const GLfloat* values) const;

    void UploadVec4(const GLchar* name, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3) const;
    void UploadVec4(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3) const;
    void UploadVec4(const GLchar* name, const GLfloat* const values) const;
    void UploadVec4(GLint location, const GLfloat* values) const;

    void UploadMatrix4(const GLchar* name, GLboolean transpose, const GLfloat* values) const;
    void UploadMatrix4(GLint location, GLboolean transpose, const GLfloat* values) const;

          Program& GetProgram()       { return mProgram; }
    const Program& GetProgram() const { return mProgram; }
};

class ScopedProgramBinding
{
    struct OldHandle
    {
        OldHandle();
        detail::ObjectHandle mOldProgram;
    } mOldHandle;

    ProgramBinding mBinding;

public:
    ScopedProgramBinding(Program& program);
    ~ScopedProgramBinding();

          ProgramBinding& GetBinding()       { return mBinding; }
    const ProgramBinding& GetBinding() const { return mBinding; }
};

class Buffer
{
    detail::ObjectHandle mHandle;

public:
    Buffer();
    ~Buffer();

    Buffer(const Buffer&) = delete;
    Buffer& operator=(const Buffer&) = delete;
    Buffer(Buffer&&) = default;
    Buffer& operator=(Buffer&&) = default;

    GLuint GetGLHandle() const { return mHandle.mHandle; }
};

class BufferBinding
{
    Buffer& mBuffer;
    GLenum mTarget;

public:
    BufferBinding(Buffer& buffer, GLenum target);

    void Upload(GLsizeiptr size, const GLvoid* data, GLenum usage);
    void Patch(GLintptr offset, GLsizeiptr size, const GLvoid* data);

          Buffer& GetBuffer()       { return mBuffer; }
    const Buffer& GetBuffer() const { return mBuffer; }

    GLenum GetTarget() const { return mTarget; }
};

class ScopedBufferBinding
{
    struct OldHandle
    {
        OldHandle(GLuint target);
        detail::ObjectHandle mOldBuffer;
    } mOldHandle;

    BufferBinding mBinding;

public:
    ScopedBufferBinding(Buffer& buffer, GLenum target);
    ~ScopedBufferBinding();

          BufferBinding& GetBinding()       { return mBinding; }
    const BufferBinding& GetBinding() const { return mBinding; }
};

class VertexArray
{
    detail::ObjectHandle mHandle;
    std::unordered_map<GLuint, std::shared_ptr<Buffer> > mVertexBuffers;
    std::shared_ptr<Buffer> mIndexBuffer;
    GLenum mIndexType = 0;

public:
    friend class VertexArrayBinding;

    VertexArray();
    ~VertexArray();

    VertexArray(const VertexArray&) = delete;
    VertexArray& operator=(const VertexArray&) = delete;
    VertexArray(VertexArray&&) = default;
    VertexArray& operator=(VertexArray&&) = default;

    GLenum GetIndexType() const;
    GLuint GetGLHandle() const { return mHandle.mHandle; }
};

class VertexArrayBinding
{
    VertexArray& mVertexArray;

public:
    VertexArrayBinding(VertexArray& vertexArray);

    void SetAttribute(
            GLuint index,
            const std::shared_ptr<Buffer>& buffer,
            GLint size,
            GLenum type,
            GLboolean normalized,
            GLsizei stride,
            GLsizei offset);

    void SetIndexBuffer(
            const std::shared_ptr<Buffer>& buffer,
            GLenum type);

          VertexArray& GetVertexArray()       { return mVertexArray; }
    const VertexArray& GetVertexArray() const { return mVertexArray; }
};

class ScopedVertexArrayBinding
{
    struct OldHandle
    {
        OldHandle();
        detail::ObjectHandle mOldVertexArray;
    } mOldHandle;

    VertexArrayBinding mBinding;

public:
    ScopedVertexArrayBinding(VertexArray& vertexArray);
    ~ScopedVertexArrayBinding();

          VertexArrayBinding& GetBinding()       { return mBinding; }
    const VertexArrayBinding& GetBinding() const { return mBinding; }
};

class Texture2D
{
    detail::ObjectHandle mHandle;

public:
    friend class Texture2DBinding;

    enum LoadFlags
    {
        NoFlags = 0,
        InvertY
    };

    Texture2D();
    ~Texture2D();

    Texture2D(const Texture2D&) = delete;
    Texture2D& operator=(const Texture2D&) = delete;
    Texture2D(Texture2D&&) = default;
    Texture2D& operator=(Texture2D&&) = default;

    GLuint GetGLHandle() const { return mHandle.mHandle; }
};

class ActiveTextureBinding
{
    GLenum mTextureIndex;

public:
    ActiveTextureBinding(GLenum textureIndex);

    GLenum GetTextureIndex() const { return mTextureIndex; }
};

class ScopedActiveTextureBinding
{
    struct OldIndex
    {
        OldIndex();
        GLenum mOldIndex;
    } mOldIndex;

    ActiveTextureBinding mBinding;

public:
    ScopedActiveTextureBinding(GLenum textureIndex);
    ~ScopedActiveTextureBinding();

          ActiveTextureBinding& GetBinding()       { return mBinding; }
    const ActiveTextureBinding& GetBinding() const { return mBinding; }
};

class Texture2DBinding
{
    Texture2D& mTexture2D;

public:
    Texture2DBinding(Texture2D& texture2D);

    void LoadImage(const char* filename, unsigned int flags);
    void CreateStorage(GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height);

    int GetWidth() const;
    int GetHeight() const;

          Texture2D& GetTexture2D()       { return mTexture2D; }
    const Texture2D& GetTexture2D() const { return mTexture2D; }
};

class ScopedTexture2DBinding
{
    struct OldHandle
    {
        OldHandle();
        detail::ObjectHandle mOldTexture;
    } mOldHandle;

    Texture2DBinding mBinding;

public:
    ScopedTexture2DBinding(Texture2D& texture2D);
    ~ScopedTexture2DBinding();

          Texture2DBinding& GetBinding()       { return mBinding; }
    const Texture2DBinding& GetBinding() const { return mBinding; }
};

class RenderBuffer
{
    detail::ObjectHandle mHandle;

public:
    RenderBuffer();
    ~RenderBuffer();

    RenderBuffer(const RenderBuffer&) = delete;
    RenderBuffer& operator=(const RenderBuffer&) = delete;
    RenderBuffer(RenderBuffer&&) = default;
    RenderBuffer& operator=(RenderBuffer&&) = default;

    GLuint GetGLHandle() const { return mHandle.mHandle; }
};

class RenderBufferBinding
{
    RenderBuffer& mRenderBuffer;

public:
    RenderBufferBinding(RenderBuffer& renderBuffer);

    void CreateStorage(GLenum internalformat, GLsizei width, GLsizei height);

          RenderBuffer& GetRenderBuffer()       { return mRenderBuffer; }
    const RenderBuffer& GetRenderBuffer() const { return mRenderBuffer; }
};

class ScopedRenderBufferBinding
{
    struct OldHandle
    {
        OldHandle();
        detail::ObjectHandle mOldRenderBuffer;
    } mOldHandle;

    RenderBufferBinding mBinding;

public:
    ScopedRenderBufferBinding(RenderBuffer& renderBuffer);
    ~ScopedRenderBufferBinding();

          RenderBufferBinding& GetBinding()       { return mBinding; }
    const RenderBufferBinding& GetBinding() const { return mBinding; }
};

struct DefaultFrameBuffer { };

class FrameBuffer
{
    detail::ObjectHandle mHandle;

    struct Attachment
    {
        Attachment(const std::shared_ptr<Texture2D>&);
        Attachment(const std::shared_ptr<RenderBuffer>&);
        std::shared_ptr<Texture2D> mTextureAttachment;
        std::shared_ptr<RenderBuffer> mRenderBufferAttachment;
    };

    std::unordered_map<GLenum, Attachment> mAttachments;

public:
    friend class FrameBufferBinding;

    FrameBuffer();
    FrameBuffer(DefaultFrameBuffer);
    ~FrameBuffer();

    FrameBuffer(const FrameBuffer&) = delete;
    FrameBuffer& operator=(const FrameBuffer&) = delete;
    FrameBuffer(FrameBuffer&&) = default;
    FrameBuffer& operator=(FrameBuffer&&) = default;

    GLuint GetGLHandle() const { return mHandle.mHandle; }
};

class FrameBufferBinding
{
    FrameBuffer& mFrameBuffer;
    GLuint mTarget;

public:
    FrameBufferBinding(FrameBuffer& frameBuffer, GLuint target);

    void Attach(GLenum attachment, const std::shared_ptr<Texture2D>& texture);
    void Attach(GLenum attachment, const std::shared_ptr<RenderBuffer>& renderBuffer);
    void Detach(GLenum attachment);

    GLenum GetStatus() const;
    void ValidateStatus() const;

          FrameBuffer& GetFrameBuffer()       { return mFrameBuffer; }
    const FrameBuffer& GetFrameBuffer() const { return mFrameBuffer; }

    GLuint GetTarget() const { return mTarget; }
};

class ScopedFrameBufferBinding
{
    struct OldHandles
    {
        OldHandles(GLuint target);
        detail::ObjectHandle mOldDrawFrameBuffer;
        detail::ObjectHandle mOldReadFrameBuffer;
    } mOldHandles;

    FrameBufferBinding mBinding;

public:
    ScopedFrameBufferBinding(FrameBuffer& frameBuffer, GLuint target);
    ~ScopedFrameBufferBinding();

          FrameBufferBinding& GetBinding()       { return mBinding; }
    const FrameBufferBinding& GetBinding() const { return mBinding; }
};

class Sampler
{
    detail::ObjectHandle mHandle;

public:
    Sampler();
    ~Sampler();

    Sampler(const Sampler&) = delete;
    Sampler& operator=(const Sampler&) = delete;
    Sampler(Sampler&&) = default;
    Sampler& operator=(Sampler&&) = default;

    void SetParameter(GLenum pname, int param);

    GLuint GetGLHandle() const { return mHandle.mHandle; }
};

class SamplerBinding
{
    Sampler& mSampler;

public:
    SamplerBinding(Sampler& sampler, GLuint textureUnit);

          Sampler& GetSampler()       { return mSampler; }
    const Sampler& GetSampler() const { return mSampler; }
};

class ScopedSamplerBinding
{
    struct OldBinding
    {
        OldBinding(GLuint textureUnit);
        GLuint mOldTextureUnit;
        detail::ObjectHandle mOldHandle;
    } mOldBinding;

    SamplerBinding mBinding;

public:
    ScopedSamplerBinding(Sampler& sampler, GLuint textureUnit);
    ~ScopedSamplerBinding();

          SamplerBinding& GetBinding()       { return mBinding; }
    const SamplerBinding& GetBinding() const { return mBinding; }
};

constexpr size_t SizeFromGLType(GLenum type)
{
    return type == GL_UNSIGNED_INT   ? sizeof(GLuint)   :
           type == GL_INT            ? sizeof(GLint)    :
           type == GL_UNSIGNED_SHORT ? sizeof(GLushort) :
           type == GL_SHORT          ? sizeof(GLshort)  :
           type == GL_UNSIGNED_BYTE  ? sizeof(GLubyte)  :
           type == GL_BYTE           ? sizeof(GLbyte)   :
           throw "Unimplemented Type";
}

void DrawArrays(GLenum mode, GLint first, GLsizei count);

void DrawElements(GLenum mode, GLenum indexType, GLint first, GLsizei count);

} // end namespace GLplus

#endif // GLPLUS_H
