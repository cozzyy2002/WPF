#pragma once

using namespace System;
using namespace System::Collections::Generic;

namespace DirectX {

	public ref class CDevice {
	public:
		ref class CCategory {
		public:
			CCategory()
				: m_clsid(NULL), m_name(nullptr), m_pinDirection((PIN_DIRECTION)-1) {}
			CCategory(const CLSID* clsid, LPCTSTR name, PIN_DIRECTION pinDirection)
				: m_clsid(clsid), m_name(gcnew System::String(name)), m_pinDirection(pinDirection) {}

			property System::String^ Name {
				System::String^ get() { return m_name; }
			}
			const CLSID& getClsId() { return *m_clsid; }
			const PIN_DIRECTION getPinDirection() { return m_pinDirection; }

			String^ ToString() override { return Name; }

		protected:
			const CLSID* m_clsid;
			const PIN_DIRECTION m_pinDirection;
			System::String^ m_name;
		};

	public:
		static property CCategory^ VideoInputDeviceCategory { CCategory^ get(); }
		static property CCategory^ AudioInputDeviceCategory { CCategory^ get(); }
		static property CCategory^ AudioRendererCategory { CCategory^ get(); }
		static property ICollection<CCategory^>^ Categories { ICollection<CCategory^>^ get(); }

		static ICollection<CDevice^>^ getDevices(CCategory^ category);

		static CDevice();
		CDevice(IMoniker* pMoniker, CCategory^ category);
		~CDevice();
		!CDevice();

		property CCategory^ Category { CCategory^ get() { return m_category; } }
		property String^ FriendlyName{ String^ get() { return m_friendlyName; } }
		property String^ Description{ String^ get() { return m_description; } }
		property String^ DevicePath{ String^ get() { return m_devicePath; } }
		bool Is(CCategory^ category) { return this->Category->getClsId() == category->getClsId(); }
		IMoniker* getMoniker() { return m_pMoniker; }
		IBaseFilter* getFilter();
		void releaseFilter();
		IPin* getPin();
		static IPin* getPin(IBaseFilter* pFilter, PIN_DIRECTION dir);

		String^ ToString() override { return FriendlyName; }

	protected:
		IMoniker* m_pMoniker;
		IBaseFilter* m_pBaseFilter;
		CCategory^ m_category;
		String^ m_friendlyName;
		String^ m_description;
		String^ m_devicePath;

		String^ getStringProperty(IPropertyBag* pb, LPCWSTR name);

		static log4net::ILog^ logger;
	};
}
