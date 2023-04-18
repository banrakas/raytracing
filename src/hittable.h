#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "material.h"
#include "util.h"

struct hit_record {
    point3 point; // point of intersection
    vec3 normal;
    float t; // ray = origin + dir * t
    const material* material;
    bool frontface;
};

typedef struct {
    point3 centre;
    float radius;
} sphere;

#define sphere(x, y, z, r) ((sphere) { .centre = { { x, y, z } }, .radius = r })

bool sphere_hit(const sphere* sp, const ray* r,
    float t_min, float t_max, struct hit_record* hit_record);

enum primitive_type {
    SPHERE,
};

union primitives {
    sphere sphere;
};

typedef struct {
    enum primitive_type type;
    union primitives object;
} hittable;

typedef struct {
    hittable* objects;
    size_t len;
    size_t cap;
} hittable_list;

hittable_list hittable_list_new(size_t cap);

void hittable_list_push(hittable_list* lst, const enum primitive_type type, const union primitives obj);

void hittable_list_destroy(hittable_list* lst);

bool is_hit(const ray* restrict r, const hittable_list* restrict lst,
    float t_min, float t_max, struct hit_record* restrict rec);
