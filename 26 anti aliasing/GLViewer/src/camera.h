#ifndef CAMERA_H
#define CAMERA_H

#include <QWidget>
#include <QMatrix4x4>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QTimer>
#include <QTime>
#include <QSet>
#include <QMap>
#include <QOpenGLFunctions_4_5_Core>

#include <vector>

#include "ViewerSetting.h"
#include "ViewerUtils.h"

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

// Default camera values
const float YAW         = -90.0f;
const float PITCH       =  0.0f;
const float SPEED       = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM        =  45.0f;


// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
    // camera Attributes
    QVector3D Position;
    QVector3D Front;
    QVector3D Up;
    QVector3D Right;
    QVector3D WorldUp;
    // euler Angles
    float Yaw;
    float Pitch;
    // camera options
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

    bool (*isMouseLeftPress) ();

    QOpenGLFunctions_4_5_Core* glFunc;
    // rotate center of camera
    QVector3D center;

    // offset of camera postion to point of mouse movement
    QVector3D movementPt;
    QVector3D startPosition;

    //  rotate position of mouse
    QVector3D mousePos4Rotate;

    //
    int SCR_WIDTH;
    int SCR_HEIGHT;
    float lastX;
    float lastY;
    bool firstMouse;
    //bool firstMouse2;

    // timing
    float deltaTime;
    float lastFrame;

    // constructor with vectors
    Camera(QOpenGLFunctions_4_5_Core* glFc, QVector3D position = QVector3D(0.0f, 0.0f, 0.0f), QVector3D up = QVector3D(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH)
        : glFunc(glFc), Front(QVector3D(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        Init();
        updateCameraVectors();
    }
    // constructor with scalar values
    Camera(QOpenGLFunctions_4_5_Core* glFc, float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch)
        : glFunc(glFc), Front(QVector3D(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = QVector3D(posX, posY, posZ);
        WorldUp = QVector3D(upX, upY, upZ);
        Yaw = yaw;
        Pitch = pitch;
        Init();
        updateCameraVectors();
    }

    void Init()
    {
        QVector3D center = QVector3D(0.0f, 0.0f, 1.0f);

        //
        SCR_WIDTH = 800;
        SCR_HEIGHT = 600;
        lastX = SCR_WIDTH / 2.0f;
        lastY = SCR_HEIGHT / 2.0f;
        firstMouse = true;

        // timing
        deltaTime = 0.0f;
        lastFrame = 0.0f;
    }

    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    QMatrix4x4 GetViewMatrix()
    {
        QMatrix4x4 result;
        result.lookAt(Position, Position + Front, Up);
        return result;
    }

    QVector3D GetCameraPos4ViewCube()
    {
        return -1.6 * Front;
    }

    QMatrix4x4 GetViewMatrix4VieweCube()
    {
        QVector3D pos = GetCameraPos4ViewCube();

        QMatrix4x4 result;
        result.lookAt(pos, QVector3D(0.0, 0.0, 0.0), Up);
        return result;
    }

    QMatrix4x4 GetViewMatrix4MouseLabel()
    {
        QVector3D pos(0.0, 0.11, 0.0);

        QMatrix4x4 result;
        result.lookAt(pos, QVector3D(0.0, 0.0, 0.0), QVector3D(0.0, 0.0, 1.0));
        return result;
    }

    // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void ProcessKeyboard(Camera_Movement direction, float deltaTime)
    {
        float velocity = MovementSpeed * deltaTime;
        if (direction == FORWARD)
            Position += Front * velocity;
            //Position -= Up * velocity;
        if (direction == BACKWARD)
            Position -= Front * velocity;
            //Position += Up * velocity;
        if (direction == LEFT)
            Position -= Right * velocity;
        if (direction == RIGHT)
            Position += Right * velocity;
    }

    void ProcessKeyPress(QKeyEvent* event)
    {
        if (event->key() == Qt::Key::Key_W)
            ProcessKeyboard(FORWARD, deltaTime);
        if (event->key() == Qt::Key::Key_S)
            ProcessKeyboard(BACKWARD, deltaTime);
        if (event->key() == Qt::Key::Key_A)
            ProcessKeyboard(LEFT, deltaTime);
        if (event->key() == Qt::Key::Key_D)
            ProcessKeyboard(RIGHT, deltaTime);
    }

    bool handle(QEvent* event)
    {
        switch (event->type())
        {
            //鼠标移动
            case QEvent::MouseMove:
            {
                auto mouseEvent = static_cast<QMouseEvent*>(event);
                //handleMouseMoveEvent(mouseEvent);
                ProcessMousePress(mouseEvent);
                break;
            }
            //定时器
            //case QEvent::Timer:
            //{
            //    auto timerEvent = static_cast<QTimerEvent*>(event);
            //    handleTimerEvent(timerEvent);
            //    break;
            //}
            //滚轮
            case QEvent::Wheel:
            {
                auto wheelEvent = static_cast<QWheelEvent*>(event);
                //handleWheelEvent(wheelEvent);
                ProcessMouseScroll(wheelEvent->angleDelta().y() > 0 ? 1 : -1);
                break;
            }
            case QEvent::MouseButtonRelease:
            {
                auto mouseEvent = static_cast<QMouseEvent*>(event);
                ProcessMouseRelease(mouseEvent);
                break;
            }
            //按键按下
            case QEvent::KeyPress:
            {
                auto keyPressEvent = static_cast<QKeyEvent*>(event);
                ProcessKeyPress(keyPressEvent);
                //m_pressedKeys.insert(keyPressEvent->key());
                ////启动定时器
                //if (!keyPressEvent->isAutoRepeat() && m_timerId == 0) {
                //    m_timerId = m_widget->startTimer(1);
                //}
                break;
            }
            default:
                return false;
        }

        return true;
    }

    void ProcessMouseRelease(QMouseEvent* e)
    {
        if (e->button() == Qt::LeftButton)    // Left button...
        {
            firstMouse = true;
        }
        else if (e->button() == Qt::RightButton)
        {
            firstMouse = true;
        }
    }

    // screenp: 通过mousePressEvent获得的当前鼠标点击的屏幕点
    float GetDepth(QPoint screenp) {
        //MyWidget宽、高
        double w = SCR_WIDTH;
        double h = SCR_HEIGHT;
        // 待读取的深度值
        float zpos = 0.0;
        // qopenglwidget内部的opengl不随qt缩放，读取像素需手动从qt屏幕坐标转成opengl坐标
        // opengl中y方向0点和qt屏幕中y方向0点不同
        glFunc->glReadPixels(screenp.x() * ViewerSetting::devicePixelRatio /** device_ratio*/, (h - screenp.y()) * ViewerSetting::devicePixelRatio/* * device_ratio*/, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &zpos);
        //zpos = zpos * 2 - 1.0;
        return zpos;
    }

    void ProcessMouseLeftButtonPress(QMouseEvent* event)
    {
        QPoint screenp = event->pos();
        float xpos = screenp.x();
        float ypos = screenp.y();

        if (firstMouse)
        {
            lastX = xpos;
            lastY = ypos;

            float centerZ = GetDepth(screenp);
            center.setZ(centerZ);

            if (abs(center.z() - 1.0f) < 1e-9)
                center = { 0.f, 0.f, 0.f };
            else
            {
                QMatrix4x4 project;
                project.perspective(this->Zoom, (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

                centerZ = center.z();
                QVector4D wcsPt = Screen2Scene({ xpos, SCR_HEIGHT - ypos }, { 0, 0, SCR_WIDTH, SCR_HEIGHT }, this->GetViewMatrix(),
                    project, &centerZ);

                center = wcsPt.toVector3D();
            }

            mousePos4Rotate = center;
            firstMouse = false;
        }

        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

        lastX = xpos;
        lastY = ypos;

        ProcessMouseMovement3(xoffset, yoffset, center);
    }

    //void GetWorldPos(float xpos, float ypos, QVector3D& result)
    //{
    //    QMatrix4x4 project;
    //    project.perspective(qDegreesToRadians(this->Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

    //    float centerZ = 0.0;
    //    QVector4D wcsPt = Screen2Scene({ xpos, SCR_HEIGHT - ypos/*ypos*/ }, { 0, 0, SCR_WIDTH, SCR_HEIGHT }, this->GetViewMatrix(),
    //        project, &centerZ);

    //    result = wcsPt.toVector3D();
    //}

    void ProcessMouseRightButtonPress(QMouseEvent* event)
    {
        float xpos = event->x();
        float ypos = event->y();

        QVector3D pt(xpos / SCR_WIDTH, ypos / SCR_HEIGHT, 0.0f);
        if (firstMouse)
        {
            movementPt = pt;//pt - Position;
            startPosition = Position;
            firstMouse = false;
        }

        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

        lastX = xpos;
        lastY = ypos;

        ProcessMouseRightButtonPressMovement(pt);
    }

    void ProcessMousePress(QMouseEvent* event)
    {
        if (event->buttons() & Qt::LeftButton)
        {
            ProcessMouseLeftButtonPress(event);
            return;
        }

        if (event->buttons() & Qt::RightButton)
        {
            ProcessMouseRightButtonPress(event);
            return;
        }
    }

    void ProcessMouseRightButtonPressMovement(const QVector3D& curPos)
    {
        auto off = 5.0 * (curPos - movementPt);
        Position = startPosition - off.x() * Right + off.y() * Up;
    }

    // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void ProcessMouseMovement3(float xoffset, float yoffset, QVector3D center, bool constrainPitch = true)
    {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        Yaw += xoffset;
        Pitch += yoffset;

        // make sure that when pitch is out of bounds, screen doesn't get flipped
        if (constrainPitch)
        {
            if (Pitch > 89.0f)
                Pitch = 89.0f;
            if (Pitch < -89.0f)
                Pitch = -89.0f;
        }

        QVector3D rPt(Position.x() - center.x(), Position.y() - center.y(), Position.z() - center.z());

        //  计算position在right、front、up坐标系下的局部坐标
        float lX = QVector3D::dotProduct(rPt, Right);
        float lY = QVector3D::dotProduct(rPt, Front);
        float lZ = QVector3D::dotProduct(rPt, Up);

        // update Front, Right and Up Vectors using the updated Euler angles
        updateCameraVectors();

        Position = lX * Right + lY * Front + lZ * Up;
        Position.setX(Position.x() + center.x());
        Position.setY(Position.y() + center.y());
        Position.setZ(Position.z() + center.z());
    }

    // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void ProcessMouseScroll(float yoffset)
    {
        float downValue = 1.0f;
        float upValue = 89.f/*45.0f*/;
        Zoom -= (float)yoffset;
        if (Zoom < downValue)
            Zoom = downValue;
        if (Zoom > upValue)
            Zoom = upValue;
    }

    void FitView(int flag)
    {
        if (flag < 0 || flag > 5)
            return;

        float depth = 10.0f;
        if (flag == 0)
        {
            Front = QVector3D(0.0, 1.0, 0.0);
            Position = QVector3D(0.0, -depth, 0.0);

            Vector3f paraAxis = ViewerUtils::normalizeToAxis(Vector3f(Right.x(), Right.y(), Right.z()), Vector3f::BasicX, Vector3f::BasicY, Vector3f::BasicZ);
            Right = QVector3D(paraAxis.X, paraAxis.Y, paraAxis.Z);
            Up = QVector3D::crossProduct(Right, Front);

            Yaw = Vector3f::BasicZ.AngleOnPlaneTo(Vector3f(Right.x(), Right.y(), Right.z()), Vector3f(Front.x(), Front.y(), Front.z())) / PI * 180.0 - 360.0;
            //Yaw = -90.f;
            Pitch = 90.f;
        }
        else if (flag == 1)
        {
            Front = QVector3D(0.0, 0.0, -1.0);
            Right = QVector3D(1.0, 0.0, 0.0);
            Up = QVector3D(0.0, 1.0, 0.0);
            Position = QVector3D(0.0, 0.0, depth);
            Yaw = -90.f;
            Pitch = 0.f;
        }
        else if (flag == 2)
        {
            Front = QVector3D(-1.0, 0.0, 0.0);
            Right = QVector3D(0.0, 0.0, -1.0);
            Up = QVector3D(0.0, 1.0, 0.0);
            Position = QVector3D(depth, 0.0, 0.0);
            Yaw = -180.f;
            Pitch = 0.f;
        }
        else if (flag == 3)
        {
            Front = QVector3D(0.0, 0.0, 1.0);
            Right = QVector3D(-1.0, 0.0, 0.0);
            Up = QVector3D(0.0, 1.0, 0.0);
            Position = QVector3D(0.0, 0.0, -depth);
            Yaw = -270.f;
            Pitch = 0.f;
        }
        else if (flag == 4)
        {
            Front = QVector3D(1.0, 0.0, 0.0);
            Right = QVector3D(0.0, 0.0, 1.0);
            Up = QVector3D(0.0, 1.0, 0.0);
            Position = QVector3D(-depth, 0.0, 0.0);
            Yaw = 0.f;
            Pitch = 0.f;
        }
        else if (flag == 5)
        {
            Front = QVector3D(0.0, -1.0, 0.0);
            Position = QVector3D(0.0, depth, 0.0);

            Vector3f paraAxis = ViewerUtils::normalizeToAxis(Vector3f(Right.x(), Right.y(), Right.z()), Vector3f::BasicX, Vector3f::BasicY, Vector3f::BasicZ);
            Right = QVector3D(paraAxis.X, paraAxis.Y, paraAxis.Z);
            Up = QVector3D::crossProduct(Right, Front);

            Yaw = Vector3f::BasicZ.AngleOnPlaneTo(Vector3f(Right.x(), Right.y(), Right.z()), Vector3f(Front.x(), Front.y(), Front.z())) / PI * 180.0 - 360.0;
            //Yaw = -90.f;
            Pitch = -90.f;
        }
        else
            return;

        firstMouse = true;
        Zoom = 45.0f;
    }

private:
    // calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors()
    {
        // calculate the new Front vector
        QVector3D front;
        front.setX(cos(qDegreesToRadians(Yaw)) * cos(qDegreesToRadians(Pitch)));
        front.setY(sin(qDegreesToRadians(Pitch)));
        front.setZ(sin(qDegreesToRadians(Yaw)) * cos(qDegreesToRadians(Pitch)));
        Front = front.normalized();
        // also re-calculate the Right and Up vector
        Right = QVector3D::crossProduct(Front, WorldUp).normalized();  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        Up = QVector3D::crossProduct(Right, Front).normalized();
    }

public:
    QVector4D Screen2Scene(
        const QVector2D& screenPoint,
        const QRect& viewportRange,
        const QMatrix4x4& modelMatrix,
        const QMatrix4x4& projectMatrix,
        float* pPointDepth = nullptr)
    {
        GLfloat pointDepth(0.0f);
        if (nullptr != pPointDepth)
        {
            pointDepth = *pPointDepth;
        }
        else
        {
            // 获取深度缓冲区中x,y的数值
            glFunc->glReadPixels(screenPoint.x(), screenPoint.y(), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &pointDepth);
        }

        //#if defined(USING_GLM_ALGORITHM)
                /// 直接使用glm的"逆投影"
        QVector3D pt(screenPoint, pointDepth);
        return QVector4D(pt.unproject(modelMatrix, projectMatrix, viewportRange), 1.0);
        //return QVector4D(glm::unProject(QVector3D(screenPoint, pointDepth), modelMatrix, projectMatrix, viewportRange), 1.0);
        //#else
        //        // 转换为标准设备坐标系
        //        glm::vec4 devicePoint(
        //            LinearMap(screenPoint.x, viewportRange.x, viewportRange.z, -1.0f, 1.0f),
        //            LinearMap(screenPoint.y, viewportRange.y, viewportRange.w, 1.0f, -1.0f),
        //            pointDepth, 1);
        //
        //        // 按照opengl管线工作方式，转换到世界坐标系
        //        return glm::inverse(projectMatrix * modelMatrix) * devicePoint;
        //#endif

    }

    /**
     * @brief 世界坐标系转换为屏幕坐标系
     * @brief worldPoint		世界坐标的点坐标点
     * @brief viewportRange 	视口范围。 各个值依次为：左上-右下
     * @brief modelViewMatrix 	模型视图矩阵
     * @brief projectMatrix 	投影矩阵
     * @brief pPointDepth   	屏幕点的深度，如果不指定(为nullptr),从深度缓冲区中读取深度值
     * @return 					窗口坐标点
     * @note 返回的窗口坐标带深度值，如果仅适用2D窗口像素坐标点，仅适用它的x,y维即可
     */
    QVector3D Scene2Screen(
        const QVector3D& worldPoint,
        const QRect& viewportRange,
        const QMatrix4x4& modelViewMatrix,
        const QMatrix4x4& projectMatrix)
    {
        //#if defined(USING_GLM_ALGORITHM)
                /// 直接使用glm的"投影"
        return worldPoint.project(modelViewMatrix, projectMatrix, viewportRange);
        //return glm::project(worldPoint, modelViewMatrix, projectMatrix, viewportRange);
        //#else
        //        /// 将世界坐标转换为设备坐标
        //        const auto& resultPoint = projectMatrix * modelViewMatrix * glm::vec4(worldPoint, 1.0f);
        //        if (FuzzyIsZero(resultPoint.w))
        //        {
        //            // 齐次坐标是0，错误
        //            LOG_ERROR("w is zero!");
        //            /// 其他错误处理
        //            return QVector3D();
        //        }
        //        QVector3D returnPoint(resultPoint);
        //        returnPoint /= resultPoint.w;
        //        returnPoint.x = LinearMap(returnPoint.x, -1.0f, 1.0f, viewportRange.x, viewportRange.z);
        //        returnPoint.y = LinearMap(returnPoint.y, 1.0f, -1.0f, viewPortRange.y, viewPortRange.w);
        //        return returnPoint;
        //#endif

    }

};
#endif
