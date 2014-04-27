#include "GLplus.hpp"

#include <stdexcept>
#include <vector>
#include <fstream>
#include <sstream>

#include "SOIL2.h"

namespace GLplus
{

using namespace detail;

static const char* StringFromGLError(GLenum err)
{
    switch (err)
    {
    case GL_NO_ERROR:          return "GL_NO_ERROR";
    case GL_INVALID_ENUM:      return "GL_INVALID_ENUM";
    case GL_INVALID_VALUE:     return "GL_INVALID_VALUE";
    case GL_INVALID_OPERATION: return "GL_INVALID_OPERATION";
    case GL_STACK_OVERFLOW:    return "GL_STACK_OVERFLOW";
    case GL_STACK_UNDERFLOW:   return "GL_STACK_UNDERFLOW";
    case GL_OUT_OF_MEMORY:     return "GL_OUT_OF_MEMORY";
    default:                   return "Unknown GL error";
    }
}


static const char* FrameBufferStatusToString(GLenum status)
{
    switch (status)
    {
    case GL_FRAMEBUFFER_COMPLETE:                      return "GL_FRAMEBUFFER_COMPLETE";
    case GL_FRAMEBUFFER_UNDEFINED:                     return "GL_FRAMEBUFFER_UNDEFINED";
    case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:         return "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT";
    case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: return "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT";
    case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:        return "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER";
    case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:        return "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER";
    case GL_FRAMEBUFFER_UNSUPPORTED:                   return "GL_FRAMEBUFFER_UNSUPPORTED";
    case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:        return "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE";
    case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:      return "GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS";
    default:                                           return "Unknown FrameBuffer status";
    }
}

void CheckGLErrors()
{
    GLenum firstError = glGetError();

    while (glGetError() != GL_NO_ERROR);

    if (firstError != GL_NO_ERROR)
    {
        throw std::runtime_error(StringFromGLError(firstError));
    }
}

Shader::Shader(GLenum shaderType)
    : mShaderType(shaderType)
{
    mHandle.mHandle = glCreateShader(shaderType);
    CheckGLErrors();
}

Shader::~Shader()
{
    glDeleteShader(mHandle.mHandle);
    CheckGLErrors();
}

void Shader::Compile(const GLchar* source)
{
    glShaderSource(mHandle.mHandle, 1, &source, NULL);
    CheckGLErrors();

    glCompileShader(mHandle.mHandle);
    CheckGLErrors();

    int status;
    glGetShaderiv(mHandle.mHandle, GL_COMPILE_STATUS, &status);
    CheckGLErrors();

    if (!status)
    {
        int logLength;
        glGetShaderiv(mHandle.mHandle, GL_INFO_LOG_LENGTH, &logLength);
        CheckGLErrors();

        std::vector<char> log(logLength);
        glGetShaderInfoLog(mHandle.mHandle, log.size(), NULL, log.data());
        CheckGLErrors();

        throw std::runtime_error(log.data());
    }
}

Program::Program()
{
    mHandle.mHandle = glCreateProgram();
    CheckGLErrors();
}

Program::~Program()
{
    glDeleteProgram(mHandle.mHandle);
    CheckGLErrors();
}

void Program::Attach(const std::shared_ptr<Shader>& shader)
{
    glAttachShader(mHandle.mHandle, shader->GetGLHandle());
    CheckGLErrors();

    switch (shader->GetShaderType())
    {
    case GL_FRAGMENT_SHADER:
        mFragmentShader = shader;
        break;
    case GL_VERTEX_SHADER:
        mVertexShader = shader;
        break;
    default:
        throw std::runtime_error("Unknown shader type.");
    }
}

void Program::Link()
{
    glLinkProgram(mHandle.mHandle);
    CheckGLErrors();

    int status;
    glGetProgramiv(mHandle.mHandle, GL_LINK_STATUS, &status);
    CheckGLErrors();

    if (!status)
    {
        int logLength;
        glGetProgramiv(mHandle.mHandle, GL_INFO_LOG_LENGTH, &logLength);
        CheckGLErrors();

        std::vector<char> log(logLength);
        glGetProgramInfoLog(mHandle.mHandle, log.size(), NULL, log.data());
        CheckGLErrors();

        throw std::runtime_error(log.data());
    }
}

Program Program::FromFiles(const char* vShaderFile, const char* fShaderFile)
{
    std::ifstream vFile(vShaderFile), fFile(fShaderFile);
    if (!vFile || !fFile)
    {
        throw std::runtime_error("Couldn't open shader file");
    }

    std::stringstream vStream, fStream;
    vStream << vFile.rdbuf();
    fStream << fFile.rdbuf();

    std::shared_ptr<GLplus::Shader> vShader = std::make_shared<GLplus::Shader>(GL_VERTEX_SHADER);
    vShader->Compile(vStream.str().c_str());

    std::shared_ptr<GLplus::Shader> fShader = std::make_shared<GLplus::Shader>(GL_FRAGMENT_SHADER);
    fShader->Compile(fStream.str().c_str());

    // attach & link
    Program program;
    program.Attach(vShader);
    program.Attach(fShader);
    program.Link();

    return program;
}

bool Program::TryGetAttributeLocation(const GLchar* name, GLint& loc) const
{
    GLint location = glGetAttribLocation(mHandle.mHandle, name);
    CheckGLErrors();
    if (location == -1)
    {
        return false;
    }

    loc = location;
    return true;
}

GLint Program::GetAttributeLocation(const GLchar* name) const
{
    GLint loc;
    if (!TryGetAttributeLocation(name, loc))
    {
        throw std::runtime_error("Couldn't find attribute.");
    }
    return loc;
}

bool Program::TryGetUniformLocation(const GLchar* name, GLint& loc) const
{
    GLint location = glGetUniformLocation(mHandle.mHandle, name);
    if (location == -1)
    {
        return false;
    }

    loc = location;
    return true;
}
GLint Program::GetUniformLocation(const GLchar* name) const
{
    GLint loc;
    if (!TryGetUniformLocation(name, loc))
    {
        throw std::runtime_error("Couldn't find uniform.");
    }
    return loc;
}

ProgramBinding::ProgramBinding(Program& program)
    : mProgram(program)
{
    glUseProgram(mProgram.GetGLHandle());
    CheckGLErrors();
}

void ProgramBinding::UploadInt(const GLchar* name, GLuint value) const
{
    UploadInt(mProgram.GetUniformLocation(name), value);
}

void ProgramBinding::UploadInt(GLint location, GLuint value) const
{
    glUniform1i(location, value);
    CheckGLErrors();
}

void ProgramBinding::UploadFloat(const GLchar* name, GLfloat value) const
{
    UploadFloat(mProgram.GetUniformLocation(name), value);
}

void ProgramBinding::UploadFloat(GLint location, GLfloat value) const
{
    glUniform1f(location, value);
    CheckGLErrors();
}

void ProgramBinding::UploadVec2(const GLchar* name, GLfloat v0, GLfloat v1) const
{
    UploadVec2(mProgram.GetUniformLocation(name), v0, v1);
}

void ProgramBinding::UploadVec2(GLint location, GLfloat v0, GLfloat v1) const
{
    glUniform2f(location, v0, v1);
    CheckGLErrors();
}

void ProgramBinding::UploadVec2(const GLchar* name, const GLfloat* values) const
{
    UploadVec2(mProgram.GetUniformLocation(name), values);
}

void ProgramBinding::UploadVec2(GLint location, const GLfloat* values) const
{
    glUniform2fv(location, 1, values);
    CheckGLErrors();
}

void ProgramBinding::UploadVec4(const GLchar* name, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3) const
{
    UploadVec4(mProgram.GetUniformLocation(name), v0, v1, v2, v3);
}

void ProgramBinding::UploadVec4(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3) const
{
    glUniform4f(location, v0, v1, v2, v3);
    CheckGLErrors();
}

void ProgramBinding::UploadVec4(const GLchar* name, const GLfloat* values) const
{
    UploadVec4(mProgram.GetUniformLocation(name), values);
}

void ProgramBinding::UploadVec4(GLint location, const GLfloat* values) const
{
    glUniform4fv(location, 1, values);
    CheckGLErrors();
}

void ProgramBinding::UploadMatrix4(const GLchar* name, GLboolean transpose, const GLfloat* values) const
{
    UploadMatrix4(mProgram.GetUniformLocation(name), transpose, values);
}

void ProgramBinding::UploadMatrix4(GLint location, GLboolean transpose, const GLfloat* values) const
{
    glUniformMatrix4fv(location, 1, transpose, values);
    CheckGLErrors();
}

ScopedProgramBinding::OldHandle::OldHandle()
{
    GLint currentProgram;
    glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);
    CheckGLErrors();

    mOldProgram.mHandle = currentProgram;
}

ScopedProgramBinding::ScopedProgramBinding(Program& program)
    : mOldHandle()
    , mBinding(program)
{ }

ScopedProgramBinding::~ScopedProgramBinding()
{
    glUseProgram(mOldHandle.mOldProgram.mHandle);
    CheckGLErrors();
}

Buffer::Buffer()
{
    glGenBuffers(1, &mHandle.mHandle);
    CheckGLErrors();
}

Buffer::~Buffer()
{
    glDeleteBuffers(1, &mHandle.mHandle);
    CheckGLErrors();
}

BufferBinding::BufferBinding(Buffer& buffer, GLenum target)
    : mBuffer(buffer)
    , mTarget(target)
{
    glBindBuffer(mTarget, mBuffer.GetGLHandle());
    CheckGLErrors();
}

void BufferBinding::Upload(GLsizeiptr size, const GLvoid* data, GLenum usage)
{
    glBufferData(mTarget, size, data, usage);
    CheckGLErrors();
}

void BufferBinding::Patch(GLintptr offset, GLsizeiptr size, const GLvoid* data)
{
    glBufferSubData(mTarget, offset, size, data);
    CheckGLErrors();
}

ScopedBufferBinding::OldHandle::OldHandle(GLuint target)
{
    GLenum binding = target == GL_ARRAY_BUFFER ? GL_ARRAY_BUFFER_BINDING
                   : target == GL_ELEMENT_ARRAY_BUFFER ? GL_ELEMENT_ARRAY_BUFFER_BINDING
                   : throw std::logic_error("Invalid Buffer target type");

    GLint oldBuffer;
    glGetIntegerv(binding, &oldBuffer);
    CheckGLErrors();

    mOldBuffer.mHandle = oldBuffer;
}

ScopedBufferBinding::ScopedBufferBinding(Buffer& buffer, GLenum target)
    : mOldHandle(target)
    , mBinding(buffer, target)
{ }

ScopedBufferBinding::~ScopedBufferBinding()
{
    glBindBuffer(GetBinding().GetTarget(), mOldHandle.mOldBuffer.mHandle);
    CheckGLErrors();
}

VertexArray::VertexArray()
{
    glGenVertexArrays(1, &mHandle.mHandle);
    CheckGLErrors();
}

VertexArray::~VertexArray()
{
    glDeleteVertexArrays(1, &mHandle.mHandle);
    CheckGLErrors();
}

GLenum VertexArray::GetIndexType() const
{
    if (!mIndexType)
    {
        throw std::runtime_error("VertexArray has no index type.");
    }
    return mIndexType;
}

VertexArrayBinding::VertexArrayBinding(VertexArray& vertexArray)
    : mVertexArray(vertexArray)
{
    glBindVertexArray(mVertexArray.GetGLHandle());
    CheckGLErrors();
}

void VertexArrayBinding::SetAttribute(
        GLuint index,
        const std::shared_ptr<Buffer>& buffer,
        GLint size,
        GLenum type,
        GLboolean normalized,
        GLsizei stride,
        GLsizei offset)
{
    glEnableVertexAttribArray(index);
    CheckGLErrors();

    ScopedBufferBinding bufferBinding(*buffer, GL_ARRAY_BUFFER);

    glVertexAttribPointer(index, size, type, normalized, stride, (char*)NULL + offset);
    CheckGLErrors();

    mVertexArray.mVertexBuffers[index] = buffer;
}

void VertexArrayBinding::SetIndexBuffer(const std::shared_ptr<Buffer>& buffer, GLenum type)
{
    // spookiest, most unobviously documented thing about the GL spec I found so far.
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer->GetGLHandle());
    CheckGLErrors();

    mVertexArray.mIndexBuffer = buffer;
    mVertexArray.mIndexType = type;
}


ScopedVertexArrayBinding::OldHandle::OldHandle()
{
    GLint oldArray;
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &oldArray);
    CheckGLErrors();

    mOldVertexArray.mHandle = oldArray;
}

ScopedVertexArrayBinding::ScopedVertexArrayBinding(VertexArray& vertexArray)
    : mOldHandle()
    , mBinding(vertexArray)
{ }

ScopedVertexArrayBinding::~ScopedVertexArrayBinding()
{
    glBindVertexArray(mOldHandle.mOldVertexArray.mHandle);
    CheckGLErrors();
}

Texture2D::Texture2D()
{
    glGenTextures(1, &mHandle.mHandle);
    CheckGLErrors();
}

Texture2D::~Texture2D()
{
    glDeleteTextures(1, &mHandle.mHandle);
    CheckGLErrors();
}

ActiveTextureBinding::ActiveTextureBinding(GLenum textureIndex)
    : mTextureIndex(textureIndex)
{
    glActiveTexture(textureIndex);
    CheckGLErrors();
}

ScopedActiveTextureBinding::OldIndex::OldIndex()
{
    GLint oldIndex;
    glGetIntegerv(GL_ACTIVE_TEXTURE, &oldIndex);
    CheckGLErrors();

    mOldIndex = oldIndex;
}

ScopedActiveTextureBinding::ScopedActiveTextureBinding(GLenum textureIndex)
    : mOldIndex()
    , mBinding(textureIndex)
{ }

ScopedActiveTextureBinding::~ScopedActiveTextureBinding()
{
    glActiveTexture(mOldIndex.mOldIndex);
    CheckGLErrors();
}

Texture2DBinding::Texture2DBinding(Texture2D& texture2D)
    : mTexture2D(texture2D)
{
    glBindTexture(GL_TEXTURE_2D, mTexture2D.GetGLHandle());
    CheckGLErrors();
}

void Texture2DBinding::LoadImage(const char* filename, unsigned int flags)
{
    unsigned int soilFlags = 0;
    if (flags & Texture2D::InvertY)
    {
        soilFlags |= SOIL_FLAG_INVERT_Y;
    }

    if (!SOIL_load_OGL_texture(filename,
                NULL, NULL, NULL,
                SOIL_LOAD_AUTO,
                mTexture2D.GetGLHandle(),
                soilFlags))
    {
        throw std::runtime_error(SOIL_last_result());
    }
}

void Texture2DBinding::CreateStorage(GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height)
{
    glTexStorage2D(GL_TEXTURE_2D, levels, internalformat, width, height);
    CheckGLErrors();
}

int Texture2DBinding::GetWidth() const
{
    int width;
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0,  GL_TEXTURE_WIDTH, &width);
    CheckGLErrors();
    return width;
}

int Texture2DBinding::GetHeight() const
{
    int height;
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0,  GL_TEXTURE_HEIGHT, &height);
    CheckGLErrors();
    return height;
}

ScopedTexture2DBinding::OldHandle::OldHandle()
{
    GLint oldTexture;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &oldTexture);
    CheckGLErrors();

    mOldTexture.mHandle = oldTexture;
}

ScopedTexture2DBinding::ScopedTexture2DBinding(Texture2D& texture2D)
    : mOldHandle()
    , mBinding(texture2D)
{ }

ScopedTexture2DBinding::~ScopedTexture2DBinding()
{
    glBindTexture(GL_TEXTURE_2D, mOldHandle.mOldTexture.mHandle);
    CheckGLErrors();
}

RenderBuffer::RenderBuffer()
{
    glGenRenderbuffers(1, &mHandle.mHandle);
    CheckGLErrors();
}

RenderBuffer::~RenderBuffer()
{
    glDeleteRenderbuffers(1, &mHandle.mHandle);
    CheckGLErrors();
}

RenderBufferBinding::RenderBufferBinding(RenderBuffer& renderBuffer)
    : mRenderBuffer(renderBuffer)
{
    glBindRenderbuffer(GL_RENDERBUFFER, mRenderBuffer.GetGLHandle());
    CheckGLErrors();
}

void RenderBufferBinding::CreateStorage(GLenum internalformat, GLsizei width, GLsizei height)
{
    glRenderbufferStorage(GL_RENDERBUFFER, internalformat, width, height);
    CheckGLErrors();
}

ScopedRenderBufferBinding::OldHandle::OldHandle()
{
    GLint oldRenderbuffer;
    glGetIntegerv(GL_RENDERBUFFER_BINDING, &oldRenderbuffer);
    CheckGLErrors();

    mOldRenderBuffer.mHandle = oldRenderbuffer;
}

ScopedRenderBufferBinding::ScopedRenderBufferBinding(RenderBuffer& renderBuffer)
    : mOldHandle()
    , mBinding(renderBuffer)
{ }

ScopedRenderBufferBinding::~ScopedRenderBufferBinding()
{
    glBindRenderbuffer(GL_RENDERBUFFER, mOldHandle.mOldRenderBuffer.mHandle);
    CheckGLErrors();
}

FrameBuffer::Attachment::Attachment(const std::shared_ptr<Texture2D>& texture)
    : mTextureAttachment(texture)
{ }

FrameBuffer::Attachment::Attachment(const std::shared_ptr<RenderBuffer>& renderBuffer)
    : mRenderBufferAttachment(renderBuffer)
{ }

FrameBuffer::FrameBuffer()
{
    glGenFramebuffers(1, &mHandle.mHandle);
    CheckGLErrors();
}

FrameBuffer::FrameBuffer(DefaultFrameBuffer)
{
    mHandle.mHandle = 0;
}

FrameBuffer::~FrameBuffer()
{
    glDeleteFramebuffers(1, &mHandle.mHandle);
    CheckGLErrors();
}

FrameBufferBinding::FrameBufferBinding(FrameBuffer& frameBuffer, GLuint target)
    : mFrameBuffer(frameBuffer)
    , mTarget(target)
{
    glBindFramebuffer(mTarget, mFrameBuffer.GetGLHandle());
    CheckGLErrors();
}

void FrameBufferBinding::Attach(GLenum attachment, const std::shared_ptr<Texture2D>& texture)
{
    glFramebufferTexture2D(mTarget, attachment, GL_TEXTURE_2D, texture->GetGLHandle(), 0);
    CheckGLErrors();

    mFrameBuffer.mAttachments.emplace(attachment, texture);
}

void FrameBufferBinding::Attach(GLenum attachment, const std::shared_ptr<RenderBuffer>& renderBuffer)
{
    glFramebufferRenderbuffer(mTarget, attachment, GL_RENDERBUFFER, renderBuffer->GetGLHandle());
    CheckGLErrors();

    mFrameBuffer.mAttachments.emplace(attachment, renderBuffer);
}

void FrameBufferBinding::Detach(GLenum attachment)
{
    glFramebufferRenderbuffer(mTarget, attachment, GL_RENDERBUFFER, 0);
    CheckGLErrors();

    mFrameBuffer.mAttachments.erase(attachment);
}

GLenum FrameBufferBinding::GetStatus() const
{
    GLenum status = glCheckFramebufferStatus(mTarget);
    CheckGLErrors();

    return status;
}

void FrameBufferBinding::ValidateStatus() const
{
    GLenum status = GetStatus();
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        throw std::runtime_error(FrameBufferStatusToString(status));
    }
}

ScopedFrameBufferBinding::OldHandles::OldHandles(GLuint target)
{
    if (target == GL_FRAMEBUFFER)
    {
        GLint oldFrameBuffer;
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &oldFrameBuffer);
        CheckGLErrors();

        mOldDrawFrameBuffer.mHandle = oldFrameBuffer;
    }
    else if (target == GL_DRAW_FRAMEBUFFER)
    {
        GLint oldDrawFrameBuffer;
        glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &oldDrawFrameBuffer);
        CheckGLErrors();

        mOldDrawFrameBuffer.mHandle = oldDrawFrameBuffer;
    }
    else if (target == GL_READ_FRAMEBUFFER)
    {
        GLint oldReadFrameBuffer;
        glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &oldReadFrameBuffer);
        CheckGLErrors();

        mOldReadFrameBuffer.mHandle = oldReadFrameBuffer;
    }
}

ScopedFrameBufferBinding::ScopedFrameBufferBinding(FrameBuffer& frameBuffer, GLuint target)
    : mOldHandles(target)
    , mBinding(frameBuffer, target)
{ }

ScopedFrameBufferBinding::~ScopedFrameBufferBinding()
{
    if (mBinding.GetTarget() == GL_FRAMEBUFFER)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, mOldHandles.mOldDrawFrameBuffer.mHandle);
        CheckGLErrors();
    }
    else if (mBinding.GetTarget() == GL_DRAW_FRAMEBUFFER)
    {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mOldHandles.mOldDrawFrameBuffer.mHandle);
        CheckGLErrors();
    }
    else if (mBinding.GetTarget() == GL_READ_FRAMEBUFFER)
    {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, mOldHandles.mOldReadFrameBuffer.mHandle);
        CheckGLErrors();
    }
    else
    {
        throw std::logic_error("Invalid FrameBuffer target type");
    }
}

Sampler::Sampler()
{
    glGenSamplers(1, &mHandle.mHandle);
    CheckGLErrors();
}

Sampler::~Sampler()
{
    glDeleteSamplers(1, &mHandle.mHandle);
    CheckGLErrors();
}

void Sampler::SetParameter(GLenum pname, int param)
{
    glSamplerParameteri(mHandle.mHandle, pname, param);
    CheckGLErrors();
}

SamplerBinding::SamplerBinding(Sampler& sampler, GLuint textureUnit)
    : mSampler(sampler)
{
    glBindSampler(textureUnit, sampler.GetGLHandle());
    CheckGLErrors();
}

ScopedSamplerBinding::OldBinding::OldBinding(GLuint textureUnit)
{
    GLint oldActiveTexture;
    glGetIntegerv(GL_ACTIVE_TEXTURE, &oldActiveTexture);
    CheckGLErrors();

    mOldTextureUnit = textureUnit;

    glActiveTexture(GL_TEXTURE0 + textureUnit);
    CheckGLErrors();

    GLint oldSampler;
    glGetIntegerv(GL_SAMPLER_BINDING, &oldSampler);
    CheckGLErrors();

    mOldHandle.mHandle = oldSampler;

    glActiveTexture(oldActiveTexture);
    CheckGLErrors();
}

ScopedSamplerBinding::ScopedSamplerBinding(Sampler& sampler, GLuint textureUnit)
    : mOldBinding(textureUnit)
    , mBinding(sampler, textureUnit)
{ }

ScopedSamplerBinding::~ScopedSamplerBinding()
{
    glBindSampler(mOldBinding.mOldTextureUnit, mOldBinding.mOldHandle.mHandle);
    CheckGLErrors();
}

void DrawArrays(GLenum mode, GLint first, GLsizei count)
{
    glDrawArrays(mode, first, count);
    CheckGLErrors();
}

void DrawElements(GLenum mode, GLenum indexType, GLint first, GLsizei count)
{
    glDrawElements(mode, count, indexType,
                   (const GLvoid*) (SizeFromGLType(indexType) * first));
    CheckGLErrors();
}

} // end namespace GLplus
