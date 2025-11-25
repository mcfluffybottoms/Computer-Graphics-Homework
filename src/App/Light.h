#pragma once

#include <GL/gl.h>
#include <QMatrix4x4>
#include <QOpenGLShaderProgram>
#include <QVector3D>

struct DirectionalLight {
	DirectionalLight(const std::shared_ptr<QOpenGLShaderProgram> & program_,  int i);
	void setUniformValues(const QVector3D & lightPosition_,
						  const QVector3D & lightColor_,
						  float ambient_,
						  float diffuse_,
						  float specular_,
						  const std::shared_ptr<QOpenGLShaderProgram> & program_);

private:
	GLint uniformLightPosition_ = 1.0f;
	GLint uniformLightColor_ = 1.0f;

	GLfloat uniformAmbient_ = 1.0f;
	GLfloat uniformDiffuse_ = 1.0f;
	GLfloat uniformSpecular_ = 1.0f;
};

struct ProjectionLight {
	ProjectionLight(const std::shared_ptr<QOpenGLShaderProgram> & program_,  int i);
	void setUniformValues(const QVector3D & lightPosition_,
						  const QVector3D & lightColor_,
						  const QVector3D & lightDir_,
						  float ambient_,
						  float diffuse_,
						  float specular_,
						  float cutOff_,
						  float outerCutOff_,
						  const std::shared_ptr<QOpenGLShaderProgram> & program_);

private:
	GLint uniformLightPosition_ = 1.0f;
	GLint uniformLightColor_ = 1.0f;
	GLint uniformLightDir_ = 1.0f;

	GLfloat uniformAmbient_ = 1.0f;
	GLfloat uniformDiffuse_ = 1.0f;
	GLfloat uniformSpecular_ = 1.0f;

	GLint uniformCutOff_ = 1.0f;
	GLint uniformOuterCutOff_ = 1.0f;
};