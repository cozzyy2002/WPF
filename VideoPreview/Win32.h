#pragma once

// Exception is thrown if FAILED(method)
#define HRESULT_CHECK(method) Win32::hResultCheck(method, #method)
// Exception is thrown if FAILED(method) and result code is not included in except value(s)
// except value may be one HRESULT or HRESULT array end with S_OK.
#define HRESULT_CHECK_EX(method, except) Win32::hResultCheck(method, #method, except)

namespace Win32 {

	HRESULT hResultCheck(HRESULT hr, System::String^ method, HRESULT hrExcept);
	HRESULT hResultCheck(HRESULT hr, System::String^ method, const HRESULT* hrExcepts = NULL);

	// Exception thrown when FAILED(hr)
	public ref class ComOperationFailedException : public System::Exception {
	public:
		ComOperationFailedException(System::String^ message, HRESULT hr)
			: Exception(message, System::Runtime::InteropServices::Marshal::GetExceptionForHR(hr))
			, hr(hr) { }

	public:
		property long HResult {
			long get() { return hr; }
		}

	protected:
		HRESULT hr;
	};
}
