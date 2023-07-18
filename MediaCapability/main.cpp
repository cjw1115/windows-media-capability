#include "pch.h"

#include <Mferror.h>
#include <mfapi.h>
#include <mfmediaengine.h>
#include <wil/resource.h>
#include <wil/stl.h>
#include "MediaEngineCallback.h"
#include <wil/common.h>
#include <wil/win32_helpers.h>
#include <wil/wrl.h>
#include <iostream>
#include <vector>

// #include "WinrtCodeQuery.h"
#include "WrlCodecQuery.h"

#include <debugapi.h>

using namespace winrt;
using namespace winrt::Windows::Foundation;

int main() {
	//init_apartment();
	//while (!IsDebuggerPresent()) {
	//	Sleep(10);
	//}

	MFStartup(MF_VERSION);
	com_ptr<IMFMediaEngineClassFactory> classFactory;
	com_ptr<IMFAttributes> creationAttributes;
	com_ptr<IMFMediaEngine> m_mediaEngine;
	com_ptr<IMFMediaEngineNotify> m_callbackHelper;

	winrt::check_hresult(MFCreateAttributes(creationAttributes.put(), 10));

	m_callbackHelper = winrt::make<MediaEngineCallbackHelper>(
		[&]() {}, [&](MF_MEDIA_ENGINE_ERR error, HRESULT hr) {},
		[&](BufferingState state) {}, [&]() {}, [&]() {});

	winrt::check_hresult(creationAttributes->SetUnknown(MF_MEDIA_ENGINE_CALLBACK,
		m_callbackHelper.get()));

	winrt::check_hresult(CoCreateInstance(CLSID_MFMediaEngineClassFactory,
		nullptr, CLSCTX_INPROC_SERVER,
		IID_PPV_ARGS(classFactory.put())));

	winrt::check_hresult(classFactory->CreateInstance(0, creationAttributes.get(),
		m_mediaEngine.put()));

	const wchar_t DefaultFeatures[] =
		L"decode-bpp=8,decode-res-x=1920,decode-res-y=1080,decode-bitrate="
		"10000000,decode-fps=30";

	std::vector<std::wstring> codecs{
		L"video/mp4; codecs=\"ac-3\";",
			L"video/mp4; codecs=\"ec-3\";",
			L"video/mp4; codecs=\"ac-4\";",

			L"video/mp4; codecs=\"hvc1\";features=\"%s\"",
			L"video/mp4; codecs=\"hev1\";features=\"%s\"",

			L"video/mp4; codecs=\"hvc1\";features=\"ext-profile=dvhe.01,%s\"",
			L"video/mp4; codecs=\"hvc1\";features=\"ext-profile=dvhe.02,%s\"",
			L"video/mp4; codecs=\"hvc1\";features=\"ext-profile=dvhe.03,%s\"",
			L"video/mp4; codecs=\"hvc1\";features=\"ext-profile=dvhe.04,%s\"",
			L"video/mp4; codecs=\"hvc1\";features=\"ext-profile=dvhe.05,%s\"",
			L"video/mp4; codecs=\"hvc1\";features=\"ext-profile=dvhe.06,%s\"",
			L"video/mp4; codecs=\"hvc1\";features=\"ext-profile=dvhe.07,%s\"",
			L"video/mp4; codecs=\"hvc1\";features=\"ext-profile=dvhe.08,%s\"",
			L"video/mp4; codecs=\"avc1\";features=\"ext-profile=dvhe.09,%s\"",
			L"video/mp4; codecs=\"hvc1\";features=\"ext-profile=dvhe.10,%s\"",

			L"video/mp4; codecs=\"hev1\";features=\"ext-profile=dvhe.01,%s\"",
			L"video/mp4; codecs=\"hev1\";features=\"ext-profile=dvhe.02,%s\"",
			L"video/mp4; codecs=\"hev1\";features=\"ext-profile=dvhe.03,%s\"",
			L"video/mp4; codecs=\"hev1\";features=\"ext-profile=dvhe.04,%s\"",
			L"video/mp4; codecs=\"hev1\";features=\"ext-profile=dvhe.05,%s\"",
			L"video/mp4; codecs=\"hev1\";features=\"ext-profile=dvhe.06,%s\"",
			L"video/mp4; codecs=\"hev1\";features=\"ext-profile=dvhe.07,%s\"",
			L"video/mp4; codecs=\"hev1\";features=\"ext-profile=dvhe.08,%s\"",

			L"video/mp4; codecs=\"avc1\";features=\"ext-profile=dvhe.09,%s\"",
			L"video/mp4; codecs=\"hev1\";features=\"ext-profile=dvhe.10,%s\"",

			L"video/mp4; codecs=\"avc1.42E01E\";features=\"%s\"",
			L"video/mp4; codecs=\"avc1.640033\";features=\"%s\"",
	};

	{
		winrt::com_ptr<IMFExtendedDRMTypeSupport> drm_type_support_factory;
		HRESULT hr = CoCreateInstance(
			CLSID_MFMediaEngineClassFactory, nullptr, CLSCTX_INPROC_SERVER,
			__uuidof(IMFExtendedDRMTypeSupport), drm_type_support_factory.put_void());

		auto key_system =
			wil::make_bstr(L"com.microsoft.playready.recommendation");

		std::wcout << L"IMFExtendedDRMTypeSupport::IsTypeSupportedEx:" << std::endl;
		for (size_t i = 0; i < codecs.size(); i++) {
			auto type_str =
				wil::str_printf<std::wstring>(codecs[i].c_str(), DefaultFeatures);
			auto type = wil::make_bstr(type_str.c_str());

			MF_MEDIA_ENGINE_CANPLAY can_play =
				MF_MEDIA_ENGINE_CANPLAY::MF_MEDIA_ENGINE_CANPLAY_NOT_SUPPORTED;

			winrt::check_hresult(drm_type_support_factory->IsTypeSupportedEx(
				type.get(), key_system.get(), &can_play));
			switch (can_play) {
			case MF_MEDIA_ENGINE_CANPLAY_NOT_SUPPORTED:
				std::wcout << L"NOT_SUPPORTED  ";
				break;
			case MF_MEDIA_ENGINE_CANPLAY_MAYBE:
				std::wcout << L"MAYBE          ";
				break;
			case MF_MEDIA_ENGINE_CANPLAY_PROBABLY:
				std::wcout << L"PROBABLY       ";
				break;
			default:
				break;
			}
			std::wcout << L" : " << type_str;
			std::wcout << std::endl;
		}
	}
	{
		std::wcout << L"================================================" << std::endl;
		std::wcout << L"IMFMediaEngine::CanPlayType:" << std::endl;
		for (size_t i = 0; i < codecs.size(); i++) {
			auto type_str =
				wil::str_printf<std::wstring>(codecs[i].c_str(), DefaultFeatures);
			auto type = wil::make_bstr(type_str.c_str());

			MF_MEDIA_ENGINE_CANPLAY can_play =
				MF_MEDIA_ENGINE_CANPLAY::MF_MEDIA_ENGINE_CANPLAY_NOT_SUPPORTED;

			winrt::check_hresult(m_mediaEngine->CanPlayType(type.get(), &can_play));
			switch (can_play) {
			case MF_MEDIA_ENGINE_CANPLAY_NOT_SUPPORTED:
				std::wcout << L"NOT_SUPPORTED  ";
				break;
			case MF_MEDIA_ENGINE_CANPLAY_MAYBE:
				std::wcout << L"MAYBE          ";
				break;
			case MF_MEDIA_ENGINE_CANPLAY_PROBABLY:
				std::wcout << L"PROBABLY       ";
				break;
			default:
				break;
			}
			std::wcout << L" : " << type_str;
			std::wcout << std::endl;
		}
	}

	std::wcout << std::endl;

	std::vector<std::wstring> mimeTypes{L"ac-3", L"ec-3", L"ac-4", L"hev1"};

	////WinrtCodecQuery query;
	// WrlCodecQuery query;
	// for (size_t i = 0; i < mimeTypes.size(); i++) {
	//   std::wcout << mimeTypes[i] << L" :" << std::endl;
	//   auto result = query.IsCodecSupportedOnPlatform(mimeTypes[i]);
	// }
	return 0;
}