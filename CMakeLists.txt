cmake_minimum_required(VERSION 3.10.0)
project(SDLShooter VERSION 0.1.0 LANGUAGES C CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 要么找包，要么添加路径，告诉编译器SDL2的头文件和库文件位置
# 记得把反斜杠改成正的
# 使用更灵活的方式设置SDL2路径
# set(SDL2_DIR "G:/Library/SDL2/cmake")

# 设置编译选项
# add_compile_options(-Wall -Wextra -pedantic)    # for Mac,-Wall -Wextra -pedantic -Werror
if(MSVC)
    add_compile_options(/W4)  # for Windows, -W4 is a common warning level
else()
    add_compile_options(-Wall -Wextra -pedantic)  # for Linux and Mac
endif()

# 设置编译输出目录，将可执行文件生成在CMakeLists.txt所在目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR})

# 根据不同平台生成不同的名称
# 设置一个变量来存储可执行文件名称
set(TARGET ${PROJECT_NAME}-${CMAKE_SYSTEM_NAME})

# 查找并载入CMake预设
# 查找是在系统环境变量里面查找
# REQUIRED表示找不到会报错
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)  # 添加这一行
find_package(SDL2_ttf REQUIRED)
find_package(SDL2_mixer REQUIRED)
# 如果find_package不工作，取消下面的注释并使用直接路径
# include_directories("G:/Library/SDL2/include")
# link_directories("G:/Library/SDL2/lib/x64")

# 设置通用源文件
set(SOURCES
    "main.cpp"
    "src/Game.cpp"
    "src/Scene.cpp"
    "src/SceneMain.cpp"
    "src/SceneEnd.cpp"
    "src/SceneTitle.cpp"
    "resources.rc"          # 添加资源文件，只对Windows有效
)

if(MSVC)
    # 在Windows上，将资源文件添加到源文件列表中
    list(APPEND SOURCES "resources.rc")
endif()

# 添加可执行文件
add_executable(${TARGET} ${SOURCES})

               
# 告诉程序链接哪些库
target_link_libraries(${TARGET} 
                        ${SDL2_LIBRARIES} 
                        SDL2_image::SDL2_image
                        SDL2_ttf::SDL2_ttf
                        SDL2_mixer::SDL2_mixer
                        )

# 不弹出控制台窗口
if(MSVC)
    target_link_options(${TARGET} PRIVATE "/SUBSYSTEM:WINDOWS")
endif()
