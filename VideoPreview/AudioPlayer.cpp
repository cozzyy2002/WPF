#include "stdafx.h"
#include "AudioPlayer.h"
#include "Win32.h"
#include <string.h>
#include <vcclr.h>

using namespace System::ComponentModel;
using namespace DirectX;
using namespace System;

CAudioPlayer::CAudioPlayer(System::String^ mediaFile)
	: mediaFile(mediaFile), pGraph(NULL), pControl(NULL), pSeeking(NULL)
{
	hStop = CreateEvent(NULL, TRUE, FALSE, NULL);

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
	} catch(Exception^ ex) {
		Console::WriteLine("Exception: {0}", ex->Message);
	}
}

CAudioPlayer::~CAudioPlayer()
{
	this->!CAudioPlayer();
}

CAudioPlayer::!CAudioPlayer()
{
	this->stop();

	if(pControl) {
		pControl->Stop();
		pControl->Release();
	}

	if(pSeeking) pSeeking->Release();
	if(pGraph) pGraph->Release();
}

void CAudioPlayer::start(bool repeat, unsigned int interval)
{
	this->repeat = repeat;
	this->interval = interval;
	::ResetEvent(hStop);

	try {
		BackgroundWorker^ worker = gcnew BackgroundWorker();
		worker->DoWork += gcnew DoWorkEventHandler(this, &CAudioPlayer::handleMediaEvent);
		worker->RunWorkerAsync();

		HRESULT_CHECK(pControl->Run());
	} catch(Exception^ ex) {
		Console::WriteLine("Exception: {0}", ex->Message);
	}
}

void CAudioPlayer::stop()
{
	// Tell worker thread to terminate
	::SetEvent(hStop);
	HRESULT_CHECK(pControl->Stop());
}

void CAudioPlayer::handleMediaEvent(Object ^sender, DoWorkEventArgs ^e)
{
	CComPtr<IMediaEvent> pEvent;
	HRESULT_CHECK(pGraph->QueryInterface(IID_IMediaEvent, (void**)&pEvent));
	HANDLE hEvents[] = {hStop, 0};
	HRESULT_CHECK(pEvent->GetEventHandle((OAEVENT*)&hEvents[1]));

	bool exit = true;
	do {
		// Wait for stop() method called or completed to play back
		DWORD wait = ::WaitForMultipleObjects(ARRAYSIZE(hEvents), hEvents, FALSE, INFINITE);
		switch(wait) {
		case WAIT_OBJECT_0:
			// stop() method called
			break;
		case WAIT_OBJECT_0 + 1:
			// Media event occurs
			long eventCode;
			LONG_PTR lParam1, lParam2;
			try {
				HRESULT_CHECK(pEvent->GetEvent(&eventCode, &lParam1, &lParam2, INFINITE));
				switch(eventCode) {
				case EC_COMPLETE:
					// Completed to play back
					if(repeat) {
						if(interval) {
							wait = ::WaitForSingleObject(hStop, interval);
							if(wait == WAIT_OBJECT_0) break;
						}
						// Seek to top of stream
						LONGLONG current = 0;
						HRESULT_CHECK(pSeeking->SetPositions(&current, AM_SEEKING_AbsolutePositioning, NULL, AM_SEEKING_NoPositioning));
						exit = false;
					}
					break;
				default:
					// Ignore other media event
					exit = false;
					break;
				}
			} catch(Exception^ ex) {
				Console::WriteLine("Exception: {0}", ex->Message);
			}
			HRESULT_CHECK(pEvent->FreeEventParams(eventCode, lParam1, lParam2));
			break;
		default:
			Console::WriteLine("Unexpected wait result: {0}", wait);
			break;
		}
	} while(!exit);
}
