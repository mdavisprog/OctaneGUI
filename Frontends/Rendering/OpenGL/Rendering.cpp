/**

MIT License

Copyright (c) 2022 Mitchell Davis <mdavisprog@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#include "../Rendering.h"
#include "OctaneGUI/OctaneGUI.h"

#include "GL/gl.h"
#include "GL/glext.h"
#include "GL/glx.h"

#include <cassert>
#include <unordered_map>
#include <vector>

#if SDL2
	#include "SDL.h"
	#include "../../Windowing/SDL2/Interface.h"
#endif

namespace Rendering
{

std::vector<GLuint> g_Textures;
GLuint g_Program = 0;
GLuint g_VertexBuffer = 0;
GLuint g_IndexBuffer = 0;
GLuint g_DefaultTexture = 0;
GLint g_UniformTexture;
GLint g_UniformProjection;
GLint g_AttribPosition;
GLint g_AttribUV;
GLint g_AttribColor;

PFNGLCREATESHADERPROC glCreateShader;
PFNGLDELETESHADERPROC glDeleteShader;
PFNGLSHADERSOURCEPROC glShaderSource;
PFNGLCOMPILESHADERPROC glCompileShader;
PFNGLGETSHADERIVPROC glGetShaderiv;
PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
PFNGLCREATEPROGRAMPROC glCreateProgram;
PFNGLDELETEPROGRAMPROC glDeleteProgram;
PFNGLUSEPROGRAMPROC glUseProgram;
PFNGLATTACHSHADERPROC glAttachShader;
PFNGLDETACHSHADERPROC glDetachShader;
PFNGLLINKPROGRAMPROC glLinkProgram;
PFNGLGETPROGRAMIVPROC glGetProgramiv;
PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;
PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
PFNGLGETATTRIBLOCATIONPROC glGetAttribLocation;
PFNGLGENBUFFERSPROC glGenBuffers;
PFNGLDELETEBUFFERSPROC glDeleteBuffers;
PFNGLBINDBUFFERPROC glBindBuffer;
PFNGLBUFFERDATAPROC glBufferData;
PFNGLBLENDFUNCSEPARATEPROC glBlendFuncSeparate;
PFNGLUNIFORM1IPROC glUniform1i;
PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv;
PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;
PFNGLBINDSAMPLERPROC glBindSampler;
PFNGLGENVERTEXARRAYSPROC glGenVertexArrays;
PFNGLBINDVERTEXARRAYPROC glBindVertexArray;
PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays;
PFNGLDRAWELEMENTSBASEVERTEXPROC glDrawElementsBaseVertex;

#define LOAD_PROCEDURE(FnName, Type)                           \
	FnName = (Type)glXGetProcAddress((const GLubyte*)#FnName); \
	assert(FnName != nullptr);

#if SDL2
SDL_GLContext g_Context = nullptr;
#endif

bool CompileResult(GLuint Shader)
{
	GLint CompileStatus;
	glGetShaderiv(Shader, GL_COMPILE_STATUS, &CompileStatus);
	if ((GLboolean)CompileStatus == GL_FALSE)
	{
		GLint Length;
		glGetShaderiv(Shader, GL_INFO_LOG_LENGTH, &Length);

		std::vector<GLchar> Buffer;
		Buffer.resize(Length);

		GLsizei Size;
		glGetShaderInfoLog(Shader, Buffer.size(), &Size, Buffer.data());

		printf("Failed to compile shader: %s\n", Buffer.data());
		return false;
	}

	return true;
}

bool LinkResult(GLuint Program)
{
	GLint LinkStatus;
	glGetProgramiv(Program, GL_LINK_STATUS, &LinkStatus);
	if ((GLboolean)LinkStatus == GL_FALSE)
	{
		GLint Length;
		glGetProgramiv(Program, GL_INFO_LOG_LENGTH, &Length);

		std::vector<GLchar> Buffer;
		Buffer.resize(Length);

		GLsizei Size;
		glGetProgramInfoLog(Program, Buffer.size(), &Size, Buffer.data());

		printf("Failed to link program: %s\n", Buffer.data());
		return false;
	}

	return true;
}

void LoadShaders()
{
	const GLchar* Version = "#version 130\n";

	const GLchar* VertexShader =
		"uniform mat4 Projection;\n"
		"in vec2 Position;\n"
		"in vec2 UV;\n"
		"in vec4 Color;\n"
		"out vec2 Fragment_UV;\n"
		"out vec4 Fragment_Color;\n"
		"void main()\n"
		"{\n"
		"	Fragment_UV = UV;\n"
		"	Fragment_Color = Color;\n"
		"	gl_Position = Projection * vec4(Position.xy, 0, 1);\n"
		"}\n";

	const GLchar* FragmentShader =
		"uniform sampler2D Texture;\n"
		"in vec2 Fragment_UV;\n"
		"in vec4 Fragment_Color;\n"
		"out vec4 Out_Color;\n"
		"void main()\n"
		"{\n"
		"	Out_Color = Fragment_Color * texture(Texture, Fragment_UV.st);\n"
		"}\n";

	const GLchar* VertexShaderInfo[2] = { Version, VertexShader };
	GLuint VertexID = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(VertexID, 2, VertexShaderInfo, nullptr);
	glCompileShader(VertexID);
	assert(CompileResult(VertexID));

	const GLchar* FragmentShaderInfo[2] = { Version, FragmentShader };
	GLuint FragmentID = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(FragmentID, 2, FragmentShaderInfo, nullptr);
	glCompileShader(FragmentID);
	assert(CompileResult(FragmentID));

	g_Program = glCreateProgram();
	glAttachShader(g_Program, VertexID);
	glAttachShader(g_Program, FragmentID);
	glLinkProgram(g_Program);
	assert(LinkResult(g_Program));

	glDetachShader(g_Program, VertexID);
	glDetachShader(g_Program, FragmentID);
	glDeleteShader(VertexID);
	glDeleteShader(FragmentID);

	g_UniformTexture = glGetUniformLocation(g_Program, "Texture");
	g_UniformProjection = glGetUniformLocation(g_Program, "Projection");
	g_AttribPosition = glGetAttribLocation(g_Program, "Position");
	g_AttribUV = glGetAttribLocation(g_Program, "UV");
	g_AttribColor = glGetAttribLocation(g_Program, "Color");

	glGenBuffers(1, &g_VertexBuffer);
	glGenBuffers(1, &g_IndexBuffer);

	assert(g_UniformTexture != -1);
	assert(g_UniformProjection != -1);
	assert(g_AttribPosition != -1);
	assert(g_AttribUV != -1);
	assert(g_AttribColor != -1);

	assert(g_Program != 0);
	assert(g_VertexBuffer != 0);
	assert(g_IndexBuffer != 0);
}

void Initialize()
{
#if SDL2
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
#endif

	LOAD_PROCEDURE(glCreateShader, PFNGLCREATESHADERPROC);
	LOAD_PROCEDURE(glDeleteShader, PFNGLDELETESHADERPROC);
	LOAD_PROCEDURE(glShaderSource, PFNGLSHADERSOURCEPROC);
	LOAD_PROCEDURE(glCompileShader, PFNGLCOMPILESHADERPROC);
	LOAD_PROCEDURE(glGetShaderiv, PFNGLGETSHADERIVPROC);
	LOAD_PROCEDURE(glGetShaderInfoLog, PFNGLGETSHADERINFOLOGPROC);
	LOAD_PROCEDURE(glCreateProgram, PFNGLCREATEPROGRAMPROC);
	LOAD_PROCEDURE(glDeleteProgram, PFNGLDELETEPROGRAMPROC);
	LOAD_PROCEDURE(glUseProgram, PFNGLUSEPROGRAMPROC);
	LOAD_PROCEDURE(glAttachShader, PFNGLATTACHSHADERPROC);
	LOAD_PROCEDURE(glDetachShader, PFNGLDETACHSHADERPROC);
	LOAD_PROCEDURE(glLinkProgram, PFNGLLINKPROGRAMPROC);
	LOAD_PROCEDURE(glGetProgramiv, PFNGLGETPROGRAMIVPROC);
	LOAD_PROCEDURE(glGetProgramInfoLog, PFNGLGETPROGRAMINFOLOGPROC);
	LOAD_PROCEDURE(glGetUniformLocation, PFNGLGETUNIFORMLOCATIONPROC);
	LOAD_PROCEDURE(glGetAttribLocation, PFNGLGETATTRIBLOCATIONPROC);
	LOAD_PROCEDURE(glGenBuffers, PFNGLGENBUFFERSPROC);
	LOAD_PROCEDURE(glDeleteBuffers, PFNGLDELETEBUFFERSPROC);
	LOAD_PROCEDURE(glBindBuffer, PFNGLBINDBUFFERPROC);
	LOAD_PROCEDURE(glBufferData, PFNGLBUFFERDATAPROC);
	LOAD_PROCEDURE(glBlendFuncSeparate, PFNGLBLENDFUNCSEPARATEPROC);
	LOAD_PROCEDURE(glUniform1i, PFNGLUNIFORM1IPROC);
	LOAD_PROCEDURE(glUniformMatrix4fv, PFNGLUNIFORMMATRIX4FVPROC);
	LOAD_PROCEDURE(glEnableVertexAttribArray, PFNGLENABLEVERTEXATTRIBARRAYPROC);
	LOAD_PROCEDURE(glVertexAttribPointer, PFNGLVERTEXATTRIBPOINTERPROC);
	LOAD_PROCEDURE(glBindSampler, PFNGLBINDSAMPLERPROC);
	LOAD_PROCEDURE(glGenVertexArrays, PFNGLGENVERTEXARRAYSPROC);
	LOAD_PROCEDURE(glBindVertexArray, PFNGLBINDVERTEXARRAYPROC);
	LOAD_PROCEDURE(glDeleteVertexArrays, PFNGLDELETEVERTEXARRAYSPROC);
	LOAD_PROCEDURE(glDrawElementsBaseVertex, PFNGLDRAWELEMENTSBASEVERTEXPROC);
}

void CreateRenderer(OctaneGUI::Window* Window)
{
#if SDL2
	SDL_Window* Instance = Windowing::Get(Window);

	if (g_Context == nullptr)
	{
		g_Context = SDL_GL_CreateContext(Instance);
		SDL_GL_MakeCurrent(Instance, g_Context);
		SDL_GL_SetSwapInterval(0);
	}
#endif

	if (g_Program == 0)
	{
		LoadShaders();

		std::vector<uint8_t> Texture { 255, 255, 255, 255 };
		g_DefaultTexture = LoadTexture(Texture, 1, 1);
	}
}

void DestroyRenderer(OctaneGUI::Window* Window)
{
}

void Paint(OctaneGUI::Window* Window, const OctaneGUI::VertexBuffer& Buffer)
{
#if SDL2
	SDL_Window* Instance = Windowing::Get(Window);
	SDL_GL_MakeCurrent(Instance, g_Context);
#endif

	const OctaneGUI::Vector2 Size = Window->GetSize();
	glViewport(0, 0, (int)Size.X, (int)Size.Y);

	glClearColor(0, 0, 0, 255);
	glClear(GL_COLOR_BUFFER_BIT);

	GLuint VertexArrayObject = 0;
	glGenVertexArrays(1, &VertexArrayObject);

	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_STENCIL_TEST);
	glEnable(GL_SCISSOR_TEST);

	float L = 0.0f;
	float R = Size.X;
	float T = 0.0f;
	float B = Size.Y;
	const float Projection[4][4] = {
		{ 2.0f / (R - L), 0.0f, 0.0f, 0.0f },
		{ 0.0f, 2.0f / (T - B), 0.0f, 0.0f },
		{ 0.0f, 0.0f, -1.0f, 0.0f },
		{ (R + L) / (L - R), (T + B) / (B - T), 0.0f, 1.0f }
	};

	glUseProgram(g_Program);
	glUniform1i(g_UniformTexture, 0);
	glUniformMatrix4fv(g_UniformProjection, 1, GL_FALSE, &Projection[0][0]);

	glBindVertexArray(VertexArrayObject);

	glBindBuffer(GL_ARRAY_BUFFER, g_VertexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_IndexBuffer);
	glEnableVertexAttribArray(g_AttribPosition);
	glEnableVertexAttribArray(g_AttribUV);
	glEnableVertexAttribArray(g_AttribColor);
	glVertexAttribPointer(g_AttribPosition, 2, GL_FLOAT, GL_FALSE, sizeof(OctaneGUI::Vertex), (GLvoid*)offsetof(OctaneGUI::Vertex, Position));
	glVertexAttribPointer(g_AttribUV, 2, GL_FLOAT, GL_FALSE, sizeof(OctaneGUI::Vertex), (GLvoid*)offsetof(OctaneGUI::Vertex, TexCoords));
	glVertexAttribPointer(g_AttribColor, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(OctaneGUI::Vertex), (GLvoid*)offsetof(OctaneGUI::Vertex, Col));

	const std::vector<OctaneGUI::Vertex>& Vertices = Buffer.GetVertices();
	const std::vector<uint32_t>& Indices = Buffer.GetIndices();

	const GLsizeiptr VertexBufferSize = (GLsizeiptr)(Vertices.size() * sizeof(OctaneGUI::Vertex));
	const GLsizeiptr IndexBufferSize = (GLsizeiptr)(Indices.size() * sizeof(uint32_t));

	glBufferData(GL_ARRAY_BUFFER, VertexBufferSize, Vertices.data(), GL_STREAM_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, IndexBufferSize, Indices.data(), GL_STREAM_DRAW);

	for (const OctaneGUI::DrawCommand& Command : Buffer.Commands())
	{
		OctaneGUI::Rect Scissor { OctaneGUI::Vector2::Zero, Size };
		const OctaneGUI::Rect Clip = Command.Clip();
		if (!Clip.IsZero())
		{
			Scissor = Clip;
		}
		glScissor((GLint)Scissor.Min.X, (GLint)(Size.Y - Scissor.Max.Y), (GLsizei)Scissor.Width(), (GLsizei)Scissor.Height());

		if (Command.TextureID() == 0)
		{
			glBindTexture(GL_TEXTURE_2D, g_DefaultTexture);
		}
		else
		{
			glBindTexture(GL_TEXTURE_2D, Command.TextureID());
		}

		glDrawElementsBaseVertex(GL_TRIANGLES, (GLsizei)Command.IndexCount(), GL_UNSIGNED_INT, (void*)(Command.IndexOffset() * sizeof(uint32_t)), (GLint)Command.VertexOffset());
	}

	glDeleteVertexArrays(1, &VertexArrayObject);

#if SDL2
	SDL_GL_SwapWindow(Instance);
#endif
}

uint32_t LoadTexture(const std::vector<uint8_t>& Data, uint32_t Width, uint32_t Height)
{
	GLuint Texture = 0;
	GLint Current = 0;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &Current);

	glGenTextures(1, &Texture);
	glBindTexture(GL_TEXTURE_2D, Texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Width, Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, static_cast<const void*>(Data.data()));

	glBindTexture(GL_TEXTURE_2D, Current);
	g_Textures.push_back(Texture);

	return Texture;
}

void Exit()
{
	if (g_VertexBuffer != 0)
	{
		glDeleteBuffers(1, &g_VertexBuffer);
		g_VertexBuffer = 0;
	}

	if (g_IndexBuffer != 0)
	{
		glDeleteBuffers(1, &g_IndexBuffer);
		g_IndexBuffer = 0;
	}

	if (g_Program != 0)
	{
		glDeleteProgram(g_Program);
		g_Program = 0;
	}

	glDeleteTextures(g_Textures.size(), g_Textures.data());
	g_Textures.clear();

#if SDL2
	if (g_Context != nullptr)
	{
		SDL_GL_DeleteContext(g_Context);
	}
#endif
}

}
