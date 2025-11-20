#pragma once
#include "SliderGroup.h"
#include <QMatrix4x4>
#include <QSet>
#include <QVector4D>
#include <QVector3D>
#include <QVector2D>

enum VIEW_TYPE {
    ORTHOGRAPHIC,
    PERSPECTIVE
};

struct Camera {
    Camera();
    ~Camera() = default;

    void setToPerspective(float fov, float aspect, float nearPlane, float farPlane);

    // matrices
    void setProjection(const QMatrix4x4& p);
    void setToOrthographic(float left, float right, float bottom, float top, float nearPlane, float farPlane);
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
    // camera movement
    void zoom(float mousePosX, float mousePosY, float distance, float width, float height, float dpr);
    void startDrag(float mousePosX, float mousePosY);
    void dragMove(float mousePosX, float mousePosY, float width, float height, float dpr);
    void endDrag();

private:
    QVector3D getWorldCoord(float screenX, float screenY, float width, float height, float distance);
    void updateCameraVectors();
    void updateView();
    float yaw_{-90.0f};
	float pitch_{0.0f};
    float fov_{1.0f};
    float left_ = 0;
    float right_ = 0;
    float top_  = 0;
    float bottom_ = 0;
    float nearPlane_{0.1f};
    float farPlane_{100.0f};
    VIEW_TYPE view_type = PERSPECTIVE;

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