#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <SDL2/SDL.h>

#include "camera.h"
#include "hittable.h"
#include "material.h"
#include "util.h"

inline vec3 vector_random_inside_unit_sphere()
{
    vec3 dir = vec3(randomf(), randomf(), randomf());
    while (lensquared(dir) > 1)
        dir = vec3(randomf(), randomf(), randomf());
    return dir;
}

vec3 vector_unit_random1()
{
    return unit_vector(vector_random_inside_unit_sphere());
}

vec3 vector_unit_random2(const vec3* normal)
{
    vec3 out = vector_random_inside_unit_sphere();

    if (dot(out, *normal) > 0.0)
        return out;
    else
        return scalarmul(out, -1.0);
}

color3 ray_color(const ray* restrict r, const hittable_list* restrict lst, int depth)
{
    struct hit_record rec = {};

    if (is_hit(r, lst, 0.001, INFINITY, &rec) && depth >= 0) {
        vec3 newray_target = add(rec.point, vector_unit_random2(&rec.normal));
        // vec3 newray_target = add(add(rec.point, rec.normal), vector_unit_random1());
        return scalarmul(ray_color(&(ray) { .origin = rec.point, .dir = newray_target },
                             lst, depth - 1),
            0.5);
    }

    // Gives background color
    vec3 unit_dir = unit_vector(r->dir);
    float t = 0.5 * (unit_dir.y + 1.0);
    return add(scalarmul((color3) { .r = 1.0, .g = 1.0, .b = 1.0 }, 1.0 - t),
        scalarmul((color3) { .r = 0.5, .g = 0.7, .b = 1.0 }, t));
}

inline void write_color(FILE* out, color3 accumluted_color, uint32_t samples_per_pixel)
{
    float r = sqrtf(accumluted_color.r / samples_per_pixel); // Gamma correction
    float g = sqrtf(accumluted_color.g / samples_per_pixel);
    float b = sqrtf(accumluted_color.b / samples_per_pixel);

    fprintf(out, "%d %d %d\n",
        (uint8_t)(255.999 * clamp(r, 0.0, 1.0)),
        (uint8_t)(255.999 * clamp(g, 0.0, 1.0)),
        (uint8_t)(255.999 * clamp(b, 0.0, 1.0)));
}

int main(void)
{
    FILE* output = stdout;

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);

    // IMAGE
    const float aspect_ratio = 16.0 / 9.0;
    const uint16_t width = 1024;
    const uint16_t height = width / aspect_ratio;
    const uint32_t samples_per_pixel = 3;
    const int max_depth = 2;

    SDL_Window* window = SDL_CreateWindow("Banrakas", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, 0);
    SDL_Surface* surface = SDL_GetWindowSurface(window);
    uint32_t* pixels = surface->pixels;

    // World

    hittable_list world = hittable_list_new(5);
    hittable_list_push(&world, SPHERE, (union primitives) { sphere(0.1, -0.1, -1.3, 0.5) });
    hittable_list_push(&world, SPHERE, (union primitives) { sphere(0.0, -100.5, -1.0, 100) });
    hittable_list_push(&world, SPHERE, (union primitives) { sphere(-0.5, -0.1, -1.0, 0.4) });

    // Camera

    const float viewport_height = 2.0;
    const float viewport_width = viewport_height * aspect_ratio;

    camera camera = camera_new(1.0,
        vec3(viewport_width, 0.0, 0.0),
        vec3(0.0, viewport_height, 0.0),
        vec3(0.0, 0.0, 0.0));

    // Render

    do {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT)
                exit(0);
        }

        for (int32_t i = height - 1; i >= 0; --i) {
            for (int32_t j = 0; j < width; ++j) {
                color3 accumulated_color = color3(0.0, 0.0, 0.0);

                for (uint32_t k = 0; k < samples_per_pixel; ++k) {
                    const float u = (j + randomf()) / (width - 1);
                    const float v = (i + randomf()) / (height - 1);

                    ray r = camera_get_ray(camera, u, v);

                    accumulated_color = add(ray_color(&r, &world, max_depth), accumulated_color);
                }

                float r = sqrtf(accumulated_color.r / samples_per_pixel);
                float g = sqrtf(accumulated_color.g / samples_per_pixel);
                float b = sqrtf(accumulated_color.b / samples_per_pixel);

                pixels[i * width + j] = SDL_MapRGB(surface->format,
                    (uint8_t)(255.999 * clamp(r, 0.0, 1.0)),
                    (uint8_t)(255.999 * clamp(g, 0.0, 1.0)),
                    (uint8_t)(255.999 * clamp(b, 0.0, 1.0)));
            }
        }

        SDL_UpdateWindowSurface(window);
    } while (true);

    fprintf(output, "P3\n%d %d\n255\n", width, height);

    for (int16_t i = height - 1; i >= 0; --i) {
        fprintf(stderr, "lines remaining: %d\r", i);

        for (uint16_t j = 0; j < width; ++j) {
            color3 accumulated_color = color3(0.0, 0.0, 0.0);

            for (uint32_t k = 0; k < samples_per_pixel; ++k) {
                const float u = (j + randomf()) / (width - 1);
                const float v = (i + randomf()) / (height - 1);

                ray r = camera_get_ray(camera, u, v);

                accumulated_color = add(ray_color(&r, &world, max_depth), accumulated_color);
            }
            write_color(output, accumulated_color, samples_per_pixel);
        }
    }

    hittable_list_destroy(&world);

    return 0;
}