// This is the main DLL file.

#include "stdafx.h"
#include "VideoPreview.h"

#pragma comment(lib, "Strmiids")

using namespace System;
using namespace DirectX;

#define HRESULT_CHECK(method) \
	do {																	\
		HRESULT hr = method;												\
		if(FAILED(hr)) {													\
			Console::WriteLine("'" #method "' failed. error={0}", hr);		\
			return;															\
		}																	\
	} while(false)

CVideoPreview::CVideoPreview(void)
{
	pBuild = NULL;
	pGraph = NULL;
	pVideoWindow = NULL;
}

CVideoPreview::~CVideoPreview()
{
	this->!CVideoPreview();
}

CVideoPreview::!CVideoPreview()
{
	if(pVideoWindow) {
		pVideoWindow->put_Owner(NULL);
		pVideoWindow->Release();
	}
	if(pBuild) pBuild->Release();
	if(pGraph) pGraph->Release();
}

void CVideoPreview::start(IntPtr hWnd, double width, double heigh)
{
	// Initialize Filter Graph Manager and Capture Graph Manager
	CComPtr<IGraphBuilder> pGraph;
	CComPtr<ICaptureGraphBuilder2> pBuild;
	HRESULT_CHECK(pGraph.CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER));
	HRESULT_CHECK(pBuild.CoCreateInstance(CLSID_CaptureGraphBuilder2, NULL, CLSCTX_INPROC_SERVER));
	HRESULT_CHECK(pBuild->SetFiltergraph(pGraph));
	this->pGraph = pGraph.Detach();
	this->pBuild = pBuild.Detach();

	// Find a camera device
	CComPtr<ICreateDevEnum> pDevEnum;
	HRESULT_CHECK(pDevEnum.CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER));
	CComPtr<IEnumMoniker> pEnum;
	HRESULT_CHECK(pDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEnum, 0));
	CComPtr<IMoniker> pCameraMoniker;
	HRESULT_CHECK(pEnum->Next(1, &pCameraMoniker, NULL));

	// Preview video
	HRESULT_CHECK(pBuild->RenderStream(&PIN_CATEGORY_PREVIEW, &MEDIATYPE_Video, pCameraMoniker, NULL, NULL));

	CComPtr<IVideoWindow> pVideoWindow;
	HRESULT_CHECK(pVideoWindow->put_Owner((OAHWND)(HANDLE)hWnd));
	HRESULT_CHECK(pVideoWindow->put_WindowStyle(WS_CHILD | WS_CLIPSIBLINGS));
	HRESULT_CHECK(pVideoWindow->SetWindowPosition(0, 0, (long)width, (long)heigh));
	this->pVideoWindow = pVideoWindow.Detach();
}

void CVideoPreview::stop()
{
}
