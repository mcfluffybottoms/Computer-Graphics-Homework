#pragma once
#include <QMouseEvent>
#include <QVector2D>
#include <QVector3D>
#include <QWheelEvent>
#include <qvector3d.h>

struct InputManager {
	QVector2D processKeyboardInput(const QSet<int> & pressedKeys, float deltaTime) const;
	QVector2D processMouseInput(QMouseEvent * event, const QVector2D & lastMousePos_) const;
	QVector3D processMouseWheel(QWheelEvent * event) const;
};