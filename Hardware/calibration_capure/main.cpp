#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include "ARLib\Webcam\VideoPlayer.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

bool directoryExists(const char *path)
{
	DWORD attribs = GetFileAttributesA(path);
	return (attribs != INVALID_FILE_ATTRIBUTES && (attribs & FILE_ATTRIBUTE_DIRECTORY));
}

int recursiveDelete(const char *path)
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

void bgr2rgb(void *data, int width, int height)
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

LRESULT CALLBACK WindowProc(HWND window, UINT msg, WPARAM wparam, LPARAM lparam)
{
	if (msg == WM_PAINT)
		return 0;
	return DefWindowProcA(window, msg, wparam, lparam);
}

int main(int argc, char **argv)
{
	const char *destinationDir = argv[1];

	if (argc < 2)
	{
		printf("usage: %s <destination dir>\n", argv[0]);
		printf("using default destination directory \"calibration\"\n", argv[0]);
		destinationDir = "calibration";
	}

	if (directoryExists(destinationDir))
	{
		printf("the destination directory does already exist\n");
		printf("do you want to overwrite the contents? y/n\n");
		char c = getchar();
		if (c != 'y')
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

	ARLib::VideoPlayer leftPlayer(0);
	ARLib::VideoPlayer rightPlayer(1);

	assert(leftPlayer.getVideoWidth() == rightPlayer.getVideoWidth() &&
		   leftPlayer.getVideoHeight() == rightPlayer.getVideoHeight());

	int width = leftPlayer.getVideoWidth();
	int height = rightPlayer.getVideoHeight();

	void *memL = malloc(width * height * 3);
	void *memR = malloc(width * height * 3);

	int captureIndex = 0;

	printf("press the C key to capture or ESCAPE to quit\n");

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

	while(42)
	{
		leftPlayer.update();
		rightPlayer.update();

		int c = getch();
		if (c == 'c')
		{
			printf("capturing");

			captureIndex++;

			void *dataL = NULL, *dataR = NULL;
			while (!dataL || !dataR)
			{
				void *dL = leftPlayer.update();
				void *dR = rightPlayer.update();
				if (dL) { dataL = dL; memcpy(memL, dL, width * height * 3); }
				if (dR) { dataR = dR; memcpy(memR, dR, width * height * 3); }
			}

			printf(".");

			bgr2rgb(memL, width, height);
			bgr2rgb(memR, width, height);

			printf(".");

			char filename[MAX_PATH];
			_snprintf(filename, MAX_PATH, "%s\\capture_%003d_L.bmp", destinationDir, captureIndex);
			//if(!stbi_write_png(filename, width, height, 3, memL, width * 3))
			if(!stbi_write_bmp(filename, width, height, 3, memL))
			{
				fprintf(stderr, "could not save \"%s\"\n", filename);
				break;
			}

			printf(".");

			_snprintf(filename, MAX_PATH, "%s\\capture_%003d_R.bmp", destinationDir, captureIndex);
			//if(!stbi_write_png(filename, width, height, 3, memR, width * 3))
			if(!stbi_write_bmp(filename, width, height, 3, memR))
			{
				fprintf(stderr, "could not save \"%s\"\n", filename);
				break;
			}

			printf("saved capture %003d\n", captureIndex);
		}
		else if (c == 27)
		{
			break;
		}

		// TODO: this doesn't work yet!
		MSG msg;
		while (PeekMessageA(&msg, window, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessageA(&msg);
		}
		HDC dc = GetDC(window);
		StretchDIBits(dc, 0, 0, 480, 640, 0, 0, 1280, 960, memL, 0, DIB_RGB_COLORS, SRCCOPY);
		ReleaseDC(window, dc);

		//Sleep(10);
	}

	free(memL);
	free(memR);

	return 0;
}