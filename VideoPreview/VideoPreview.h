// VideoPreview.h

#pragma once

#include "HWndHostControl.h"

using namespace System;

namespace DirectX {

	public ref class CVideoPreview
		: public System::ComponentModel::INotifyPropertyChanged
	{
	public:
		CVideoPreview(void);
		~CVideoPreview();
		!CVideoPreview();

		void setup(System::Windows::Controls::Decorator^ parent);
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

		bool isStarted;

		void setup(IntPtr hwnd, double width, double height);
		void onPropertyChanged(System::String^ name, System::Object^ value)
		{
			Console::WriteLine("Property '{0}' is changed: {1}", name, value);
			PropertyChanged(this, gcnew System::ComponentModel::PropertyChangedEventArgs(name));
		}
	};
}
