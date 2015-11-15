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

		CDevice();
	};

}