#include "stdafx.h"
#include "AudioPlayer.h"
#include "Win32.h"

using namespace DirectX;
using namespace System;

CAudioPlayer::CAudioPlayer()
	: pGraph(NULL), pControl(NULL)
{
}

CAudioPlayer::~CAudioPlayer()
{
	this->!CAudioPlayer();
}

CAudioPlayer::!CAudioPlayer()
{
	if(pControl) {
		pControl->Stop();
		pControl->Release();
	}

	if(pGraph) pGraph->Release();
}

void CAudioPlayer::setup(System::String^ mediaFile)
{
	try {
		CComPtr<IGraphBuilder> pGraph;
		HRESULT_CHECK(pGraph.CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER));

		pGraph->RenderFile(_T("C:\\Windows\\Media\\ringout.wav"), NULL);

		// Create IMediaControl object
		CComPtr<IMediaControl> pControl;
		HRESULT_CHECK(pGraph.QueryInterface<IMediaControl>(&pControl));

		// Move COM objects to this member variables
		this->pGraph = pGraph.Detach();
		this->pControl = pControl.Detach();
	} catch(Exception^ ex) {
		Console::WriteLine("Exception: ", ex->Message);
	}
}

void CAudioPlayer::start()
{
	HRESULT_CHECK(pControl->Run());
}

void CAudioPlayer::stop()
{
	HRESULT_CHECK(pControl->Stop());
}
