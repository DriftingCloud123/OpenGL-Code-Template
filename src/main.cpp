#include <iostream>
// 先引用glad再引用glfw
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// 工具
#include <stb_image.h>   // 图片处理
#include "Shader.h"      // 着色器
#include "Camera.h"      // 摄像机
// 数学库
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Callback Function 回调函数
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

// 帧时间记录
float deltaTime = 0.0f; // 当前帧与上一帧的时间差
float lastFrame = 0.0f; // 上一帧的时间

// 摄像机配置
// glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f,  3.0f);
// glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
// glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);
Camera camera = Camera(
    // pos
    glm::vec3(0.0f, 0.0f, 3.0f),
    // Worldup
    glm::vec3(0.0f, 1.0f, 0.0f),
    // yaw
    -90.0f,
    //pitch
    0.0f
);

// 摄像机俯仰角
// glm::vec3 direction(1.0f);
// float yaw = -90.0f;
// float pitch = 0;

// 摄像机视野 Field of View
// float fov = 45.0f;

// 窗口配置
int WINDOW_WIDTH = 800;
int WINDOW_HEIGHT = 600;
float lastX = WINDOW_WIDTH / 2.0f, lastY = WINDOW_HEIGHT / 2.0f;

// 鼠标追踪
bool firstMouse = true;

// 顶点属性配置
// 每个立方体的局部坐标
float vertices[] = {
    // 位置 面的法向量
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
};

// 绘图顺序索引，本模板如果不适用glDrawElements则无效
unsigned int indices[] = {
    // 第一个三角形
    0, 1, 2,
    // 第二个三角形
    0, 2, 3
};


// 光源位置
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

int main(){

    // ============================================
    // 初始化部分
    // ============================================
    
    // 初始化GLFW
    if (!glfwInit())
    {
        std::cout << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // OpenGL版本号3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);  // 主版本号为3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);  // 次版本号为3
    // 说明是核心模式
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 说明是核心模式

    #if defined(__APPLE__)  // 适配macOS平台
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);    // 移除所有在当前请求版本中的废弃功能
    #endif

    // 创建窗口
    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "LearnOpenGL", NULL, NULL);
    if(window == NULL){
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
	// 将窗口的上下文设置为当前线程的主上下文
    glfwMakeContextCurrent(window);

    // 初始化glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    // 设置绘图模式(默认是GL_FILL)
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    // 开启深度测试
    glEnable(GL_DEPTH_TEST);

    // do something
    // 从坐标到图形的一般顺序：
    // 顶点着色器 → 图元装配 → 光栅化 → 片段着色器 → 深度测试/混合 → 像素
    /*
     * 参考内容：
     ┌────────────────这部分已被类封装────────────────
     |* 顶点着色器源码字符串
     |* 片段着色器源码字符串
     |* 编译顶点着色器
     |* 编译片段着色器
     |* 创建着色器程序，链接，检查错误
     |* 删除着色器对象
     └───────────────────────────────────────────────
     * 创建 VAO 和 VBO，上传顶点数据，配置顶点属性与绘制索引
     * 进入渲染循环：每帧 glUseProgram、绑定 VAO、glDrawArrays/glDrawElements
     * 循环结束后清理：删除 VAO、VBO、着色器程序
     */

    // ============================================
    // 着色器部分
    // ============================================

    Shader CubeShader("assets/shaders/cube_shader.vert", "assets/shaders/cube_shader.frag");
    Shader LighterShader("assets/shaders/lighter_shader.vert", "assets/shaders/lighter_shader.frag");

    // ============================================
    // 将数据传输到GPU并配置绘制说明
    // ============================================
    // 创建VBO、VAO、EBO
    // VBO：顶点缓冲对象
    // VAO：顶点数组对象
    // EBO：索引顶点对象
    unsigned int CubeVAO, VBO;
    glGenVertexArrays(1, &CubeVAO);
    glGenBuffers(1, &VBO);
    
    // 绑定VAO
    glBindVertexArray(CubeVAO);
    // 复制顶点数组到缓冲中供OpenGL使用
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // 把之前定义的顶点数据复制到缓冲的内存中
    // 将新建的VBO绑定到GL_ARRAY_BUFFER目标上
    // 此时对GL_ARRAY_BUFFER目标上的缓冲调用都会用来配置当前绑定的缓冲
    // GL_STATIC_DRAW意思是很少改变
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    // 设置顶点属性指针
    // 管理位置属性
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    // 开启VAO管理的第一个属性
    glEnableVertexAttribArray(0);
    // 管理法向量属性
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3* sizeof(float)));
    glEnableVertexAttribArray(1);
    
    // 设置灯的VAO
    unsigned int LighterVAO;
    glGenVertexArrays(1, &LighterVAO);
    glBindVertexArray(LighterVAO);
    // 只需要绑定VBO不用再次设置VBO的数据，因为箱子的VBO数据中已经包含了正确的立方体顶点数据
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // 设置灯立方体的顶点属性（对我们的灯来说仅仅只有位置数据）
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    /* 当我们渲染一个物体时要使用着色器程序 */
    // 绘制物体示例：
    // glUseProgram(shaderProgram);
    // glBindVertexArray(VAO);
    // someOpenGLFunctionThatDrawsOurTriangle();
    
    // 解绑，防止意外修改
    glBindVertexArray(0);   // 解绑VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0);   // 上传数据到GPU并在VAO进行配置之后，可以解绑VBO，防止意外修改

    // 配置坐标变换矩阵
    // 局部坐标 => 世界坐标 => 观察坐标 => 裁剪坐标 => 屏幕坐标
    // 共需要三个矩阵：模型矩阵、观察矩阵和投影矩阵
    CubeShader.use();
    glm::mat4 model(1.0f);
    CubeShader.setMat4("model", model);

    glm::mat4 view;
    view = camera.GetViewMatrix();
    CubeShader.setMat4("view", view);

    // 投影矩阵（裁剪坐标 => 屏幕坐标）
    glm::mat4 projection(1.0f);
    projection = glm::perspective(glm::radians(camera.Zoom), (float)WINDOW_WIDTH / WINDOW_HEIGHT, 0.1f, 100.0f);
    CubeShader.setMat4("projection", projection);

    // CubeShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
    // CubeShader.setVec3("lightColor",  1.0f, 1.0f, 1.0f);
    // 设置光照属性
    CubeShader.setVec3("light.ambient",  0.2f, 0.2f, 0.2f);
    CubeShader.setVec3("light.diffuse",  0.5f, 0.5f, 0.5f); // 将光照调暗了一些以搭配场景
    CubeShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f); 
    // ========================================================================================================
    // 设置物体材质
    CubeShader.use();
    CubeShader.setVec3("material.ambient",  1.0f, 0.5f, 0.31f);
    CubeShader.setVec3("material.diffuse",  1.0f, 0.5f, 0.31f);
    CubeShader.setVec3("material.specular", 0.5f, 0.5f, 0.5f);
    CubeShader.setFloat("material.shininess", 32.0f);

    LighterShader.use();
    model = glm::mat4(1.0f);
    model = glm::translate(model, lightPos);
    model = glm::scale(model, glm::vec3(0.2f));
    LighterShader.setMat4("model", model);

    view = camera.GetViewMatrix();
    LighterShader.setMat4("view", view);

    projection = glm::mat4(1.0f);
    projection = glm::perspective(glm::radians(camera.Zoom), (float)WINDOW_WIDTH / WINDOW_HEIGHT, 0.1f, 100.0f);
    LighterShader.setMat4("projection", projection);


    

    // ============================================
    // 准备绘制
    // ============================================

    // 设置视口的尺寸，OpenGL从(-1, 1)进行坐标映射
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    // 注册视口转换函数，每当窗口调整大小的时候调用这个函数
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // 隐藏光标并捕捉
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    // 注册光标函数，每当光标移动的时候调用这个函数
    glfwSetCursorPosCallback(window, mouse_callback);

    // 注册滚轮函数，同理
    glfwSetScrollCallback(window, scroll_callback);

    // ============================================
    // 进入渲染循环
    // ============================================


    // 渲染循环(Render Loop)
    while(!glfwWindowShouldClose(window)){
        // 输入
        processInput(window);

        // 计算每帧时间差用于平衡不同设备摄像机速度
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // 每帧同步摄像机视角
        // 摄像机旋转
        glm::mat4 view;
        view = camera.GetViewMatrix();
        // 摄像机缩放
        projection = glm::perspective(glm::radians(camera.Zoom), (float)WINDOW_WIDTH / WINDOW_HEIGHT, 0.1f, 100.0f);

        /* ================== 渲染指令放在下面 ================== */

        // 清理背景并重新渲染
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);   // 设置背景渲染颜色
        // glClearColor(0.5f, 0.5f, 0.5f, 1.0f);   // 设置背景渲染颜色
        // glClear(GL_COLOR_BUFFER_BIT);           // 清空背景并重新渲染指定颜色
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);     // 清空背景并重新渲染指定颜色，同时清空深度缓冲

        // change the light's position values over time (can be done anywhere in the render loop actually, but try to do it at least before using the light source positions)
        lightPos.x = 1.0f + sin(glfwGetTime()) * 0.5f;
        lightPos.y = cos(glfwGetTime()) * 0.5f;

        // 绘制物体
        CubeShader.use();
        CubeShader.setMat4("view", view);
        CubeShader.setMat4("projection", projection);
        CubeShader.setVec3("light.position", lightPos);
        CubeShader.setVec3("viewPos", camera.Position);
        glBindVertexArray(CubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glm::vec3 lightColor;
        lightColor.x = sin(glfwGetTime() * 2.0f);
        lightColor.y = sin(glfwGetTime() * 0.7f);
        lightColor.z = sin(glfwGetTime() * 1.3f);

        glm::vec3 diffuseColor = lightColor   * glm::vec3(0.5f); // 降低影响
        glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f); // 很低的影响

        CubeShader.setVec3("light.ambient", ambientColor);
        CubeShader.setVec3("light.diffuse", diffuseColor);

        LighterShader.use();
        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.2f));
        LighterShader.setMat4("model", model);
        LighterShader.setMat4("view", view);
        LighterShader.setMat4("projection", projection);
        glBindVertexArray(LighterVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // 交换缓冲并检查调用事件
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    
    // ============================================
    // 清除资源
    // ============================================
    glDeleteVertexArrays(1, &LighterVAO);
    glDeleteBuffers(1, &CubeVAO);
    LighterShader.destroy();
    CubeShader.destroy();
    glfwDestroyWindow(window);

    glfwTerminate();

    return 0;
}

// 回调函数framebuffer_size_callback：用于窗口大小发生变化时重新进行视口转换
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    WINDOW_WIDTH = width;
    WINDOW_HEIGHT = height;
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
}

// 回调函数processInput：用于处理键盘行为
void processInput(GLFWwindow *window)
{
    // 按下Esc键退出
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // 摄像机移动
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(Camera_Movement::FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(Camera_Movement::BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(Camera_Movement::LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(Camera_Movement::RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        camera.ProcessKeyboard(Camera_Movement::DOWN, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.ProcessKeyboard(Camera_Movement::UP, deltaTime);
}

// 回调函数mouse_callback：用于处理鼠标移动行为
void mouse_callback(GLFWwindow* window, double xpos, double ypos){
    // 防止第一次视角跳变
    if(firstMouse) // 这个bool变量初始时是设定为true的
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    // 计算每帧鼠标偏离量
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // 注意这里是相反的，因为屏幕中y坐标是从底部往顶部依次增大的
    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset, true);
}

// 回调函数mouse_callback：用于处理鼠标滚轮行为
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}