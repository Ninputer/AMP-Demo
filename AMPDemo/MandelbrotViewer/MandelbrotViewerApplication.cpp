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
    if (SUCCEEDED(hr))
    {
        hr = InitializeRenderArea();
    }

	if (SUCCEEDED(hr))
    {
        // Update main window postition based on height of the ribbon
        hr = UpdateRenderAreaPosition(applicationWindow, m_renderAreaWindow);
    }

	return hr;
}

HRESULT MandelbrotViewerApplication::InitializeRenderArea()
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

	if (SUCCEEDED(hr))
    {
		hr = windowFactory->Create(
			location,
			size,
			renderAreaMessageHandler,
			applicationWindow,
			&m_renderAreaWindow);
	}

	if (SUCCEEDED(hr))
    {
		hr = m_renderAreaWindow->SetMessageHandler(renderAreaMessageHandler);
	}

	return hr;
}

HRESULT MandelbrotViewerApplication::UpdateRenderAreaPosition(IWindow* mainWindow, IWindow* childWindow)
{
    RECT rect;
    HRESULT hr = mainWindow->GetClientRect(&rect);

    if (SUCCEEDED(hr))
    {
        hr = childWindow->SetRect(rect);
    }

    return hr;
}

HRESULT MandelbrotViewerApplication::OnSize(unsigned int width, unsigned int height)
{
	if (!m_renderAreaWindow)
    {
        return S_OK;
    }

    ComPtr<IWindow> mainWindow;
    HRESULT hr = GetMainWindow(&mainWindow);
    if (SUCCEEDED(hr))
    {
        hr = UpdateRenderAreaPosition(mainWindow, m_renderAreaWindow);
    }

    return hr;
}