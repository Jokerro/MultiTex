#include "stdafx.h"
#include "Shaders.h"

int Shaders::Init(char * fileVertexShader, char * fileFragmentShader)
{
	vertexShader = esLoadShader(GL_VERTEX_SHADER, fileVertexShader);

	if ( vertexShader == 0 )
		return -1;

	fragmentShader = esLoadShader(GL_FRAGMENT_SHADER, fileFragmentShader);

	if ( fragmentShader == 0 )
	{
		glDeleteShader( vertexShader );
		return -2;
	}

	program = esLoadProgram(vertexShader, fragmentShader);

	//finding location of uniforms / attributes
	positionAttribute = glGetAttribLocation(program, "a_posL");

	uvAttribute = glGetAttribLocation(program, "a_uvPos");

	textureUniform[0] = glGetUniformLocation(program, "u_tex0");
	textureUniform[1] = glGetUniformLocation(program, "u_tex1");
	textureMaskUniform = glGetUniformLocation(program, "u_tex_mask");

	matrixTransform = glGetUniformLocation(program, "u_matT");

	return 0;
}

Shaders::~Shaders()
{
	glDeleteProgram(program);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}