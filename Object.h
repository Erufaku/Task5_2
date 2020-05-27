#pragma once
#define _USE_MATH_DEFINES
#include <vector>
#include <iostream>
#include <fstream>
#include <cmath>
#include <memory>
#include <string>
#include <chrono>
#include <sstream>
#include <map>
#include "AABBox.h"
#define _USE_MATH_DEFINES
class Ray
{
public:
    Ray() { orig = 0; dir = 0; }
    Ray(const Vec3f& orig, const Vec3f& dir) : orig(orig), dir(dir)
    {
        invdir.x = 1 / dir.x;
        invdir.y = 1 / dir.y;
        invdir.z = 1 / dir.z;
        sign[0] = (invdir.x < 0);
        sign[1] = (invdir.y < 0);
        sign[2] = (invdir.z < 0);
    }
    Vec3f orig, dir; // ray orig and dir 
    Vec3f invdir;
    int sign[3];
};
class Object
{
public:
    int pos;
    std::string name = "";
    Object();
    virtual ~Object();
    virtual bool intersect(const Ray& r, float& t) const = 0;
    virtual Vec3f getMassCenter() const = 0;
};
class AABBox : public Object
{
public:
    Vec3f bounds[2];
    Vec3f massCenter;
    AABBox(const Vec3f& b0, const Vec3f& b1) { bounds[0] = b0, bounds[1] = b1; getMassCenter(); }
    bool intersect(const Ray& r, float& t) const;
    Vec3f getMassCenter() const;
};
struct Options
{
    int width;
    int height;
    float fov;
    Vec3f backgroundColor;
    float dist_to_screen;
    Vec3f vup;
    Vec3f camera;
    Vec3f normal;
    Vec3f right;
    Matrix44 cameraToWorld;
    float max_dist;
    Vec3f target;
    std::string output_file;
    int num_objects;
    std::map <float, Object*> distances;
};