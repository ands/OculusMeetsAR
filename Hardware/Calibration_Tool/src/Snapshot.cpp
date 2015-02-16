#include "StdAfx.h"
#include "Snapshot.h"
#include <conio.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

const char* Snapshot::destinationDir;
bool Snapshot::done;
int Snapshot::captureIndex;
ARLib::VideoPlayer *Snapshot::leftPlayer,*Snapshot::rightPlayer;
int Snapshot::width, Snapshot::height;
void *Snapshot::memL, *Snapshot::memR;

Snapshot::Snapshot(void)
{
	done = false;
	captureIndex = 0;
}

Snapshot::~Snapshot(void)
{
}

bool Snapshot::directoryExists(const char *path)
{
	DWORD attribs = GetFileAttributesA(path);
	return (attribs != INVALID_FILE_ATTRIBUTES && (attribs & FILE_ATTRIBUTE_DIRECTORY));
}

int Snapshot::recursiveDelete(const char *path)
{
    char dir[MAX_PATH + 1];
    SHFILEOPSTRUCTA fos = {0};

    strncpy(dir, path, MAX_PATH);
    dir[strlen(dir) + 1] = 0; // double null terminate for SHFileOperation

    // delete the folder and everything inside
    fos.wFunc = FO_DELETE;
    fos.pFrom = dir;
    fos.fFlags = FOF_NO_UI;
    return SHFileOperationA(&fos);
}

void Snapshot::bgr2rgb(void *data, int width, int height)
{
	unsigned char *p = (unsigned char*)data;
	for (int i = 0; i < width * height; i++)
	{
		unsigned char tmp = p[0];
		p[0] = p[2];
		p[2] = tmp;
		p += 3;
	}
}

void Snapshot::rotate(void *in, void *out)
{
	unsigned char *i = (unsigned char*)in;
	unsigned char *o = (unsigned char*)out;

	for (int y = 0; y < height; y++)
	{
		unsigned char *col = o, *row = i;
		for (int x = 0; x < width; x++)
		{
			col[0] = row[0];
			col[1] = row[1];
			col[2] = row[2];
			row += 3;
			col += height * 3;
		}
		i += width * 3;
		o += 3;
	}
}

LRESULT CALLBACK Snapshot::WindowProc(HWND window, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg)
	{
		case WM_PAINT:
		{
			PAINTSTRUCT paint;
			HDC DeviceContext = BeginPaint(window, &paint);
			// ignore
			EndPaint(window, &paint);
			return 0;
		} break;
		case WM_KEYDOWN:
		{
			if (wparam == VK_SPACE)
			{
				printf("capturing");

				captureIndex++;

				void *dataL = NULL, *dataR = NULL;
				while (!dataL || !dataR)
				{
					void *dL = leftPlayer->update();
					void *dR = rightPlayer->update();
					if (dL) { dataL = dL; memcpy(memL, dL, width * height * 3); }
					if (dR) { dataR = dR; memcpy(memR, dR, width * height * 3); }
				}

				printf(".");

				bgr2rgb(memL, width, height);
				bgr2rgb(memR, width, height);

				printf(".");

				char filename[MAX_PATH];
				_snprintf(filename, MAX_PATH, "%s\\capture_%003d_L.bmp", destinationDir, captureIndex);
				if(!stbi_write_bmp(filename, width, height, 3, memL))
				{
					fprintf(stderr, "could not save \"%s\"\n", filename);
					done = true;
				}

				printf(".");

				_snprintf(filename, MAX_PATH, "%s\\capture_%003d_R.bmp", destinationDir, captureIndex);
				if(!stbi_write_bmp(filename, width, height, 3, memR))
				{
					fprintf(stderr, "could not save \"%s\"\n", filename);
					done = true;
				}

				printf("saved capture %003d\n", captureIndex);
				return 0;
			}
			else if (wparam == VK_ESCAPE)
			{
				done = true;
				return 0;
			}
		} break;
		case WM_CLOSE:
		{
			done = true;
			return 0;
		} break;
		default:
			return DefWindowProcA(window, msg, wparam, lparam);
	}
}

int Snapshot::startCapture()
{
	destinationDir = "images";

	if (directoryExists(destinationDir))
	{
		printf("the destination directory does already exist\n");
		printf("do you want to overwrite the contents? (1-Yes/2-No)\n");
		int input=0;
		std::cin>>input;
		if (input==2)
			return 0;

		if (recursiveDelete(destinationDir))
		{
			fprintf(stderr, "could not clean destination directory \"%s\"\n", destinationDir);
			return -1;
		}
	}

	if (!CreateDirectoryA(destinationDir, NULL))
	{
		fprintf(stderr, "could not create destination directory \"%s\"\n", destinationDir);
		return -1;
	}

	// video initialization
	leftPlayer = new ARLib::VideoPlayer(0);
	rightPlayer = new ARLib::VideoPlayer(1);

	assert(leftPlayer->getVideoWidth() == rightPlayer->getVideoWidth() &&
		   leftPlayer->getVideoHeight() == rightPlayer->getVideoHeight());

	width = leftPlayer->getVideoWidth();
	height = rightPlayer->getVideoHeight();

	memL = new unsigned char[width * height * 3];
	memR = new unsigned char[width * height * 3];

	// window
	WNDCLASSA windowClass = {};
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = WindowProc;
	windowClass.hInstance = GetModuleHandle(NULL);
	windowClass.lpszClassName = "VideoWindowClass";

	if (!RegisterClassA(&windowClass))
	{
		fprintf(stderr, "could not create window class\n");
		return -1;
	}

	HWND window = CreateWindowExA(
		0, windowClass.lpszClassName, "Live Capture", WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		CW_USEDEFAULT, CW_USEDEFAULT, 960, 640, 0, 0, windowClass.hInstance, 0);

	if (!window)
	{
		fprintf(stderr, "could not create window\n");
		return -1;
	}

	// bitmap
	BITMAPINFO bmp = {};
	bmp.bmiHeader.biSize = sizeof(bmp.bmiHeader);
    bmp.bmiHeader.biWidth = height;
    bmp.bmiHeader.biHeight = -width;
    bmp.bmiHeader.biPlanes = 1;
    bmp.bmiHeader.biBitCount = 24;
    bmp.bmiHeader.biCompression = BI_RGB;


	printf("press SPACE to capture or ESCAPE to quit\n");

	while(!done)
	{
		// message handling
		MSG msg;
		while (PeekMessageA(&msg, window, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessageA(&msg);
		}

		// video display
		HDC dc = GetDC(window);
		SetStretchBltMode(dc, HALFTONE);
		void *streamL = leftPlayer->update();
		if (streamL) {
			rotate(streamL, memL);
			StretchDIBits(dc, 0, 0, 480, 640, 0, 0, height, width, memL, &bmp, DIB_RGB_COLORS, SRCCOPY);
		}
		void *streamR = rightPlayer->update();
		if (streamR) {
			rotate(streamR, memR);
			StretchDIBits(dc, 480, 0, 480, 640, 0, 0, height, width, memR, &bmp, DIB_RGB_COLORS, SRCCOPY);
		}
		ReleaseDC(window, dc);
	}

	CloseWindow(window);
	delete[] memL;
	delete[] memR;
	delete leftPlayer;
	delete rightPlayer;

	return 0;
}