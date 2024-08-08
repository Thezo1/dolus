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

int main(int argc, char *argv[])
{
    // v3 BackgroundColor = V3(0.2f, 0.3f, 0.5f);
    v3 BackgroundColor = V3(0.0f, 0.0f, 0.0f);
    v3 color1 = V3(0.9, 0.6, 0.75);
    v3 color2 = V3(0.7, 0.1, 0.25);
    v3 color3 = v3_mul(color1, color2);

    ImageU32 image = {};
    image.width = 1290;
    image.height = 720;
    
    Sphere s = sphere(V3(0.0f, 0.0f, 0.0f), 1);
    s.material.color = color1;
    
    PointLight light = {0};
    light.position = Point(-10.0f, 10.0f, -10.0f);
    light.intensity = V3(1.0f, 1.0f, 1.0f);
    
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

    // NOTE: The y axis is flipped, that's why I am using top to bottom in y
    u32 *Out = image.pixels;
    for( int y = image.height - 1;
         y > 0;
         --y)
    {
        for( int x = 0;
             x < image.width;
             ++x)
        {
            f32 world_x = -half_wall_width + pixel_width * x;
            f32 world_y = half_wall_heigth - pixel_height * y;
            v4 position = Point(world_x, world_y, wall_z);

            Ray r = {};
            r.origin = ray_origin;
            r.direction = v4_normalize(v4_sub(position, ray_origin));

            Tvalue t = ray_intersect_sphere(r, s);

            if(t.hit)
            {
                f32 tmin = 0.0f;
                if((t.t1 > 0) && (t.t1 < t.t2))
                {
                    tmin = t.t1;
                }
                else if((t.t2 > 0) && (t.t2 < t.t1))
                {
                    tmin = t.t2;
                }

                v4 point = ray_position(r, tmin);
                v4 normal = normal_at_point(s, point);
                v4 eye = v4_neg(r.direction);
                v3 color = lightning(s.material, light, point, eye, normal);
                
                *Out++ = pack_color_little(color);
            }
            else
            {
                *Out++ = pack_color_little(BackgroundColor);
            }
        }
    }

    save_to_bpm(image, "output.bmp");
    // save_to_ppm(image, "output.ppm");
    
    printf("Hello Dolus\n");
    return(0);
}
