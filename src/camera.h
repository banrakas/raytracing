#pragma once

#include "util.h"

typedef struct {
    vec3 horizontal, vertical;
    point3 origin;
    float focal_length;
    vec3 lower_left_corner;
} camera;

camera camera_new(float focal_length, vec3 horizontal, vec3 vertical, point3 origin);

inline ray camera_get_ray(const camera camera, float u, float v)
{
    return (ray) {
        .origin = camera.origin,
        .dir = add(
            sub(camera.lower_left_corner, camera.origin),
            add(scalarmul(camera.horizontal, u), scalarmul(camera.vertical, v))),
    };
}
