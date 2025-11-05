#pragma once

#include <Base/GLWidget.hpp>

#include <GL/gl.h>
#include <QElapsedTimer>
#include <QMatrix4x4>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLVertexArrayObject>

#include "Camera.h"

#include <functional>
#include <memory>

class Window final : public fgl::GLWidget
{
	Q_OBJECT
public:
	Window() noexcept;
	~Window() override;

public: // fgl::GLWidget
	void onInit() override;
	void onRender() override;
	void onResize(size_t width, size_t height) override;

protected:
	void mousePressEvent(QMouseEvent * event) override;
	void mouseMoveEvent(QMouseEvent * event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;
	#if QT_CONFIG(wheelevent)
		void wheelEvent(QWheelEvent *event) override;
	#endif
private:
	class PerfomanceMetricsGuard final
	{
	public:
		explicit PerfomanceMetricsGuard(std::function<void()> callback);
		~PerfomanceMetricsGuard();

		PerfomanceMetricsGuard(const PerfomanceMetricsGuard &) = delete;
		PerfomanceMetricsGuard(PerfomanceMetricsGuard &&) = delete;

		PerfomanceMetricsGuard & operator=(const PerfomanceMetricsGuard &) = delete;
		PerfomanceMetricsGuard & operator=(PerfomanceMetricsGuard &&) = delete;

	private:
		std::function<void()> callback_;
	};

private:
	[[nodiscard]] PerfomanceMetricsGuard captureMetrics();

signals:
	void updateUI();

private:
	void changeCameraPerspective(float width, float height);

	GLint mvpUniform_ = -1;
	GLfloat widthUniform_ = 1000.0f;
	GLfloat heightUniform_ = 1000.0f;
    GLfloat fromXUniform_ = -2.5;
    GLfloat fromYUniform_ = -1.5;
    GLfloat sizeXUniform_ = 1000.0;
    GLfloat sizeYUniform_ = 1000.0;
	GLint maxItersUniform_ = 100;

	QOpenGLBuffer vbo_{QOpenGLBuffer::Type::VertexBuffer};
	QOpenGLBuffer ibo_{QOpenGLBuffer::Type::IndexBuffer};
	QOpenGLVertexArrayObject vao_;
	std::unique_ptr<QOpenGLShaderProgram> program_;

	// navigation
	QMatrix4x4 model_;
	Camera* camera = nullptr;

	// count fps
	QElapsedTimer timer_;
	size_t frameCount_ = 0;
	struct {
		size_t fps = 0;
	} ui_;
	bool animated_ = true;
};
