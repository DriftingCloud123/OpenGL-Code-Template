#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

/**
 * @brief 相机移动方向枚举
 *
 * 把移动抽象成方向，避免依赖具体的窗口系统输入方式。
 */
enum class Camera_Movement {
    FORWARD,   ///< 向前
    BACKWARD,  ///< 向后
    LEFT,      ///< 向左
    RIGHT,     ///< 向右
    UP,        ///< 向上
    DOWN       ///< 向下
};

// ===== 相机默认参数 =====
const float YAW         = -90.0f;   ///< 默认偏航角（度），-90 度使相机初始朝向 -Z
const float PITCH       =  0.0f;    ///< 默认俯仰角（度）
const float SPEED       =  7.5f;    ///< 默认移动速度
const float SENSITIVITY =  0.1f;    ///< 默认鼠标灵敏度
const float ZOOM        =  45.0f;   ///< 默认缩放（视场角 FOV，度）


/**
 * @brief 抽象相机类
 *
 * 负责处理输入，并根据欧拉角计算对应的方向向量和视图矩阵，供 OpenGL 渲染使用。
 */
class Camera
{
public:
    // ===== 相机属性 =====
    glm::vec3 Position;     ///< 相机在世界空间中的位置
    glm::vec3 Front;        ///< 相机前方方向向量（单位向量）
    glm::vec3 Up;           ///< 相机上方方向向量（单位向量）
    glm::vec3 Right;        ///< 相机右方方向向量（单位向量）
    glm::vec3 WorldUp;      ///< 世界上方方向向量，通常为 (0, 1, 0)

    // ===== 欧拉角 =====
    float Yaw;              ///< 偏航角（度），绕 Y 轴旋转，控制左右看
    float Pitch;            ///< 俯仰角（度），绕 X 轴旋转，控制上下看

    // ===== 相机选项 =====
    float MovementSpeed;    ///< 移动速度，单位/秒
    float MouseSensitivity; ///< 鼠标灵敏度
    float Zoom;             ///< 缩放（视场角 FOV，度），越小视野越窄越"放大"

    /**
     * @brief 使用向量参数构造相机
     * @param position 相机初始位置，默认原点 (0, 0, 0)
     * @param up       世界上方向向量，默认 (0, 1, 0)
     * @param yaw      初始偏航角（度），默认 YAW
     * @param pitch    初始俯仰角（度），默认 PITCH
     */
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
           glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
           float yaw = YAW, float pitch = PITCH)
        : Front(glm::vec3(0.0f, 0.0f, -1.0f)),
          MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }

    /**
     * @brief 使用标量参数构造相机
     * @param posX 相机位置 X 分量
     * @param posY 相机位置 Y 分量
     * @param posZ 相机位置 Z 分量
     * @param upX  世界上方向 X 分量
     * @param upY  世界上方向 Y 分量
     * @param upZ  世界上方向 Z 分量
     * @param yaw  初始偏航角（度）
     * @param pitch 初始俯仰角（度）
     */
    Camera(float posX, float posY, float posZ,
           float upX, float upY, float upZ,
           float yaw, float pitch)
        : Front(glm::vec3(0.0f, 0.0f, -1.0f)),
          MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = glm::vec3(posX, posY, posZ);
        WorldUp = glm::vec3(upX, upY, upZ);
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }

    /**
     * @brief 获取视图矩阵
     * @return 基于当前位置、朝向和上方向计算出的 view 矩阵（LookAt）
     */
    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(Position, Position + Front, Up);
    }

    /**
     * @brief 处理键盘输入，移动相机
     *
     * 参数使用枚举而不是具体的按键码，方便与窗口系统解耦。
     * @param direction 移动方向（Camera_Movement 枚举）
     * @param deltaTime 距上一帧的时间（秒），用于让移动速度与帧率无关
     */
    void ProcessKeyboard(Camera_Movement direction, float deltaTime)
    {
        float velocity = MovementSpeed * deltaTime;
        if (direction == Camera_Movement::FORWARD)
            Position += Front * velocity;
        if (direction == Camera_Movement::BACKWARD)
            Position -= Front * velocity;
        if (direction == Camera_Movement::LEFT)
            Position -= Right * velocity;
        if (direction == Camera_Movement::RIGHT)
            Position += Right * velocity;
        if (direction == Camera_Movement::UP)
            Position += WorldUp * velocity;
        if (direction == Camera_Movement::DOWN)
            Position -= WorldUp * velocity;
    }

    /**
     * @brief 处理鼠标移动输入，旋转相机
     * @param xoffset       鼠标在 X 方向的偏移量（像素）
     * @param yoffset       鼠标在 Y 方向的偏移量（像素）
     * @param constrainPitch 是否把俯仰角限制在 [-89, 89] 度，默认 true，
     *                       防止视角翻转
     */
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
    {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        Yaw   += xoffset;
        Pitch += yoffset;

        // 限制俯仰角，避免视角超过上下 90 度导致画面翻转
        if (constrainPitch)
        {
            if (Pitch > 89.0f)
                Pitch = 89.0f;
            if (Pitch < -89.0f)
                Pitch = -89.0f;
        }

        // 根据更新后的欧拉角重新计算 Front / Right / Up
        updateCameraVectors();
    }

    /**
     * @brief 处理鼠标滚轮输入，缩放视野（改变 FOV）
     * @param yoffset 滚轮在垂直方向的偏移量
     */
    void ProcessMouseScroll(float yoffset)
    {
        Zoom -= (float)yoffset;
        if (Zoom < 1.0f)
            Zoom = 1.0f;
        if (Zoom > 45.0f)
            Zoom = 45.0f;
    }

private:
    /**
     * @brief 根据当前欧拉角（Yaw / Pitch）重新计算 Front、Right、Up
     *
     * 每次偏航角或俯仰角改变后都要调用，保证方向向量与视角一致。
     */
    void updateCameraVectors()
    {
        // 计算新的前方向量
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);

        // 重新计算右方和上方向量
        // 需要归一化：当视线越接近上下 90 度时，叉积长度越接近 0，不归一化会导致移动变慢
        Right = glm::normalize(glm::cross(Front, WorldUp));
        Up    = glm::normalize(glm::cross(Right, Front));
    }
};
#endif