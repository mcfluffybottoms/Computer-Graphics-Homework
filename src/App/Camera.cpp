#include "Camera.h"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
inline float degreesToRadians(float degrees)
{
	return degrees * static_cast<float>(M_PI) / 180.0f;
}

// constructors
Camera::Camera() {
    updateCameraVectors();
}
void Camera::setToOthographic(float left, float right, float bottom, float top, float nearPlane, float farPlane) {
    projection_.setToIdentity();
    projection_.ortho(left, right, bottom, top, nearPlane, farPlane);
}
void Camera::setToPerspective(float fov, float aspect, float nearPlane, float farPlane) {
    projection_.setToIdentity();
	projection_.perspective(fov, aspect, nearPlane, farPlane);
}
// getters and setters
float& Camera::yaw() {
    if_need_to_recalc_view = true;
    return yaw_;
}
float& Camera::pitch() {
    if_need_to_recalc_view = true;
    return pitch_;
}
float& Camera::minPitch() {
    if_need_to_recalc_view = true;
    return min_pitch_;
}
float& Camera::maxPitch() {
    if_need_to_recalc_view = true;
    return max_pitch_;
}
float& Camera::mouseSensitivity() {
    if_need_to_recalc_view = true;
    return mouse_sensitivity_;
}
float& Camera::moveSpeed() {
    if_need_to_recalc_view = true;
    return move_speed_;
}
float Camera::yaw() const {
    return yaw_;
}
float Camera::pitch() const {
    return pitch_;
}
float Camera::minPitch() const {
    return min_pitch_;
}
float Camera::maxPitch() const {
    return max_pitch_;
}
float Camera::mouseSensitivity() const {
    return mouse_sensitivity_;
}
float Camera::moveSpeed() const {
    return move_speed_;
}
void Camera::setProjection(const QMatrix4x4& p) {
    if_need_to_recalc_view = true;
    projection_ = p;
}
void Camera::setView(const QMatrix4x4& v) {
    view_ = v;
}
QMatrix4x4& Camera::getView() {
    if(if_need_to_recalc_view) {
        updateView();
    }
    return view_;
}
QMatrix4x4 Camera::getProjection() const {
    return projection_;
}
QVector3D& Camera::position() {
    if_need_to_recalc_view = true;
    return position_;
}
QVector3D& Camera::y() {
    if_need_to_recalc_view = true;
    return y_;
}
QVector3D& Camera::x() {
    if_need_to_recalc_view = true;
    return x_;
}
QVector3D& Camera::z() {
    if_need_to_recalc_view = true;
    return z_;
}
QVector3D Camera::position() const {
    return position_;
}
QVector3D Camera::y() const {
    return y_;
}
QVector3D Camera::x() const {
    return x_;
}
QVector3D Camera::z() const {
    return z_;
}
// proccessing movement
QVector2D Camera::getWorldCoord(float screenX, float screenY, float width, float height) const {
    float x = (2.0f * screenX) / width - 1.0f;
    float y = 1.0f - (2.0f * screenY) / height;
    QVector4D screenPos(x, y, 0.0f, 1.0f);
    QMatrix4x4 inv = (projection_ * view_).inverted();
    QVector4D worldPos = inv * screenPos;
    if (worldPos.w() != 0.0f) {
        worldPos /= worldPos.w();
    }
    
    return QVector2D(worldPos.x(), worldPos.y());
}
void Camera::zoom(float distance) {
    position_ += y_ * distance * zoom_speed_;
    updateCameraVectors();
}
void Camera::startDrag(float mousePosX, float mousePosY) {
    isDragging_ = true;
    lastMousePos_ = getWorldCoord(mousePosX, mousePosY, 1.0f, 1.0f);
    dragStartPosition_ = position_;
}
void Camera::dragMove(float mousePosX, float mousePosY) {
    if (!isDragging_)
        return;

    QVector2D currWorld = getWorldCoord(mousePosX, mousePosY, 1.0f, 1.0f);
    QVector2D delta = (lastMousePos_ - currWorld) * mouse_sensitivity_;
    float deltaLength = delta.length();
    if (deltaLength > max_move_speed_) {
        delta = delta.normalized() * max_move_speed_;
    }
    position_ += QVector3D(delta, 0.0f);
    updateCameraVectors();
    lastMousePos_ = currWorld;
}
void Camera::endDrag() {
    isDragging_ = false;
}
void Camera::updateCameraVectors() {
    QVector3D y;
	y.setX(cos(degreesToRadians(yaw_)) * cos(degreesToRadians(pitch_)));
	y.setY(sin(degreesToRadians(pitch_)));
	y.setZ(sin(degreesToRadians(yaw_)) * cos(degreesToRadians(pitch_)));
	y_ = y.normalized();

	x_ = QVector3D::crossProduct(y_, worldUp_).normalized();
	z_ = QVector3D::crossProduct(x_, y_).normalized();

	if_need_to_recalc_view = true;
}
void Camera::updateView() {
	view_.setToIdentity();
	view_.lookAt(position_, position_ + y_, z_);
}