# OpenGL VS Code 现代化通用模板工程 (Modern OpenGL Template)

基于 **C++17 + MinGW-w64 + GLFW + GLAD + GLM** 的通用图形学开发模板。

---

## 📁 目录结构

```text
OpenGL_Codes/
├── .vscode/
│   ├── c_cpp_properties.json
│   ├── tasks.json
│   └── launch.json
├── assets/                     # 存放静态资产（着色器、贴图、模型等）
│   ├── shaders/                # 存放顶点着色器与片段着色器源码
│   │   ├── shader.vert         # 示例顶点着色器
│   │   └── shader.frag         # 示例片段着色器
│   └── textures/               # 实验贴图图片素材 (.jpg, .png)
├── build/                      # 编译输出目录
│   └── .gitkeep
├── include/                    # 存放自定义模块头文件
│   └── Shader.h                # 标准着色器加载与编译辅助类
├── src/                        # 存放自定义模块源文件与程序入口
│   └── main.cpp                # 程序入口与渲染主循环
├── vendor/                     # 存放第三方依赖（头文件、实现与静态库）
│   ├── include/                # glad, GLFW, KHR, glm
│   ├── src/                    # glad.c
│   └── lib/                    # libglfw3.a
├── .gitignore
└── README.md
```

## 🛠️ 使用指南

### 🚀 快速上手

使用 **VS Code** 打开本项目，打开左侧栏的**运行与调试**（Ctrl+Shift+D），选择`Debug OpenGL (gdb)`或者`Release OpenGL (Run - O3)`，之后开始调试（F5）或者运行（Ctrl + F5），即可看到渲染出来的橙色矩形

### ⚙️ 编写项目
在`src`下编写源码，项目会将`src`下的所有cpp文件以及`vendor\src\glad.c`一起编译，最终放到`build`下

编译成功的`exe`文件需在根目录下进行运行，否则会找不到相关资源，如：
```bash
.\build\main.exe
```

### 🔥 自定义着色器
着色器源码建议存放在`assets\shaders`下，可以编写自己的着色器，里面存放了最简单的着色器，推荐配合插件 _Shader Studio_ 使用。

着色器类在`include\Shader.h`下，完整定义了`Shader`类，目前支持顶点着色器、几何着色器（可选）和片段着色器三种，在`assets\shaders`下编写完着色器源码之后，使用源码的路径初始化Shader对象：
```cpp
Shader ourShader('path/to/vertexShade.vert', 'path/to/fragmentShader.frag', 'path/to/geometryShader.geom');
```

之后可以使用`Shader`类的各种方法来对着色器进行控制，如：
```
ourShader.use()     //相当于glUseProgram
ourShader.destroy() //相当于glDeleteProgram
……
```

## 🎉 感谢

感谢教程网站 [LearnOpenGL](https://learnopengl-cn.github.io/) ，这个网站在我的学习中帮助了我许多，源码中的大多数都是参考了这个网站。