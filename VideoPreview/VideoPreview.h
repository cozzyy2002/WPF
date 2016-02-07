// VideoPreview.h

#pragma once

#include "HWndHostControl.h"
#include "Device.h"

using namespace System;

namespace DirectX {

	public ref class CVideoPreview
		: public System::ComponentModel::INotifyPropertyChanged
	{
	public:
		static CVideoPreview(void);
		CVideoPreview(void);
		~CVideoPreview();
		!CVideoPreview();

		void setup(CDevice^ camera, System::Windows::Controls::Decorator^ parent);
		void setSize(System::Windows::Size size);
		void start();
		void stop();

		property bool IsStarted {
			bool get() { return isStarted; }
			void set(bool value)
			{
				if(value != isStarted) {
					isStarted = value;
					onPropertyChanged("IsStarted", isStarted);
				}
			}
		}
		virtual event System::ComponentModel::PropertyChangedEventHandler^ PropertyChanged;

	protected:
		IGraphBuilder *pGraph;
		IVideoWindow* pVideoWindow;
		IMediaControl* pControl;
		Win32::CHWndHostControl^ hWndHost;
		System::Windows::Threading::DispatcherOperation^ setupResult;

		bool isStarted;

		void setup(CDevice^ camera, IntPtr hwnd, double width, double height);
		void onPropertyChanged(System::String^ name, System::Object^ value)
		{
			if(logger->IsDebugEnabled) logger->DebugFormat("Property '{0}' is changed: {1}", name, value);
			PropertyChanged(this, gcnew System::ComponentModel::PropertyChangedEventArgs(name));
		}
		bool setupResultCheck();

		static log4net::ILog^ logger;
	};
}
