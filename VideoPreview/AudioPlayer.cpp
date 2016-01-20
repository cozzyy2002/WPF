#include "stdafx.h"
#include "AudioPlayer.h"
#include "Win32.h"
#include <string.h>
#include <vcclr.h>

using namespace System::ComponentModel;
using namespace DirectX;
using namespace System;

CAudioPlayer::CAudioPlayer()
	: pGraph(NULL), pControl(NULL), pSeeking(NULL)
{
	hShutdown = CreateEvent(NULL, TRUE, FALSE, NULL);
}

CAudioPlayer::~CAudioPlayer()
{
	this->!CAudioPlayer();
}

CAudioPlayer::!CAudioPlayer()
{
	// Tell worker thread to terminate
	::SetEvent(hShutdown);

	if(pControl) {
		pControl->Stop();
		pControl->Release();
	}

	if(pSeeking) pSeeking->Release();
	if(pGraph) pGraph->Release();
}

void CAudioPlayer::setup(System::String^ mediaFile)
{
	try {
		CComPtr<IGraphBuilder> pGraph;
		HRESULT_CHECK(pGraph.CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER));

		pin_ptr<const wchar_t> str = PtrToStringChars(mediaFile);
		HRESULT_CHECK(pGraph->RenderFile(str, NULL));

		// Create IMediaControl object
		CComPtr<IMediaControl> pControl;
		HRESULT_CHECK(pGraph.QueryInterface<IMediaControl>(&pControl));
		CComPtr<IMediaSeeking> pSeeking;
		HRESULT_CHECK(pGraph.QueryInterface<IMediaSeeking>(&pSeeking));

		// Move COM objects to this member variables
		this->pGraph = pGraph.Detach();
		this->pControl = pControl.Detach();
		this->pSeeking = pSeeking.Detach();

		BackgroundWorker^ worker = gcnew BackgroundWorker();
		worker->DoWork += gcnew DoWorkEventHandler(this, &CAudioPlayer::handleMediaEvent);
		worker->RunWorkerAsync();
	} catch(Exception^ ex) {
		Console::WriteLine("Exception: {0}", ex->Message);
	}
}

void CAudioPlayer::handleMediaEvent(Object ^sender, DoWorkEventArgs ^e)
{
	try {
		CComPtr<IMediaEvent> pEvent;
		HRESULT_CHECK(pGraph->QueryInterface(IID_IMediaEvent, (void**)&pEvent));
		HANDLE hEvents[] = {hShutdown, 0};
		HRESULT_CHECK(pEvent->GetEventHandle((OAEVENT*)&hEvents[1]));

		bool exit = false;
		do {
			DWORD wait = ::WaitForMultipleObjects(ARRAYSIZE(hEvents), hEvents, FALSE, INFINITE);
			switch(wait) {
			case WAIT_OBJECT_0:
				// hShutdown is set
				exit = true;
				break;
			case WAIT_OBJECT_0 + 1:
				// Media event occurs
				{
					long eventCode;
					LONG_PTR lParam1, lParam2;
					HRESULT_CHECK(pEvent->GetEvent(&eventCode, &lParam1, &lParam2, INFINITE));
					switch(eventCode) {
					case EC_COMPLETE:
						LONGLONG current = 0;
						pSeeking->SetPositions(&current, AM_SEEKING_AbsolutePositioning, NULL, AM_SEEKING_NoPositioning);
						break;
					}
					HRESULT_CHECK(pEvent->FreeEventParams(eventCode, lParam1, lParam2));
				}
				break;
			default:
				Console::WriteLine("Unexpected wait result: {0}", wait);
				exit = true;
				break;
			}
		} while(!exit);
	} catch(Exception^ ex) {
		Console::WriteLine("Exception: {0}", ex->Message);
	}
}

void CAudioPlayer::start()
{
	HRESULT_CHECK(pControl->Run());
}

void CAudioPlayer::stop()
{
	::SetEvent(hShutdown);
	HRESULT_CHECK(pControl->Stop());
}
