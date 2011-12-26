#include "stdafx.h"
#include "RenderArea.h"


RenderAreaMessageHandler::RenderAreaMessageHandler(void)
{
}


RenderAreaMessageHandler::~RenderAreaMessageHandler(void)
{
}

HRESULT RenderAreaMessageHandler::OnCreate()
{
	return S_OK;
}

HRESULT RenderAreaMessageHandler::OnEraseBackground()
{
	return S_OK;
}

HRESULT RenderAreaMessageHandler::OnRender()
{
	return S_OK;
}

HRESULT RenderAreaMessageHandler::OnSize(unsigned int width, unsigned int height)
{
	return S_OK;
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
	return S_OK;
}