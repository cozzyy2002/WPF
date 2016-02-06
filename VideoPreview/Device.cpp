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
	try {
		CComVariant var;
		HRESULT_CHECK(pb->Read(name, &var, NULL));
		return gcnew String(var.bstrVal);
	} catch(Exception^ ex) {
		Console::WriteLine("{0}: {1}", ex->GetType(), ex->Message);
		return String::Format("<Unknown {0}>", gcnew String(name));
	}
}

IBaseFilter* CDevice::getFilter()
{
	CComPtr<IBaseFilter> pDevice;
	HRESULT_CHECK(getMoniker()->BindToObject(0, 0, IID_IBaseFilter, (void**)&pDevice));
	return pDevice.Detach();
}

IPin* CDevice::getPin(PIN_DIRECTION dir)
{
	return getPin(getFilter(), dir);
}

/*static*/ IPin* CDevice::getPin(IBaseFilter* pFilter, PIN_DIRECTION dir)
{
	CComPtr<IEnumPins> pEnumPins;
	HRESULT_CHECK(pFilter->EnumPins(&pEnumPins));
	CComPtr<IPin> pPin;
	while(S_OK == pEnumPins->Next(1, &pPin, NULL)) {
		PIN_DIRECTION pinDir;
		HRESULT_CHECK(pPin->QueryDirection(&pinDir));
		if(pinDir == dir) {
			return pPin.Detach();
		}
	}
	throw gcnew System::Exception("getPin(): No such pin");
}
