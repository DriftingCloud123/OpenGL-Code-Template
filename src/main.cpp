#include<iostream>
// 先引用glad再引用glfw
#include<glad/glad.h>
#include<GLFW/glfw3.h>
// 工具
#include<stb_image.h>   // 图片处理
#include<Shader.h>      // 着色器
#include<Camera.h>      // 摄像机
// 数学库
#include<cmath>
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
glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f,  3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);
// 摄像机俯仰角
glm::vec3 direction(1.0f);
float yaw = -90.0f;
float pitch = 0;
// 摄像机视野 Field of View
float fov = 45.0f;

// 窗口配置
int WINDOW_WIDTH = 800;
int WINDOW_HEIGHT = 600;
float lastX = WINDOW_WIDTH / 2.0f, lastY = WINDOW_HEIGHT / 2.0f;

// 鼠标追踪
bool firstMouse = true;

// 顶点属性配置
// 每个立方体的局部坐标
float vertices[] = {
//  --------位置--------  --材质坐标--
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
};

// 正方体的世界坐标
glm::vec3 cubePositions[] = {
    glm::vec3( 0.0f,  0.0f,  0.0f), 
    glm::vec3( 2.0f,  5.0f, -15.0f), 
    glm::vec3(-1.5f, -2.2f, -2.5f),  
    glm::vec3(-3.8f, -2.0f, -12.3f),  
    glm::vec3( 2.4f, -0.4f, -3.5f),  
    glm::vec3(-1.7f,  3.0f, -7.5f),  
    glm::vec3( 1.3f, -2.0f, -2.5f),  
    glm::vec3( 1.5f,  2.0f, -2.5f), 
    glm::vec3( 1.5f,  0.2f, -1.5f), 
    glm::vec3(-1.3f,  1.0f, -1.5f)  
};

// 绘图顺序索引，本模板如果不适用glDrawElements则无效
unsigned int indices[] = {
    // 第一个三角形
    0, 1, 2,
    // 第二个三角形
    0, 2, 3
};


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
    GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
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

    Shader ourShader("assets/shaders/shader.vert", "assets/shaders/shader.frag");

    // ============================================
    // 将数据传输到GPU并配置绘制说明
    // ============================================
    // 创建VBO、VAO、EBO
    // VBO：顶点缓冲对象
    // VAO：顶点数组对象
    // EBO：索引顶点对象
    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    
    // 绑定VAO
    glBindVertexArray(VAO);
    // 复制顶点数组到缓冲中供OpenGL使用
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // 绑定EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    // 把之前定义的顶点数据复制到缓冲的内存中
    // 将新建的VBO绑定到GL_ARRAY_BUFFER目标上
    // 此时对GL_ARRAY_BUFFER目标上的缓冲调用都会用来配置当前绑定的缓冲
    // GL_STATIC_DRAW意思是很少改变
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // 将索引存到EBO中
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    // 设置顶点属性指针
    // 管理位置属性
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    // 开启VAO管理的第一个属性
    glEnableVertexAttribArray(0);


    // 管理纹理坐标属性
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3* sizeof(float)));
    glEnableVertexAttribArray(1);

    /* 当我们渲染一个物体时要使用着色器程序 */
    // 绘制物体示例：
    // glUseProgram(shaderProgram);
    // glBindVertexArray(VAO);
    // someOpenGLFunctionThatDrawsOurTriangle();
    
    // 解绑，防止意外修改
    glBindVertexArray(0);   // 解绑VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0);   // 上传数据到GPU并在VAO进行配置之后，可以解绑VBO，防止意外修改
    // 不要单独解绑EBO，EBO绑定属于VAO状态，解绑会清掉VAO记录的索引缓冲，❌glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);   

    // ============================================
    // 设置纹理
    // ============================================
    // 生成纹理并绑定
    unsigned int texture1, texture2;
    glGenTextures(1, &texture1);
    // 在绑定纹理之前先激活纹理单元槽位。纹理单元GL_TEXTURE0默认总是被激活，因而该行可以省略
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture1);
    // 为当前绑定的纹理对象设置环绕、过滤方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);   
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // 加载并生成纹理
    stbi_set_flip_vertically_on_load(true); // 上下颠倒翻转读取
    int width, height, nrChannels;
    unsigned char *data = stbi_load("assets/textures/container.jpg", &width, &height, &nrChannels, 0);
    if (data){
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else{
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    // 第二张纹理
    glGenTextures(1, &texture2);
    glActiveTexture(GL_TEXTURE1); // 激活1号纹理单元
    glBindTexture(GL_TEXTURE_2D, texture2);
    // 为当前绑定的纹理对象设置环绕、过滤方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);   
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    data = stbi_load("assets/textures/awesomeface.png", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    // 设置片段着色器各纹理槽号
    // 这里是将着色器的材质指向材质的槽位（如果不指定，默认是0），即和glActiveTexture里面的GL_TEXTURE0对应
    // 如果只有一个材质可以不绑定。但是如果要用多个材质，则需要绑定，否则会出错
    ourShader.use();
    ourShader.setInt("texture1", 0);
    ourShader.setInt("texture2", 1);

    // 配置坐标变换矩阵
    // 局部坐标 => 世界坐标 => 观察坐标 => 裁剪坐标 => 屏幕坐标
    // 共需要三个矩阵：模型矩阵、观察矩阵和投影矩阵
    ourShader.use();
    // 模型矩阵（局部坐标 => 世界坐标）
    glm::mat4 model(1.0f);
    model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    ourShader.setMat4("model", model);

    // 观察矩阵（世界坐标 => 观察坐标）
    glm::mat4 view;
    view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    ourShader.setMat4("view", view);

    // 投影矩阵（裁剪坐标 => 屏幕坐标）
    glm::mat4 projection(1.0f);
    int screenWidth, screenHeight;
    glfwGetFramebufferSize(window, &screenWidth, &screenHeight);
    // 正射投影使用glm::ortho，透视投影使用glm::perspective
    projection = glm::perspective(glm::radians(fov), (float)screenWidth / screenHeight, 0.1f, 100.0f);
    ourShader.setMat4("projection", projection);

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
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        ourShader.setMat4("view", view);
        // 摄像机缩放
        projection = glm::perspective(glm::radians(fov), (float)screenWidth / screenHeight, 0.1f, 100.0f);
        ourShader.setMat4("projection", projection);

        /* ================== 渲染指令放在下面 ================== */

        // 清理背景并重新渲染
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);   // 设置背景渲染颜色
        // glClear(GL_COLOR_BUFFER_BIT);           // 清空背景并重新渲染指定颜色
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);     // 清空背景并重新渲染指定颜色，同时清空深度缓冲
        // 绘制前绑定
        ourShader.use();

        // 这里绑定VAO即可，VAO已记录属性数据源(VBO)与索引缓冲(EBO)
        // 同时绑定VAO会自动切换EBO绑定
        glBindVertexArray(VAO);
        // glBindBuffer(GL_ARRAY_BUFFER, VBO);
        // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

        // 绘制(使用glDrawElements如果没有配置EBO，可以手动指定indices数组，为0则使用配置的EBO)
        // glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(indices[0]), GL_UNSIGNED_INT, 0/*indices*/);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        for(unsigned int i = 0; i < 10; i++){
            glm::mat4 model(1.0f);
            model = glm::translate(model, cubePositions[i]);
            float angle = 20.0f * i; 
            model = glm::rotate(model, glm::radians(angle)+(float)glfwGetTime(), glm::vec3(1.0f, 0.3f, 0.5f));
            ourShader.setMat4("model", model);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // 交换缓冲并检查调用事件
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    
    // ============================================
    // 清除资源
    // ============================================
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    ourShader.destroy();
    glfwDestroyWindow(window);

    glfwTerminate();

    return 0;
}

// 回调函数framebuffer_size_callback：用于窗口大小发生变化时重新进行视口转换
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

// 回调函数processInput：用于处理键盘行为
void processInput(GLFWwindow *window)
{
    // 按下Esc键退出
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float cameraSpeed = deltaTime * 2.5f; // adjust accordingly
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        cameraPos -= cameraUp * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        cameraPos += cameraUp * cameraSpeed;
}

// 回调函数mouse_callback：用于处理鼠标移动行为
void mouse_callback(GLFWwindow* window, double xpos, double ypos){
    if(firstMouse) // 这个bool变量初始时是设定为true的
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // 注意这里是相反的，因为y坐标是从底部往顶部依次增大的
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.05f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;
    yaw   += xoffset;
    pitch += yoffset;

    if(pitch > 89.0f)
        pitch = 89.0f;
    if(pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

// 回调函数mouse_callback：用于处理鼠标滚轮行为
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    fov -= (float)yoffset;
    if (fov < 1.0f)
        fov = 1.0f;
    if (fov > 45.0f)
        fov = 45.0f;
}