#pragma once
#include <Windows.h>
#include <Windows.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h> 

#ifndef RENDER_H
#define RENDER_H

#define PI 3.14159265

#define TOP_DOWN -1
#define BOTTOM_UP 1

struct v2 
{
public:
	float x;
	float y;

	v2() 
	{
		x = 0;
		y = 0;
	}

	v2(float x_coord, float y_coord)
	{
		x = x_coord;
		y = y_coord;
	}

	v2(int x_coord, int y_coord)
	{
		x = (float)x_coord;
		y = (float)y_coord;
	}

	v2 operator+(v2 vec)
	{
		return v2(vec.x + this->x, vec.y + this->y);
	}

	v2 operator+(float scalar)
	{
		return v2(scalar + this->x, scalar + this->y);
	}

	v2 operator*(float scalar)
	{
		return v2(scalar * this->x, scalar * this->y);
	}
};

struct win32_video_buffer
{
	BITMAPINFO bitMapInfo;
	void* memory;
	int width;
	int height;
	int pitch;
	int bytesPerPixel;
};


void InitBuffer(win32_video_buffer* buffer, int width, int height);
int DisplayBufferInWindow(const win32_video_buffer *buffer, HDC deviceCtx, int windowWidth, int windowHeight);
void FlipBuffer(win32_video_buffer *buffer, win32_video_buffer *tempBuffer);
void ClearBuffer(win32_video_buffer *buffer, uint8_t r, uint8_t g, uint8_t b);

// Draw "Primitives"
void PlotPixel(const win32_video_buffer *buffer, int x, int y, uint32_t color);
void DrawLine(const win32_video_buffer *buffer, v2 source, v2 dest, uint32_t color);
void DrawRectangle(const win32_video_buffer *buffer, v2 vertex1, v2 vertex2, v2 vertex3, v2 vertex4, uint32_t color);
void DrawRectangleFill(const win32_video_buffer *Source, uint32_t X, uint32_t Y, uint32_t Width, uint32_t Height,
	uint8_t R, uint8_t G, uint8_t B);
void DrawTriangle(const win32_video_buffer *buffer, v2 vertex1, v2 vertex2, v2 vertex3, uint32_t color);

// Draw helpful stuff
void DrawGrid(const win32_video_buffer *buffer, int cellLength);

// Transformations
void Translate(v2 *vertex, const v2 delta);
void Rotate(const v2 center, v2 *vertex, const float degrees);

bool WithinScreenBoundaries(int x, int y, int screenWidth, int screenHeight);
int Clamp(int value, int min, int max);
uint32_t AARRGGBB_Color(uint8_t a, uint8_t r, uint8_t g, uint8_t b);

#endif