#include "stdafx.h"
#include "Device.h"
#include "Win32.h"

using namespace DirectX;
using namespace System;
using namespace System::Collections::Generic;

/*static property*/ CDevice::CCategory^ CDevice::VideoInputDeviceCategory::get()
{
	return gcnew CCategory(&CLSID_VideoInputDeviceCategory, _T("Video Capture Sources"));
}

/*static property*/ CDevice::CCategory^ CDevice::AudioInputDeviceCategory::get()
{
	return gcnew CCategory(&CLSID_AudioInputDeviceCategory, _T("Audio Capture Sources"));
}

/*static property*/ CDevice::CCategory^ CDevice::AudioRendererCategory::get()
{
	return gcnew CCategory(&CLSID_AudioRendererCategory, _T("Audio Renderers"));
}

/*static property*/ ICollection<CDevice::CCategory^>^ CDevice::Categories::get()
{
	List<CCategory^>^ list = gcnew List<CCategory^>(5);
	list->Add(VideoInputDeviceCategory);
	list->Add(AudioInputDeviceCategory);
	list->Add(AudioRendererCategory);
	return list;
}

/*static*/ ICollection<CDevice^>^ CDevice::getDevices(CCategory^ category)
{
	List<CDevice^>^ list = gcnew List<CDevice^>();

	CComPtr<ICreateDevEnum> pDevEnum;
	HRESULT_CHECK(pDevEnum.CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER));
	CComPtr<IEnumMoniker> pEnum;
	if(S_OK != HRESULT_CHECK(pDevEnum->CreateClassEnumerator(category->getClsId(), &pEnum, 0))) {
		Console::WriteLine("Device is not found: {0}", category->Name);
		return list;
	}
	CComPtr<IMoniker> pMoniker;
	while(SUCCEEDED(HRESULT_CHECK(pEnum->Next(1, &pMoniker, NULL))) && pMoniker)
	{
		list->Add(gcnew CDevice(pMoniker.Detach(), category));
	}
	return list;
}

CDevice::CDevice(IMoniker* pMoniker, CCategory^ category)
	: m_pMoniker(pMoniker), m_category(category)
{
	CComPtr<IPropertyBag> pb;
	HRESULT_CHECK(pMoniker->BindToStorage(NULL, NULL, IID_PPV_ARGS(&pb)));
	
	m_friendlyName = getStringProperty(pb, L"FriendlyName");
	m_description = getStringProperty(pb, L"Description");
	m_devicePath = getStringProperty(pb, L"DevicePath");
}

String^ CDevice::getStringProperty(IPropertyBag* pb, LPCWSTR name)
{
	CComVariant var;
	HRESULT hr = pb->Read(name, &var, NULL);
	if(SUCCEEDED(hr))
	{
		return gcnew String(var.bstrVal);
	} else {
		Console::WriteLine("IPropertyBag::Read('{0}') of '{1}' failed. error=0x{2,8:x}",
							gcnew String(name), m_category->Name, hr);
		return nullptr;
	}
}
