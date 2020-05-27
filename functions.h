#pragma once
#include <iostream>
#include <fstream>
#include <cmath>
#include <memory>
#include <string>
#include <chrono>
#include <sstream>
#include <map>
#include "AABBox.h"
#include "Object.h"
#include "Vec3f.h"
#include "Vec2f.h"
#include "Matrix44.h"
inline
float dotProduct(const Vec3f& a, const Vec3f& b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}
bool trace(const Ray& r, const std::vector<Object*>& objects, float& tNear, Object** hitObject, Options options);

Vec3f castRay(const Ray& r, const std::vector<Object*>& objects, const Options& options);

void render(const Options& options, const std::vector<Object*>& objects);

std::vector<Object*> GetFigures(std::string FileName, Options& options);

void autotest1();
void autotest2();
void autotest3();


