set(SRCS
    main.cpp
    Window.cpp
    Window.h

    Camera.cpp
    Camera.h

    OpenGLContext.cpp
    OpenGLContext.h

    SceneRenderer.cpp
    SceneRenderer.h

    Entity.cpp
    Entity.h

    EntityModel.cpp
    EntityModel.h

    InputManager.cpp
    InputManager.h

    Resources.h

    SliderGroup.cpp
    SliderGroup.h
    
    Shaders/diffuse.fs
    Shaders/diffuse.vs
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