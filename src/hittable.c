#include <stdio.h>
#include <stdlib.h>

#include "hittable.h"

static inline void set_face_normal(struct hit_record* rec,
    const ray* r, const vec3 outward_normal)
{
    rec->frontface = dot(r->dir, outward_normal) < 0.0;
    rec->normal = rec->frontface ? outward_normal : scalarmul(outward_normal, -1.0);
}

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

    set_face_normal(hit_record, r, scalarmul(sub(hit_record->point, sp->centre), 1 / sp->radius));

    return true;
}

bool is_hit(const ray* restrict r, const hittable_list* restrict lst,
    float t_min, float t_max, struct hit_record* restrict rec)
{
    bool hit_atleast_once = false;
    bool hit = false;
    float closest_hit_distance = t_max;
    struct hit_record tmp = { .t = NAN };

    for (size_t i = 0; i < lst->len; ++i) {
        switch (lst->objects[i].type) {
        case SPHERE:
            hit = sphere_hit(&lst->objects[i].object.sphere, r, t_min, closest_hit_distance, &tmp);
            break;
        default:
            fputs("ERROR: undefined hittable object", stderr);
            abort();
        }

        if (hit) {
            closest_hit_distance = tmp.t;
            hit_atleast_once = true;
            *rec = tmp;
        }
    }

    return hit_atleast_once;
}

hittable_list hittable_list_new(size_t cap)
{
    hittable_list lst = {
        .objects = malloc(sizeof(hittable) * cap),
        .len = 0,
        .cap = cap
    };
    return lst;
}

void hittable_list_push(hittable_list* lst, const enum primitive_type type, const union primitives obj)
{
    const size_t len = lst->len;
    if (len == lst->cap)
        lst->objects = realloc(lst->objects, sizeof(hittable) * (len + 1));

    lst->objects[len].type = type;
    lst->objects[len].object = obj;
    ++lst->len;
    ++lst->cap;
}

void hittable_list_destroy(hittable_list* lst)
{
    free(lst->objects);
    lst->len = 0;
    lst->cap = 0;
}
