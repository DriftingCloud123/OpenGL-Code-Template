cmake_minimum_required(VERSION 3.16)

project(OpenGLTemplate LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 生成 compile_commands.json 供 IntelliSense 使用
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 所有产物统一输出到构建目录根部，便于在项目根目录下运行 exe 时找到 assets
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# 第三方源码（glad、stb_image 实现）
file(GLOB VENDOR_SOURCES CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/vendor/src/*.c ${CMAKE_SOURCE_DIR}/vendor/src/*.cpp)

# 项目源码
file(GLOB PROJECT_SOURCES CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/src/*.cpp)

add_executable(main ${PROJECT_SOURCES} ${VENDOR_SOURCES})

target_include_directories(main PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/vendor/include
)

target_link_directories(main PRIVATE ${CMAKE_SOURCE_DIR}/vendor/lib)

target_link_libraries(main PRIVATE
    glfw3
    opengl32
    gdi32
    # 静态链接 assimp（模型加载），zlib 为其依赖
    assimp
    zlibstatic
    ole32
    uuid
)

if(MSVC)
    target_compile_options(main PRIVATE /W3)
else()
    target_compile_options(main PRIVATE -Wall -Wextra)
endif()

# 让 exe 与 assets 保持相对位置：构建后把 assets 复制到构建目录
add_custom_command(TARGET main POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets
        ${CMAKE_BINARY_DIR}/assets
    COMMENT "Copying assets to build directory"
)
