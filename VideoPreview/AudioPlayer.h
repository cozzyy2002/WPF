#pragma once

#include "Device.h"

namespace DirectX {

	public ref class CAudioPlayer : public System::ComponentModel::INotifyPropertyChanged {
	public:
		static CAudioPlayer();
		CAudioPlayer(System::String^ mediaFile);
		CAudioPlayer(System::String^ mediaFile, CDevice^ speaker);
		~CAudioPlayer();
		!CAudioPlayer();

		void start();
		void stop();
		void rewind();

		property bool IsPlaying {
			bool get() { return isPlaying; }
		}
		virtual event System::ComponentModel::PropertyChangedEventHandler^ PropertyChanged;

	protected:
		void init();

		bool isPlaying;
		void setIsPlaying(bool value) {
			if(isPlaying != value) {
				isPlaying = value;
				PropertyChanged(this, gcnew System::ComponentModel::PropertyChangedEventArgs("IsPlaying"));
			}
		}

		void onPlayingCompleted() { setIsPlaying(false); }

		System::Windows::Threading::Dispatcher^ dispatcher;
		System::Threading::Tasks::Task^ handleEventTask;

		IGraphBuilder *pGraph;
		IMediaControl* pControl;
		IMediaSeeking* pSeeking;

		HANDLE hStop;
		HANDLE hEndOfStream;
		void handleMediaEvent();

		static log4net::ILog^ logger;
	};

}
