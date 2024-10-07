#include "pixlpch.h"
#include "WinApplication.h"

namespace PIXL {
	int PIXL::WinApplication::Run(PIXL::PIXLSample* pixlSample)
	{
		MessageBox(NULL, L"Text", L"Title", MB_ICONINFORMATION);
		while (true) {
			printf("Running...\n");
		}
	}
}