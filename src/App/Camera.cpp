#include "Camera.h"
#include <cmath>
#include <qvector4d.h>


#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
inline float degreesToRadians(float degrees)
{
	return degrees * static_cast<float>(M_PI) / 180.0f;
}

// constructors
Camera::Camera()
{
	updateCameraVectors();
}

void Camera::setToPerspective(float fov, float aspect, float nearPlane, float farPlane)
{
	view_type = VIEW_TYPE::PERSPECTIVE;
	projection_.setToIdentity();
	fov_ = fov;
	nearPlane_ = nearPlane;
	farPlane_ = farPlane;
	projection_.perspective(fov, aspect, nearPlane, farPlane);
}

void Camera::setToOrthographic(float left, float right, float bottom, float top, float nearPlane, float farPlane)
{
	view_type = VIEW_TYPE::ORTHOGRAPHIC;
	projection_.setToIdentity();
	left_ = left;
	right_ = right;
	top_ = top;
	bottom_ = bottom;
	nearPlane_ = nearPlane;
	farPlane_ = farPlane;
	projection_.ortho(left, right, bottom, top, nearPlane, farPlane);
}

void Camera::setProjection(const QMatrix4x4 & p)
{
	if_need_to_recalc_view = true;
	projection_ = p;
}

void Camera::setView(const QMatrix4x4 & v)
{
	view_ = v;
}

QMatrix4x4 & Camera::getView()
{
	if (if_need_to_recalc_view)
	{
		updateView();
	}
	return view_;
}

QMatrix4x4 Camera::getProjection() const
{
	return projection_;
}

QVector3D & Camera::position()
{
	if_need_to_recalc_view = true;
	return position_;
}

QVector3D & Camera::y()
{
	if_need_to_recalc_view = true;
	return y_;
}

QVector3D & Camera::x()
{
	if_need_to_recalc_view = true;
	return x_;
}

QVector3D & Camera::z()
{
	if_need_to_recalc_view = true;
	return z_;
}

QVector3D Camera::position() const
{
	return position_;
}

QVector3D Camera::y() const
{
	return y_;
}

QVector3D Camera::x() const
{
	return x_;
}

QVector3D Camera::z() const
{
	return z_;
}

// proccessing movement
QVector3D Camera::getWorldCoord(float screenX, float screenY, float width, float height, float distance)
{
	float nx = 2.0f * screenX / width - 1.0f;
	float ny = 1.0f - 2.0f * screenY / height;
	float X_NDC;
	float Y_NDC;
	if (view_type == VIEW_TYPE::PERSPECTIVE)
	{
		X_NDC = 2.0f * distance / projection_(0, 0);
		Y_NDC = 2.0f * distance / projection_(1, 1);
		float wx = nx * X_NDC + position_.x();
		float wy = ny * Y_NDC + position_.y();
		float wz = position_.z() - distance;
		return QVector3D(wx, wy, wz);
	}
	else
	{
		float right = 2.0f / projection_(0, 0);
		float top = 2.0f / projection_(1, 1);
		float vx = nx * (right / 2.0f);
		float vy = ny * (top / 2.0f);
		float vz = -distance;
		QMatrix4x4 viewMatrix = getView();
		QMatrix4x4 inverseView = viewMatrix.inverted();
		return inverseView * QVector3D(vx, vy, vz);
	}
}

void Camera::zoom(float mousePosX, float mousePosY, float distance, float width, float height, float dpr)
{
	QVector3D before = getWorldCoord(mousePosX, mousePosY, width, height, distance);
	if (view_type == VIEW_TYPE::PERSPECTIVE)
	{
		fov_ = fov_ * std::pow(1.1f, -distance);
		setToPerspective(fov_, width / height, nearPlane_, farPlane_);
	}
	else
	{
		float zoomFactor = std::pow(1.1f, -distance);
		left_ *= zoomFactor;
		right_ *= zoomFactor;
		bottom_ *= zoomFactor;
		top_ *= zoomFactor;
		setToPerspective(fov_, width / height, nearPlane_, farPlane_);
		setToOrthographic(left_, right_, bottom_, top_, nearPlane_, farPlane_);
	}
	QVector3D after = getWorldCoord(mousePosX, mousePosY, width, height, distance);
	QVector3D delta = before - after;
	position_ += delta;
	updateCameraVectors();
}

void Camera::startDrag(float mousePosX, float mousePosY)
{
	isDragging_ = true;
	lastMousePos_ = QVector2D(mousePosX, mousePosY);
	dragStartPosition_ = position_;
}

void Camera::dragMove(float mousePosX, float mousePosY, float width, float height, float dpr)
{
	if (!isDragging_)
		return;
	QVector3D currentWorld = getWorldCoord(mousePosX, mousePosY, width, height, 1);
	QVector3D startWorld = getWorldCoord(lastMousePos_.x(), lastMousePos_.y(), width, height, 1);
	QVector3D delta = startWorld - currentWorld;
	position_ = dragStartPosition_ + dpr * delta;
	updateCameraVectors();
}

void Camera::endDrag()
{
	isDragging_ = false;
}

void Camera::updateCameraVectors()
{
	QVector3D forward;
	forward.setX(cos(degreesToRadians(yaw_)) * cos(degreesToRadians(pitch_)));
	forward.setY(sin(degreesToRadians(pitch_)));
	forward.setZ(sin(degreesToRadians(yaw_)) * cos(degreesToRadians(pitch_)));
	y_ = forward.normalized();
	x_ = QVector3D::crossProduct(y_, worldUp_).normalized();
	z_ = QVector3D::crossProduct(x_, y_).normalized();
	if_need_to_recalc_view = true;
}

void Camera::updateView()
{
	view_.setToIdentity();
	view_.lookAt(position_, position_ + y_, z_);
}