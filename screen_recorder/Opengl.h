#pragma once
#include <GL/glew.h>
#include "SimpleShader.h"

#ifdef _DEBUG
	#pragma comment(lib, "glew32d.lib")
#else
	#pragma comment(lib, "glew32.lib")
#endif

class OpenglConvertRgbToNV12
{
public:
	OpenglConvertRgbToNV12();
	~OpenglConvertRgbToNV12();

	bool Initial();

	bool ConvertRgbToNV12(void *data, int width, int height, void *result, uint8_t *nv12_data[4], uint32_t linesize[4]);

private:
	GLuint __vbo;
	GLuint __vao;
	GLuint __ebo;
	GLuint __tex_crood_buff;
	GLuint __fbo;
	GLuint __rbo;
	GLuint __texture_frame;
	GLuint __texture_src;
	Shader __shader_y;
	Shader __shader_uv;

	static GLfloat	__vertices[12];
	static GLuint	__indices[6];

	static const GLchar	*__vs_sh_src;
	static const GLchar	*__y_sh_src;
	static const GLchar	*__uv_sh_src;

	GLfloat			__tex_crood[8];

	void __FillTextureCroodBuff(int width, int height);
	bool __ConvertRgbToY(void *data, int width, int height, void *result);
	bool __ConvertRgbToUV(void *data, int width, int height, void *result);
};

