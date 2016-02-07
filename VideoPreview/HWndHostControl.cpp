#include "stdafx.h"
#include "HWndHostControl.h"

using namespace System;
using namespace System::Runtime::InteropServices;
using namespace System::Windows::Controls;
using namespace Win32;

static CHWndHostControl::CHWndHostControl()
{
	logger = log4net::LogManager::GetLogger(CHWndHostControl::typeid);
}

CHWndHostControl::CHWndHostControl()
{
}

void CHWndHostControl::attach(Decorator^ parent)
{
	this->parent = parent;
	this->parent->Child = this;
}

HandleRef CHWndHostControl::BuildWindowCore(HandleRef hwndParent)
{
	this->Width = this->parent->ActualWidth;
	this->Height = this->parent->ActualHeight;

	HWND hwnd = CreateWindowEx(
					0, _T("Static"), _T(""), WS_CHILD | WS_VISIBLE, 0, 0,
					(int)this->Width, (int)this->Height,
					(HWND)hwndParent.Handle.ToPointer(), NULL, NULL, NULL);
	if(!hwnd) {
		if(logger->IsErrorEnabled) logger->ErrorFormat("CreateWindowEx() failed. error=0x{0:x}", GetLastError());
	}
	return HandleRef(this, IntPtr(hwnd));
}

void CHWndHostControl::DestroyWindowCore(HandleRef hwnd)
{
	DestroyWindow((HWND)hwnd.Handle.ToPointer());
}

IntPtr CHWndHostControl::WndProc(IntPtr hwnd, int msg, IntPtr wParam, IntPtr lParam, bool% handled)
{
	handled = false;
	return IntPtr(NULL);
}
