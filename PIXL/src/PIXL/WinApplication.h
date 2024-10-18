#pragma once
#include "windows.h"
#include "PIXLSample.h"
#include "Utility.h"

namespace PIXL {
	class WinApplication
	{
	public:
		static int Run(PIXL::PIXLSample* sample, WinArgs args);
	};

}