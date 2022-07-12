#pragma once

#include <windows.h>
#include <immintrin.h>
#include <stdint.h>
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image.h>
#include <stb_image_resize.h>
#include <stb_image_write.h>

#define SETTINGSBUFFERSIZE 100

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef int32_t bool32;


inline float
NormalizeBetween(float X, float MinX, float MaxX, float A, float B)
{
	float Result = ((B - A) * ((X - MinX) / (MaxX - MinX)) + A);
	return Result;
}

struct render_buffer
{
    u8* Memory;
    int Width;
    int Height;
    int Pitch;
    int BytesPerPixel;
};

union RGBA
{
	unsigned int Color;
	struct {
		unsigned char r;
		unsigned char g;
		unsigned char b;
		unsigned char a;
	};
};


void
DrawBackground(unsigned char *Pixels, int Width, int Height);