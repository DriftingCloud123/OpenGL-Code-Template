#include<iostream>
// 先引用glad再引用glfw
#include<glad/glad.h>
#include<GLFW/glfw3.h>

// Callback Function
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

void processInput(GLFWwindow *window)
{
    // 按下Esc键退出
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// 顶点坐标
float vertices[] = {
     0.5f,  0.5f, 0.0f, // 右上
     0.5f, -0.5f, 0.0f, // 右下
    -0.5f, -0.5f, 0.0f, // 左下
    -0.5f,  0.5f, 0.0f, // 左上
};

// 创建绘图索引
unsigned int indices[] = {
    // 第一个三角形
    0, 1, 3,
    // 第二个三角形
    1, 2, 3
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
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);    //移除所有在当前请求版本中的废弃功能
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

    // do something
    // 从坐标到图形的一般顺序：
    // 顶点着色器 → 图元装配 → 光栅化 → 片段着色器 → 深度测试/混合 → 像素
    /*
     * 参考内容：
     * 顶点着色器源码字符串
     * 片段着色器源码字符串
     * 编译顶点着色器
     * 编译片段着色器
     * 创建着色器程序，链接，检查错误
     * 删除着色器对象
     * 创建 VAO 和 VBO，上传顶点数据，配置顶点属性
     * 进入渲染循环：每帧 glUseProgram、绑定 VAO、glDrawArrays
     * 循环结束后清理：删除 VAO、VBO、着色器程序
     */

    // ============================================
    // 着色器部分
    // ============================================

    // 顶点/片段着色器源码
    const char *vertexShaderSource = "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "void main()\n"
        "{\n"
        "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
        "}\0";

    const char *fragmentShaderSource = "#version 330 core\n"
        "out vec4 FragColor;\n"
        "void main()\n"
        "{\n"
        "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
        "}\n\0";
    
    // 创建顶点/片段着色器对象
    unsigned int vertexShader, fragmentShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    // 把着色器源码附加到着色器对象上，然后编译它们
    int  success;
    char infoLog[512];
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // 编译错误报错检查
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // 编译错误报错检查
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    // 着色器程序
    // 创建着色器程序
    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();
    // 将着色器附加到程序，之后进行链接
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // 检测链接(着色器程序)错误
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        glDeleteProgram(shaderProgram);
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }
    // 之后可以激活程序对象，每个着色器调用和渲染调用都会使用这个程序对象
    // glUseProgram(shaderProgram);
    // 在把着色器对象链接到程序对象以后，记得删除着色器对象，我们不再需要它们了
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    // 开启VAO管理的第一个属性
    glEnableVertexAttribArray(0);

    // 当我们渲染一个物体时要使用着色器程序
    // 绘制物体
    // glUseProgram(shaderProgram);
    // glBindVertexArray(VAO);
    // someOpenGLFunctionThatDrawsOurTriangle();

    
    // 解绑，防止意外修改
    glBindVertexArray(0);   // 解绑VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0);   // 上传数据到GPU并在VAO进行配置之后，可以解绑VBO，防止意外修改
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);   // 不要单独解绑EBO，EBO绑定属于VAO状态，解绑会清掉VAO记录的索引缓冲

    // ============================================
    // 准备绘制
    // ============================================

    // 设置视口的尺寸，OpenGL从(-1, 1)进行坐标映射
    glViewport(0, 0, 800, 600);
    // 注册这个函数，告诉GLFW我们希望每当窗口调整大小的时候调用这个函数
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // ============================================
    // 进入渲染循环
    // ============================================

    // 渲染循环(Render Loop)
    while(!glfwWindowShouldClose(window)){
        // 输入
        processInput(window);

        // 渲染指令放在这里

        // 清理背景并重新渲染
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);   // 设置背景渲染颜色
        glClear(GL_COLOR_BUFFER_BIT);           // 清空背景并重新渲染指定颜色

        // 绘制前绑定
        glUseProgram(shaderProgram);
        // 这里绑定VAO即可，VAO已记录属性数据源(VBO)与索引缓冲(EBO)
        // 同时绑定VAO会自动切换EBO绑定
        glBindVertexArray(VAO);
        // glBindBuffer(GL_ARRAY_BUFFER, VBO);
        // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

        // 绘制(如果没有配置EBO，可以手动指定indices，为0则使用EBO)
        glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(indices[0]), GL_UNSIGNED_INT, 0/*indices*/);
        // 注意：要用 glDrawArrays 画这个矩形，需要先把 vertices 扩展为 6 个顶点
        // glDrawArrays(GL_TRIANGLES, 0, 6);

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
    glDeleteProgram(shaderProgram);
    glfwDestroyWindow(window);

    glfwTerminate();

    return 0;
}

// 当用户改变窗口的大小的时候，视口也应该被调整。我们可以对窗口注册一个回调函数
// Callback Function，在每次窗口大小被调整的时候被调用
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}