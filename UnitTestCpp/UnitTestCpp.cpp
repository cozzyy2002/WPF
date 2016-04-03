// This is the main DLL file.

#include "stdafx.h"

#include "UnitTestCpp.h"

#include "VideoPreview\Win32.h"

using namespace NUnit::Framework;
using namespace System;
using namespace Win32;

namespace UnitTestCpp {

	public ref class Checker {
		enum class _Type { CHECK, CHECK_EX, CHECK_EX_ANY };
		_Type type;

	public:
		Checker() : type(_Type::CHECK) {}
		Checker(HRESULT hrExcept) : type(_Type::CHECK_EX), hrExcept(hrExcept) {}
		Checker(const HRESULT* hrExcepts) : type(_Type::CHECK_EX_ANY), hrExcepts(hrExcepts) {}
		void check() {
			switch(type) {
			case _Type::CHECK:
				HRESULT_CHECK(hr);
				break;
			case _Type::CHECK_EX:
				HRESULT_CHECK_EX(hr, hrExcept);
				break;
			case _Type::CHECK_EX_ANY:
				HRESULT_CHECK_EX(hr, hrExcepts);
				break;
			}
		}

		HRESULT hr;
		HRESULT hrExcept;
		const HRESULT* hrExcepts;
	};

	[TestFixture]
	public ref class Win32Test : AssertionHelper
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
			Expect(ex->HResult, Is::EqualTo(checker->hr));
			Assert::That(ex->InnerException, Is::Not->Null);
			Expect(ex->InnerException->GetType(), Is::EqualTo(exceptionType));
			Expect(ex->InnerException->HResult, Is::EqualTo(checker->hr));
		}

        [TestCase(E_ACCESSDENIED)]
        [TestCase(E_NOINTERFACE)]
        void ExceptionTest(HRESULT hr)
        {
            try {
				throw gcnew Win32::ComOperationFailedException("test", hr);
			} catch (Exception^ ex) {
                Assert::That(ex->Message, Is::EqualTo("test"));

                Exception^ inner = ex->InnerException;
                Assert::That(inner, Is::Not->Null);
                Expect(inner->HResult, Is::EqualTo(hr));
                Expect(inner->Message, Is::Not->EqualTo(""), inner->Message);
				Console::WriteLine("InnerException({0}) = {1}:{2}", hr, inner->GetType(), inner->Message);
            }
        }
	};
}
