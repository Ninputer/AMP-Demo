#pragma once

#include "WindowMessageHandlerImpl.h"
#include "WindowLayout.h"
#include "WindowLayoutChildInterface.h"
#include "WindowMessageHandlerImpl.h"
#ifdef KINECT_CTRL
#include "NuiApi.h"
#pragma comment(lib, "Kinect10.lib")
#endif
#include <ppl.h>

class RenderAreaMessageHandler : 
    public IInitializable,
    public Hilo::WindowApiHelpers::WindowMessageHandler
{
public:
    RenderAreaMessageHandler(void);
    ~RenderAreaMessageHandler(void);

protected:
    // IInitiliazable
    HRESULT __stdcall Initialize();

    // WindowMessageHandler Events
    HRESULT OnCreate();
    HRESULT OnDestroy();
    HRESULT OnEraseBackground();
    HRESULT OnRender();
    HRESULT OnSize(unsigned int width, unsigned int height);
    HRESULT OnLeftMouseButtonDown(D2D1_POINT_2F mousePosition);
    HRESULT OnLeftMouseButtonUp(D2D1_POINT_2F mousePosition);
    HRESULT OnMouseMove(D2D1_POINT_2F mousePosition);
    HRESULT OnMouseEnter(D2D1_POINT_2F mousePosition);
    HRESULT OnMouseWheel(D2D1_POINT_2F mousePosition, short delta, int keys);
    HRESULT OnKeyDown(unsigned int vKey);

    bool QueryInterfaceHelper(const IID &iid, void **object)
    {
        return 
            CastHelper<IInitializable>::CastTo(iid, this, object) ||
            Hilo::WindowApiHelpers::WindowMessageHandler::QueryInterfaceHelper(iid, object);
    }

private:
    ComPtr<ID2D1Factory> m_d2dFactory;
    ComPtr<ID2D1HwndRenderTarget> m_renderTarget;
    HANDLE m_pDepthStreamHandle;
    HANDLE m_pVideoStreamHandle;

    HANDLE m_hNextSkeletonEvent;
    HANDLE m_hNextDepthFrameEvent;
    HANDLE m_hNextColorFrameEvent;
    HANDLE m_hEvNuiProcessStop;

    Concurrency::task_group tasks;

    bool m_useDouble;

    //mouse control
    double m_centerx;
    double m_centery;
    double m_lastcenterx;
    double m_lastcentery;
    double m_scale;
    bool m_mousepressed;
    D2D1_POINT_2F m_mousepressedpos;

#ifdef KINECT_CTRL
    //kinect control
    ComPtr<INuiSensor> m_pNuiSensor;
#endif

    bool m_left_stretched;
    bool m_right_stretched;
    D2D1_POINT_2F m_lefthandpos;
    D2D1_POINT_2F m_righthandpos;
    bool m_resizing;
    double m_lastscale;

#ifdef KINECT_CTRL
    HRESULT Nui_Init();
    void Nui_GotSkeletonAlert();
#endif
};

