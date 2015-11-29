#include "stdafx.h"
#include "Win32.h"

using namespace System;

HRESULT Win32::hResultCheck(HRESULT hr, String^ method)
{
	if(FAILED(hr)) {
		String^ msg = String::Format("'{1}' failed. error=0x{0:x}", hr, method);
		throw gcnew Win32::ComOperationFailedException(msg, hr);
	}
	return hr;
}
