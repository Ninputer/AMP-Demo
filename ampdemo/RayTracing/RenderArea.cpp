#include "stdafx.h"
#include "RenderArea.h"
#include "render.h"
#include <sstream>

RenderAreaMessageHandler::RenderAreaMessageHandler(void) 
    : 
    m_hNextSkeletonEvent(nullptr),
    m_pDepthStreamHandle(nullptr),
    m_pVideoStreamHandle(nullptr),
    m_hNextDepthFrameEvent(nullptr),
    m_hNextColorFrameEvent(nullptr),
    m_phi(-270.0f), 
    m_theta(-85.0f), 
    m_lastphi(0.0f), 
    m_lasttheta(0.0f), 
    m_eyedist(60.0f), 
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
        const unsigned int width = rect.right;
        const unsigned int height = rect.bottom;

        int aa_factor = 1;

        std::vector<unsigned int> data(width * height * aa_factor * aa_factor);

        array_view<unsigned int, 2> arrayview(height * aa_factor, width * aa_factor, data);
        arrayview.discard_data();

        LARGE_INTEGER frequency, before, after;
        QueryPerformanceFrequency(&frequency);
        QueryPerformanceCounter(&before);

        render_reflection<float>(arrayview, m_phi, m_theta, m_eyedist, aa_factor);

        arrayview.synchronize();

        QueryPerformanceCounter(&after);

        double millisecond = (after.QuadPart - before.QuadPart) * 1000.0 / frequency.QuadPart; 

        std::wstringstream msg;
        msg << L"Ray Tracing Viewer: last frame render time ";
        msg << millisecond;
        msg << " ms";

        HWND hParent;
        hr = window->GetParentWindowHandle(&hParent);

        if (SUCCEEDED(hr))
        {
            SetWindowText(hParent, msg.str().c_str());
        }

        ComPtr<ID2D1Bitmap> bitmap;
        hr = m_renderTarget->CreateBitmap(
            D2D1::SizeU(width * aa_factor, height * aa_factor),
            static_cast<void*>(data.data()),
            width * 4 * aa_factor,
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

        m_lastphi = m_phi;
        m_lasttheta = m_theta;
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
        float dx = mousePosition.x - m_mousepressedpos.x;
        float dy = -mousePosition.y + m_mousepressedpos.y;

        m_phi = m_lastphi - dx / (3.55f);
        m_theta = m_lasttheta - dy / (3.55f);

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
        m_eyedist *= 1.1f;
    }
    else if (delta < 0)
    {
        m_eyedist /= 1.1f;
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