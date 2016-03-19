// This is the main DLL file.

#include "stdafx.h"

#include "UnitTestCpp.h"

#include "VideoPreview\Win32.h"

using namespace NUnit::Framework;
using namespace System;
using namespace Win32;

namespace UnitTestCpp {

	public ref class Checker {
	public:
		Checker(HRESULT hrExcept) : hrExcept(hrExcept), hrExcepts(NULL) {}
		Checker(const HRESULT* hrExcepts) : hrExcept(S_OK), hrExcepts(hrExcepts) {}
		Checker() : hrExcept(S_OK), hrExcepts(NULL) {}
		void check() {
			if(FAILED(hrExcept)) HRESULT_CHECK_EX(hr, hrExcept);
			else if(hrExcepts) HRESULT_CHECK_EX(hr, hrExcepts);
			else HRESULT_CHECK(hr);
		}

		HRESULT hr;
		HRESULT hrExcept;
		const HRESULT* hrExcepts;
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

		static array<Checker^>^ checkers;

		static Win32Test() {
			static const HRESULT hrExcepts[] = {E_INVALIDARG, E_FAIL, S_OK};
			checkers = gcnew array<Checker^> {
				gcnew Checker(),
				gcnew Checker(E_INVALIDARG),
				gcnew Checker(hrExcepts),
			};
		}

		[Test, TestCaseSource("checkers")]
		void error(Checker^ checker)
		{
			checker->hr = E_NOTIMPL;
			Type^ exceptionType = NotImplementedException::typeid;

			ComOperationFailedException^ ex =
			Assert::Throws<ComOperationFailedException^>(gcnew TestDelegate(checker, &Checker::check));
			Assert::That(ex->HResult, Is::EqualTo(checker->hr));
			Assert::That(ex->InnerException->GetType(), Is::EqualTo(exceptionType));
			Assert::That(ex->InnerException->HResult, Is::EqualTo(checker->hr));
		}
	};
}
