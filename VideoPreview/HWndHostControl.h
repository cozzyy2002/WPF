#pragma once

namespace Win32 {

	public ref class CHWndHostControl :
		public System::Windows::Interop::HwndHost {
	public:
		static CHWndHostControl();
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

		static log4net::ILog^ logger;
	};
}