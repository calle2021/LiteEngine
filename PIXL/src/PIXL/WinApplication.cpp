#include "pixlpch.h"
#include "WinApplication.h"

namespace PIXL {
	int PIXL::WinApplication::Run(PIXL::PIXLSample* pixlSample, WinArgs args)
	{
		MessageBox(NULL, L"Text", L"Title", MB_ICONINFORMATION);
		return 0;
	}
}