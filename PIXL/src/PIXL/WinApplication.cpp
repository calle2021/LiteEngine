#include "pixlpch.h"
#include "WinApplication.h"
#include "Logger.h"

namespace PIXL {
	int PIXL::WinApplication::Run(PIXL::PIXLSample* pixlSample, WinArgs args)
	{
		PIXL_CORE_INFO("Welcome to PIXL!");
		MessageBox(NULL, L"Text", L"Title", MB_ICONINFORMATION);
		return 0;
	}
}