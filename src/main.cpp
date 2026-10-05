#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>

// Callback Function
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

int main(){
    // 初始化GLFW
    glfwInit();
    // OpenGL版本号3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);  // 主版本号为3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);  // 次版本号为3
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 说明是核心模式

    GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
    if(window == NULL){
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // 初始化glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // 设置视口的尺寸，OpenGL从(-1, 1)进行坐标映射
    glViewport(0, 0, 800, 600);

    // 注册这个函数，告诉GLFW我们希望每当窗口调整大小的时候调用这个函数
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // 渲染循环(Render Loop)
    while(!glfwWindowShouldClose(window)){
        // 输入
        processInput(window);

        // 渲染指令
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // 交换缓冲并检查调用事件
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}

// 当用户改变窗口的大小的时候，视口也应该被调整。我们可以对窗口注册一个回调函数
// Callback Function，在每次窗口大小被调整的时候被调用
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}