#pragma once

namespace Win32 {

	public ref class CHWndHostControl :
		public System::Windows::Interop::HwndHost {
	public:
		CHWndHostControl();
		void attach(System::Windows::Controls::Decorator^ parent);

	protected:
		virtual System::Runtime::InteropServices::HandleRef BuildWindowCore(System::Runtime::InteropServices::HandleRef hwndParent) override;
		virtual void DestroyWindowCore(System::Runtime::InteropServices::HandleRef hwnd) override;
		virtual System::IntPtr WndProc(
										System::IntPtr hwnd,
										int msg,
										System::IntPtr wParam,
										System::IntPtr lParam,
										bool% handled
		) override;

		System::Windows::Controls::Decorator^ parent;
	};


	// Exception thrown when FAILED(hr)
	public ref class ComOperationFailedException : public System::Exception {
	internal:
		ComOperationFailedException(System::String^ message, HRESULT hr)
			: Exception(message), hr(hr) {}

	public:
		property long HResult {
			long get() { return hr; }
		}

	protected:
		HRESULT hr;
	};
}