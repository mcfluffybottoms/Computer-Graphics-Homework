#include "Camera.h"
#include <QtCore>
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
	projection_.setToIdentity();
	fov_ = fov;
	nearPlane_ = nearPlane;
	farPlane_ = farPlane;
	projection_.perspective(fov, aspect, nearPlane, farPlane);
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

float & Camera::yaw()
{
	if (if_need_to_recalc_view)
	{
		updateCameraVectors();
	}
	return yaw_;
}

float & Camera::pitch()
{
	if (if_need_to_recalc_view)
	{
		updateCameraVectors();
	}
	return pitch_;
}

// proccessing movement
QVector3D Camera::getWorldCoord(float screenX, float screenY, float width, float height, float distance)
{
	float nx = 2.0f * screenX / width - 1.0f;
	float ny = 1.0f - 2.0f * screenY / height;
	float X_NDC;
	float Y_NDC;
	X_NDC = 2.0f * distance / projection_(0, 0);
	Y_NDC = 2.0f * distance / projection_(1, 1);
	float wx = nx * X_NDC + position_.x();
	float wy = ny * Y_NDC + position_.y();
	float wz = position_.z() - distance;
	return QVector3D(wx, wy, wz);
}

void Camera::zoom(const QVector3D & mouseData, float width, float height, float dpr)
{

	QVector3D before = getWorldCoord(mouseData.x(), mouseData.y(), width, height, mouseData.z());
	fov_ = fov_ * std::pow(1.1f, -mouseData.z());
	setToPerspective(fov_, width / height, nearPlane_, farPlane_);
	QVector3D after = getWorldCoord(mouseData.x(), mouseData.y(), width, height, mouseData.z());
	QVector3D delta = before - after;
	position_ += delta;
	updateCameraVectors();
}

void Camera::move(const QVector2D & moveBy)
{
	position_.setX(position_.x() + moveBy.x() * moveSpeed_);
	position_.setY(position_.y() + moveBy.y() * moveSpeed_);
	if_need_to_recalc_view = true;
}

void Camera::rotate(const QVector2D & rotateBy)
{
	yaw_ += rotateBy.x() * 0.2f;
	pitch_ += rotateBy.y() * 0.2f;
	pitch_ = std::clamp(pitch_, -89.0f, 89.0f);
	updateCameraVectors();
	updateView();
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