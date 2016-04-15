// NewTrainingFramework.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../Utilities/utilities.h" 
#include "Vertex.h"
#include "Index.h"
#include "Shaders.h"
#include "Camera.h"
#include "Globals.h"
#include "Helper.h"
#include <conio.h>
#include <iostream>
#include <stdio.h>
#include <math.h>


GLuint vboId;
GLuint iboId;
GLuint textureGrassHandle;
GLuint textureMaskHandle;
GLuint textureDirtHandle;
Shaders myShaders;
int vertices, indices;

bool up = true;

GLfloat time = 0, ampFactor = 0.01;

Camera camera(Vector3(0.0f, 0.0, 100.0f));

Matrix transformMatrix;

Matrix modelMatrix;
Matrix viewMatrix;
Matrix projectionMatrix;

int Init ( ESContext *esContext )
{
	glClearColor ( 0.0f, 0.0f, 0.0f, 1.0f );
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_CULL_FACE);

	//triangle data (heap)
	FILE *pFile;
	fopen_s(&pFile, "../Resources/Models/Terrain.nfg", "r");
	fscanf_s(pFile, "NrVertices: %d", &vertices);
	Vertex *verticesData = new Vertex[vertices];
	for (int i = 0; i<vertices; i++) {
		fscanf_s(pFile, " %*d. pos:[%f, %f, %f]; norm:[%*f, %*f, %*f]; binorm:[%*f, %*f, %*f]; tgt:[%*f, %*f, %*f]; uv:[%f, %f]; ", &verticesData[i].pos.x, &verticesData[i].pos.y, &verticesData[i].pos.z, &verticesData[i].uv.x, &verticesData[i].uv.y);
	}
		
	//buffer object
	glGenBuffers(1, &vboId); //buffer object name generation
	glBindBuffer(GL_ARRAY_BUFFER, vboId); //buffer object binding
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*vertices, verticesData, GL_STATIC_DRAW); //creation and initializion of buffer onject storage
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	fscanf_s(pFile, "NrIndices: %d", &indices);
	Index *indicesData = new Index[indices];
	for (int i = 0; i<indices/3; i++) {
		fscanf_s(pFile, " %*d. %u, %u, %u ", &indicesData[i].x, &indicesData[i].y, &indicesData[i].z);
	}
	
	glGenBuffers(1, &iboId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short)*indices, indicesData, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	fclose(pFile);

	Helper::loadTexture(&textureGrassHandle, "../Resources/Textures/Grass.tga");
	Helper::loadTexture(&textureDirtHandle, "../Resources/Textures/Dirt.tga");
	Helper::loadTexture(&textureMaskHandle, "../Resources/Textures/mask.tga");

	Vector3 modelPivot = Helper::getModelPivot(vertices, verticesData);
	modelMatrix.SetIdentity();
	modelMatrix.SetTranslation(-modelPivot.x, -modelPivot.y, -modelPivot.z);
	modelMatrix = modelMatrix * modelMatrix.SetRotationX(90.0f);
	viewMatrix = camera.GetViewMatrix();
	projectionMatrix.SetPerspective(45.0f, float(Globals::screenWidth) / float(Globals::screenHeight), 0.1f, 500.0f);

	transformMatrix = modelMatrix * viewMatrix * projectionMatrix;

	delete []verticesData;
	delete []indicesData;
	//creation of shaders and program 
	return myShaders.Init("../Resources/Shaders/TriangleShaderVS.vs", "../Resources/Shaders/TriangleShaderFS.fs");

}

void Draw ( ESContext *esContext )
{
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(myShaders.program);

	glBindBuffer(GL_ARRAY_BUFFER, vboId);

	GLfloat* ptr = 0;
	if (myShaders.positionAttribute != -1) //attribute passing to shader, for uniforms use glUniform1f(time, deltaT); glUniformMatrix4fv( m_pShader->matrixWVP, 1, false, (GLfloat *)&rotationMat );
	{
		glEnableVertexAttribArray(myShaders.positionAttribute);
		glVertexAttribPointer(myShaders.positionAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), ptr);
	}

	if (myShaders.uvAttribute != -1) //attribute passing to shader, for uniforms use glUniform1f(time, deltaT); glUniformMatrix4fv( m_pShader->matrixWVP, 1, false, (GLfloat *)&rotationMat );
	{
		glEnableVertexAttribArray(myShaders.uvAttribute);
		glVertexAttribPointer(myShaders.uvAttribute, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), ptr+3);
	}	
	
	if (myShaders.matrixTransform != -1) {
		glUniformMatrix4fv(myShaders.matrixTransform, 1, false, (GLfloat*)&transformMatrix);
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboId);

	unsigned short textureUnit = 0;
	//grass texture
	glActiveTexture(GL_TEXTURE0 + textureUnit);
	glBindTexture(GL_TEXTURE_2D, textureGrassHandle);
	glUniform1i(myShaders.textureUniform[textureUnit], textureUnit);

	//dirt texture
	glActiveTexture(GL_TEXTURE0 + ++textureUnit);
	glBindTexture(GL_TEXTURE_2D, textureDirtHandle);
	glUniform1i(myShaders.textureUniform[textureUnit], textureUnit);

	//mask texture
	glActiveTexture(GL_TEXTURE0 + ++textureUnit);
	glBindTexture(GL_TEXTURE_2D, textureMaskHandle);
	glUniform1i(myShaders.textureMaskUniform, textureUnit++);

	glDrawElements(GL_TRIANGLES, indices, GL_UNSIGNED_SHORT, 0);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	eglSwapBuffers ( esContext->eglDisplay, esContext->eglSurface );
}

void Update(ESContext *esContext, float deltaTime)
{
	viewMatrix = camera.GetViewMatrix();
	transformMatrix = modelMatrix * viewMatrix * projectionMatrix;
	time += deltaTime;
}

void Key ( ESContext *esContext, unsigned char key, bool bIsPressed)
{
	camera.ProcessKeyboard(CameraMovement(key));
}

void CleanUp()
{
	glDeleteBuffers(1, &vboId);
	glDeleteBuffers(1, &iboId);
	glDeleteTextures(1, &textureGrassHandle);
}

int _tmain(int argc, _TCHAR* argv[])
{
	ESContext esContext;

    esInitContext ( &esContext );

	esCreateWindow ( &esContext, "Hello Model", Globals::screenWidth, Globals::screenHeight, ES_WINDOW_RGB | ES_WINDOW_DEPTH);

	if ( Init ( &esContext ) != 0 )
		return 0;

	esRegisterDrawFunc ( &esContext, Draw );
	esRegisterUpdateFunc ( &esContext, Update );
	esRegisterKeyFunc ( &esContext, Key);

	esMainLoop ( &esContext );

	//releasing OpenGL resources
	CleanUp();

	//identifying memory leaks
	MemoryDump();
	printf("Press any key...\n");
	_getch();

	return 0;
}
