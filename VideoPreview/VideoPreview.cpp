// This is the main DLL file.

#include "stdafx.h"
#include "VideoPreview.h"
#include "Win32.h"

#pragma comment(lib, "Strmiids")

using namespace DirectX;
using namespace System;
using namespace System::Windows;
using namespace System::Windows::Controls;
using namespace System::Windows::Threading;

static CVideoPreview::CVideoPreview(void)
{
	logger = log4net::LogManager::GetLogger(CVideoPreview::typeid);
}

CVideoPreview::CVideoPreview(void) : isStarted(false)
{
	pGraph = NULL;
	pVideoWindow = NULL;
	pControl = NULL;
	hWndHost = nullptr;
	setupResult = nullptr;
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
void CVideoPreview::setup(CDevice^ camera, Decorator^ parent)
{
	// Create control to host video window
	hWndHost = gcnew Win32::CHWndHostControl();
	hWndHost->attach(parent);
	IntPtr hwnd = hWndHost->Handle;
	double width = parent->ActualWidth;
	double height = parent->ActualHeight;
	if(logger->IsDebugEnabled) logger->DebugFormat("Video window: hwnd=0x{0:x},size=({1},{2})", hwnd, width, height);

	setupResult = Dispatcher::CurrentDispatcher->BeginInvoke(
		DispatcherPriority::SystemIdle,
		gcnew Action<CDevice^, IntPtr, double, double>(this, &CVideoPreview::setup),
		camera, hwnd, width, height);

	if(logger->IsDebugEnabled) logger->DebugFormat("Video window is created.");
}

/**
 * Setup COM objects for DirectShow
 */
void CVideoPreview::setup(CDevice^ camera, IntPtr hwnd, double width, double height)
{
	if(!camera->Is(CDevice::VideoInputDeviceCategory)) {
		throw gcnew Exception(String::Format("'{0}' is not {1}", camera->FriendlyName, CDevice::VideoInputDeviceCategory->Name));
	}

	try {
		// Initialize Filter Graph Manager and Capture Graph Manager
		CComPtr<IGraphBuilder> pGraph;
		CComPtr<ICaptureGraphBuilder2> pBuild;
		HRESULT_CHECK(pGraph.CoCreateInstance(CLSID_FilterGraph));
		HRESULT_CHECK(pBuild.CoCreateInstance(CLSID_CaptureGraphBuilder2));
		HRESULT_CHECK(pBuild->SetFiltergraph(pGraph));

		// Create capture filter for the camera device
		CComPtr<IBaseFilter> pCap(camera->getFilter());

		// Connect filters in the Filter Graph
		HRESULT_CHECK(pGraph->AddFilter(pCap, NULL));
		HRESULT_CHECK(pBuild->RenderStream(&PIN_CATEGORY_PREVIEW, &MEDIATYPE_Video, pCap, NULL, NULL));

		// Setup video window
		CComPtr<IVideoWindow> pVideoWindow;
		HRESULT_CHECK(pGraph.QueryInterface(&pVideoWindow));
		HRESULT_CHECK(pVideoWindow->put_Owner((OAHWND)(HANDLE)hwnd));
		HRESULT_CHECK(pVideoWindow->put_WindowStyle(WS_CHILD | WS_CLIPSIBLINGS));
		HRESULT_CHECK(pVideoWindow->SetWindowPosition(0, 0, (long)width, (long)height));

		// Create IMediaControl object
		CComPtr<IMediaControl> pControl;
		HRESULT_CHECK(pGraph.QueryInterface(&pControl));

		// Move COM objects to this member variables
		this->pGraph = pGraph.Detach();
		this->pVideoWindow = pVideoWindow.Detach();
		this->pControl = pControl.Detach();
		if(logger->IsDebugEnabled) logger->DebugFormat("Setup DirectShow is completed.");

	} catch(Exception^ ex) {
		if(logger->IsErrorEnabled) logger->ErrorFormat("Exception: ", ex->Message);
	}
}

void CVideoPreview::setSize(Size size)
{
	if(this->pVideoWindow) {
		this->pVideoWindow->SetWindowPosition(0, 0, (long)size.Width, (long)size.Height);
		this->hWndHost->Width = size.Width;
		this->hWndHost->Height = size.Height;
	}
}

void CVideoPreview::start()
{
	if(!setupResultCheck()) return;

	HRESULT_CHECK(pVideoWindow->put_Visible(OATRUE));
	HRESULT_CHECK(pControl->Run());

	this->IsStarted = true;
}

void CVideoPreview::stop()
{
	if(!setupResultCheck()) return;

	HRESULT_CHECK(pControl->Stop());
	HRESULT_CHECK(pVideoWindow->put_Visible(OAFALSE));

	this->IsStarted = false;
}

bool CVideoPreview::setupResultCheck()
{
	if(setupResult == nullptr) {
		if(logger->IsErrorEnabled) logger->ErrorFormat("setup() is not called.");
		return false;
	}

	try {
		DispatcherOperationStatus st = setupResult->Wait(TimeSpan(0, 0, 5));
		if(st != DispatcherOperationStatus::Completed) {
			if(logger->IsErrorEnabled) logger->ErrorFormat("Failed to wait for setup to complete: {0}", st);
			return false;
		}
	} catch(Exception^ ex) {
		if(logger->IsErrorEnabled) logger->ErrorFormat("{0}: {1}", ex->GetType(), ex->Message);
		return false;
	}

	if(!pVideoWindow || !pControl) {
		if(logger->IsErrorEnabled) logger->ErrorFormat("COM object(s) is not properly created.");
		return false;
	}
	return true;
}