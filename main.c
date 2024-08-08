#include<stdio.h>
#include<stdint.h>

#include "dolus_math.h"
#include "dolus.h"

#define WIDTH 1290
#define HEIGHT 720

static u32 IMAGE[HEIGHT][WIDTH];

typedef float f32;
typedef double f64;

typedef uint32_t u32;
typedef uint8_t u8;

void save_to_ppm(const char *filename)
{
    FILE *file;
    file = fopen(filename, "wb");
    if(!file)
    {
        printf("Cannot open file");
        exit(1);
    }

    fprintf(file, "P6\n%d %d\n255\n", WIDTH, HEIGHT);
    for(int y = 0; 
            y < HEIGHT; 
            ++y)
    {
        for(int x = 0; 
                x < WIDTH; 
                ++x)
        {

            // TODO: try alpha blending
            u32 pixel = IMAGE[y][x];
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

int main(int argc, char *argv[])
{
    v3 BackgroundColor = V3(0.2f, 0.3f, 0.5f);
    v3 color1 = V3(0.9, 0.6, 0.75);
    v3 color2 = V3(0.7, 0.1, 0.25);
    v3 color3 = v3_mul(color1, color2);

    Sphere s = sphere(V3(0.0f, 0.0f, 0.0f), 1);

    f32 wall_z = 10.0f;
    f32 wall_height = 7.0f;
    f32 wall_width = 7.0f;

    v4 ray_origin = Point(0.0f, 0.0f, -20.0f);

    f32 width = WIDTH;
    f32 height = HEIGHT;

    if(width > height)
    {
        wall_height = wall_width * ((f32)height / (f32)width);
    }
    else if(height > width)
    {
        wall_width = wall_height * ((f32)width / (f32)height);
    }
    
    f32 pixel_width = wall_width / WIDTH;
    f32 pixel_height = wall_height / HEIGHT;
    f32 half_wall_width = wall_width * 0.5;
    f32 half_wall_heigth = wall_height * 0.5;
    
    for( int x = 0;
         x < WIDTH;
         ++x)
    {
        for( int y = 0;
             y < HEIGHT;
             ++y)
        {
            f32 world_x = -half_wall_width + pixel_width * x;
            f32 world_y = half_wall_heigth - pixel_height * y;
            v4 position = Point(world_x, world_y, wall_z);
            
            Ray r = {};
            r.origin = ray_origin;
            r.direction = normalize_v4(v4_sub(position, ray_origin));

            Tvalue t = ray_intersect_sphere(r, s);

            if(t.hit)
            {
                IMAGE[y][x] = pack_color_little(color1);
            }
            else
            {
                IMAGE[y][x] = pack_color_little(BackgroundColor);
            }
        }
    }

    save_to_ppm("output.ppm");
    
    printf("Hello Dolus\n");
    return(0);
}
