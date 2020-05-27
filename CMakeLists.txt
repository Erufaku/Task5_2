cmake_minimum_required(VERSION 3.15)


project(RayTracer)

set(CMAKE_CXX_STANDARD 14)



add_executable(RayTracer main.cpp Vec3f.cpp Vec3f.h Vec2f.cpp Vec2f.h functions.cpp functions.h AABBox.cpp AABbox.h Object.cpp Object.h Matrix44.h Matrix44.cpp CImg.h)
