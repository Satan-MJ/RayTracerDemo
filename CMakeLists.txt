CMAKE_MINIMUM_REQUIRED(VERSION 3.5)
PROJECT(RayTracing CXX)

IF(NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE Release)
ENDIF()

ADD_SUBDIRECTORY(deps/vecmath)

SET(PA1_SOURCES
        src/image.cpp
        src/main.cpp
        src/mesh.cpp
        src/scene.cpp)

SET(PA1_INCLUDES
        include/camera.hpp
        include/group.hpp
        include/hit.hpp
        include/image.hpp
        include/light.hpp
        include/material.hpp
        include/mesh.hpp
        include/object3d.hpp
        include/plane.hpp
        include/ray.hpp
        include/scene.hpp
        include/sphere.hpp
        include/transform.hpp
        include/triangle.hpp
        include/texture.hpp
        include/constant_medium.hpp
        include/random.hpp
        include/transformation.hpp
        src/transformation.cpp
        include/pdf.hpp
        include/quad.hpp
        include/hemisphere_pdf.hpp
        include/object_pdf.hpp
        src/object_pdf.cpp
        include/mixture_pdf.hpp
        include/cosine_pdf.hpp
        include/interval.hpp
        include/aabb.hpp
        include/bvh_node.hpp
        include/surface.hpp
        include/curve.hpp
        include/bernstein.hpp
        include/scene_provider.hpp
)

SET(CMAKE_CXX_STANDARD 11)

ADD_EXECUTABLE(${PROJECT_NAME} ${PA1_SOURCES} ${PA1_INCLUDES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} vecmath)
TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} PRIVATE include)
