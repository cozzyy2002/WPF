// UnitTestCpp.h

#pragma once

#include "VideoPreview\Win32.h"

using namespace NUnit::Framework;
using namespace System;
using namespace Win32;

namespace UnitTestCpp {

	ref class Result {
	public:
		Result(HRESULT hr) : hr(hr) {}
		void result() { HRESULT_CHECK(hr); }

		HRESULT hr;
	};

	[TestFixture]
	public ref class Win32Test
	{
	public:
		[Test]
		void noError() {
			HRESULT hr = S_OK;
			Assert::That(HRESULT_CHECK(hr), Is::EqualTo(hr));
		}
		[Test]
		void noError_Except() {
			HRESULT hr = E_ACCESSDENIED;
			Assert::That(HRESULT_CHECK_EX(hr, hr), Is::EqualTo(hr));
		}
		[Test]
		void noError_ExceptAny() {
			HRESULT hrs[] = {E_ABORT, E_ACCESSDENIED, S_OK};
			Assert::That(HRESULT_CHECK_EX(E_ACCESSDENIED, hrs), Is::EqualTo(E_ACCESSDENIED));
		}
		[Test]
		void error()
		{
			Result^ result = gcnew Result(E_ABORT);

			ComOperationFailedException^ ex =
			Assert::Throws<ComOperationFailedException^>(gcnew TestDelegate(result, &Result::result));
			Assert::That(ex->HResult, Is::EqualTo(result->hr));
			Assert::That(ex->InnerException->HResult, Is::EqualTo(result->hr));
		}
	};
}
