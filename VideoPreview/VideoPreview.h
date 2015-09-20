// VideoPreview.h

#pragma once

using namespace System;

namespace DirectX {

	public ref class CVideoPreview
	{
	public:
		CVideoPreview(void);
		~CVideoPreview();
		!CVideoPreview();

		void start(System::IntPtr hWnd, double width, double heigh);
		void stop();

	public:
		IGraphBuilder *pGraph;
		IVideoWindow* pVideoWindow;
		IMediaControl* pControl;
	};
}
