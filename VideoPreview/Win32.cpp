#include "stdafx.h"
#include "Win32.h"

#pragma comment(lib, "Quartz.lib")

using namespace System;

HRESULT Win32::hResultCheck(HRESULT hr, String^ method, HRESULT hrExcept)
{
	HRESULT hrExcepts[] = {hrExcept, S_OK};
	return hResultCheck(hr, method, hrExcepts);
}

HRESULT Win32::hResultCheck(HRESULT hr, String^ method, const HRESULT* hrExcepts)
{
	if(hrExcepts) {
		while(FAILED(*hrExcepts)) {
			if(hr == *(hrExcepts++)) return hr;
		}
	}

	if(FAILED(hr)) {
		String^ hrMsg = nullptr;
		TCHAR buffer[MAX_ERROR_TEXT_LEN];
		DWORD ret = AMGetErrorText(hr, buffer, MAX_ERROR_TEXT_LEN);
		if(0 < ret) {
			hrMsg = gcnew String(buffer);
		}

		String^ msg = String::Format("'{0}' failed. {1}(0x{2:x})", method, (hrMsg != nullptr) ? hrMsg : "<unknown HRESULT>", hr);
			throw gcnew Win32::ComOperationFailedException(msg, hr);
	}
	return hr;
}
