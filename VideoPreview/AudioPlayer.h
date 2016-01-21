#pragma once

namespace DirectX {

	public ref class CAudioPlayer {
	public:
		CAudioPlayer(System::String^ mediaFile);
		~CAudioPlayer();
		!CAudioPlayer();

		void start(bool repeat, unsigned int interval);
		void start(bool repeat) { start(repeat, 0); }
		void start() { start(false); }
		void stop();

	protected:
		System::String^ mediaFile;
		bool repeat;
		unsigned int interval;

		IGraphBuilder *pGraph;
		IMediaControl* pControl;
		IMediaSeeking* pSeeking;

		HANDLE hStop;
		void handleMediaEvent(System::Object ^sender, System::ComponentModel::DoWorkEventArgs ^e);
	};

}
