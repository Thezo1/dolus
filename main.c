#include<stdio.h>
#include<stdint.h>

#define internal static

typedef uint8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef  float f32;
typedef  double f64;

#include "dolus_math.h"
#include "dolus.h"

#pragma pack(push, 1)
typedef struct BitMapHeader
{
    u16 FileType;     
    u32 FileSize;     
    u16 Reserved1;    
    u16 Reserved2;    
    u32 BitmapOffset;
    u32 Size;
    i32 Width;
    i32 Height;
    u16 Planes;
    u16 BitsPerPixel;
    u32 Compression;
	u32 SizeOfBitmap;
	i32 HorzResolution;
	i32 VertResolution; 
	u32 ColorsUsed;
	u32 ColorsImportant;

}BitMapHeader;
#pragma pack(pop)

typedef struct ImageU32
{
    u32 width, height;
    u32 *pixels;
} ImageU32;

internal u32 get_pixel_size(ImageU32 image)
{
    return(sizeof(u32)*image.width*image.height);
}

internal void save_to_bpm(ImageU32 image, char *filename)
{
    BitMapHeader Header = {};
    
    u32 OutputPixelSize = get_pixel_size(image);
    Header.FileType = 0x4D42;     
    Header.FileSize = sizeof(Header) + OutputPixelSize;     
    Header.BitmapOffset = sizeof(Header);
    Header.Size = sizeof(Header) - 14;
    Header.Width = image.width;
    Header.Height = image.height;
    Header.Planes = 1;
    Header.BitsPerPixel = 32;
    Header.Compression = 0;
    Header.SizeOfBitmap = OutputPixelSize;
    Header.HorzResolution = 0;
	Header.VertResolution = 0; 
    Header.ColorsUsed = 0;
    Header.ColorsImportant = 0;

    FILE *OutFile;
    OutFile = fopen(filename, "wb");
    if(OutFile)
    {
        fwrite(&Header, sizeof(Header), 1, OutFile);
        fwrite(image.pixels, OutputPixelSize, 1, OutFile);
        fclose(OutFile);
    }
    else
    {
        fprintf(stderr, "[Error] Unable to wirte to file %s\n", filename);
        exit(1);
    }
}

void save_to_ppm(ImageU32 image, const char *filename)
{
    FILE *file;
    file = fopen(filename, "wb");
    if(!file)
    {
        printf("Cannot open file");
        exit(1);
    }

    fprintf(file, "P6\n%d %d\n255\n", image.width, image.height);
    for(int y = 0; 
            y < image.height; 
            ++y)
    {
        for(int x = 0; 
                x < image.width; 
                ++x)
        {

            // TODO: try alpha blending
            u32 pixel = image.pixels[y * image.width + x];
            u8 bytes[3] = 
            {
                (pixel & 0x0000ff) >> 8*0,
                (pixel & 0x00ff00) >> 8*1,
                (pixel & 0xff0000) >> 8*2,
            };
            fwrite(bytes, sizeof(bytes), 1, file);

        }

    }
    fclose(file);
}

internal Computation prepare_computation(World *world, X intersection, Ray *ray)
{
    Computation result = {};
    result.t = intersection.t;
    result.object_index = intersection.object_index;
    
    Sphere sphere = world->spheres[result.object_index];
    
    result.point = ray_position(*ray, result.t);
    result.eyev = v4_neg(ray->direction);
    result.normalv = normal_at_point(sphere, result.point);
    result.over_point = v4_add(result.point, (v4_scalar_mul(result.normalv, EPSILON)));    
    if(v4_dot(result.normalv, result.eyev) < 0)
    {
        result.inside = true;
        result.normalv = v4_neg(result.normalv);
    }
    else
    {
        result.inside = false;
    }
    return(result);
}

internal WorldIntersects intersect_world(World *world, Ray *ray)
{
    WorldIntersects result = {};
    result.intersect_count = 0;

    // NOTE: Spheres are always first in the world
    for(int sphere_index = 0;
        sphere_index < world->sphere_count;
        ++sphere_index)
    {
        Sphere sphere = world->spheres[sphere_index];
        Tvalue t = ray_intersect_sphere(*ray, sphere);

        if(t.hit)
        {
            X t_value = {};
            if(t.t1 > EPSILON)
            {
                t_value.t = t.t1;
                t_value.object_index = sphere_index;
                result.t_values[result.intersect_count++] = t_value;
                if(t.t2 > EPSILON)
                {
                    t_value.t = t.t2;
                    t_value.object_index = sphere_index;
                    result.t_values[result.intersect_count++] = t_value;
                }
            }
            else if(t.t2 > EPSILON)
            {
                t_value.t = t.t2;
                t_value.object_index = sphere_index;
                result.t_values[result.intersect_count++] = t_value;                
            }
        }
    }

    return(result);
}

internal v3 lightning(World *world, Material material, v4 point, v4 eyev, v4 normalv)
{
    v3 diffuse = {0.0f, 0.0f, 0.0f};
    v3 specular = {0.0f, 0.0f, 0.0f};
    v3 ambient = {0.0f, 0.0f, 0.0f};

    for(int light_index = 0;
        light_index < world->light_count;
        ++light_index)
    {
        PointLight light = world->lights[light_index];
        
        /// NOTE: test for shadows
        v4 V = v4_sub(light.position, point);
        f32 distance = v4_length(V);
        v4 direction = v4_normalize(V);

        Ray r = {};
        r.origin = point;
        r.direction = direction;

        bool is_shadowed = false;
        WorldIntersects xs = intersect_world(world, &r);
        if(xs.intersect_count > 0)
        {
            f32 lowest_so_far = FLT_MAX;
            for(int intersect_index = 0;
                intersect_index < xs.intersect_count;
                ++intersect_index)
            {
                // TODO: when I begin to check for -ve hits, refactor?
                if(xs.t_values[intersect_index].t < lowest_so_far)
                {
                    lowest_so_far = xs.t_values[intersect_index].t;
                }
            }

            if(lowest_so_far < distance)
            {
                is_shadowed = true;
            }
            else
            {
                is_shadowed = false;
            }
        }

        v3 effective_color = v3_mul(material.color, light.intensity);
        ambient = v3_add(ambient, v3_scalar_mul(effective_color, material.ambient));
        if(!is_shadowed)
        {
            v4 lightv = v4_normalize(v4_sub(light.position, point));
            f32 light_dot_normal = v4_dot(lightv, normalv);
    
            if(light_dot_normal < 0)
            {
                // do nothing
            }
            else
            {
                diffuse = v3_add(diffuse, v3_scalar_mul(effective_color, (material.diffuse * light_dot_normal)));
                v4 reflectv = v4_reflect(v4_neg(lightv), normalv);
                f32 reflect_dot_eye = v4_dot(reflectv, eyev);
                if(reflect_dot_eye <= 0)
                {
                    // do nothing
                }
                else
                {
                    f32 factor = POW(reflect_dot_eye, material.shininess);
                    specular = v3_add(specular, v3_scalar_mul(light.intensity, material.specular * factor));
                }
            }
        }
    }

    v3 result = v3_add(ambient, v3_add(diffuse, specular));
    return(result);
}

int main(int argc, char *argv[])
{
    // v3 BackgroundColor = V3(0.2f, 0.3f, 0.5f);
    v3 BackgroundColor = V3(0.0f, 0.0f, 0.0f);
    v3 color1 = V3(0.9, 0.6, 0.75);
    v3 color2 = V3(0.7, 0.1, 0.25);
    v3 color3 = v3_mul(color1, color2);

    ImageU32 image = {};
    image.width = 1280;
    image.height = 750;
    // image.width = 100;
    // image.height = 50;

    
    // scene
    m4x4 transform = m4x4_scale_matrix(V3(10.0f, 0.01f, 10.0f));
    
    Sphere floor = sphere(origin(), 1.0f);
    set_sphere_transform(&floor, transform);
    floor.material.color = V3(1.0f, 0.9f, 0.9f);
    floor.material.specular = 0.0f;

    Sphere left_wall = sphere(origin(), 1.0f);
    m4x4 translate = m4x4_translation_matrix(V3(0.0f, 0.0f, 5.0f));
    m4x4 rotationY = m4x4_rotateY_matrix(-PI32/4);
    m4x4 rotationX = m4x4_rotateX_matrix(PI32/2);
    
    m4x4 scale = m4x4_scale_matrix(V3(10.0f, 0.01f, 10.0f));

    transform = m4x4_mul(translate, m4x4_mul(rotationY, m4x4_mul(rotationX, scale)));
    set_sphere_transform(&left_wall, transform);
    left_wall.material = floor.material;

    Sphere right_wall = sphere(origin(), 1.0f);
    translate = m4x4_translation_matrix(V3(0.0f, 0.0f, 5.0f));
    rotationY = m4x4_rotateY_matrix(PI32/4);
    rotationX = m4x4_rotateX_matrix(PI32/2);
    scale = m4x4_scale_matrix(V3(10.0f, 0.01f, 10.0f));
    
    transform = m4x4_mul(translate, m4x4_mul(rotationY, m4x4_mul(rotationX, scale)));
    set_sphere_transform(&right_wall, transform);
    right_wall.material = floor.material;

    Sphere middle = sphere(origin(), 1.0f);
    middle.material.color = V3(1.0f, 0.435f, 0.380f);
    middle.material.diffuse = 0.7f;
    middle.material.specular = 0.3f;
    transform = m4x4_translation_matrix(V3(-0.5f, 1.0f, 0.5f));
    set_sphere_transform(&middle, transform);

    Sphere right = sphere(origin(), 1.0f);
    right.material.color = V3(0.816f, 0.549f, 0.549f);
    right.material.diffuse = 0.7f;
    right.material.specular = 0.3f;
    
    transform = m4x4_mul(m4x4_translation_matrix(V3(1.5f, 0.5f, 0.5f)), m4x4_scale_matrix(V3(0.5f, 0.5f, 0.5f)));
    set_sphere_transform(&right, transform);

    Sphere left = sphere(origin(), 1.0f);
    left.material.color = V3(0.98f, 0.50f, 0.45f);
    left.material.diffuse = 0.7f;
    left.material.specular = 0.3f;
    
    transform = m4x4_mul(m4x4_translation_matrix(V3(-1.5f, 0.33f, -0.75f)), m4x4_scale_matrix(V3(0.33f, 0.33f, 0.33f)));
    set_sphere_transform(&left, transform);    
    
    PointLight light1 = {0};
    light1.position = Point(-10.0f, 10.0f, -10.0f);
    light1.intensity = V3(1.0f, 1.0f, 1.0f);

    PointLight light2 = {0};
    light2.position = Point(10.0f, 10.0f, -10.0f);
    light2.intensity = V3(0.35f, 0.2f, 0.35f);

    Sphere spheres[6] = {};
    spheres[0] = floor;
    spheres[1] = left_wall;
    spheres[2] = right_wall;
    spheres[3] = middle;
    spheres[4] = right;
    spheres[5] = left;

    PointLight lights[2] = {};
    lights[0] = light1;
    lights[1] = light2;

    World world = {};
    world.object_count = 6;
    world.sphere_count = 6;
    world.spheres = spheres;
    world.light_count = 2;
    world.lights = lights;
    
    // scene
    
    f32 wall_z = 10.0f;
    f32 wall_height = 7.0f;
    f32 wall_width = 7.0f;

    v4 ray_origin = Point(0.0f, 0.0f, -20.0f);

    if(image.width > image.height)
    {
        wall_height = wall_width * ((f32)image.height / (f32)image.width);
    }
    else if(image.height > image.width)
    {
        wall_width = wall_height * ((f32)image.width / (f32)image.height);
    }

    f32 pixel_width = wall_width / image.width;
    f32 pixel_height = wall_height / image.height;
    f32 half_wall_width = wall_width * 0.5;
    f32 half_wall_heigth = wall_height * 0.5;
    
    u32 OutputPixelSize = get_pixel_size(image);
    image.pixels = (u32 *)malloc(OutputPixelSize);

    v4 from = Point(0.0f, 1.5f, -5.0f);
    v4 to = Point(0.0f, 1.0f, 0.0f);
    v4 up = Vector(0.0f, 1.0f, 0.0f);
    m4x4 world_view_transform = view_transform(from, to, up);

    Camera cam = camera(image.width, image.height, PI32/3);
    cam.transform = world_view_transform;

    // NOTE: The y axis is flipped, that's why I am using top to bottom in y
    u32 *Out = image.pixels;
    printf("The rays are casting\n");
    for( int y = image.height - 1;
         y > 0;
         --y)
    {
        for( int x = 0;
             x < image.width;
             ++x)
        {
            f32 x_offset = (x + 0.5) * cam.pixel_size;
            f32 y_offset = (y + 0.5) * cam.pixel_size;

            f32 worldX = cam.half_width - x_offset;
            f32 worldY = cam.half_height - y_offset;

            m4x4 invert = {};
            m4x4_invert(cam.transform, &invert);
            v4 pixel = m4x4_mul_v4(invert, Point(worldX, worldY, -1));
            v4 origin = m4x4_mul_v4(invert, Point(0.0f, 0.0f, 0.0f));
            v4 direction = v4_normalize(v4_sub(pixel, origin));
            
            Ray r = {};
            r.origin = origin;
            r.direction = direction;

            WorldIntersects xs = intersect_world(&world, &r);
            if(xs.intersect_count > 0)
            {
                f32 lowest_so_far = FLT_MAX;
                int lowest_index = 0;
                for(int intersect_index = 0;
                    intersect_index < xs.intersect_count;
                    ++intersect_index)
                {
                    if(xs.t_values[intersect_index].t < lowest_so_far)
                    {
                        lowest_so_far = xs.t_values[intersect_index].t;
                        lowest_index = intersect_index;
                    }
                }
                Computation comp = prepare_computation(&world, xs.t_values[lowest_index], &r);
                    
                v4 point = comp.over_point;
                v4 normal = comp.normalv;
                v4 eye = comp.eyev;

                v3 color = lightning(&world, world.spheres[comp.object_index].material, point, eye, normal);
                *Out++ = pack_color_little(color);
            }
            else
            {
                *Out++ = pack_color_little(BackgroundColor);
            }
        }
        printf("\rThe rays are casting: row %d...   ", y);
    }

    save_to_bpm(image, "output.bmp");
    // save_to_ppm(image, "output.ppm");
    
    printf("\nHello Dolus\n");
    return(0);
}
