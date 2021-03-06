#ifndef M_PLANE_HDR
#define M_PLANE_HDR
#include "m_vec.h"

namespace m {

struct quat;
struct perspective;

enum pointPlane {
    kPointPlaneBack,
    kPointPlaneOnPlane,
    kPointPlaneFront
};

struct plane {
    constexpr plane()
        : d(0.0f)
    {
    }

    plane(const vec3 &p1, const vec3 &p2, const vec3 &p3) {
        setupPlane(p1, p2, p3);
    }

    plane(const vec3 &pp, const vec3 &nn) {
        setupPlane(pp, nn);
    }

    plane(float a, float b, float c, float d) {
        setupPlane(a, b, c, d);
    }

    plane(const vec3 &nn, float dd)
        : n(nn)
        , d(dd)
    {
    }

    void setupPlane(const vec3 &p1, const vec3 &p2, const vec3 &p3) {
        n = (p2 - p1) ^ (p3 - p1);
        n.normalize();
        d = -n * p1;
    }

    void setupPlane(const vec3 &pp, vec3 nn) {
        n = nn;
        n.normalize();
        d = -n * pp;
    }

    void setupPlane(const vec3 &nn, float dd) {
        n = nn;
        d = dd;
    }

    void setupPlane(float a, float b, float c, float dd) {
        n = vec3(a, b, c);
        d = dd / n.abs();
        n.normalize();
    }

    vec3 project(const vec3 &point) const {
        float distance = getDistanceFromPlane(point);
        return vec3(point.x - n.x * distance,
                    point.y - n.y * distance,
                    point.z - n.z * distance);
    }

    bool getIntersection(float *f, const vec3 &p, const vec3 &v) const {
        const float q = n * v;
        // Plane and line are parallel?
        if (m::abs(q) < kEpsilon)
            return false;

        *f = -(n * p + d) / q;
        return true;
    }

    float getDistanceFromPlane(const vec3 &p) const {
        return p * n + d;
    }

    pointPlane classify(const vec3 &p, float epsilon = kEpsilon) const {
        const float distance = getDistanceFromPlane(p);
        if (distance > epsilon)
            return kPointPlaneFront;
        else if (distance < -epsilon)
            return kPointPlaneBack;
        return kPointPlaneOnPlane;
    }

    bool isPlaneBetween(const vec3 &a, const vec3 &b) const {
        const float q = n * (b - a);
        if (m::abs(q) < kEpsilon)
            return true;

        const float t = -(n * a + d) / q;
        return (t > -kEpsilon && t < 1.0f + kEpsilon);
    }

    vec3 n;
    float d;
};

struct frustum {
    void setup(const m::vec3 &origin, const m::quat &orient, const m::perspective &project);
    bool testSphere(const m::vec3 &point, float radius) const;
private:
    enum {
        kPlaneNear,
        kPlaneLeft,
        kPlaneRight,
        kPlaneUp,
        kPlaneDown,
        kPlaneFar,
        kPlanes
    };
    plane m_planes[kPlanes];
};

inline bool frustum::testSphere(const m::vec3 &point, float radius) const {
    radius = -radius;
    for (size_t i = 0; i < 6; i++)
        if (m_planes[i].getDistanceFromPlane(point) < radius)
            return false;
    return true;
}

}

#endif
