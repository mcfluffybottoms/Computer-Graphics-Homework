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
	sceneRenderer->camera->setToPerspective(fov, aspect, nearPlane, farPlane);
}

void Window::setupSliders(QVBoxLayout * layout)
{
	slidersGroup_ = new SlidersGroup("Fractal Parameters");
	layout->addWidget(slidersGroup_, 1);
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
	sceneRenderer = new SceneRenderer(std::make_shared<OpenGLContext>(f));
	sceneRenderer->onInit(this);
	// Camera
	sceneRenderer->camera = new Camera();
	sceneRenderer->camera->position().setX(0.0415709);
	sceneRenderer->camera->position().setY(-0.0718041);
	sceneRenderer->camera->position().setZ(20);
	changeCameraPerspective((float)width(), (float)height());
}

void Window::onRender()
{
	const auto guard = captureMetrics();

	// Calculate MVP matrix
	// model_.setToIdentity();
	// model_.translate(0, 0, -2);
	const auto mvp = sceneRenderer->camera->getProjection() * sceneRenderer->camera->getView();

	sceneRenderer->onRender(size(), slidersGroup_, mvp);

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
	if (sceneRenderer->camera)
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

void Window::mousePressEvent(QMouseEvent * event)
{
	lastMousePos_ = {static_cast<float>(event->pos().x()), static_cast<float>(event->pos().y())};
}

void Window::mouseMoveEvent(QMouseEvent * event)
{
	if (sceneRenderer->camera && (event->buttons() & Qt::RightButton))
	{
		QVector2D delta = inputManager->processMouseInput(event, lastMousePos_);

		sceneRenderer->rotateObj(delta);
	}

	fgl::GLWidget::mouseMoveEvent(event);
}

#if QT_CONFIG(wheelevent)
void Window::wheelEvent(QWheelEvent * event)
{
	if (sceneRenderer->camera)
	{
		QVector3D result = inputManager->processMouseWheel(event);
		const qreal dpr = devicePixelRatio();
		sceneRenderer->camera->zoom(result, (float)QWidget::width(), (float)QWidget::height(), dpr);
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
		sceneRenderer->camera->move(moveBy);
	}
}