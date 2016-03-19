// This is the main DLL file.

#include "stdafx.h"

#include "UnitTestCpp.h"

#include "VideoPreview\Win32.h"

using namespace NUnit::Framework;
using namespace System;
using namespace Win32;

namespace UnitTestCpp {

	ref class Result {
	public:
		Result(HRESULT hr) : hr(hr), hrExcept(S_OK), hrExcepts(NULL) {}
		Result(HRESULT hr, HRESULT hrExcept) : hr(hr), hrExcept(hrExcept), hrExcepts(NULL) {}
		Result(HRESULT hr, HRESULT* hrExcepts) : hr(hr), hrExcept(S_OK), hrExcepts(hrExcepts) {}
		void result() {
			if(FAILED(hrExcept)) HRESULT_CHECK_EX(hr, hrExcept);
			else if(hrExcepts) HRESULT_CHECK_EX(hr, hrExcepts);
			else HRESULT_CHECK(hr);
		}

		HRESULT hr;
		HRESULT hrExcept;
		HRESULT* hrExcepts;
	};

	[TestFixture]
	public ref class Win32Test
	{
	public:
		[TestCase(S_OK)]
		[TestCase(S_FALSE)]
		void noError(HRESULT hr) {
			Assert::That(HRESULT_CHECK(hr), Is::EqualTo(hr));
		}
		[Test]
		void noError_Except() {
			HRESULT hr = E_ACCESSDENIED;
			Assert::That(HRESULT_CHECK_EX(hr, hr), Is::EqualTo(hr));
		}
		[Test]
		void noError_ExceptAny() {
			HRESULT hr = E_ACCESSDENIED;
			HRESULT hrs[] = {E_ABORT, E_FAIL, hr, S_OK};
			Assert::That(HRESULT_CHECK_EX(hr, hrs), Is::EqualTo(hr));
		}

		ref struct ErrorData {
			ErrorData(HRESULT hr, Type^ type) : hr(hr), type(type) {}
			HRESULT hr;
			Type^ type;
		};

		static array<ErrorData^>^ errorDatas = {
			gcnew ErrorData(E_FAIL, System::Runtime::InteropServices::COMException::typeid),
			gcnew ErrorData(E_ACCESSDENIED, UnauthorizedAccessException::typeid),
			gcnew ErrorData(E_NOTIMPL, NotImplementedException::typeid),
		};

		[Test, TestCaseSource("errorDatas")]
		void error(ErrorData^ errorData)
		{
			Result^ result = gcnew Result(errorData->hr);

			ComOperationFailedException^ ex =
			Assert::Throws<ComOperationFailedException^>(gcnew TestDelegate(result, &Result::result));
			Assert::That(ex->HResult, Is::EqualTo(result->hr));
			Assert::That(ex->InnerException->GetType(), Is::EqualTo(errorData->type));
			Assert::That(ex->InnerException->HResult, Is::EqualTo(result->hr));
		}

		[Test, TestCaseSource("errorDatas")]
		void error_Except(ErrorData^ errorData)
		{
			Result^ result = gcnew Result(errorData->hr, E_NOT_SET);
			ComOperationFailedException^ ex =
			Assert::Throws<ComOperationFailedException^>(gcnew TestDelegate(result, &Result::result));
			Assert::That(ex->HResult, Is::EqualTo(result->hr));
			Assert::That(ex->InnerException->GetType(), Is::EqualTo(errorData->type));
			Assert::That(ex->InnerException->HResult, Is::EqualTo(result->hr));
		}
	};
}
