#include "camera.h"

camera camera_new(float focal_length, vec3 horizontal, vec3 vertical, point3 origin)
{
    return (camera) {
        .focal_length = focal_length,
        .horizontal = horizontal,
        .vertical = vertical,
        .origin = origin,
        .lower_left_corner = add(
            add(origin, vec3(0.0, 0.0, -focal_length)),
            add(scalarmul(horizontal, -0.5), scalarmul(vertical, -0.5))),
    };
}
