#pragma once

#include "WindowLayout.h"
#include "WindowApplicationImpl.h"

class MandelbrotViewerApplication : public WindowApplication
{
private:
	HRESULT InitializeRenderArea();
	HRESULT UpdateRenderAreaPosition(Hilo::WindowApiHelpers::IWindow* mainWindow, Hilo::WindowApiHelpers::IWindow* childWindow);

	unsigned int m_renderAreaIndex;

protected:
	ComPtr<IWindow> m_renderAreaWindow;

	HRESULT OnSize(unsigned int width, unsigned int height);
	HRESULT OnMouseWheel(D2D1_POINT_2F mousePosition, short delta, int keys);

public:
	MandelbrotViewerApplication(void);
	~MandelbrotViewerApplication(void);

	HRESULT __stdcall Initialize();
};

