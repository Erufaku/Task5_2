#include "AABBox.h"
bool AABBox::intersect(const Ray& r, float& t) const
{
    float tmin, tmax, tymin, tymax, tzmin, tzmax;
    tmin = (bounds[r.sign[0]].x - r.orig.x) * r.invdir.x;
    tmax = (bounds[1 - r.sign[0]].x - r.orig.x) * r.invdir.x;
    tymin = (bounds[r.sign[1]].y - r.orig.y) * r.invdir.y;
    tymax = (bounds[1 - r.sign[1]].y - r.orig.y) * r.invdir.y;

    if ((tmin > tymax) || (tymin > tmax))
        return false;

    if (tymin > tmin)
        tmin = tymin;
    if (tymax < tmax)
        tmax = tymax;

    tzmin = (bounds[r.sign[2]].z - r.orig.z) * r.invdir.z;
    tzmax = (bounds[1 - r.sign[2]].z - r.orig.z) * r.invdir.z;

    if ((tmin > tzmax) || (tzmin > tmax))
        return false;

    if (tzmin > tmin)
        tmin = tzmin;
    if (tzmax < tmax)
        tmax = tzmax;

    t = tmin;

    if (t < 0) {
        t = tmax;
        if (t < 0) return false;
    }

    return true;
}
Vec3f AABBox::getMassCenter() const {
    Vec3f MassCenter;
    MassCenter.x = (bounds[1].x + bounds[0].x) / 2;
    MassCenter.y = (bounds[1].y + bounds[0].y) / 2;
    MassCenter.z = (bounds[1].z + bounds[0].z) / 2;
    return MassCenter;
}