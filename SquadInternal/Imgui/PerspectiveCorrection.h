#pragma once
#include "../externals/inireader/IniReader.h"


class PerspectiveCorrection
{
public:
	PerspectiveCorrection(const char * configPath);
	void FixStuff();
private:
	bool hackInstalled = false;
	int Width = 640;
	int Height = 480;
};



