#pragma once
#include "../Utilities/utilities.h"

class Shaders 
{
public:
	GLuint program, vertexShader, fragmentShader;
	char fileVS[260];
	char fileFS[260];

	GLint positionAttribute;
	GLint uvAttribute, matrixTransform;
	GLint textureUniform[2];
	GLint texture0Uniform, textureMaskUniform, texture1Uniform;
	GLint timeUniform;
	GLint ampFactorUniform;

	int Init(char * fileVertexShader, char * fileFragmentShader);
	~Shaders();
};