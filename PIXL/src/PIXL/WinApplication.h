#pragma once
#include "windows.h"
#include "PIXLSample.h"

namespace PIXL {

	struct WinArgs
	{
		HINSTANCE hins;
		HINSTANCE pins;
		LPWSTR cmd;
		int showcmd;
	};

	class WinApplication
	{
	public:
		static int Run(PIXL::PIXLSample* sample, WinArgs args);
	};

}