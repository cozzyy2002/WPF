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
			Console::WriteLine("'" #method "' failed. error=0x{0:x}", hr);	\
			return;															\
		}																	\
	} while(false)

CVideoPreview::CVideoPreview(void)
{
	pGraph = NULL;
	pVideoWindow = NULL;
	pControl = NULL;
}

CVideoPreview::~CVideoPreview()
{
	this->!CVideoPreview();
}

CVideoPreview::!CVideoPreview()
{
	if(pControl) {
		pControl->Stop();
		pControl->Release();
	}

	// Remove owner of video window before releasing Filter Graph Manager.
	if(pVideoWindow) {
		pVideoWindow->put_Owner(NULL);
		pVideoWindow->Release();
	}
	if(pGraph) pGraph->Release();
}

void CVideoPreview::start(IntPtr hWnd, double width, double height)
{
	Console::WriteLine("CVideoPreview::start(0x{0:x},{1},{2})", hWnd, width, height);

	// Initialize Filter Graph Manager and Capture Graph Manager
	CComPtr<IGraphBuilder> pGraph;
	CComPtr<ICaptureGraphBuilder2> pBuild;
	HRESULT_CHECK(pGraph.CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER));
	HRESULT_CHECK(pBuild.CoCreateInstance(CLSID_CaptureGraphBuilder2, NULL, CLSCTX_INPROC_SERVER));
	HRESULT_CHECK(pBuild->SetFiltergraph(pGraph));

	// Find a camera device
	CComPtr<ICreateDevEnum> pDevEnum;
	HRESULT_CHECK(pDevEnum.CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER));
	CComPtr<IEnumMoniker> pEnum;
	HRESULT_CHECK(pDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEnum, 0));
	if(!pEnum) {
		Console::WriteLine("Video input device is not found.");
		return;
	}
	CComPtr<IMoniker> pMoniker;
	HRESULT_CHECK(pEnum->Next(1, &pMoniker, NULL));

	// Create capture filter
	CComPtr<IBaseFilter> pCap;
	HRESULT_CHECK(pMoniker->BindToObject(0, 0, IID_IBaseFilter, (void**)&pCap));

	// Connect filters in the Filter Graph
	HRESULT_CHECK(pGraph->AddFilter(pCap, NULL));
	HRESULT_CHECK(pBuild->RenderStream(&PIN_CATEGORY_PREVIEW, &MEDIATYPE_Video, pCap, NULL, NULL));

	// Setup video window
	CComPtr<IVideoWindow> pVideoWindow;
	HRESULT_CHECK(pGraph.QueryInterface<IVideoWindow>(&pVideoWindow));
	HRESULT_CHECK(pVideoWindow->put_Owner((OAHWND)(HANDLE)hWnd));
	HRESULT_CHECK(pVideoWindow->put_WindowStyle(WS_CHILD | WS_CLIPSIBLINGS));
	HRESULT_CHECK(pVideoWindow->SetWindowPosition(0, 0, (long)width, (long)height));

	// Start preview
	CComPtr<IMediaControl> pControl;
	HRESULT_CHECK(pGraph.QueryInterface<IMediaControl>(&pControl));
	HRESULT_CHECK(pControl->Run());

	// Move COM objects to this member variables
	this->pGraph = pGraph.Detach();
	this->pVideoWindow = pVideoWindow.Detach();
	this->pControl = pControl.Detach();
}

void CVideoPreview::stop()
{
	HRESULT_CHECK(pControl->Stop());
}