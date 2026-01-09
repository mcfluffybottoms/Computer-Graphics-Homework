#include "Window.h"
#include "App/SceneRenderer.h"
#include "App/SliderGroup.h"

#include "Camera.h"

#include <QLabel>
#include <QMouseEvent>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QScreen>
#include <QVBoxLayout>
#include <iostream>
#include <QMatrix4x4>
#include <QOpenGLContext>
#include <type_traits>

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <tinygltf/tiny_gltf.h>

void Window::changeCameraPerspective(float width, float height)
{
	const float aspect = float(width)/float(height) ;
	const float fov = 60.0f;
	const float nearPlane = 0.1f;
	const float farPlane = 100.0f;
	sceneRenderer->camera_->setToPerspective(fov, aspect, nearPlane, farPlane);
}

void Window::setupSliders(QVBoxLayout * layout)
{
	slidersGroup_ = new SlidersGroup("Scene Parameters", data);
	layout->addWidget(slidersGroup_, 1);
}

Window::Window() noexcept
{
	// SharedData
	data = std::make_shared<SharedData>();
	// InputManaager
	inputManager = new InputManager();

	// Set Layout
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

}

Window::~Window()
{
	{
		if (inputManager)
			delete inputManager;
		// Free resources with context bounded.
		const auto guard = bindContext();
	}
}

void Window::onInit()
{
	// Configure shaders
	auto * f = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_3_Core>();
	if (f)
	{
		f->initializeOpenGLFunctions();
	}
	else
	{
		qWarning() << "No functions() on context_!";
	}
	data->size = this->size();
	sceneRenderer = new SceneRenderer(data, std::make_shared<OpenGLContext>(f));
	sceneRenderer->onInit(this);
	
	// Camera
	sceneRenderer->camera_ = new Camera();
	sceneRenderer->camera_->position().setX(0.0415709);
	sceneRenderer->camera_->position().setY(-0.0718041);
	sceneRenderer->camera_->position().setZ(20);
	changeCameraPerspective((float)width(), (float)height());
}

void Window::onRender()
{
	GLint defaultFramebuffer;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &defaultFramebuffer);
	
	const auto guard = captureMetrics();

	sceneRenderer->onRender(defaultFramebuffer);

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
	sceneRenderer->onResize(this);

	// Configure matrix
	if (sceneRenderer->camera_)
	{
		changeCameraPerspective((float)width, (float)height);
	}
	data->size = size();
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

void Window::mousePressEvent(QMouseEvent * event)
{
	lastMousePos_ = {static_cast<float>(event->pos().x()), static_cast<float>(event->pos().y())};
}

void Window::mouseMoveEvent(QMouseEvent * event)
{
	if (sceneRenderer->camera_ && (event->buttons() & Qt::RightButton))
	{
		QVector2D delta = inputManager->processMouseInput(event, lastMousePos_);

		sceneRenderer->rotateObj(delta);
	}

	fgl::GLWidget::mouseMoveEvent(event);
}

#if QT_CONFIG(wheelevent)
void Window::wheelEvent(QWheelEvent * event)
{
	if (sceneRenderer->camera_)
	{
		QVector3D result = inputManager->processMouseWheel(event);
		const qreal dpr = devicePixelRatio();
		sceneRenderer->camera_->zoom(result, (float)QWidget::width(), (float)QWidget::height(), dpr);
	}
	fgl::GLWidget::wheelEvent(event);
}
#endif

void Window::keyPressEvent(QKeyEvent * event)
{
	keyboardInput.insert(event->key());
	fgl::GLWidget::keyReleaseEvent(event);
}

void Window::keyReleaseEvent(QKeyEvent * event)
{
	keyboardInput.remove(event->key());
	fgl::GLWidget::keyReleaseEvent(event);
}

void Window::processKeyboardInput()
{
	if (!keyboardInput.empty())
	{
		QVector2D moveBy = inputManager->processKeyboardInput(keyboardInput, 0.001);
		sceneRenderer->camera_->move(moveBy);
	}
}