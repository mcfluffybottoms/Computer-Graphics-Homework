#pragma once

#include <QOpenGLShaderProgram>
#include <qopenglext.h>

struct ShaderManager {
    ShaderManager();
    ~ShaderManager();

    void initUniformValues();
    void initGeometryUniformValues();
	void initAOUniformValues();
	void initBlurUniformValues();
	void initLightUniformValues();


    void bindGeometryAttributes();

    void setGeometryUniformValues(const QMatrix4x4& mvp, const QMatrix4x4& transform);
	void setAOUniformValues(const std::array<QVector3D, 64> & kernel, const QMatrix4x4& projMatrix, float tanHalfFOV, float aspectRatio);
	void setBlurUniformValues();
	void setLightUniformValues(bool dir, bool proj, const QMatrix4x4& mvp, const QMatrix4x4& transform, const QSize& size);

    // shader setup
	std::shared_ptr<QOpenGLShaderProgram> m_geometry_program_;
	std::shared_ptr<QOpenGLShaderProgram> m_ao_program_;
	std::shared_ptr<QOpenGLShaderProgram> m_blur_program_;
	std::shared_ptr<QOpenGLShaderProgram> m_lighting_program_;
private:    
    // uniform values for geometry pass
    GLint mvpUniform_ = -1;
	GLint modelUniform_ = -1;
	GLint invertedTransposedMatrixUniform_ = -1;

    // uniform values for ssao
    GLuint kernelUniform_;
    GLint projMatrixUniform_ = -1;
    GLuint aspectUniform_;
    GLint tanHalfFOVUniform_ = -1;

    // uniform values for blur


    // uniform for lighting
    GLboolean hasDirectionalUniform_ = -1;
	GLboolean hasProjectionUniform_ = -1;
    GLint mvpLightUniform_ = -1;
	GLint modelLightUniform_ = -1;
	GLint invertedTransposedMatrixLightUniform_ = -1;

};