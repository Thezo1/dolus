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
    v3 color;
    f32 ambient;
    f32 diffuse;
    f32 specular;
    f32 shininess;
} Material;

typedef struct
{
    v4 center;
    f32 radius;
    m4x4 transform;
    Material material;
} Sphere;

typedef struct
{
    bool hit;
    f32 t1, t2;
} Tvalue;

typedef struct
{
    // NOTE: intensity = color
    v3 intensity;
    v4 position;
} PointLight;

typedef struct
{
    u32 object_count;
    
    u32 sphere_count;
    Sphere *spheres;

    u32 light_count;
    PointLight *lights;
    
} World;

typedef struct
{
    f32 t;
    int object_index;
} X;

typedef struct
{
    X t_values[256];
    int intersect_count;
} WorldIntersects;

typedef struct
{
    f32 t;
    int object_index;
    bool inside;
    v4 point, over_point;
    v4 eyev;
    v4 normalv;
} Computation;

typedef struct
{
    u32 h_size;
    u32 v_size;
    f32 field_of_view;
    f32 half_width, half_height;
    f32 pixel_size;
    m4x4 transform;
} Camera;

extern inline v4 ray_position(Ray ray, f32 t)
{
    v4 result = {};
    result = v4_add(ray.origin, v4_scalar_mul(ray.direction, t));
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
    
    f32 a = v4_dot(ray.direction, ray.direction);
    f32 b = 2 * v4_dot(ray.direction, sphere_to_ray);
    f32 c = v4_dot(sphere_to_ray, sphere_to_ray) - 1;
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

extern inline v4 normal_at_point(Sphere s, v4 Point)
{
    m4x4 invert = {};
    m4x4_invert(s.transform, &invert);
    v4 object_point = m4x4_mul_v4(invert, Point);
    v4 object_normal = v4_sub(object_point, s.center);
    
    m4x4 transpose = m4x4_transpose(invert);
    v4 world_normal = m4x4_mul_v4(transpose, object_normal);
    
    world_normal.w = 0;
    return(v4_normalize(world_normal));
}

extern inline Material material()
{
    Material result = {0};
    result.color = V3(1.0f, 1.0f, 1.0f);
    result.ambient = 0.1f;
    result.diffuse = 0.9f;
    result.specular = 0.9f;
    result.shininess = 200.0f;
    return(result);
}

extern inline Sphere sphere(v3 center, f32 radius)
{
    Sphere result = {0};
    result.center = Point(center.x, center.y, center.z);
    result.radius = 1;
    result.transform = m4x4_identity();
    result.material = material();
    return(result);
}


extern inline v3 origin()
{
    return(V3(0.0f, 0.0f, 0.0f));
}

extern inline Camera camera(u32 h_size, u32 v_size, f32 field_of_view)
{
    Camera result = {};
    result.h_size = h_size;
    result.v_size = v_size;
    
    result.transform = m4x4_identity();
    f32 half_view = TAN(field_of_view / 2);
    f32 aspect_ratio = (f32)h_size / (f32)v_size;
    if(aspect_ratio >= 1)
    {
        result.half_width = half_view;
        result.half_height = half_view / aspect_ratio;
    }
    else
    {
        result.half_width = half_view * aspect_ratio;
        result.half_height = half_view;
    }
    result.pixel_size = (result.half_width * 2)  / (f32)result.h_size;
    return(result);
}

#endif
