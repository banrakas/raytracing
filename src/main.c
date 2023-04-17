#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "util.h"

struct hit_record {
    point3 point; // point of intersection
    vec3 normal;
    float t; // ray = origin + dir * t
    bool frontface;
};

inline void set_face_normal(struct hit_record* rec, const ray* r, const vec3* outward_normal)
{
    rec->frontface = dot(r->dir, outward_normal) < 0.0;
    rec->normal = rec->frontface ? outward_normal : scalarmul(outward_normal, -1.0);
}

typedef struct {
    point3 centre;
    float radius;
} sphere;

bool sphere_hit(const sphere* sp, const ray* r,
    float t_min, float t_max, struct hit_record* hit_record)
{
    const vec3 oc = sub(r->origin, sp->centre);
    const float a = lensquared(r->dir);
    const float b_2 = dot(r->dir, oc);
    const float c = lensquared(oc) - sp->radius * sp->radius;

    const float discriminant = b_2 * b_2 - a * c;

    if (discriminant < 0)
        return false;

    const float sqrtd = sqrt(discriminant);
    float root = (-b_2 - sqrtd) / a;

    if (root < t_min || root > t_max) {
        root = (-b_2 + sqrtd) / a;

        if (root < t_min || root > t_max)
            return false;
    }

    hit_record->t = root;
    hit_record->point = ray_at(*r, root);

    set_face_normal(hit_record, r, &scalarmul(sub(hit_record->point, sp->centre), 1 / sp->radius));

    return true;
}

typedef struct {
    enum {
        SPHERE,
    } type;
    union {
        sphere sphere;
    };
} hittable;

typedef struct {
    hittable* objects;
    size_t len;
    size_t cap;
} hittable_list;

hittable_list hittable_list_new(size_t cap)
{
    hittable_list lst = {
        .objects = malloc(sizeof(hittable) * cap),
        .len = 0,
        .cap = cap
    };
    return lst;
}

void hittable_list_push(hittable_list* lst, const hittable obj)
{
    if (lst->len == lst->cap)
        lst->objects = realloc(lst->objects, sizeof(hittable) * (lst->len + 1));

    lst->objects[len] = obj;
    ++lst->len;
    ++lst->cap;
}

color3 ray_color(const ray* r)
{
    const sphere sp = { .centre = { .x = 0.3, .y = -0.45, .z = -2.0 }, .radius = 0.5 };
    struct hit_record rec = {};

    if (sphere_hit(&sp, r, 0, 1000, &rec))
        return scalarmul(add(rec.normal, (color3) { .data = { 1.0, 1.0, 1.0 } }), 0.5);

    // Gives background color
    vec3 unit_dir = unit_vector(r->dir);
    float t = 0.5 * (unit_dir.y + 1.0);
    return add(scalarmul((color3) { .r = 1.0, .g = 1.0, .b = 1.0 }, 1.0 - t),
        scalarmul((color3) { .r = 0.5, .g = 0.7, .b = 1.0 }, t));
}

inline void write_color(FILE* out, color3 color)
{
    fprintf(out, "%d %d %d\n",
        (uint8_t)(color.r * 255),
        (uint8_t)(color.g * 255),
        (uint8_t)(color.b * 255));
}

int main(void)
{
    FILE* output = stdout;

    // IMAGE
    const float aspect_ratio = 16.0 / 9.0;
    const uint16_t width = 1024;
    const uint16_t height = width / aspect_ratio;

    // Camera

    const float viewport_height = 2.0;
    const float viewport_width = viewport_height * aspect_ratio;
    const float focal_length = 1.0;

    const point3 origin = { .x = 0.0, .y = 0.0, .z = 0.0 };
    const vec3 horizontal = { .x = viewport_width, .y = 0.0, .z = 0.0 };
    const vec3 vertical = { .x = 0.0, .y = viewport_height, .z = 0.0 };
    const vec3 lower_left_corner = add(
        add(origin, (vec3) { .data = { 0.0, 0.0, -focal_length } }),
        add(scalarmul(horizontal, -0.5), scalarmul(vertical, -0.5)));

    // Render

    fprintf(output, "P3\n%d %d\n255\n", width, height);

    for (int16_t i = height - 1; i >= 0; --i) {
        fprintf(stderr, "scan lines remaining: %d\r", i);

        for (uint16_t j = 0; j < width; ++j) {
            const float u = (float)j / (width - 1);
            const float v = (float)i / (height - 1);

            ray r = {
                .origin = origin,
                .dir = add(
                    sub(lower_left_corner, origin),
                    add(scalarmul(horizontal, u), scalarmul(vertical, v)))
            };

            color3 color = ray_color(&r);
            write_color(output, color);
        }
    }

    return 0;
}