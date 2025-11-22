#include "InputManager.h"

QVector2D InputManager::processKeyboardInput(const QSet<int> & pressedKeys, float deltaTime) const {
    QVector2D moveBy{0, 0};
	float velocity = deltaTime;

	if (pressedKeys.contains(Qt::Key_W))
	{
		moveBy.setX(moveBy.x() + velocity);
	}
	if (pressedKeys.contains(Qt::Key_S))
	{
		moveBy.setX(moveBy.x() - velocity);
	}
	if (pressedKeys.contains(Qt::Key_A))
	{
		moveBy.setY(moveBy.y() - velocity);
	}
	if (pressedKeys.contains(Qt::Key_D))
	{
		moveBy.setY(moveBy.y() + velocity);
	}
	return moveBy;
}

QVector2D InputManager::processMouseInput(QMouseEvent * event) const {
    return {(float)event->pos().x(), (float)event->pos().y()};
}

QVector3D InputManager::processMouseWheel(QWheelEvent * event) const {
    float delta = (float)event->angleDelta().y() / 120.0f;
    return {(float)event->position().x(), (float)event->position().y(), delta};
}
