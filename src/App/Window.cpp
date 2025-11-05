#include "Window.h"

#include <QMouseEvent>
#include <QLabel>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QVBoxLayout>
#include <QScreen>

#include <array>
#include <qmatrix4x4.h>

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <tinygltf/tiny_gltf.h>

namespace
{

const std::array<GLfloat, 12> vertices = {
    -5.f, -5.f,
    5.f, -5.f,
    5.f,  5.f,
    -5.f,  5.f,
};
const std::array<GLuint, 6> indices = {0, 1, 2, 2, 3, 0};

}// namespace


void Window::changeCameraPerspective(float width, float height) {
	const float aspect = float(height) / float(width);
    const float left = -1.0f;
    const float right = 1.0f;
    const float top = 1.0f * aspect;
    const float bottom = -1.0f * aspect;
	const float nearPlane = 0.1f;
    const float farPlane = 100.0f;
	camera->setToOrthographic(left, right, bottom, top, nearPlane, farPlane);
}

Window::Window() noexcept
{
	const auto formatFPS = [](const auto value) {
		return QString("FPS: %1").arg(QString::number(value));
	};

	auto fps = new QLabel(formatFPS(0), this);
	fps->setStyleSheet("QLabel { color : white; }");

	auto layout = new QVBoxLayout();
	layout->addWidget(fps, 1);

	setLayout(layout);

	timer_.start();

	connect(this, &Window::updateUI, [=] {
		fps->setText(formatFPS(ui_.fps));
	});
}

Window::~Window()
{
	{
		// Free resources with context bounded.
		const auto guard = bindContext();
		program_.reset();
		if(camera) delete camera;
	}
}

void Window::onInit()
{
	// Configure shaders
	program_ = std::make_unique<QOpenGLShaderProgram>(this);
	program_->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/Shaders/fractal.vs");
	program_->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment,
									  ":/Shaders/fractal.fs");
	program_->link();
	// Create VAO object
	vao_.create();
	vao_.bind();

	// Create VBO
	vbo_.create();
	vbo_.bind();
	vbo_.setUsagePattern(QOpenGLBuffer::StaticDraw);
	vbo_.allocate(vertices.data(), static_cast<int>(vertices.size() * sizeof(GLfloat)));

	// Create IBO
	ibo_.create();
	ibo_.bind();
	ibo_.setUsagePattern(QOpenGLBuffer::StaticDraw);
	ibo_.allocate(indices.data(), static_cast<int>(indices.size() * sizeof(GLuint)));

	// Bind attributes
	program_->bind();

	program_->enableAttributeArray(0);
	program_->setAttributeBuffer(0, GL_FLOAT, 0, 2, static_cast<int>(2 * sizeof(GLfloat)));

	// Uniform values for shader
	mvpUniform_ = program_->uniformLocation("mvp");

	// Camera
	camera = new Camera();
	changeCameraPerspective((float)width(), (float)height());
		
	// Release all
	program_->release();

	vao_.release();

	ibo_.release();
	vbo_.release();

	// Еnable depth test and face culling
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	// Clear all FBO buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Window::onRender()
{
	const auto guard = captureMetrics();

	// Clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Calculate MVP matrix
	model_.setToIdentity();
	model_.translate(0, 0, -2);
	const auto mvp = camera->getProjection() * camera->getView() * model_;
	// Bind VAO and shader program
	program_->bind();
	vao_.bind();
	// Update uniform value
	program_->setUniformValue(mvpUniform_, mvp);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

	// Release VAO and shader program
	vao_.release();
	program_->release();

	++frameCount_;

	// Request redraw if animated
	if (animated_)
	{
		update();
	}
}

void Window::onResize(const size_t width, const size_t height)
{
	// Configure viewport
	glViewport(0, 0, static_cast<GLint>(width), static_cast<GLint>(height));

	// Configure matrix
	if (camera)
	{
		changeCameraPerspective((float)width, (float)height);
	}
}

Window::PerfomanceMetricsGuard::PerfomanceMetricsGuard(std::function<void()> callback)
	: callback_{ std::move(callback) }
{
}

Window::PerfomanceMetricsGuard::~PerfomanceMetricsGuard()
{
	if (callback_)
	{
		callback_();
	}
}

auto Window::captureMetrics() -> PerfomanceMetricsGuard
{
	return PerfomanceMetricsGuard{
		[&] {
			if (timer_.elapsed() >= 1000)
			{
				const auto elapsedSeconds = static_cast<float>(timer_.restart()) / 1000.0f;
				ui_.fps = static_cast<size_t>(std::round(frameCount_ / elapsedSeconds));
				frameCount_ = 0;
				emit updateUI();
			}
		}
	};
}

void Window::mousePressEvent(QMouseEvent * event)
{
	if (camera && event->button() == Qt::LeftButton)
	{
		camera->startDrag((float)event->pos().x(), (float)event->pos().y());
	}
	fgl::GLWidget::mousePressEvent(event);
}

void Window::mouseMoveEvent(QMouseEvent * event)
{
	if (!camera)
		return;

	if (event->buttons() & Qt::LeftButton)
	{
		const qreal dpr = devicePixelRatio();
		camera->dragMove((float)event->pos().x(), (float)event->pos().y(), (float)QWidget::width(), (float)QWidget::height(), (float)dpr);
	}
	
	fgl::GLWidget::mouseMoveEvent(event);
}

void Window::mouseReleaseEvent(QMouseEvent* event) {
	if (camera && event->button() == Qt::RightButton) {
		camera->endDrag();
	}
	fgl::GLWidget::mouseReleaseEvent(event);
}

#if QT_CONFIG(wheelevent)
void Window::wheelEvent(QWheelEvent *event) {
    if (!camera) {
        fgl::GLWidget::wheelEvent(event);
        return;
    }
    float delta = (float)event->angleDelta().y() / 120.0f;
	const qreal dpr = devicePixelRatio();
    camera->zoom((float)event->position().x(), (float)event->position().y(), delta, (float)QWidget::width(), (float)QWidget::height(), dpr);
    fgl::GLWidget::wheelEvent(event);
}
#endif