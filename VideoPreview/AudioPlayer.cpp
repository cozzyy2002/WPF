#include "stdafx.h"
#include "AudioPlayer.h"
#include "Win32.h"
#include <string.h>
#include <vcclr.h>

using namespace System::ComponentModel;
using namespace DirectX;
using namespace System;
using namespace System::Windows::Threading;

CAudioPlayer::CAudioPlayer(System::String^ mediaFile)
	: dispatcher(Dispatcher::CurrentDispatcher)
	, pGraph(NULL), pControl(NULL), pSeeking(NULL)
	, logger(log4net::LogManager::GetLogger(this->GetType()))
{
	if(logger->IsDebugEnabled) logger->DebugFormat("CAudioPlayer({0})", mediaFile);

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
		CComPtr<IMediaEventEx> pMediaEvent;
		HRESULT_CHECK(pGraph.QueryInterface(&pMediaEvent));
		HRESULT_CHECK(pMediaEvent->SetNotifyFlags(AM_MEDIAEVENT_NONOTIFY));
		HANDLE h;	// Temporally handle for interior_ptr<HANDLE> hEndOfStream
		HRESULT_CHECK(pMediaEvent->GetEventHandle((OAEVENT*)&h));
		hEndOfStream = h;

		// Move COM objects to this member variables
		this->pGraph = pGraph.Detach();
		this->pControl = pControl.Detach();
		this->pSeeking = pSeeking.Detach();
	} catch(Exception^ ex) {
		if(logger->IsErrorEnabled) logger->ErrorFormat("CAudioPlayer() Exception: {0}", ex->Message);
	}
}

CAudioPlayer::~CAudioPlayer()
{
	if(logger->IsDebugEnabled) logger->Debug("~CAudioPlayer()");

	this->!CAudioPlayer();
}

CAudioPlayer::!CAudioPlayer()
{
	try {
		this->stop();

		if(pControl) {
			pControl->Stop();
			pControl->Release();
		}

		if(pSeeking) pSeeking->Release();
		if(pGraph) pGraph->Release();
	} catch(Exception^ ex) {
		if(logger->IsErrorEnabled) logger->ErrorFormat("!CAudioPlayer() Exception: {0}", ex->Message);
	}
}

void CAudioPlayer::start()
{
	if(logger->IsDebugEnabled) logger->DebugFormat("start(): IsPlaying={0}", IsPlaying);

	if(IsPlaying) return;
	setIsPlaying(true);

	::ResetEvent(hStop);

	try {
		handleEventTask = gcnew System::Threading::Tasks::Task(gcnew Action(this, &CAudioPlayer::handleMediaEvent));
		handleEventTask->Start();

		HRESULT_CHECK(pControl->Run());
	} catch(Exception^ ex) {
		if(logger->IsErrorEnabled) logger->ErrorFormat("start() Exception: {0}", ex->Message);
	}
}

void CAudioPlayer::stop()
{
	if(logger->IsDebugEnabled) logger->DebugFormat("stop(): IsPlaying={0}", IsPlaying);

	try {
		// Tell worker thread to terminate
		::SetEvent(hStop);
		handleEventTask->Wait();
		HRESULT_CHECK(pControl->Stop());

		setIsPlaying(false);
	} catch(Exception^ ex) {
		if(logger->IsErrorEnabled) logger->ErrorFormat("stop() Exception: {0}", ex->Message);
	}
}

void CAudioPlayer::rewind()
{
	if(logger->IsDebugEnabled) logger->DebugFormat("rewind(): IsPlaying={0}", IsPlaying);

	try {
		// Seek to top of stream
		LONGLONG current = 0;
		HRESULT_CHECK(pSeeking->SetPositions(&current, AM_SEEKING_AbsolutePositioning, NULL, AM_SEEKING_NoPositioning));
	} catch(Exception^ ex) {
		if(logger->IsErrorEnabled) logger->ErrorFormat("rewind() Exception: {0}", ex->Message);
	}
}

void CAudioPlayer::handleMediaEvent()
{
	HANDLE hEvents[] = {hStop, hEndOfStream};

	// Wait for stop() method called or completed to play back
	DWORD wait = ::WaitForMultipleObjects(ARRAYSIZE(hEvents), hEvents, FALSE, INFINITE);
	switch(wait) {
	case WAIT_OBJECT_0:
		// stop() method called
		break;
	case WAIT_OBJECT_0 + 1:
		// End of stream
		try {
			// Completed to play back
			if(logger->IsDebugEnabled) logger->DebugFormat("handleMediaEvent(): End of stream");
			dispatcher->BeginInvoke(gcnew Action(this, &CAudioPlayer::onPlayingCompleted));
		} catch(Exception^ ex) {
			if(logger->IsFatalEnabled) logger->FatalFormat("handleMediaEvent() Exception: {0}", ex->Message);
		}
		break;
	default:
		if(logger->IsFatalEnabled) logger->FatalFormat("handleMediaEvent() Unexpected wait result: {0}, error={1}", wait, ::GetLastError());
		break;
	}
}
