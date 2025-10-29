#pragma once
#include <QMatrix4x4>
#include <QSet>
#include <QVector4D>
#include <QVector3D>
#include <QVector2D>

struct Camera {
    Camera();
    ~Camera() = default;
    // set new projection
    void setToOthographic(float left, float right, float bottom, float top, float nearPlane, float farPlane);
    void setToPerspective(float fov, float aspect, float nearPlane, float farPlane);

    float& yaw();
    float& pitch();
    float& minPitch();
    float& maxPitch();
    float& mouseSensitivity();
    float& moveSpeed();
    float yaw() const;
    float pitch() const;
    float minPitch() const;
    float maxPitch() const;
    float mouseSensitivity() const;
    float moveSpeed() const;
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
    // camera movement
    void zoom(float distance);
    void startDrag(float mousePosX, float mousePosY);
    void dragMove(float mousePosX, float mousePosY);
    void endDrag();

private:
    QVector2D getWorldCoord(float x, float y, float width, float height) const;
    void updateCameraVectors();
    void updateView();
    float yaw_{-90.0f};
	float pitch_{0.0f};
    float max_pitch_{89.0f};
	float min_pitch_{-89.0f};

    float mouse_sensitivity_{0.01f};

	float move_speed_{0.01f};
    float max_move_speed_{1.0f};
    float zoom_speed_{0.5f};

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
    QVector2D lastMousePos_{0.0f, 0.0f};
    QVector3D dragStartPosition_{0.0f, 0.0f, 0.0f};
};