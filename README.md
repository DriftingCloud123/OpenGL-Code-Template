# OpenGL VS Code 现代化通用模板工程 (Modern OpenGL Template)

基于 **C++17 + MinGW-w64 + GLFW + GLAD + GLM** 的轻量级、开箱即用图形学开发模板。

---

## 📁 目录结构

```text
OpenGL_Codes/
├── .vscode/
│   ├── c_cpp_properties.json   # 包含路径、C++17标准、MinGW编译器配置
│   ├── tasks.json              # 自动化构建脚本（支持 src/*.cpp 多文件编译）
│   └── launch.json             # F5 调试配置（预先绑定编译任务与 GDB 调试器）
├── build/                      # 编译输出目录（可执行程序生成在此处）
│   └── .gitkeep
├── include/
│   ├── glad/                   # OpenGL 4.6 / 3.3 函数加载器
│   ├── GLFW/                   # 跨平台窗口与输入处理库
│   ├── KHR/                    # Khronos 跨平台标准头
│   ├── glm/                    # OpenGL 数学库（向量、矩阵变换、四元数）
│   └── Shader.h                # 标准着色器加载与编译辅助类
├── lib/
│   └── libglfw3.a              # MinGW-w64 64位静态链接库
├── shaders/                    # 存放顶点着色器与片段着色器源码
│   ├── shader.vert             # 示例顶点着色器
│   └── shader.frag             # 示例片段着色器
├── textures/                   # 存放实验贴图图片素材 (.jpg, .png)
├── src/
│   ├── glad.c                  # GLAD 实现文件
│   └── main.cpp                # 程序入口与渲染主循环
├── .gitignore                  # Git 忽略文件（过滤可执行文件与缓存）
└── README.md                   # 模板使用说明文档
```

---

## 🚀 极速上手使用指南

### 1. 从模板新建一个项目
1. 将 `OpenGL_Codes` 文件夹整体复制一份，重命名为你想要的项目名（如 `Lab1_Transform`）。
2. 在 **VS Code** 中点击 **“文件” -> “打开文件夹”**，选择进入新目录。

### 2. 编译与调试
* **快捷键 F5**：自动触发编译并启动 **GDB 单步断点调试**。
* **快捷键 Ctrl + Shift + B**：仅执行编译，生成可执行文件到 `build/main.exe`。

### 3. 多源文件支持
* 本模板已配置通配符编译，只要你在 `src/` 目录下添加任意新的 `.cpp` 文件（如 `src/Camera.cpp`、`src/Mesh.cpp`），**无需修改任何配置文件**，按 `F5` 即可自动全部打包编译。

### 4. 辅助工具类 `Shader.h` 使用示例
模板中已自带标准着色器加载器：
```cpp
#include <Shader.h>

// 构造函数自动读取并编译着色器
Shader ourShader("shaders/shader.vert", "shaders/shader.frag");

// 激活程序
ourShader.use();

// 便捷传递 Uniform 矩阵或向量
ourShader.setMat4("model", modelMatrix);
ourShader.setVec3("lightColor", glm::vec3(1.0f));
```
