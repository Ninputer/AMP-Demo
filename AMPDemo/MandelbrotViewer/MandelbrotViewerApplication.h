#pragma once

#include "WindowLayout.h"
#include "WindowApplicationImpl.h"

class MandelbrotViewerApplication : public WindowApplication
{
private:
	HRESULT InitializeRenderArea(Hilo::WindowApiHelpers::IWindow** window);

	unsigned int m_renderAreaIndex;

protected:
	ComPtr<IWindowLayout> m_WindowLayout;

public:
	MandelbrotViewerApplication(void);
	~MandelbrotViewerApplication(void);

	HRESULT __stdcall Initialize();
};

