#include "basic.h"

#include "draw_bitmap.h"
#include "scaling.h"

#include "draw_bitmap.cpp"
#include "scaling.cpp"



struct Resolution
{
	int Width;
	int Height;
};


#pragma pack(pop)

int StringToInt(  // function will return 0 if non digit string is passed;
	char *String, // pointer to the char array
	int ReadFrom, // Position to read from
	int ReadTo	  // Position to read to, including. Pass in negative number to read to the end
	)
{
	
	int Num = 0;
	if(ReadTo >= 0)
	{
		for(; ReadFrom <= ReadTo; ++ReadFrom)
		{
			if(String[ReadFrom] > 47 && String[ReadFrom] < 58)
			{
				Num += String[ReadFrom] - '0';
				Num *= 10;
			}
		}
		return Num/10; // Num is multiplies by 10 one extra time before exiting the loop so we correct for that
	}
	
	// If program reaches here it means that ReadTo is less than 0
	for(;String[ReadFrom] != '\0'; ++ReadFrom)
	{
		if(String[ReadFrom] > 47 && String[ReadFrom] < 58)
		{
			Num += String[ReadFrom] - '0';
			Num *= 10;
		}
	}
	return Num/10; // Num is multiplies by 10 one extra time before exiting the loop so we correct for that  
}

void Print(char *String)
{
	HANDLE OutputHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	
	int StringLength = 0;
	char *Scan = String;
	while (*Scan != 0)
	{
		++StringLength;
		++Scan;
	}
	
	DWORD BytesWritten;
	WriteFile(OutputHandle, String, StringLength, &BytesWritten, nullptr);
}
void mstrcat(char* destination, const char* source)
{
	// make ptr point to the end of destination string
	char* ptr = destination + strlen(destination);

	// Appends characters of source to the destination string
	while (*source != '\0')
		*ptr++ = *source++;

	// null terminate destination string
	*ptr = '\0';
}


static bool Running;
static const unsigned BackgroundColor = 0xFFBFCDDB;

/* These 4 can probably be removed, but I don't know how. */
static int ImgWidth, ImgHeight;
static unsigned char *ImagePixels;
static int bytesPerPixel;
static unsigned char* toFree = NULL;
static int savePixels;


LRESULT CALLBACK MainWindowCallback(
_In_ HWND Window,
_In_ UINT Message,
_In_ WPARAM WParam,
_In_ LPARAM LParam
)
{
	LRESULT Result = 0;
	switch(Message)
	{
		case WM_PAINT:
		{
			HDC WindowDC = GetWindowDC(Window);
			if(WindowDC == NULL)
				break;

			// TODO: Optimize so it doesn't resize if it doesn't have to
			PAINTSTRUCT PaintStruct;
			HDC PaintContext = BeginPaint(Window, &PaintStruct);
			if(PaintContext != NULL)
			{
				RECT WindowRect;
				Result = GetWindowRect(Window, &WindowRect);
				double AspectRatio = (double)ImgHeight / (double)ImgWidth;
				int WindowWidth  = WindowRect.right  - WindowRect.left;
				int WindowHeight = WindowRect.bottom - WindowRect.top;
				int DestWidth = (int)((double)WindowWidth * .85);
				DestWidth += DestWidth % 4;
				int DestHeight  = (int)((double)DestWidth * AspectRatio);
				while(DestWidth*1.25f >  WindowWidth || DestHeight*1.25f > WindowHeight)
				{
					DestWidth  *= .99;
					DestHeight *= .99;
				}
				DestWidth -= DestWidth % 4;
				BITMAPINFO DIB = {};
				DIB.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
				DIB.bmiHeader.biWidth = DestWidth;
				DIB.bmiHeader.biHeight = -DestHeight;
				DIB.bmiHeader.biPlanes = 1;
				DIB.bmiHeader.biBitCount = bytesPerPixel * 8;
				DIB.bmiHeader.biCompression = BI_RGB;
				DIB.bmiHeader.biSizeImage = 0;
				DIB.bmiHeader.biClrUsed = 0;
				DIB.bmiHeader.biClrImportant = 0;
				DIB.bmiHeader.biXPelsPerMeter = 0;
				DIB.bmiHeader.biYPelsPerMeter = 0;
				DIB.bmiColors[0] = {};

				if(Result != 0)
				{
					// Use stb_image_resize for resizing because GDI doesn't do a good job at that
					long PixelSize = DestHeight * DestWidth * bytesPerPixel;
					unsigned char *ResizedPixels = (unsigned char *)VirtualAlloc(NULL, PixelSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
					if(ResizedPixels == NULL)
					{
						goto CleanUpPaint;
					}
					if (toFree == NULL)
					{
						toFree = ResizedPixels;
					}
					else
					{
						VirtualFree(toFree, 0, MEM_RELEASE);
						toFree = ResizedPixels;
					}


					if(bytesPerPixel == 3)
					{
						ScaleImageRGB(ImagePixels, ImgWidth, ImgHeight, ResizedPixels, DestWidth, DestHeight, bytesPerPixel);
					}
					else
					{
						ScaleImageRGBA(ImagePixels, ImgWidth, ImgHeight, ResizedPixels, DestWidth, DestHeight);
					}

					int DestX = (int)((float)(WindowWidth - DestWidth) / 2.0f);
					int DestY = (int)((float)(WindowHeight - DestHeight) / 2.0f);

					if(bytesPerPixel == 4)
						DrawBackground(ResizedPixels, DestWidth, DestHeight);

					if(ResizedPixels != NULL)
					{
						SetDIBitsToDevice( WindowDC, 
						DestX, DestY,
						DestWidth, DestHeight,
						0, 0,
						0, DestHeight,
						ResizedPixels,
						&DIB,
						DIB_RGB_COLORS
						);

					}
				}
			}
			
			CleanUpPaint:
			ReleaseDC(Window, WindowDC);
			EndPaint(Window, &PaintStruct);
			break;
		}
		case WM_GETMINMAXINFO:
		{
			MINMAXINFO *Info = (MINMAXINFO *)LParam;
			Info->ptMinTrackSize.x = 100;
			Info->ptMinTrackSize.y = 100;
		}
		break;

		case WM_SIZE:
		{
			return 0;
		}
		case WM_DESTROY:
		{
			Running = false;
			break;
		}
		case WM_CLOSE:
		{
			Running = false;
			break;
		}
		case WM_ACTIVATEAPP:
		{
			//OutputDebugStringA("WM_ACTIVATEAPP");
			break;
		}
		default:
		{
			Result = DefWindowProcA(Window, Message, WParam, LParam);
			break;
		}


	}
	return(Result);
}

void
DrawBackground(unsigned char * Pixels, int Width, int Height)
{
	unsigned int *FrameBuffer = (unsigned int *)Pixels;

	for(int Y = 0; Y < Height; ++Y)
	{
		for(int X = 0; X < Width; ++X)
		{
			RGBA Pixel = {FrameBuffer[X + (Y * Width)]};
			
			if(Pixel.a < 0xFF)
			{
				RGBA BGColor = {BackgroundColor};
				RGBA Out;
				float Alpha = NormalizeBetween(Pixel.a, 0, 255, 1, 0);
				Out.r = Alpha * BGColor.r + (1 - Alpha) * Pixel.r;
				Out.g = Alpha * BGColor.g + (1 - Alpha) * Pixel.g;
				Out.b = Alpha * BGColor.b + (1 - Alpha) * Pixel.b;
				Out.a = 1;
				FrameBuffer[X + (Y * Width)] = Out.Color;
			}
		}
	}
}


INT WinMain(HINSTANCE Instance, HINSTANCE PrevInstance,
    LPSTR wCommandLine, INT ShowCode)
{

	HRESULT Result;
	
	char *CommandLine = GetCommandLineA();
	char *Identifier;
    long IdentifierLength;
    
    char *Scan = CommandLine;
    while (*Scan != 0)
    {
        long Terminator = ' ';
        if (*Scan == '"')
        {
            Terminator = '"';
            ++Scan;
        }
        
        Identifier = Scan;
        IdentifierLength = 0;
        
        while ((*Scan != Terminator) && (*Scan != 0))
        {
            ++IdentifierLength;
            ++Scan;
        }
        
        if (*Scan == '"')
        {
            ++Scan;
        }
        
        while (*Scan == ' ')
        {
            ++Scan;
        }
    }
    
    char FileName[512];
    memcpy(FileName, Identifier, IdentifierLength);
	FileName[IdentifierLength] = 0;

	ImagePixels = stbi_load(FileName, &ImgWidth, &ImgHeight, &bytesPerPixel, 0);
	if(ImagePixels == NULL) return 1;

	{
		unsigned int PixelCount = ImgWidth * ImgHeight; 
		if(bytesPerPixel == 3)
		{
			unsigned char *BytesPtr = ImagePixels;
			for(int Index = 0; Index < PixelCount; ++Index)
			{
				unsigned char red   = *(BytesPtr);
				unsigned char blue  = *(BytesPtr+2);
				unsigned char green = *(BytesPtr+1);
				*BytesPtr++ = blue;
				*BytesPtr++ = green;
				*BytesPtr++ = red;
			}
		}
		else if(bytesPerPixel == 4)
		{
			unsigned char *BytesPtr = ImagePixels;
			for(int Index = 0; Index < PixelCount; ++Index)
			{
				unsigned char red   = *(BytesPtr);
				unsigned char blue  = *(BytesPtr+2);
				unsigned char green = *(BytesPtr+1);
				unsigned char alpha = *(BytesPtr+3);
				*BytesPtr++ = blue;
				*BytesPtr++ = green;
				*BytesPtr++ = red;
				*BytesPtr++ = alpha;
			}
		}
	}
	
	WNDCLASSEXA WindowClass = {};
	WindowClass.cbSize = sizeof(WNDCLASSEX);
	WindowClass.style = CS_HREDRAW | CS_VREDRAW;
	WindowClass.lpfnWndProc = MainWindowCallback;
	WindowClass.hInstance = Instance;
	WindowClass.hIcon = (HICON)LoadImageA(WindowClass.hInstance, (char *)1, IMAGE_ICON, 0, 0, LR_SHARED);
	WindowClass.hCursor = (HCURSOR)LoadImageA(nullptr, (LPCSTR)IDC_ARROW, IMAGE_CURSOR, 0, 0, LR_SHARED);
	WindowClass.hbrBackground = (HBRUSH)BLACK_BRUSH;
	// WindowClass.lpszMenuName = ;
	WindowClass.lpszClassName = "VIMGImageViewerWindowClass";
	
	if(RegisterClassExA(&WindowClass))
	{
		HWND WindowHandle = CreateWindowExA(0, WindowClass.lpszClassName, "VIMG", WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, ImgWidth * 1.15, ImgHeight * 1.15, 0, 0, Instance, 0);
		if(WindowHandle)
		{

			
			MSG Message;
			Running = true;
			while(Running)
			{
				BOOL MessageResult = GetMessage(&Message, 0, 0, 0);
				if (MessageResult > 0)
				{
					TranslateMessage(&Message);
					DispatchMessageA(&Message);
				}
				else
				{
					break;
				}
			}
		}
	}
	return 0;
}
