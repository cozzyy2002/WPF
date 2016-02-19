#include "stdafx.h"
#include "Device.h"
#include "Win32.h"

using namespace DirectX;
using namespace System;
using namespace System::Collections::Generic;

/*static property*/ CDevice::CCategory^ CDevice::VideoInputDeviceCategory::get()
{
	return gcnew CCategory(&CLSID_VideoInputDeviceCategory, _T("Video Capture Sources"), PINDIR_OUTPUT);
}

/*static property*/ CDevice::CCategory^ CDevice::AudioInputDeviceCategory::get()
{
	return gcnew CCategory(&CLSID_AudioInputDeviceCategory, _T("Audio Capture Sources"), PINDIR_OUTPUT);
}

/*static property*/ CDevice::CCategory^ CDevice::AudioRendererCategory::get()
{
	return gcnew CCategory(&CLSID_AudioRendererCategory, _T("Audio Renderers"), PINDIR_INPUT);
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
	HRESULT_CHECK(pDevEnum.CoCreateInstance(CLSID_SystemDeviceEnum));
	CComPtr<IEnumMoniker> pEnum;
	if(S_OK == HRESULT_CHECK(pDevEnum->CreateClassEnumerator(category->getClsId(), &pEnum, 0))) {
		CComPtr<IMoniker> pMoniker;
		while(SUCCEEDED(HRESULT_CHECK(pEnum->Next(1, &pMoniker, NULL))) && pMoniker)
		{
			list->Add(gcnew CDevice(pMoniker.Detach(), category));
		}
	} else {
		if(logger->IsErrorEnabled) logger->ErrorFormat("Device is not found: {0}", category->Name);
	}
	return list;
}

static CDevice::CDevice()
{
	logger = log4net::LogManager::GetLogger(CDevice::typeid);
}

CDevice::CDevice(IMoniker* pMoniker, CCategory^ category)
	: m_pMoniker(pMoniker), m_pBaseFilter(NULL), m_category(category)
{
	CComPtr<IPropertyBag> pb;
	HRESULT_CHECK(pMoniker->BindToStorage(NULL, NULL, IID_PPV_ARGS(&pb)));
	
	m_friendlyName = getStringProperty(pb, L"FriendlyName");
	m_description = getStringProperty(pb, L"Description");
	m_devicePath = getStringProperty(pb, L"DevicePath");
}

CDevice::~CDevice()
{
	this->!CDevice();
}

CDevice::!CDevice()
{
	if(this->m_pMoniker) this->m_pMoniker->Release();
	if(this->m_pBaseFilter) this->m_pBaseFilter->Release();
}

String^ CDevice::getStringProperty(IPropertyBag* pb, LPCWSTR name)
{
	CComVariant var;
	HRESULT_CHECK(var.ChangeType(VT_BSTR));
	return SUCCEEDED(pb->Read(name, &var, NULL)) ?
		gcnew String(var.bstrVal) :
		String::Format("<Unknown {0}>", gcnew String(name));
}

/**
 * Returns IBaseFilter object bound to IMoniker
 */
IBaseFilter* CDevice::getFilter()
{
	if(!m_pBaseFilter) {
		CComPtr<IBaseFilter> pFilter;
		HRESULT_CHECK(getMoniker()->BindToObject(0, 0, IID_IBaseFilter, (void**)&pFilter));
		m_pBaseFilter = pFilter.Detach();
	}
	return m_pBaseFilter;
}

void CDevice::releaseFilter()
{
	if(m_pBaseFilter) {
		m_pBaseFilter->Release();
		m_pBaseFilter = NULL;
	}
}

IPin* CDevice::getPin()
{
	return getPin(getFilter(), Category->getPinDirection());
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
		pPin.Release();
	}
	throw gcnew System::Exception("getPin(): No such pin");
}
