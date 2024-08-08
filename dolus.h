#ifndef _DOLUS_H
#define _DOLUS_H

#include "dolus_math.h"

typedef struct
{
    v4 origin;
    
    // NOTE: unit vector
    v4 direction;
} Ray;

typedef struct
{
    v4 center;
    f32 radius;
    m4x4 transform;
} Sphere;

typedef struct
{
    bool hit;
    f32 t1, t2;
} Tvalue;

extern inline v4 ray_position(Ray ray, f32 t)
{
    v4 result = {};
    result = v4_add(ray.origin, v4_scalar_mul(ray.origin, t));
    return(result);
}

extern inline void transform_ray(m4x4 matrix, Ray *r)
{
    r->origin = v4_transform(matrix, r->origin);
    r->direction = v4_transform(matrix, r->direction);
}

extern inline void set_sphere_transform(Sphere *s, m4x4 transform)
{
    s->transform = transform;
}

extern inline Tvalue ray_intersect_sphere(Ray ray, Sphere s)
{
    Tvalue result = {};

    m4x4 inverted_transform = {};
    m4x4_invert(s.transform, &inverted_transform);
    transform_ray(inverted_transform, &ray);
    
    v4 sphere_to_ray = v4_sub(ray.origin, s.center);
    
    f32 a = dot_v4(ray.direction, ray.direction);
    f32 b = 2 * dot_v4(ray.direction, sphere_to_ray);
    f32 c = dot_v4(sphere_to_ray, sphere_to_ray) - 1;
    f32 discriminant = square(b) - (4 * a * c);

    if(discriminant < 0)
    {
        result.hit = false;
    }
    else
    {
        result.hit = true;
        result.t1 = (- b - square_root(discriminant)) / (2 * a);
        result.t2 = (- b + square_root(discriminant)) / (2 * a);
    }
    return(result);
}

extern inline Sphere sphere(v3 center, f32 radius)
{
    Sphere result = {};
    result.center = Point(center.x, center.y, center.z);
    result.radius = 1;
    result.transform = m4x4_identity();
    return(result);
}

#endif
