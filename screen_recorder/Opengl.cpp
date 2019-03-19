#include "pch.h"
#include "Opengl.h"

GLfloat OpenglConvertRgbToNV12::__vertices[12] = 
{
	// Positions        
	 1.0f,   1.0f, 0.0f,  
	 1.0f,	-1.0f, 0.0f,   
	-1.0f,	-1.0f, 0.0f,  
	-1.0f,	 1.0f, 0.0f, 
};

GLuint OpenglConvertRgbToNV12::__indices[6] = 
{
	0, 1, 3, // First Triangle
	1, 2, 3  // Second Triangle
};

const GLchar *OpenglConvertRgbToNV12::__vs_sh_src = 
"#version 330 core\n\
layout(location = 0) in vec3 aPos;\n\
layout(location = 1) in vec2 aTexCoord;\n\
\n\
out vec2 TexCoord;\n\
void main()\n\
{\n\
	gl_Position = vec4(aPos, 1.0);\n\
	TexCoord = vec2(aTexCoord.x, aTexCoord.y);\n\
}";

const GLchar *OpenglConvertRgbToNV12::__y_sh_src = 
"#version 330 core\n\
out vec4 FragColor;\n\
in vec2 TexCoord;\n\
float y;\n\
vec4 rgb;\n\
uniform sampler2DRect ourTexture;\n\
void main()\n\
{\n\
	rgb = texture2DRect(ourTexture, TexCoord);\n\
	y = (0.257 * rgb.r) + (0.504 * rgb.g) + (0.098 * rgb.b) + 0.0625;\n\
	FragColor = vec4(y, 0, 0, 0);\n\
}";

const GLchar *OpenglConvertRgbToNV12::__uv_sh_src =
"#version 330 core\n\
out vec4 FragColor;\n\
in vec2 TexCoord;\n\
float u, v;\n\
vec4 rgb;\n\
uniform sampler2DRect    ourTexture;\n\
void main()\n\
{\n\
	rgb = texture2DRect(ourTexture, TexCoord);\n\
	v = (0.439 * rgb.r) - (0.368 * rgb.g) - (0.071 * rgb.b) + 0.500;\n\
	u = (-0.148 * rgb.r) - (0.291 * rgb.g) + (0.439 * rgb.b) + 0.500;\n\
	FragColor = vec4(u, v, 0, 0);\n\
}";


OpenglConvertRgbToNV12::OpenglConvertRgbToNV12():
	__vbo(0),
	__vao(0),
	__ebo(0),
	__tex_crood_buff(0),
	__fbo(0),
	__rbo(0),
	__texture_frame(0),
	__texture_src(0)
{

}


OpenglConvertRgbToNV12::~OpenglConvertRgbToNV12()
{
}

bool OpenglConvertRgbToNV12::Initial()
{
	glewInit();

	glGenVertexArrays(1, &__vao);
	glGenBuffers(1, &__vbo);
	glGenBuffers(1, &__ebo);
	glGenBuffers(1, &__tex_crood_buff);

	glBindVertexArray(__vao);

	glBindBuffer(GL_ARRAY_BUFFER, __vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(__vertices), __vertices, GL_STATIC_DRAW);

	/*glBindBuffer(GL_ARRAY_BUFFER, __tex_crood_buff);
	glBufferData(GL_ARRAY_BUFFER, sizeof(__tex_crood), __tex_crood, GL_STATIC_DRAW);*/

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, __ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(__indices), __indices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, __vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), NULL);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, __tex_crood_buff);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), NULL);
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glGenTextures(1, &__texture_frame);
	glBindTexture(GL_TEXTURE_RECTANGLE, __texture_frame);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_RECTANGLE, 0);

	glGenTextures(1, &__texture_src);
	glBindTexture(GL_TEXTURE_RECTANGLE, __texture_src);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_RECTANGLE, 0);

	//生成RenderBufferObject, 
	glGenRenderbuffers(1, &__rbo);
	//绑定rbo
	glBindRenderbuffer(GL_RENDERBUFFER, __rbo);
	//为rbo开辟存储空间
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	//生成fbo
	glGenFramebuffers(1, &__fbo);
	//绑定fbo
	glBindFramebuffer(GL_FRAMEBUFFER, __fbo);
	//绑定rbo到fbo
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_COMPONENT, GL_RENDERBUFFER, __rbo);
	//绑定纹理到fbo
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_RECTANGLE, __texture_frame, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	GLenum e = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (e != GL_FRAMEBUFFER_COMPLETE)
		throw 0;

	__shader_y.LoadFormStr(__vs_sh_src, __y_sh_src);
	__shader_uv.LoadFormStr(__vs_sh_src, __uv_sh_src);

	return true;
}

bool OpenglConvertRgbToNV12::ConvertRgbToNV12(void *data, int width, int height, void *result, uint8_t *nv12_data[4], uint32_t linesize[4])

{
	__FillTextureCroodBuff(width, height);
	__ConvertRgbToY(data, width, height, result);

	char *uv_start = (char*)result;
	uv_start += height * width;
	__ConvertRgbToUV(data, width, height, uv_start);

	nv12_data[0] = reinterpret_cast<uint8_t*>(result);
	linesize[0] = width;
	nv12_data[1] = reinterpret_cast<uint8_t*>(uv_start);
	linesize[1] = width;
	nv12_data[2] = 0;
	linesize[2] = 0;
	nv12_data[3] = 0;
	linesize[3] = 0;
	return true;
}

void OpenglConvertRgbToNV12::__FillTextureCroodBuff(int width, int height)
{
	__tex_crood[0] = width;
	__tex_crood[1] = height;
	__tex_crood[2] = width;
	__tex_crood[3] = 0.0f;
	__tex_crood[4] = 0.0f;
	__tex_crood[5] = 0.0f;
	__tex_crood[6] = 0.0f;
	__tex_crood[7] = height;
}

bool OpenglConvertRgbToNV12::__ConvertRgbToY(void *data, int width, int height, void *result)
{
	__shader_y.Use();

	glBindBuffer(GL_ARRAY_BUFFER, __tex_crood_buff);
	glBufferData(GL_ARRAY_BUFFER, sizeof(__tex_crood), __tex_crood, GL_STATIC_DRAW);

	glBindTexture(GL_TEXTURE_RECTANGLE, __texture_frame);
	glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_RECTANGLE, 0);

	glBindRenderbuffer(GL_RENDERBUFFER, __rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);

	glBindTexture(GL_TEXTURE_RECTANGLE, __texture_src);
	glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	glBindFramebuffer(GL_FRAMEBUFFER, __fbo);

	glViewport(0, 0, width, height);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);

	glBindVertexArray(__vao);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


	glReadBuffer(GL_COLOR_ATTACHMENT0);
	glReadPixels(0, 0, width, height, GL_RED, GL_UNSIGNED_BYTE, result);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_RECTANGLE, 0);

	return true;
}

bool OpenglConvertRgbToNV12::__ConvertRgbToUV(void *data, int width, int height, void *result)
{
	int scale_width(0);
	int scale_height(0);

	scale_width = (width / 2) + (width % 2);
	scale_height = (height / 2) + (height % 2);

	__shader_uv.Use();

	glBindBuffer(GL_ARRAY_BUFFER, __tex_crood_buff);
	glBufferData(GL_ARRAY_BUFFER, sizeof(__tex_crood), __tex_crood, GL_STATIC_DRAW);

	glBindTexture(GL_TEXTURE_RECTANGLE, __texture_frame);
	glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA8, scale_width, scale_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_RECTANGLE, 0);

	glBindRenderbuffer(GL_RENDERBUFFER, __rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, scale_width, scale_height);

	glBindTexture(GL_TEXTURE_RECTANGLE, __texture_src);
	glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	glBindFramebuffer(GL_FRAMEBUFFER, __fbo);

	glViewport(0, 0, scale_width, scale_height);
	glBindVertexArray(__vao);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindTexture(GL_TEXTURE_RECTANGLE, 0);

	glReadPixels(0, 0, scale_width, scale_height, GL_RG, GL_UNSIGNED_BYTE, result);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_RECTANGLE, 0);

	return true;
}
