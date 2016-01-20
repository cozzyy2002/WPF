#pragma once

namespace DirectX {

	public ref class CAudioPlayer {
	public:
		CAudioPlayer();
		~CAudioPlayer();
		!CAudioPlayer();

		void setup(System::String^ mediaFile);
		void start();
		void stop();

	protected:
		IGraphBuilder *pGraph;
		IMediaControl* pControl;
		IMediaSeeking* pSeeking;

		HANDLE hShutdown;
		void handleMediaEvent(System::Object ^sender, System::ComponentModel::DoWorkEventArgs ^e);
	};

}
