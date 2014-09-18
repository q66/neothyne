// file automatically generated by ./r_generator.py
#ifndef R_COMMON_HDR
#define R_COMMON_HDR
#include <SDL2/SDL_opengl.h>
#include <stdint.h>

#ifdef DEBUG_GL
#   define GL_INFO const char *file, size_t line
#   define GL_INFOP , GL_INFO
#else
#   define GL_INFO
#   define GL_INFOP
#endif

namespace gl {
    void init(void);
    bool has(const char *ext);
    GLuint CreateShader(GLenum shaderType GL_INFOP);
    void ShaderSource(GLuint shader, GLsizei count, const GLchar** string, const GLint* length GL_INFOP);
    void CompileShader(GLuint shader GL_INFOP);
    void AttachShader(GLuint program, GLuint shader GL_INFOP);
    GLuint CreateProgram(GL_INFO);
    void LinkProgram(GLuint program GL_INFOP);
    void UseProgram(GLuint program GL_INFOP);
    GLint GetUniformLocation(GLuint program, const GLchar* name GL_INFOP);
    void EnableVertexAttribArray(GLuint index GL_INFOP);
    void DisableVertexAttribArray(GLuint index GL_INFOP);
    void UniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value GL_INFOP);
    void BindBuffer(GLenum target, GLuint buffer GL_INFOP);
    void GenBuffers(GLsizei n, GLuint* buffers GL_INFOP);
    void VertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid* pointer GL_INFOP);
    void BufferData(GLenum target, GLsizeiptr size, const GLvoid* data, GLenum usage GL_INFOP);
    void ValidateProgram(GLuint program GL_INFOP);
    void GenVertexArrays(GLsizei n, GLuint* arrays GL_INFOP);
    void BindVertexArray(GLuint array GL_INFOP);
    void DeleteProgram(GLuint program GL_INFOP);
    void DeleteBuffers(GLsizei n, const GLuint* buffers GL_INFOP);
    void DeleteVertexArrays(GLsizei n, const GLuint* arrays GL_INFOP);
    void Uniform1i(GLint location, GLint v0 GL_INFOP);
    void Uniform1f(GLint location, GLfloat v0 GL_INFOP);
    void Uniform2f(GLint location, GLfloat v0, GLfloat v1 GL_INFOP);
    void Uniform3fv(GLint location, GLsizei count, const GLfloat* value GL_INFOP);
    void GenerateMipmap(GLenum target GL_INFOP);
    void DeleteShader(GLuint shader GL_INFOP);
    void GetShaderiv(GLuint shader, GLenum pname, GLint* params GL_INFOP);
    void GetProgramiv(GLuint program, GLenum pname, GLint* params GL_INFOP);
    void GetShaderInfoLog(GLuint shader, GLsizei maxLength, GLsizei* length, GLchar* infoLog GL_INFOP);
    void ActiveTexture(GLenum texture GL_INFOP);
    void GenFramebuffers(GLsizei n, GLuint* ids GL_INFOP);
    void BindFramebuffer(GLenum target, GLuint framebuffer GL_INFOP);
    void FramebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level GL_INFOP);
    void DrawBuffers(GLsizei n, const GLenum* bufs GL_INFOP);
    GLenum CheckFramebufferStatus(GLenum target GL_INFOP);
    void DeleteFramebuffers(GLsizei n, const GLuint* framebuffers GL_INFOP);
    void Clear(GLbitfield mask GL_INFOP);
    void ClearColor(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha GL_INFOP);
    void FrontFace(GLenum mode GL_INFOP);
    void CullFace(GLenum mode GL_INFOP);
    void Enable(GLenum cap GL_INFOP);
    void Disable(GLenum cap GL_INFOP);
    void DrawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid* indices GL_INFOP);
    void DepthMask(GLboolean flag GL_INFOP);
    void BindTexture(GLenum target, GLuint texture GL_INFOP);
    void TexImage2D(GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid* data GL_INFOP);
    void DeleteTextures(GLsizei n, const GLuint* textures GL_INFOP);
    void GenTextures(GLsizei n, GLuint* textures GL_INFOP);
    void TexParameterf(GLenum target, GLenum pname, GLfloat param GL_INFOP);
    void TexParameteri(GLenum target, GLenum pname, GLint param GL_INFOP);
    void DrawArrays(GLenum mode, GLint first, GLsizei count GL_INFOP);
    void BlendEquation(GLenum mode GL_INFOP);
    void BlendFunc(GLenum sfactor, GLenum dfactor GL_INFOP);
    void DepthFunc(GLenum func GL_INFOP);
    void ColorMask(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha GL_INFOP);
    void ReadPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid* data GL_INFOP);
    void Viewport(GLint x, GLint y, GLsizei width, GLsizei height GL_INFOP);
    void GetIntegerv(GLenum pname, GLint* data GL_INFOP);
    const GLubyte* GetString(GLenum name GL_INFOP);
    const GLubyte* GetStringi(GLenum name, GLuint index GL_INFOP);
    GLenum GetError(GL_INFO);
    void GetTexLevelParameteriv(GLenum target, GLint level, GLenum pname, GLint* params GL_INFOP);
    void GetCompressedTexImage(GLenum target, GLint lod, GLvoid* img GL_INFOP);
    void CompressedTexImage2D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid* data GL_INFOP);
}

#if defined(DEBUG_GL) && !defined(R_COMMON_NO_DEFINES)
    #define CreateShader(...)             CreateShader(__VA_ARGS__, __FILE__, __LINE__)
    #define ShaderSource(...)             ShaderSource(__VA_ARGS__, __FILE__, __LINE__)
    #define CompileShader(...)            CompileShader(__VA_ARGS__, __FILE__, __LINE__)
    #define AttachShader(...)             AttachShader(__VA_ARGS__, __FILE__, __LINE__)
    #define CreateProgram(...)            CreateProgram(/* no arg */ __FILE__, __LINE__)
    #define LinkProgram(...)              LinkProgram(__VA_ARGS__, __FILE__, __LINE__)
    #define UseProgram(...)               UseProgram(__VA_ARGS__, __FILE__, __LINE__)
    #define GetUniformLocation(...)       GetUniformLocation(__VA_ARGS__, __FILE__, __LINE__)
    #define EnableVertexAttribArray(...)  EnableVertexAttribArray(__VA_ARGS__, __FILE__, __LINE__)
    #define DisableVertexAttribArray(...) DisableVertexAttribArray(__VA_ARGS__, __FILE__, __LINE__)
    #define UniformMatrix4fv(...)         UniformMatrix4fv(__VA_ARGS__, __FILE__, __LINE__)
    #define BindBuffer(...)               BindBuffer(__VA_ARGS__, __FILE__, __LINE__)
    #define GenBuffers(...)               GenBuffers(__VA_ARGS__, __FILE__, __LINE__)
    #define VertexAttribPointer(...)      VertexAttribPointer(__VA_ARGS__, __FILE__, __LINE__)
    #define BufferData(...)               BufferData(__VA_ARGS__, __FILE__, __LINE__)
    #define ValidateProgram(...)          ValidateProgram(__VA_ARGS__, __FILE__, __LINE__)
    #define GenVertexArrays(...)          GenVertexArrays(__VA_ARGS__, __FILE__, __LINE__)
    #define BindVertexArray(...)          BindVertexArray(__VA_ARGS__, __FILE__, __LINE__)
    #define DeleteProgram(...)            DeleteProgram(__VA_ARGS__, __FILE__, __LINE__)
    #define DeleteBuffers(...)            DeleteBuffers(__VA_ARGS__, __FILE__, __LINE__)
    #define DeleteVertexArrays(...)       DeleteVertexArrays(__VA_ARGS__, __FILE__, __LINE__)
    #define Uniform1i(...)                Uniform1i(__VA_ARGS__, __FILE__, __LINE__)
    #define Uniform1f(...)                Uniform1f(__VA_ARGS__, __FILE__, __LINE__)
    #define Uniform2f(...)                Uniform2f(__VA_ARGS__, __FILE__, __LINE__)
    #define Uniform3fv(...)               Uniform3fv(__VA_ARGS__, __FILE__, __LINE__)
    #define GenerateMipmap(...)           GenerateMipmap(__VA_ARGS__, __FILE__, __LINE__)
    #define DeleteShader(...)             DeleteShader(__VA_ARGS__, __FILE__, __LINE__)
    #define GetShaderiv(...)              GetShaderiv(__VA_ARGS__, __FILE__, __LINE__)
    #define GetProgramiv(...)             GetProgramiv(__VA_ARGS__, __FILE__, __LINE__)
    #define GetShaderInfoLog(...)         GetShaderInfoLog(__VA_ARGS__, __FILE__, __LINE__)
    #define ActiveTexture(...)            ActiveTexture(__VA_ARGS__, __FILE__, __LINE__)
    #define GenFramebuffers(...)          GenFramebuffers(__VA_ARGS__, __FILE__, __LINE__)
    #define BindFramebuffer(...)          BindFramebuffer(__VA_ARGS__, __FILE__, __LINE__)
    #define FramebufferTexture2D(...)     FramebufferTexture2D(__VA_ARGS__, __FILE__, __LINE__)
    #define DrawBuffers(...)              DrawBuffers(__VA_ARGS__, __FILE__, __LINE__)
    #define CheckFramebufferStatus(...)   CheckFramebufferStatus(__VA_ARGS__, __FILE__, __LINE__)
    #define DeleteFramebuffers(...)       DeleteFramebuffers(__VA_ARGS__, __FILE__, __LINE__)
    #define Clear(...)                    Clear(__VA_ARGS__, __FILE__, __LINE__)
    #define ClearColor(...)               ClearColor(__VA_ARGS__, __FILE__, __LINE__)
    #define FrontFace(...)                FrontFace(__VA_ARGS__, __FILE__, __LINE__)
    #define CullFace(...)                 CullFace(__VA_ARGS__, __FILE__, __LINE__)
    #define Enable(...)                   Enable(__VA_ARGS__, __FILE__, __LINE__)
    #define Disable(...)                  Disable(__VA_ARGS__, __FILE__, __LINE__)
    #define DrawElements(...)             DrawElements(__VA_ARGS__, __FILE__, __LINE__)
    #define DepthMask(...)                DepthMask(__VA_ARGS__, __FILE__, __LINE__)
    #define BindTexture(...)              BindTexture(__VA_ARGS__, __FILE__, __LINE__)
    #define TexImage2D(...)               TexImage2D(__VA_ARGS__, __FILE__, __LINE__)
    #define DeleteTextures(...)           DeleteTextures(__VA_ARGS__, __FILE__, __LINE__)
    #define GenTextures(...)              GenTextures(__VA_ARGS__, __FILE__, __LINE__)
    #define TexParameterf(...)            TexParameterf(__VA_ARGS__, __FILE__, __LINE__)
    #define TexParameteri(...)            TexParameteri(__VA_ARGS__, __FILE__, __LINE__)
    #define DrawArrays(...)               DrawArrays(__VA_ARGS__, __FILE__, __LINE__)
    #define BlendEquation(...)            BlendEquation(__VA_ARGS__, __FILE__, __LINE__)
    #define BlendFunc(...)                BlendFunc(__VA_ARGS__, __FILE__, __LINE__)
    #define DepthFunc(...)                DepthFunc(__VA_ARGS__, __FILE__, __LINE__)
    #define ColorMask(...)                ColorMask(__VA_ARGS__, __FILE__, __LINE__)
    #define ReadPixels(...)               ReadPixels(__VA_ARGS__, __FILE__, __LINE__)
    #define Viewport(...)                 Viewport(__VA_ARGS__, __FILE__, __LINE__)
    #define GetIntegerv(...)              GetIntegerv(__VA_ARGS__, __FILE__, __LINE__)
    #define GetString(...)                GetString(__VA_ARGS__, __FILE__, __LINE__)
    #define GetStringi(...)               GetStringi(__VA_ARGS__, __FILE__, __LINE__)
    #define GetError(...)                 GetError(/* no arg */ __FILE__, __LINE__)
    #define GetTexLevelParameteriv(...)   GetTexLevelParameteriv(__VA_ARGS__, __FILE__, __LINE__)
    #define GetCompressedTexImage(...)    GetCompressedTexImage(__VA_ARGS__, __FILE__, __LINE__)
    #define CompressedTexImage2D(...)     CompressedTexImage2D(__VA_ARGS__, __FILE__, __LINE__)
#endif
#endif
