#include "render.h"

int Clamp(int value, int min, int max)
{
	if (value > max)
	{
		return max;
	}
	else if (value < min)
	{
		return min;
	}
	return value;
}

void ClearBuffer(win32_video_buffer *buffer, uint8_t r, uint8_t g, uint8_t b)
{
	uint32_t color = (r << 16) | (g << 8) | (b);

	uint8_t* row = (uint8_t*)buffer->memory;

	// from top to bottom
	for (int y = 0; y < buffer->height; y++)
	{
		uint32_t* pixel = (uint32_t*)row;
		// from left to right
		for (int x = 0; x < buffer->width; x++)
		{
			*pixel = color;

			// increment pixel
			pixel++;
		}

		// increment row
		row += buffer->pitch;
	}
}

uint32_t AARRGGBB_Color(uint8_t a, uint8_t r, uint8_t g, uint8_t b)
{
	return (a << 24) | (r << 16) | (g << 8) | (b);
}

void PlotPixel(const win32_video_buffer *buffer, int x, int y, uint32_t color)
{
	uint8_t *row = (uint8_t*)buffer->memory; // first row
	row += buffer->pitch * y; // offset in y to where the line starts

	uint32_t *pixel = (uint32_t *)row;
	pixel += x; // offset in x to where the line starts

	*pixel = color;
}

bool WithinScreenBoundaries(int x, int y, int screenWidth, int screenHeight)
{
	if (x < 0 || x >= screenWidth || y < 0 || y >= screenHeight)
	{
		return false;
	}
	return true;
}

void DrawLine(const win32_video_buffer *buffer, v2 source, v2 dest, uint32_t color)
{		
	int x1 = (int)source.x;
	int y1 = (int)source.y;
	int x2 = (int)dest.x;
	int y2 = (int)dest.y;

	float slope = ((float)y2 - (float)y1) / ((float)x2 - (float)x1);

	int x = x1;
	int y = y1;

	int dx = abs(x2 - x1);
	int dy = abs(y2 - y1);

	if (dx > dy)  // x-dominant line
	{
		for (int offset = 0; offset < dx; offset++)
		{
			if (x2 - x1 > 0)
			{
				y = slope * ((x + offset) - x1) + y1;
				if (WithinScreenBoundaries((x + offset), y, buffer->width, buffer->height))
				{
					PlotPixel(buffer, (x + offset), y, color);
				}				
			}
			else if (x2 - x1 < 0)
			{
				y = slope * ((x - offset) - x1) + y1;
				if (WithinScreenBoundaries((x - offset), y, buffer->width, buffer->height))
				{
					PlotPixel(buffer, (x - offset), y, color);
				}
			}
		}
	}
	else if (dy > dx)  // y-dominant line
	{
		for (int offset = 0; offset < dy; offset++)
		{
			if (y2 - y1 > 0)
			{
				x = ((y + offset) - y1) / slope + x1;
				if(WithinScreenBoundaries(x, y + offset, buffer->width, buffer->height))
				{
					PlotPixel(buffer, x, (y + offset), color);
				}
				
			}
			else if (y2 - y1 < 0)
			{
				x = ((y - offset) - y1) / slope + x1;
				if (WithinScreenBoundaries(x, y - offset, buffer->width, buffer->height))
				{
					PlotPixel(buffer, x, (y - offset), color);
				}
			}
		}
	}
	else if (dx == dy)  // 45 degree angle line
	{
		for (int offset = 0; offset < dx; offset++)
		{
			if (x2 - x1 > 0 && y2 - y1 > 0)
			{
				if (WithinScreenBoundaries(x + offset, y + offset, buffer->width, buffer->height))
				{
					PlotPixel(buffer, x + offset, y + offset, color);  // Q1
				}				
			}
			else if (x2 - x1 < 0 && y2 - y1 > 0)
			{
				if (WithinScreenBoundaries(x - offset, y + offset, buffer->width, buffer->height))
				{
					PlotPixel(buffer, x - offset, y + offset, color);  // Q2
				}
			}
			else if (y2 - y1 < 0 && x2 - x1 < 0)
			{
				if (WithinScreenBoundaries(x - offset, y - offset, buffer->width, buffer->height))
				{
					PlotPixel(buffer, x - offset, y - offset, color);  // Q3
				}
			}
			else if (y2 - y1 < 0 && x2 - x1 > 0)
			{
				if (WithinScreenBoundaries(x + offset, y - offset, buffer->width, buffer->height))
				{
					PlotPixel(buffer, x + offset, y - offset, color);  // Q4
				}
			}
		}
	}
}

void DrawGrid(const win32_video_buffer *buffer, int cellLength)
{
	uint8_t* row = (uint8_t*)buffer->memory;

	// from top to bottom
	for (int y = 0; y < buffer->height; y++)
	{
		uint32_t* pixel = (uint32_t*)row;
		// from left to right
		for (int x = 0; x < buffer->width; x++)
		{
			uint8_t Blue = 255;
			if (x % cellLength == 0 || y % cellLength == 0)
			{
				// "draw" data into pixel pointer
				*pixel = Blue;
			}

			// increment pixel 
			pixel++;
		}

		// increment row
		row += buffer->pitch;
	}
}

void Rotate(const v2 center, v2 *vertex, const float degrees)
{
	float x = vertex->x - center.x;
	float y = vertex->y - center.y;
	float rad = degrees * PI / 180;

	vertex->x = (cos(rad) * x - sin(rad) * y) + center.x;
	vertex->y = (sin(rad) * x + cos(rad) * y) + center.y;
}

void Translate(v2 *vertex, const v2 delta)
{
	vertex->x += delta.x;
	vertex->y += delta.y;
}

void DrawTriangle(const win32_video_buffer *buffer, v2 vertex1, v2 vertex2, v2 vertex3, uint32_t color)
{
	DrawLine(buffer, vertex1, vertex2, color);
	DrawLine(buffer, vertex2, vertex3, color);
	DrawLine(buffer, vertex3, vertex1, color);
}

void DrawRectangle(const win32_video_buffer *buffer, v2 vertex1, v2 vertex2, v2 vertex3, v2 vertex4, uint32_t color)
{
	DrawLine(buffer, vertex1, vertex2, color);
	DrawLine(buffer, vertex2, vertex3, color);
	DrawLine(buffer, vertex3, vertex4, color);
	DrawLine(buffer, vertex4, vertex1, color);
}

// Don't need this, if the buffer is already bottom-up
void FlipBuffer(win32_video_buffer *buffer, win32_video_buffer *tempBuffer)
{
	int bytesPerPixel = 4;
	int sizeOfBuffer = buffer->width * buffer->height * bytesPerPixel;

	int pitch = buffer->pitch;
	int width = buffer->width;
	int height = buffer->height;

	// Copy upside-down version of main buffer to temp buffer
	uint8_t *srcRow = (uint8_t *)buffer->memory;									// first row
	uint8_t *destRow = (uint8_t *)tempBuffer->memory + ((height - 1)  * pitch);		// last row

	for (int y = 0, z = height; y < height; y++, z--)
	{
		uint32_t *srcPixel = (uint32_t *)srcRow;
		uint32_t *destPixel = (uint32_t *)destRow;

		for (int x = 0; x < width; x++)
		{
			*destPixel = *srcPixel;

			srcPixel++;
			destPixel++;
		}

		srcRow += pitch;
		destRow -= pitch;
	}

	// copy temp buffer to main buffer
	memcpy(buffer->memory, tempBuffer->memory, sizeOfBuffer);
}


int DisplayBufferInWindow(const win32_video_buffer *buffer,HDC deviceCtx, int windowWidth, int windowHeight)
{
	return StretchDIBits(deviceCtx,
		0, 0, windowWidth, windowHeight,
		0, 0, buffer->width, buffer->height,
		buffer->memory, &buffer->bitMapInfo, DIB_RGB_COLORS, SRCCOPY);
}

void InitBuffer(win32_video_buffer* buffer, int width, int height)
{
	if (buffer->memory)
	{
		VirtualFree(buffer->memory, 0, MEM_RELEASE);
	}

	buffer->width = width;
	buffer->height = height;

	buffer->bitMapInfo.bmiHeader.biSize = sizeof(buffer->bitMapInfo.bmiHeader);
	buffer->bitMapInfo.bmiHeader.biWidth = buffer->width;
	buffer->bitMapInfo.bmiHeader.biHeight = buffer->height * BOTTOM_UP;
	buffer->bitMapInfo.bmiHeader.biPlanes = 1;
	buffer->bitMapInfo.bmiHeader.biBitCount = 32;
	buffer->bitMapInfo.bmiHeader.biCompression = BI_RGB;

	int bytesPerPixel = 4;
	int bitmapMemorySize = buffer->width * buffer->height * bytesPerPixel;

	buffer->memory = VirtualAlloc(0, bitmapMemorySize, MEM_COMMIT, PAGE_READWRITE);
	buffer->pitch = width * bytesPerPixel;
}

void DrawRectangleFill(win32_video_buffer *source, uint32_t x, uint32_t y, uint32_t width, uint32_t height,
	uint8_t R, uint8_t G, uint8_t B)
{
	if (x < 0) x = 0;
	if (y < 0) y = 0;

	uint32_t minX = x;
	uint32_t maxX = x + width;
	uint32_t minY = y;
	uint32_t maxY = y + height;

	uint8_t *row = (uint8_t *)source->memory + source->pitch * minY;

	for (uint32_t y = minY; y < maxY; y++)
	{
		uint32_t *pixel = (uint32_t *)row + minX;

		for (uint32_t x = minX; x < maxX; x++)
		{
			//       0xAARRGGBB;
			*pixel = R << 16 | G << 8 | B;
			pixel++;
		}
		row += source->pitch;
	}
}
