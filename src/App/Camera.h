#pragma once
#include "SliderGroup.h"
#include <QMatrix4x4>
#include <QSet>
#include <QVector4D>
#include <QVector3D>
#include <QVector2D>

struct Camera {
    Camera();
    ~Camera() = default;

    void setToPerspective(float fov, float aspect, float nearPlane, float farPlane);
    // matrices
    void setProjection(const QMatrix4x4& p);
    void setView(const QMatrix4x4& v);
    QMatrix4x4& getView();
    QMatrix4x4 getProjection() const;
    //vectors
    QVector3D& position();
    QVector3D& y();
    QVector3D& x();
    QVector3D& z();
    QVector3D position() const;
    QVector3D y() const;
    QVector3D x() const;
    QVector3D z() const;

    //yaw and pitch
    float& yaw();
    float& pitch();

    // camera movement
    void zoom(const QVector3D& mouseData, float width, float height, float dpr);
    void move(const QVector2D& moveBy);
    void rotate(const QVector2D& rotateBy);

private:
    QVector3D getWorldCoord(float screenX, float screenY, float width, float height, float distance);
    void updateCameraVectors();
    void updateView();
    float yaw_{-90.0f};
	float pitch_{0.0f};
    float fov_{1.0f};
    float nearPlane_{0.1f};
    float farPlane_{100.0f};

    // camera options 
    float moveSpeed_ = 3.0f;

    // position of a camera
    QVector3D position_{0.0f, 0.0f, 0.0f};
    
    // basis
    QVector3D y_{0.0f, 1.0f, 0.0f}; // up - axis y 
	QVector3D x_{1.0f, 0.0f, 0.0f}; // axis x 
	QVector3D z_{0.0f, 0.0f, -1.0f}; // axis z
    QVector3D worldUp_{0.0f, 1.0f, 0.0f};

    // 
    QMatrix4x4 projection_;
    QMatrix4x4 view_;
    bool if_need_to_recalc_view = true;

    // drag logic
    bool isDragging_ = false;
    QVector3D lastMousePos_{0.0f, 0.0f, 0.0f};
    QVector3D dragStartPosition_{0.0f, 0.0f, 0.0f};
};