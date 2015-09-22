// VideoPreview.h

#pragma once

using namespace System;

namespace DirectX {

	public ref class CVideoPreview
		: public System::ComponentModel::INotifyPropertyChanged
	{
	public:
		CVideoPreview(void);
		~CVideoPreview();
		!CVideoPreview();

		void start(System::IntPtr hWnd, double width, double heigh);
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

		bool isStarted;

		void onPropertyChanged(System::String^ name, System::Object^ value)
		{
			Console::WriteLine("Property '{0}' is changed: {1}", name, value);
			PropertyChanged(this, gcnew System::ComponentModel::PropertyChangedEventArgs(name));
		}
	};
}
