#include "stdafx.h"
#include "Device.h"

using namespace DirectX;

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

CDevice::CDevice()
{
}
