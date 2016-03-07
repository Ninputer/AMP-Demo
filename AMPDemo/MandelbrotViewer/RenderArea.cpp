#include "stdafx.h"
#include "RenderArea.h"
#include "mandelbrot.h"
#include "d3d11.h"
#include "dxgi.h"

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
    m_left_stretched(false),
    m_right_stretched(false),
    m_lastscale(0.5),
    m_resizing(false),
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

#ifdef KINECT_CTRL
    if (SUCCEEDED(hr))
    {
        hr = Nui_Init();
    }
#endif

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

    tasks.wait();

#ifdef KINECT_CTRL
    if (m_pNuiSensor != nullptr)
    {
        m_pNuiSensor->NuiShutdown();
    }
#endif

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

        static const unsigned int max_iter = 4096;

        if (m_useDouble)
        {
            generate_mandelbrot<double>(
                arrayview, 
                std::min(static_cast<unsigned int>(64 * log(1 + m_scale) * 4), max_iter), 
                static_cast<double>(m_centerx - dx), 
                static_cast<double>(m_centery - dy), 
                static_cast<double>(m_centerx + dx), 
                static_cast<double>(m_centery + dy));
        }
        else
        {
            generate_mandelbrot<float>(
                arrayview, 
                std::min(static_cast<unsigned int>(64 * log(1 + m_scale) * 4), max_iter), 
                static_cast<float>(m_centerx - dx), 
                static_cast<float>(m_centery - dy), 
                static_cast<float>(m_centerx + dx), 
                static_cast<float>(m_centery + dy));
        }

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

#ifdef KINECT_CTRL
HRESULT RenderAreaMessageHandler::Nui_Init()
{
    ComPtr<IWindow> window;
    HRESULT hr = GetWindow(&window);

    HWND hWnd;
    if (SUCCEEDED(hr))
    {
        hr = window->GetWindowHandle(&hWnd);
    }

    m_hNextDepthFrameEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    m_hNextColorFrameEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    m_hNextSkeletonEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);

    hr = NuiCreateSensorByIndex(0, &m_pNuiSensor);

    if (FAILED(hr))
    {
        //kinect not usable
        return S_FALSE;
    }

    DWORD nuiFlags = NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX | NUI_INITIALIZE_FLAG_USES_SKELETON |  NUI_INITIALIZE_FLAG_USES_COLOR;
    hr = m_pNuiSensor->NuiInitialize( nuiFlags );

    //BSTR id = m_pNuiSensor->NuiDeviceConnectionId();

    if (FAILED(hr))
    {
        //kinect not usable
        return S_FALSE;
    }

    hr = m_pNuiSensor->NuiSkeletonTrackingEnable(m_hNextSkeletonEvent, 0);

    if (SUCCEEDED(hr))
    {
        hr = m_pNuiSensor->NuiImageStreamOpen(
            NUI_IMAGE_TYPE_COLOR,
            NUI_IMAGE_RESOLUTION_640x480,
            0,
            2,
            m_hNextColorFrameEvent,
            &m_pVideoStreamHandle );
    }

    if (SUCCEEDED(hr))
    {
        hr = m_pNuiSensor->NuiImageStreamOpen(
            HasSkeletalEngine(m_pNuiSensor) ? NUI_IMAGE_TYPE_DEPTH_AND_PLAYER_INDEX : NUI_IMAGE_TYPE_DEPTH,
            NUI_IMAGE_RESOLUTION_320x240,
            0,
            2,
            m_hNextDepthFrameEvent,
            &m_pDepthStreamHandle );
    }

    m_hEvNuiProcessStop = CreateEvent(nullptr, FALSE, FALSE, nullptr);

    tasks.run([this]
    {
        NUI_IMAGE_FRAME imageFrame;
        HANDLE hEvents[4] = { m_hEvNuiProcessStop, m_hNextDepthFrameEvent, m_hNextColorFrameEvent, m_hNextSkeletonEvent };
        int    nEventIdx;

        bool continueProcessing = true;
        while ( continueProcessing )
        {
            // Wait for any of the events to be signalled
            nEventIdx = WaitForMultipleObjects(4, hEvents, FALSE, 33);

            // Process signal events
            switch ( nEventIdx )
            {
            case WAIT_TIMEOUT:
                continue;

            // If the stop event, stop looping and exit
            case WAIT_OBJECT_0:
                continueProcessing = false;
                continue;

            case WAIT_OBJECT_0 + 1:
                m_pNuiSensor->NuiImageStreamGetNextFrame(
                    m_pDepthStreamHandle,
                    0,
                    &imageFrame );
                m_pNuiSensor->NuiImageStreamReleaseFrame(m_pDepthStreamHandle, &imageFrame);
                break;

            case WAIT_OBJECT_0 + 2:
                m_pNuiSensor->NuiImageStreamGetNextFrame(
                    m_pVideoStreamHandle,
                    0,
                    &imageFrame );
                m_pNuiSensor->NuiImageStreamReleaseFrame(m_pVideoStreamHandle, &imageFrame);
                break;

            case WAIT_OBJECT_0 + 3:
                Nui_GotSkeletonAlert( );
                break;
            }
        }
    });

    return hr;
}
#endif

#ifdef KINECT_CTRL
void RenderAreaMessageHandler::Nui_GotSkeletonAlert()
{
    NUI_SKELETON_FRAME SkeletonFrame = {0};

    bool bFoundSkeleton = false;
    int skeletonIndex = -1;

    if (SUCCEEDED(m_pNuiSensor->NuiSkeletonGetNextFrame( 0, &SkeletonFrame )))
    {
        for ( int i = 0 ; i < NUI_SKELETON_COUNT ; i++ )
        {
            if(SkeletonFrame.SkeletonData[i].eTrackingState == NUI_SKELETON_TRACKED ||
                (SkeletonFrame.SkeletonData[i].eTrackingState == NUI_SKELETON_POSITION_ONLY))
            {
                if(skeletonIndex < 0 || 
                    SkeletonFrame.SkeletonData[i].Position.z < SkeletonFrame.SkeletonData[skeletonIndex].Position.z)
                {
                    skeletonIndex = i;
                    bFoundSkeleton = true;
                }
            }
        }
    }

    // no skeletons!
    if( !bFoundSkeleton )
    {
        return;
    }

    // smooth out the skeleton data
    HRESULT hr = m_pNuiSensor->NuiTransformSmooth(&SkeletonFrame,NULL);
    if (FAILED(hr))
    {
        return;
    }

    //use only the first person found
    NUI_SKELETON_DATA& skeleton = SkeletonFrame.SkeletonData[skeletonIndex];
    Vector4 *joints = skeleton.SkeletonPositions;

    Vector4 leftHand = joints[NUI_SKELETON_POSITION_HAND_LEFT];
    Vector4 rightHand = joints[NUI_SKELETON_POSITION_HAND_RIGHT];
    Vector4 soulderCenter = joints[NUI_SKELETON_POSITION_SHOULDER_CENTER];
    Vector4 head = joints[NUI_SKELETON_POSITION_HEAD];

    //determin the arm is stretched

    static const float stretched_dist = 0.4f;

    bool left_stretched = soulderCenter.z - leftHand.z >= stretched_dist;
    bool right_stretched = soulderCenter.z - rightHand.z >= stretched_dist;

    ComPtr<IWindow> window;
    HWND hWnd;

    hr = GetWindow(&window);
    hr = window->GetParentWindowHandle(&hWnd);

    if (left_stretched != m_left_stretched)
    {
        m_left_stretched = left_stretched;

        if (left_stretched)
        {
            m_lefthandpos = D2D1::Point2F(leftHand.x, leftHand.y);

            m_lastcenterx = m_centerx;
            m_lastcentery = m_centery;
        }
    }


    if(right_stretched != m_right_stretched)
    {
        m_right_stretched = right_stretched;

        if (right_stretched)
        {
            m_righthandpos = D2D1::Point2F(rightHand.x, rightHand.y);

            m_lastcenterx = m_centerx;
            m_lastcentery = m_centery;
        }
    }

    if (leftHand.y > head.y && rightHand.y > head.y)
    {
        //hands raised
        PostMessage(hWnd, WM_SYSCOMMAND, SC_MAXIMIZE,0);
        return;
    }

    bool isresizing = left_stretched && right_stretched && (leftHand.z - rightHand.z < 0.1f);

    if (m_resizing != isresizing)
    {
        if (isresizing)
        {
            m_lefthandpos = D2D1::Point2F(leftHand.x, leftHand.y);
            m_righthandpos = D2D1::Point2F(rightHand.x, rightHand.y);
            m_resizing = isresizing;
        }
        else
        {
            if (left_stretched || right_stretched)
            {
                return;
            }
            else
            {
                m_resizing = false;
            }
            m_lastscale = m_scale;
        }
    }

    if(m_left_stretched ^ m_right_stretched)
    {
        //handle single hand movemehnts
        double dx, dy;
        if (m_left_stretched)
        {
            dx = m_lefthandpos.x - leftHand.x;
            dy = m_lefthandpos.y - leftHand.y;       
        }
        else if(m_right_stretched)
        {
            dx = m_righthandpos.x - rightHand.x;
            dy = m_righthandpos.y - rightHand.y;
        }

        m_centerx = m_lastcenterx + dx * 5.0 / m_scale;
        m_centery = m_lastcentery + dy * 6.0 / m_scale;

        window->RedrawWindow();
    }
    else if(m_resizing)
    {
        //handle two hands resizing

        float cdx = rightHand.x - leftHand.x;
        float cdy = rightHand.y - leftHand.y;

        float pdx = m_righthandpos.x - m_lefthandpos.x;
        float pdy = m_righthandpos.y = m_lefthandpos.y;

        float scale_diff = sqrt((cdx * cdx + cdy * cdy) / (pdx * pdx + pdy * pdy));

        scale_diff = std::max(0.1f, scale_diff);

        m_scale = m_lastscale * scale_diff;

        window->RedrawWindow();
    }
}
#endif