#include "stdafx.h"
#include "MandelbrotViewerApplication.h"
#include "RenderArea.h"
#include "Resource.h"

MandelbrotViewerApplication::MandelbrotViewerApplication(void)
{
}


MandelbrotViewerApplication::~MandelbrotViewerApplication(void)
{
}

HRESULT MandelbrotViewerApplication::Initialize()
{
	HRESULT hr = WindowApplication::Initialize();

	ComPtr<IWindow> applicationWindow;

	if (SUCCEEDED(hr))
	{
		hr = GetMainWindow(&applicationWindow);
	}

	if (SUCCEEDED(hr))
	{
		hr = applicationWindow->SetTitle(L"Mandelbrot Set Viewer");		
	}

	if (SUCCEEDED(hr))
	{
		hr = applicationWindow->SetSize(640, 640);
	}

	if (SUCCEEDED(hr))
    {
        // Set the large icon
        hr = applicationWindow->SetLargeIcon(::LoadIcon(HINST_THISCOMPONENT, MAKEINTRESOURCE(IDI_MANDELBROTVIEWER)));
    }

    if (SUCCEEDED(hr))
    {
        // Set the small icon
        hr = applicationWindow->SetSmallIcon(::LoadIcon(HINST_THISCOMPONENT, MAKEINTRESOURCE(IDI_SMALL)));
    }

	// Create render area...
    ComPtr<IWindow> renderAreaWindow;
    if (SUCCEEDED(hr))
    {
        hr = InitializeRenderArea(&renderAreaWindow);
    }

	

	return hr;
}

HRESULT MandelbrotViewerApplication::InitializeRenderArea(IWindow** window)
{
	using namespace Hilo::Direct2DHelpers;

	static POINT location = {0, 0};
    static SIZE size =
    {
        static_cast<long>(Direct2DUtility::ScaleValueForCurrentDPI(640)), 
        static_cast<long>(Direct2DUtility::ScaleValueForCurrentDPI(640))
    };

	ComPtr<IWindowMessageHandler> renderAreaMessageHandler;
	HRESULT hr = SharedObject<RenderAreaMessageHandler>::Create(&renderAreaMessageHandler);

	ComPtr<IWindow> applicationWindow;
	if (SUCCEEDED(hr))
    {
		hr = GetMainWindow(&applicationWindow);
	}

	ComPtr<IWindowFactory> windowFactory;
	if (SUCCEEDED(hr))
    {
		hr = GetWindowFactory(&windowFactory);
	}

	ComPtr<IWindow> renderArea;
	if (SUCCEEDED(hr))
    {
		hr = windowFactory->Create(
			location,
			size,
			renderAreaMessageHandler,
			applicationWindow,
			&renderArea);
	}

	if (SUCCEEDED(hr))
    {
		hr = renderArea->SetMessageHandler(renderAreaMessageHandler);
	}

	if (SUCCEEDED(hr))
	{
		hr = AssignToOutputPointer(window, renderArea);
	}

	return hr;
}