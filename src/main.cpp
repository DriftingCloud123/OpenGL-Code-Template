#include<iostream>
// 先引用glad再引用glfw
#include<glad/glad.h>
#include<GLFW/glfw3.h>
// 引用工具
#include<stb_image.h>   // 图片处理
#include<Shader.h>      // 着色器

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
//     ---- 位置 ----       ---- 颜色 ----     - 纹理坐标 -
     0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // 右上
     0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // 右下
    -0.5f, -0.5f, 0.0f,   0.0f, 0.5f, 1.0f,   0.0f, 0.0f,   // 左下
    -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // 左上
};

// 创建绘图索引
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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(0 * sizeof(float)));   // 位置属性
    // 开启VAO管理的第一个属性
    glEnableVertexAttribArray(0);

    // 设置并管理其他属性
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));   // 颜色属性
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));   // 材质坐标属性
    glEnableVertexAttribArray(2);

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
    // 材质绑定
    // ============================================
    //生成并绑定材质
    unsigned int texture;
    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0);   // 绑定前先激活槽位
    glBindTexture(GL_TEXTURE_2D, texture);
    // 为当前绑定的纹理对象设置环绕、过滤方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // 加载并生成纹理
    stbi_set_flip_vertically_on_load(true);     // 上下颠倒反转读取，对应坐标
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

    // 将着色器的材质指向材质的槽位（如果不指定，默认是0），即和glActiveTexture里面的GL_TEXTURE0对应
    // 本例中可以不绑定，但是如果要用多个材质，需要绑定
    ourShader.use();
    ourShader.setInt("ourTexture", 0);


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
        ourShader.use();
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
    ourShader.destroy();
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