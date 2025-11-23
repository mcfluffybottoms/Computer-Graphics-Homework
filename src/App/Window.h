#pragma once

#include <Base/GLWidget.hpp>

#include <QElapsedTimer>
#include <QMatrix4x4>

#include "Camera.h"
#include "SliderGroup.h"
#include "InputManager.h"
#include "SceneRenderer.h"

#include <functional>
#include <qopenglcontext.h>

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
	void keyPressEvent(QKeyEvent * event) override;
	void keyReleaseEvent(QKeyEvent * event) override;
	void mousePressEvent(QMouseEvent * event) override;
	void mouseMoveEvent(QMouseEvent * event) override;
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
	// boxes
	void setupSliders(QVBoxLayout* layout);
    SlidersGroup* slidersGroup_ = nullptr;

	const QOpenGLContext* context;
	SceneRenderer* sceneRenderer;

	// navigation
	void changeCameraPerspective(float width, float height);
	void processKeyboardInput();
	QSet<int> keyboardInput;
	QMatrix4x4 model_;
	QVector2D lastMousePos_;

	InputManager* inputManager = nullptr;
	//ResourceManager* resourceManager = nullptr;

	// count fps
	QElapsedTimer timer_;
	size_t frameCount_ = 0;
	struct {
		size_t fps = 0;
	} ui_;
	bool animated_ = true;
};