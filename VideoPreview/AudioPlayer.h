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
		System::String^ mediaFile;
		bool isPlaying;
		void setIsPlaying(bool value) {
			if(isPlaying != value) {
				isPlaying = value;
				PropertyChanged(this, gcnew System::ComponentModel::PropertyChangedEventArgs("IsPlaying"));
			}
		}

		void onPlayingCompleted() { setIsPlaying(false); }

		System::Windows::Threading::Dispatcher^ dispatcher;

		IGraphBuilder *pGraph;
		IMediaControl* pControl;
		IMediaSeeking* pSeeking;

		HANDLE hStop;
		void handleMediaEvent(System::Object ^sender, System::ComponentModel::DoWorkEventArgs ^e);
	};

}
