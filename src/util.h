#pragma once

#include <math.h>
#include <stdlib.h>

typedef float f32;
typedef double f64;

typedef union {
    float data[3];
    struct {
        float x;
        float y;
        float z;
    };
    struct {
        float r;
        float g;
        float b;
    };
} vec3;

typedef vec3 point3;

/* Colour ought to be b/w 0 and 1 */
typedef vec3 color3;

#define vec3(x, y, z) ((vec3) { .data = { x, y, z } })
#define point3(x, y, z) vec3(x, y, z)
#define color3(r, g, b) vec3(r, g, b)

inline vec3 add(vec3 a, vec3 b)
{
    return (vec3) { .data = { a.x + b.x, a.y + b.y, a.z + b.z } };
}

inline vec3 sub(vec3 a, vec3 b)
{
    return (vec3) { .data = { a.x - b.x, a.y - b.y, a.z - b.z } };
}

inline float dot(vec3 a, vec3 b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

inline vec3 cross(vec3 a, vec3 b)
{
    return (vec3) {
        .data = { a.y * b.z - b.y * a.z, b.x * a.z - a.x * b.z, a.x * b.y - b.x * a.y }
    };
}

inline vec3 scalarmul(vec3 a, float t)
{
    return (vec3) {
        .data = { a.x * t, a.y * t, a.z * t }
    };
}

inline float lensquared(vec3 v)
{
    return v.x * v.x + v.y * v.y + v.z * v.z;
}

inline float len(vec3 v)
{
    return sqrtf(lensquared(v));
}

inline vec3 unit_vector(vec3 v)
{
    float l = len(v);
    return (vec3) {
        .x = v.x / l,
        .y = v.y / l,
        .z = v.z / l,
    };
}

typedef struct {
    point3 origin;
    vec3 dir;
} ray;

inline point3 ray_at(ray r, float t)
{
    return add(r.origin, scalarmul(r.dir, t));
}

inline float degree_to_radian(float degree)
{
    return M_PI * degree / 180;
}

inline float randomf()
{
    return (float)rand() / RAND_MAX;
}

inline float clamp(float x, float min, float max)
{
    if (x < min)
        return min;
    if (x > max)
        return max;
    return x;
}
