#pragma once

using namespace System;
using namespace System::Collections::Generic;

namespace DirectX {

	public ref class CDevice {
	public:
		ref class CCategory {
		public:
			CCategory() : m_clsid(NULL) {}
			CCategory(const CLSID* clsid, LPCTSTR name)
				: m_clsid(clsid), m_name(gcnew System::String(name)) {}

			property System::String^ Name {
				System::String^ get() { return m_name; }
			}
			const CLSID& getClsId() { return *m_clsid; }

			String^ ToString() override { return Name; }

		protected:
			const CLSID* m_clsid;
			System::String^ m_name;
		};

	public:
		static property CCategory^ VideoInputDeviceCategory { CCategory^ get(); }
		static property CCategory^ AudioInputDeviceCategory { CCategory^ get(); }
		static property CCategory^ AudioRendererCategory { CCategory^ get(); }
		static property ICollection<CCategory^>^ Categories { ICollection<CCategory^>^ get(); }

		static ICollection<CDevice^>^ getDevices(CCategory^ category);

		CDevice(IMoniker* pMoniker, CCategory^ category);
		~CDevice() { this->!CDevice(); }
		!CDevice() { if(this->m_pMoniker) this->m_pMoniker->Release(); }

		property CCategory^ Category { CCategory^ get() { return m_category; } }
		property String^ FriendlyName{ String^ get() { return m_friendlyName; } }
		property String^ Description{ String^ get() { return m_description; } }
		property String^ DevicePath{ String^ get() { return m_devicePath; } }
		bool Is(CCategory^ category) { return this->Category->getClsId() == category->getClsId(); }
		IMoniker* getMoniker() { return m_pMoniker; }
		IBaseFilter* getFilter();
		IPin* getPin(PIN_DIRECTION dir);
		static IPin* getPin(IBaseFilter* pFilter, PIN_DIRECTION dir);

		String^ ToString() override { return FriendlyName; }

	protected:
		IMoniker* m_pMoniker;
		CCategory^ m_category;
		String^ m_friendlyName;
		String^ m_description;
		String^ m_devicePath;

		String^ getStringProperty(IPropertyBag* pb, LPCWSTR name);
	};
}
