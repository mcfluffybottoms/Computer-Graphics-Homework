#include "Light.h"
#include <QtMath>

DirectionalLight::DirectionalLight(const std::shared_ptr<QOpenGLShaderProgram> & program_, int i)
{
    QString base = QString("dirLight[%1].").arg(i);
	uniformAmbient_ = program_->uniformLocation(base + "ambient");
	uniformDiffuse_ = program_->uniformLocation(base + "diffuse");
	uniformSpecular_ = program_->uniformLocation(base + "specular");

	uniformLightPosition_ = program_->uniformLocation(base + "lightPos");
	uniformLightColor_ = program_->uniformLocation(base + "lightColor");
}

ProjectionLight::ProjectionLight(const std::shared_ptr<QOpenGLShaderProgram> & program_, int i)
{
    QString base = QString("projLight[%1].").arg(i);
	uniformAmbient_ = program_->uniformLocation(base + "ambient");
	uniformDiffuse_ = program_->uniformLocation(base + "diffuse");
	uniformSpecular_ = program_->uniformLocation(base + "specular");

	uniformLightPosition_ = program_->uniformLocation(base + "lightPos");
	uniformLightColor_ = program_->uniformLocation(base + "lightColor");
	uniformLightDir_ = program_->uniformLocation(base + "lightDir");

	uniformCutOff_ = program_->uniformLocation(base + "cutOff");
	uniformOuterCutOff_ = program_->uniformLocation(base + "outerCutOff");
}

void DirectionalLight::setUniformValues(const QVector3D & lightPosition_,
										const QVector3D & lightColor_,
										float ambient_,
										float diffuse_,
										float specular_,
										const std::shared_ptr<QOpenGLShaderProgram> & program_)
{
	program_->setUniformValue(uniformAmbient_, ambient_);
	program_->setUniformValue(uniformDiffuse_, diffuse_);
	program_->setUniformValue(uniformSpecular_, specular_);

	program_->setUniformValue(uniformLightColor_, lightColor_.x(), lightColor_.y(), lightColor_.z());
	program_->setUniformValue(uniformLightPosition_, lightPosition_.x(), lightPosition_.y(), lightPosition_.z());
}

void ProjectionLight::setUniformValues(const QVector3D & lightPosition_,
									   const QVector3D & lightColor_,
									   const QVector3D & lightDir_,
									   float ambient_,
									   float diffuse_,
									   float specular_,
									   float cutOff_,
									   float outerCutOff_,
									   const std::shared_ptr<QOpenGLShaderProgram> & program_)
{
	program_->setUniformValue(uniformAmbient_, ambient_);
	program_->setUniformValue(uniformDiffuse_, diffuse_);
	program_->setUniformValue(uniformSpecular_, specular_);

	program_->setUniformValue(uniformLightColor_, lightColor_.x(), lightColor_.y(), lightColor_.z());
	program_->setUniformValue(uniformLightPosition_, lightPosition_.x(), lightPosition_.y(), lightPosition_.z());
	program_->setUniformValue(uniformLightDir_, lightDir_.x(), lightDir_.y(), lightDir_.z());

	program_->setUniformValue(uniformCutOff_, std::cos(qDegreesToRadians(cutOff_)));
	program_->setUniformValue(uniformOuterCutOff_, std::cos(qDegreesToRadians(outerCutOff_)));
}