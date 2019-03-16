#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <GL/glew.h>; // 包含glew来获取所有的必须OpenGL头文件

class Shader
{
public:
	// 程序ID
	GLuint Program;

	Shader()
	{
		
	}

	void LoadFromFile(const GLchar* vertexPath, const GLchar* fragmentPath)
	{
		// 1. 从文件路径中获取顶点/片段着色器
		std::string vertexCode;
		std::string fragmentCode;
		std::ifstream vShaderFile;
		std::ifstream fShaderFile;
		// 保证ifstream对象可以抛出异常：
		vShaderFile.exceptions(std::ifstream::badbit);
		fShaderFile.exceptions(std::ifstream::badbit);
		try
		{
			// 打开文件
			vShaderFile.open(vertexPath);
			fShaderFile.open(fragmentPath);
			std::stringstream vShaderStream, fShaderStream;
			// 读取文件的缓冲内容到流中
			vShaderStream << vShaderFile.rdbuf();
			fShaderStream << fShaderFile.rdbuf();
			// 关闭文件
			vShaderFile.close();
			fShaderFile.close();
			// 转换流至GLchar数组
			vertexCode = vShaderStream.str();
			fragmentCode = fShaderStream.str();
		}
		catch (std::ifstream::failure e)
		{
			std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
		}
		const GLchar* vShaderCode = vertexCode.c_str();
		const GLchar* fShaderCode = fragmentCode.c_str();

		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &vShaderCode, NULL);
		glCompileShader(vertexShader);

		GLint success;
		GLchar infoLog[512];
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);		//查询着色器是否成功编译
		if (!success)
		{
			glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
			throw "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n";
		}

		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);		//创建片段着色器
		glShaderSource(fragmentShader, 1, &fShaderCode, NULL);
		glCompileShader(fragmentShader);
		// Check for compile time errors
		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
			throw 1;
		}
		// Link shaders
		GLuint shaderProgram = glCreateProgram();						//创建程序
		glAttachShader(shaderProgram, vertexShader);
		glAttachShader(shaderProgram, fragmentShader);
		glLinkProgram(shaderProgram);

		glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
			throw 1;
		}
		glDeleteShader(vertexShader);									//释放着色器
		glDeleteShader(fragmentShader);

		this->Program = shaderProgram;
	}

	void LoadFormStr(const GLchar* vShaderCode, const GLchar* fShaderCode)
	{
		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &vShaderCode, NULL);
		glCompileShader(vertexShader);

		GLint success;
		GLchar infoLog[512];
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);		//查询着色器是否成功编译
		if (!success)
		{
			glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
			throw "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n";
		}

		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);		//创建片段着色器
		glShaderSource(fragmentShader, 1, &fShaderCode, NULL);
		glCompileShader(fragmentShader);
		// Check for compile time errors
		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
			throw 1;
		}
		// Link shaders
		GLuint shaderProgram = glCreateProgram();						//创建程序
		glAttachShader(shaderProgram, vertexShader);
		glAttachShader(shaderProgram, fragmentShader);
		glLinkProgram(shaderProgram);

		glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
			throw 1;
		}
		glDeleteShader(vertexShader);									//释放着色器
		glDeleteShader(fragmentShader);

		this->Program = shaderProgram;
	}

	// 使用程序
	void Use() 
	{
		glUseProgram(this->Program);
	}
};

