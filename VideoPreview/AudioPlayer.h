#pragma once

namespace DirectX {

	public ref class CAudioPlayer : public System::ComponentModel::INotifyPropertyChanged {
	public:
		CAudioPlayer(System::String^ mediaFile);
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

		log4net::ILog^ logger;
	};

}
