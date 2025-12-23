set(SRCS
    main.cpp
    Window.cpp
    Window.h

    Camera.cpp
    Camera.h

    OpenGLContext.cpp
    OpenGLContext.h

    Vector3DInputWidget.cpp
    Vector3DInputWidget.h

    ShaderManager.cpp
    ShaderManager.h

    SceneRenderer.cpp
    SceneRenderer.h

    Light.cpp
    Light.h

    Entity.cpp
    Entity.h

    EntityModel.cpp
    EntityModel.h

    InputManager.cpp
    InputManager.h

    Resources.h

    SliderGroup.cpp
    SliderGroup.h
    
    Shaders/phonglight.fs
    Shaders/phonglight.vs
    Textures/voronoi.png

    resources.qrc
)

find_package(Qt5 COMPONENTS Widgets REQUIRED)
add_executable(demo-app ${SRCS})

target_link_libraries(demo-app
    PRIVATE
        Qt5::Widgets
        FGL::Base
        thirdparty::tinygltf
        
)