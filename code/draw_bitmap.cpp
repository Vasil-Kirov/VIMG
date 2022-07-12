#include "draw_bitmap.h"

void WriteToFile(HANDLE File, void *stuff, int bytes_to_write)
{
	WriteFile(File, stuff, bytes_to_write, nullptr, nullptr);
}


void DrawPixelsToBitmap(render_buffer *PixelBuffer, const char *name)
{
	HANDLE File = CreateFileA(name, GENERIC_READ | GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

	BITMAPINFO bmi = {};
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = PixelBuffer->Width;
	bmi.bmiHeader.biHeight = PixelBuffer->Height;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = PixelBuffer->BytesPerPixel * 8;
	bmi.bmiHeader.biCompression = BI_RGB;
//	bmi.bmiHeader.biClrUsed = 256;

	{
		int RowSize = PixelBuffer->BytesPerPixel * PixelBuffer->Width;

		int PixelsSize = (RowSize * abs(PixelBuffer->Height));

		i64 FileSize = PixelsSize + 14 + 40 + 2 /* 2? */;
		int zero = 0;
		int StrideToPixels = 54;


		WriteToFile(File, (void*)"BM", 2);
		WriteToFile(File, (void*)&FileSize, 4);
		WriteToFile(File, (void*)&zero, 2);
		WriteToFile(File, (void*)&zero, 2);
		WriteToFile(File, (void*)&StrideToPixels, 4);
		WriteToFile(File, &bmi.bmiHeader, sizeof(BITMAPINFOHEADER));
		WriteToFile(File, (void *)&zero, 2);
		WriteToFile(File, (void*)PixelBuffer->Memory, PixelsSize);

		CloseHandle(File);
	}
}