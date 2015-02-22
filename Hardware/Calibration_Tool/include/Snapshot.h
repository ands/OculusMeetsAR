#pragma once

#include "ARLib\Webcam\VideoPlayer.h"

class Snapshot
{
public:
	Snapshot(void);
	~Snapshot(void);

	static bool Snapshot::directoryExists(const char *path);
	static int Snapshot::recursiveDelete(const char *path);
	static void Snapshot::bgr2rgb(void *data, int width, int height);
	static void Snapshot::rotate(void *in, void *out);
	static LRESULT CALLBACK Snapshot::WindowProc(HWND window, UINT msg, WPARAM wparam, LPARAM lparam);
	int Snapshot::startCapture();

	static const char *destinationDir;
	static bool done;
	static int captureIndex;
	static ARLib::VideoPlayer *leftPlayer, *rightPlayer;
	static int width, height;
	static void *memL, *memR;
};