#include <windows.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define SETTINGSBUFFERSIZE 100

typedef long long int          s64;
typedef unsigned long long int u64;
typedef long int               s32;
typedef unsigned long int      u32;
typedef short int              s16;
typedef unsigned short int     u16;
typedef char                    s8;
typedef unsigned char           u8; 
// typedefs to make bitmap structs easier to read


struct Resolution
{
	int Width;
	int Height;
};

struct bitmap_file_header
{
    u16 Type;
    u32 Size;
    u16 Reserved1;
    u16 Reserved2;
    u32 BitmapOffset;
};

struct bitmap_header
{
    u32 InfoSize;
    s32 Width;
    s32 Height;
    u16 Planes;
    u16 BitsPerPixel;
    u32 Compression;
    u32 BitmapSize;
    s32 HorizontalResolution;
    s32 VerticalResolution;
    u32 ColoursUsed;
    u32 ColoursImportant;
};



struct bitmap_bitfields
{
    u32 RedMask;
    u32 GreenMask;
    u32 BlueMask;
};
#pragma pack(pop)

int StringToInt(  // function will return 0 if non digit string is passed;
	char *String,   // pointer to the char array
	int ReadFrom,   // Position to read from
	int ReadTo	    // Position to read to, including. Pass in negative number to read to the end
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

	/* These 3 can probably be removed, but I don't know how. */
	static int ImgWidth, ImgHeight;
	static unsigned char *ImagePixels;
	static BITMAPINFO DIB; 

	
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
				PAINTSTRUCT PaintStruct;
				if(BeginPaint(Window, &PaintStruct) != NULL)
				{
					RECT WindowRect;
					Result = GetWindowRect(Window, &WindowRect);
					int WindowWidth  = PaintStruct.rcPaint.right  - PaintStruct.rcPaint.left;
					int WindowHeight = PaintStruct.rcPaint.bottom - PaintStruct.rcPaint.top;
					int DestWidth  = ImgWidth;
					int DestHeight = ImgHeight;
					while(DestWidth+50 >  WindowWidth || DestHeight+50 > WindowHeight)
					{
						DestWidth  *= .99;
						DestHeight *= .99;
					}
					if(Result != 0)
					{
						int DestX = (WindowWidth  - DestWidth)  / 2; 
						int DestY = (WindowHeight - DestHeight) / 2; 
						Result = StretchDIBits(PaintStruct.hdc, 		// Device context for painting
												DestX, DestY,   		            // X and Y position of the destination
												DestWidth, DestHeight,        	// Height and Width of the destination
												0, 0, 					                // X and Y position of the source (should always be 0 to paint everything)
												ImgWidth, ImgHeight,          	// Width and Height of the source
												ImagePixels, 		               	// A pointer to the array of pixels
												&DIB,					                  // A BITMAPINFO structure that contains data about the image
												DIB_RGB_COLORS, 	            	// Specifies that the image contains RGB (Actually BGR) values
												SRCCOPY);				                // Specifies that the source should be directly copied to the destination
					}
				}
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



INT WinMain(HINSTANCE Instance, HINSTANCE PrevInstance,
    LPSTR wCommandLine, INT ShowCode)
{

	// Read from file
	HANDLE Settings;
	{
	// Get path to executable
	char SettingsFilePath[100];
	GetModuleFileName(NULL, SettingsFilePath, sizeof(SettingsFilePath));
	// Get the lenght of SettingsFilePath
	int PathLength = 0;
	for (int Index = 0; SettingsFilePath[Index] != '\0'; ++Index)
	{
		++PathLength;
	}
	// Remove executable name and add "settings.txt" to the path
	int Position = 0;
	for (int Index = PathLength; SettingsFilePath[Index] != '\\'; --Index)
	{
		Position = Index;
	}
	SettingsFilePath[Position] = '\0';
	mstrcat(SettingsFilePath, "settings.txt");
	
	// Read settings file
	Settings = CreateFileA(SettingsFilePath, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	}
	if(Settings == INVALID_HANDLE_VALUE)
	{
		Print("Opening settings file has failed!\n");
		return 1;
		// TODO: recreate settings file on corruption!
	}
	char SettingsReadBuffer[SETTINGSBUFFERSIZE] = {0};
	if(ReadFile(Settings, SettingsReadBuffer, SETTINGSBUFFERSIZE, NULL, nullptr) == 0)
	{
		Print("Opening settings file has failed!\n");
		return 1;
	}
	
	
	// Set resolution from file
	Resolution res;
	{
		int StartPositionW = 0;
		int StartPositionH = 0;
		for(int Index = 0; SettingsReadBuffer[Index] != '\0'; ++Index)
		{
			if(SettingsReadBuffer[Index] == 'w')
			{
				StartPositionW = Index;
			}
			else if(SettingsReadBuffer[Index] == 'h')
			{
				res.Width = StringToInt(SettingsReadBuffer, StartPositionW, Index);
				StartPositionH = Index;
			}
			else if(SettingsReadBuffer[Index] == '*')
			{
				res.Height = StringToInt(SettingsReadBuffer, StartPositionH, Index);
				break;
			}
		}
	}
	
	if(CloseHandle(Settings) == 0)
	{
		Print("Failed to close settings file!");
		return 1;
	}
	// Reading from settings over!

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

	int bytesPerPixel;
	ImagePixels = stbi_load(FileName, &ImgWidth, &ImgHeight, &bytesPerPixel, 0);
	if(ImagePixels == NULL) return 1;

  // Changing the RGB from stbi_load to a BGR for Windows.
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

  // Getting DIB info
	bitmap_header bitmapHeader;
	{
			bitmapHeader.InfoSize = sizeof(bitmap_header);
			bitmapHeader.Width = ImgWidth;
			bitmapHeader.Height = -ImgHeight;
			bitmapHeader.Planes = 1;
			bitmapHeader.BitsPerPixel = bytesPerPixel * 8;
			bitmapHeader.Compression = BI_RGB;
			bitmapHeader.BitmapSize = ImgWidth * ImgHeight * bytesPerPixel;
			bitmapHeader.HorizontalResolution = 0;
			bitmapHeader.VerticalResolution = 0;
			bitmapHeader.ColoursUsed = 0;
			bitmapHeader.ColoursImportant = 0;
	}
	bitmap_header *bitmapHeaderPtr = &bitmapHeader;
	
	WNDCLASSEXA WindowClass = {};
	WindowClass.cbSize = sizeof(WNDCLASSEX);
	WindowClass.style = CS_HREDRAW | CS_VREDRAW;
	WindowClass.lpfnWndProc = MainWindowCallback;
	WindowClass.hInstance = Instance;
	WindowClass.hIcon = (HICON)LoadImageA(WindowClass.hInstance, (char *)1, IMAGE_ICON, 0, 0, LR_SHARED);
	WindowClass.hCursor = (HCURSOR)LoadImageA(nullptr, (s8*)IDC_ARROW, IMAGE_CURSOR, 0, 0, LR_SHARED);
	WindowClass.hbrBackground = (HBRUSH)BLACK_BRUSH;
	// WindowClass.lpszMenuName = ;
	WindowClass.lpszClassName = "VIMGImageViewerWindowClass";


	if(RegisterClassExA(&WindowClass))
	{
		HWND WindowHandle = CreateWindowExA(0, WindowClass.lpszClassName, "VIMG", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 0, 0, res.Width, res.Height, 0, 0, Instance, 0);
		if(WindowHandle)
		{
			DIB = {};
			memcpy(&DIB.bmiHeader, bitmapHeaderPtr, sizeof(bitmap_header));
			DIB.bmiColors[1] = {};
			
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
