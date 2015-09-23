// This is the main DLL file.

#include "stdafx.h"
#include "VideoPreview.h"

#pragma comment(lib, "Strmiids")

using namespace DirectX;
using namespace System;
using namespace System::Windows::Controls;
using namespace System::Windows::Threading;

#define HRESULT_CHECK(method) hResultCheck(method, #method)

static HRESULT hResultCheck(HRESULT hr, String^ method)
{
	if(FAILED(hr)) {
		throw gcnew Win32::ComOperationFailedException(String::Format("'{1}' failed. error=0x{0:x}", hr, method), hr);
	}
	return hr;
}

CVideoPreview::CVideoPreview(void) : isStarted(false)
{
	pGraph = NULL;
	pVideoWindow = NULL;
	pControl = NULL;
	hWndHost = nullptr;
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

/**
 * Setup CHWndHostControl object and call setup(hwnd, double, double) method in lowest priority.
 *
 * Call this method on Load event of host window
 */
void CVideoPreview::setup(Decorator^ parent)
{
	// Create control to host video window
	hWndHost = gcnew Win32::CHWndHostControl();
	hWndHost->attach(parent);
	IntPtr hwnd = hWndHost->Handle;
	double width = parent->ActualWidth;
	double height = parent->ActualHeight;
	Console::WriteLine("Video window: hwnd=0x{0:x},size=({1},{2})", hwnd, width, height);

	Dispatcher::CurrentDispatcher->BeginInvoke(
		DispatcherPriority::SystemIdle,
		gcnew Action<IntPtr, double, double>(this, &CVideoPreview::setup),
		hwnd, width, height);

	Console::WriteLine("Video window is created.");
}

/**
 * Setup DirectShow COM objects
 */
void CVideoPreview::setup(IntPtr hwnd, double width, double height)
{
	try {
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
		if(S_OK != HRESULT_CHECK(pDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEnum, 0))) {
			Console::WriteLine("Video input device is not found.");
			return;
		}
		CComPtr<IMoniker> pMoniker;
		HRESULT_CHECK(pEnum->Next(1, &pMoniker, NULL));

		// Create capture filter for the camera device
		CComPtr<IBaseFilter> pCap;
		HRESULT_CHECK(pMoniker->BindToObject(0, 0, IID_IBaseFilter, (void**)&pCap));

		// Connect filters in the Filter Graph
		HRESULT_CHECK(pGraph->AddFilter(pCap, NULL));
		HRESULT_CHECK(pBuild->RenderStream(&PIN_CATEGORY_PREVIEW, &MEDIATYPE_Video, pCap, NULL, NULL));

		// Setup video window
		CComPtr<IVideoWindow> pVideoWindow;
		HRESULT_CHECK(pGraph.QueryInterface<IVideoWindow>(&pVideoWindow));
		HRESULT_CHECK(pVideoWindow->put_Owner((OAHWND)(HANDLE)hwnd));
		HRESULT_CHECK(pVideoWindow->put_WindowStyle(WS_CHILD | WS_CLIPSIBLINGS));
		HRESULT_CHECK(pVideoWindow->SetWindowPosition(0, 0, (long)width, (long)height));

		// Create IMediaControl object
		CComPtr<IMediaControl> pControl;
		HRESULT_CHECK(pGraph.QueryInterface<IMediaControl>(&pControl));

		// Move COM objects to this member variables
		this->pGraph = pGraph.Detach();
		this->pVideoWindow = pVideoWindow.Detach();
		this->pControl = pControl.Detach();
		Console::WriteLine("Setup DirectShow is completed.");

	} catch(Exception^ ex) {
		Console::WriteLine("Exception: ", ex->Message);
	}
}

void CVideoPreview::start()
{
	if(!pVideoWindow || !pControl) {
		Console::WriteLine("COM object(s) is not properly created.");
		return;
	}

	HRESULT_CHECK(pVideoWindow->put_Visible(OATRUE));
	HRESULT_CHECK(pControl->Run());

	this->IsStarted = true;
}

void CVideoPreview::stop()
{
	if(!pVideoWindow || !pControl) {
		Console::WriteLine("COM object(s) is not properly created.");
		return;
	}

	HRESULT_CHECK(pVideoWindow->put_Visible(OAFALSE));
	HRESULT_CHECK(pControl->Stop());

	this->IsStarted = false;
}
