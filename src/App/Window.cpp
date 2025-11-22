#include "Window.h"

#include <QLabel>
#include <QMouseEvent>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QScreen>
#include <QVBoxLayout>

#include <iostream>

#include <array>
#include <qboxlayout.h>
#include <qmatrix4x4.h>

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <tinygltf/tiny_gltf.h>

namespace
{

const std::array<GLfloat, 12> vertices = {
	-5.f,
	-5.f,
	5.f,
	-5.f,
	5.f,
	5.f,
	-5.f,
	5.f,
};
const std::array<GLuint, 6> indices = {0, 1, 2, 2, 3, 0};

}// namespace

void Window::changeCameraPerspective(float width, float height)
{
	const float aspect = float(height) / float(width);
	const float fov = -1.0f;
	const float nearPlane = 0.1f;
	const float farPlane = 100.0f;
	camera->setToPerspective(fov, aspect, nearPlane, farPlane);
}

void Window::setupSliders(QVBoxLayout * layout)
{
	slidersGroup_ = new SlidersGroup("Fractal Parameters");
	layout->addWidget(slidersGroup_, 1);
}

void Window::initUniformValues()
{
	// Uniform values for shader
	mvpUniform_ = program_->uniformLocation("mvp");
}

void Window::setUniformValues(const QMatrix4x4& mvp)
{
	program_->setUniformValue(mvpUniform_, mvp);
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
	setupSliders(layout);
	setLayout(layout);

	timer_.start();

	connect(this, &Window::updateUI, [=] {
		fps->setText(formatFPS(ui_.fps));
	});

	setFixedWidth(1000);
	setFixedHeight(1000);

	// InputManaager
	inputManager = new InputManager();

	// ResourceManager
	resourceManager = new ResourceManager();
}

Window::~Window()
{
	{
		if (camera)
			delete camera;
		if (inputManager)
			delete inputManager;
		if (resourceManager)
			delete resourceManager;
		// Free resources with context bounded.
		const auto guard = bindContext();
		program_.reset();
	}
}

void Window::onInit()
{
	// Configure shaders
	program_ = std::make_unique<QOpenGLShaderProgram>(this);
	program_->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/Shaders/diffuse.vs");
	program_->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment,
											   ":/Shaders/diffuse.fs");
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
	program_->setAttributeBuffer(0, GL_FLOAT, 0, 2, static_cast<int>(7 * sizeof(GLfloat)));

	program_->enableAttributeArray(1);
	program_->setAttributeBuffer(1, GL_FLOAT, static_cast<int>(2 * sizeof(GLfloat)), 3,
								 static_cast<int>(7 * sizeof(GLfloat)));

	program_->enableAttributeArray(2);
	program_->setAttributeBuffer(2, GL_FLOAT, static_cast<int>(5 * sizeof(GLfloat)), 2,
								 static_cast<int>(7 * sizeof(GLfloat)));

	// Uniform values for shader
	initUniformValues();

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
	setUniformValues(mvp);

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

	processKeyboardInput();
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
	: callback_{std::move(callback)}
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
		}};
}

void Window::mouseMoveEvent(QMouseEvent * event)
{
	if (camera && event->buttons() && Qt::LeftButton)
	{
		QVector2D result = inputManager->processMouseInput(event);
		camera->rotate(result);
	}

	fgl::GLWidget::mouseMoveEvent(event);
}

#if QT_CONFIG(wheelevent)
void Window::wheelEvent(QWheelEvent * event)
{
	if (camera)
	{
		QVector3D result = inputManager->processMouseWheel(event);
		const qreal dpr = devicePixelRatio();
		camera->zoom(result, (float)QWidget::width(), (float)QWidget::height(), dpr);
	}
	fgl::GLWidget::wheelEvent(event);
}
#endif

void Window::keyPressEvent(QKeyEvent * event) {
	keyboardInput.insert(event->key());
	fgl::GLWidget::keyReleaseEvent(event);
}
void Window::keyReleaseEvent(QKeyEvent * event) {
	keyboardInput.remove(event->key());
	fgl::GLWidget::keyReleaseEvent(event);
}

void Window::processKeyboardInput() {
	if(!keyboardInput.empty()) {
		QVector2D moveBy = inputManager->processKeyboardInput(keyboardInput, 0.001);
		camera->move(moveBy);
	}
}