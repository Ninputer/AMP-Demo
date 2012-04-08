#include "stdafx.h"
#include "RenderArea.h"
#include "render.h"

RenderAreaMessageHandler::RenderAreaMessageHandler(void) 
    : 
    m_hNextSkeletonEvent(nullptr),
    m_pDepthStreamHandle(nullptr),
    m_pVideoStreamHandle(nullptr),
    m_hNextDepthFrameEvent(nullptr),
    m_hNextColorFrameEvent(nullptr),
    m_centerx(0.0), 
    m_centery(0.0), 
    m_lastcenterx(0.0), 
    m_lastcentery(0.0), 
    m_scale(0.5), 
    m_mousepressed(false),
    m_useDouble(false)
{
}


RenderAreaMessageHandler::~RenderAreaMessageHandler(void)
{
}

HRESULT RenderAreaMessageHandler::OnCreate()
{
    using namespace Hilo::Direct2DHelpers;
    using namespace D2D1;

    if (m_renderTarget != nullptr)
    {
        return S_OK;
    }

    ComPtr<IWindow> window;

    HRESULT hr = GetWindow(&window);

    HWND hWnd;
    if (SUCCEEDED(hr))
    {
        hr = window->GetWindowHandle(&hWnd);
    }

    RECT rect;
    if (SUCCEEDED(hr))
    {
        hr = window->GetClientRect(&rect);
    }

    if (SUCCEEDED(hr))
    {
        hr = m_d2dFactory->CreateHwndRenderTarget(
            RenderTargetProperties(),
            HwndRenderTargetProperties(hWnd, SizeU(rect.right, rect.bottom)),
            &m_renderTarget);
    }

    Concurrency::accelerator default_acc;

    if (default_acc.get_supports_limited_double_precision() || default_acc.get_supports_double_precision())
    {
        m_useDouble = true;
    }

    return hr;
}

HRESULT RenderAreaMessageHandler::OnDestroy()
{
    SetEvent(m_hEvNuiProcessStop);

    return S_OK;
}


HRESULT RenderAreaMessageHandler::OnEraseBackground()
{
    return S_OK;
}

HRESULT RenderAreaMessageHandler::OnRender()
{
    using namespace Concurrency;

    ComPtr<IWindow> window;

    HRESULT hr = GetWindow(&window);

    RECT rect;
    if (SUCCEEDED(hr))
    {
        hr = window->GetClientRect(&rect);
    }

    if (SUCCEEDED(hr))
    {
        double d = 1 / m_scale;

        const unsigned int width = rect.right;
        const unsigned int height = rect.bottom;

        double dx = d * width / 640;
        double dy = d * height / 640;

        std::vector<unsigned int> data(width * height);

        array_view<unsigned int, 2> arrayview(height, width, data);
		arrayview.discard_data();
		render_material<float>(arrayview);

        arrayview.synchronize();

        ComPtr<ID2D1Bitmap> bitmap;
        hr = m_renderTarget->CreateBitmap(
            D2D1::SizeU(width, height),
            static_cast<void*>(data.data()),
            width * 4,
            D2D1::BitmapProperties(
            D2D1::PixelFormat(
            DXGI_FORMAT_B8G8R8A8_UNORM,
            D2D1_ALPHA_MODE_IGNORE
            )),
            &bitmap);

        if (SUCCEEDED(hr))
        {
            m_renderTarget->BeginDraw();
            m_renderTarget->Clear();

            m_renderTarget->DrawBitmap(bitmap, 
                D2D1::RectF(0.0, 0.0, static_cast<float>(width), static_cast<float>(height)));

            m_renderTarget->EndDraw();
        }
    }
    return hr;
}

HRESULT RenderAreaMessageHandler::OnSize(unsigned int width, unsigned int height)
{
    using namespace D2D1;

    HRESULT hr = m_renderTarget->Resize(SizeU(width, height));

    return hr;
}

HRESULT RenderAreaMessageHandler::OnLeftMouseButtonDown(D2D1_POINT_2F mousePosition)
{
    ComPtr<IWindow> window;
    HRESULT hr = GetWindow(&window);

    if (SUCCEEDED(hr))
    {
        hr = window->SetCapture();
    }

    if (SUCCEEDED(hr))
    {
        m_mousepressed = true;
        m_mousepressedpos = mousePosition;

        m_lastcenterx = m_centerx;
        m_lastcentery = m_centery;
    }

    return hr;
}

HRESULT RenderAreaMessageHandler::OnLeftMouseButtonUp(D2D1_POINT_2F mousePosition)
{
    HRESULT hr = S_OK;

    if (m_mousepressed)
    {
        ComPtr<IWindow> window;
        hr = GetWindow(&window);


        hr = window->ReleaseCapture();

        m_mousepressed = false;

    }

    return hr;
}

HRESULT RenderAreaMessageHandler::OnMouseMove(D2D1_POINT_2F mousePosition)
{
    HRESULT hr = S_OK;
    if (m_mousepressed)
    {
        double dx = mousePosition.x - m_mousepressedpos.x;
        double dy = -mousePosition.y + m_mousepressedpos.y;

        m_centerx = m_lastcenterx - dx / (320 * m_scale);
        m_centery = m_lastcentery - dy / (320 * m_scale);

        ComPtr<IWindow> window;
        hr = GetWindow(&window);

        if (SUCCEEDED(hr))
        {
            hr = window->RedrawWindow();
        }
    }
    return hr;
}

HRESULT RenderAreaMessageHandler::OnMouseEnter(D2D1_POINT_2F mousePosition)
{
    return S_OK;
}

HRESULT RenderAreaMessageHandler::OnMouseWheel(D2D1_POINT_2F mousePosition, short delta, int keys)
{
    if (delta > 0)
    {
        m_scale *= 1.2;
    }
    else if (delta < 0)
    {
        m_scale /= 1.2;
    }

    ComPtr<IWindow> window;
    HRESULT hr = GetWindow(&window);

    if (SUCCEEDED(hr))
    {
        hr = window->RedrawWindow();
    }

    return hr;
}

HRESULT RenderAreaMessageHandler::OnKeyDown(unsigned int vKey)
{
    return S_OK;
}

HRESULT RenderAreaMessageHandler::Initialize()
{
    using namespace Hilo::Direct2DHelpers;

    HRESULT hr = Direct2DUtility::GetD2DFactory(&m_d2dFactory);

    return hr;
}