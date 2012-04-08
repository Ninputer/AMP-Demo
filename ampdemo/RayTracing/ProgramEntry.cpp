// MandelbrotViewer.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "RayTracingApplication.h"

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	ComPtr<IWindowApplication> mainApp;

	HRESULT hr = SharedObject<RayTracingApplication>::Create(&mainApp);

	if (SUCCEEDED(hr))
	{
		hr = mainApp->RunMessageLoop();
	}

	return 0;
}

