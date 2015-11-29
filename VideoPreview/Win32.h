#pragma once

#define HRESULT_CHECK(method) Win32::hResultCheck(method, #method)

namespace Win32 {

	HRESULT hResultCheck(HRESULT hr, System::String^ method);

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