#include <QOpenGLShaderProgram>

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
	void setAOUniformValues();
	void setBlurUniformValues();
	void setLightUniformValues();

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


    // uniform values for blur


    // uniform for lighting
    //GLint uniformViewPos_ = 1.0f;
};