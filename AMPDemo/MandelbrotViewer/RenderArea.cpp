#include "stdafx.h"
#include "RenderArea.h"
#include "mandelbrot.h"
#include "d3d11.h"
#include "dxgi.h"

RenderAreaMessageHandler::RenderAreaMessageHandler(void)
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

	return hr;
}

HRESULT RenderAreaMessageHandler::OnEraseBackground()
{
	return S_OK;
}

HRESULT RenderAreaMessageHandler::OnRender()
{
	using namespace Concurrency;

	double real_center = -0.746826546356156;
    double imag_center = 0.113746476309378;
    double scale = 1000;

	double d = 1 / scale;

	const unsigned int width = 640;
	const unsigned int height = 640;
	std::vector<unsigned int> data(width * height);

	array_view<unsigned int, 2> arrayview(width, height, data);

	generate_mandelbrot(arrayview, 1024, real_center - d, imag_center - d, real_center + d, imag_center + d);

	arrayview.synchronize();

	ComPtr<ID2D1Bitmap> bitmap;
	HRESULT hr = m_renderTarget->CreateBitmap(
		D2D1::SizeU(width, height),
		static_cast<void*>(data.data()),
		width * sizeof(unsigned int),
		D2D1::BitmapProperties(
			D2D1::PixelFormat(
				DXGI_FORMAT_B8G8R8A8_UNORM,
				D2D1_ALPHA_MODE_IGNORE
			)),
		&bitmap);



	m_renderTarget->BeginDraw();
	m_renderTarget->Clear();

	m_renderTarget->DrawBitmap(bitmap, 
		D2D1::RectF(0.0, 0.0, static_cast<float>(width), static_cast<float>(height)));

	m_renderTarget->EndDraw();
	return S_OK;
}

/*
	array<unsigned int, 2> a(64, 64);

	parallel_for_each(a.grid, [&a](index<2> i) restrict(direct3d)
	{
		a[i] = 0xCCCCCCCC;
	});

	ComPtr<IUnknown> bufferPtr = direct3d::get_buffer(a);

	ComPtr<ID3D11Buffer> buffer;
	HRESULT hr = bufferPtr.QueryInterface(&buffer);

	ComPtr<ID3D11Device> device;
	if (SUCCEEDED(hr))
	{
		buffer->GetDevice(&device);
	}

	static const DXGI_FORMAT format = DXGI_FORMAT_B8G8R8A8_UNORM;

	//Create a texture
	ComPtr<ID3D11Texture2D> texture;
	D3D11_TEXTURE2D_DESC tdesc;

	tdesc.Width = 64;
	tdesc.Height = 64;
	tdesc.MipLevels = 1;
	tdesc.ArraySize = 1;

	tdesc.SampleDesc.Count = 1;
	tdesc.SampleDesc.Quality = 0;
	tdesc.Usage = D3D11_USAGE_DEFAULT;
	tdesc.Format = format;    
	tdesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	tdesc.CPUAccessFlags = 0;    
	tdesc.MiscFlags = 0;

	hr = device->CreateTexture2D(
		&tdesc,
		nullptr,
		&texture);

	
	ComPtr<ID3D11DeviceContext> context;
	if (SUCCEEDED(hr))
	{
		device->GetImmediateContext(&context);
	}

	context->CopyResource(buffer, texture);

	ComPtr<IDXGISurface> dxgiSurface;
	hr = texture.QueryInterface(&dxgiSurface);

	ComPtr<ID2D1Bitmap> sharedBitmap;

	if (SUCCEEDED(hr))
	{
		D2D1_BITMAP_PROPERTIES bprop;
		bprop.pixelFormat = D2D1::PixelFormat(
			format,
			D2D1_ALPHA_MODE_IGNORE);
		bprop.dpiX = 0.0f;
		bprop.dpiY = 0.0f;

		hr = m_renderTarget->CreateSharedBitmap(
			__uuidof(IDXGISurface),
			dxgiSurface,
			&bprop,
			&sharedBitmap);
	}
*/

HRESULT RenderAreaMessageHandler::OnSize(unsigned int width, unsigned int height)
{
	using namespace D2D1;

	HRESULT hr = m_renderTarget->Resize(SizeU(width, height));

	return hr;
}

HRESULT RenderAreaMessageHandler::OnLeftMouseButtonDown(D2D1_POINT_2F mousePosition)
{
	return S_OK;
}

HRESULT RenderAreaMessageHandler::OnLeftMouseButtonUp(D2D1_POINT_2F mousePosition)
{
	return S_OK;
}

HRESULT RenderAreaMessageHandler::OnMouseMove(D2D1_POINT_2F mousePosition)
{
	return S_OK;
}

HRESULT RenderAreaMessageHandler::OnMouseEnter(D2D1_POINT_2F mousePosition)
{
	return S_OK;
}

HRESULT RenderAreaMessageHandler::OnMouseWheel(D2D1_POINT_2F mousePosition, short delta, int keys)
{
	return S_OK;
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