#include "WrlCodecQuery.h"

#include <assert.h>

bool WrlCodecQuery::IsCodecSupportedOnPlatform(std::wstring mime) {
  ComPtr<ICodecQuery> query;

  HRESULT hr = ActivateInstance(
      HStringReference(RuntimeClass_Windows_Media_Core_CodecQuery).Get(),
      &query);
  assert(SUCCEEDED(hr));

  auto codec = _codecSubTypeMap[mime];
  auto kind = _codecKindMap[mime];
  if (codec.empty()) {
    return false;
  }

  HString codecHstr;
  codecHstr.Set(codec.c_str());

  ComPtr<IAsyncOperation<IVectorView<ABI::Windows::Media::Core::CodecInfo*>*>>
      asyncResult;
  hr = query->FindAllAsync(kind, CodecCategory::CodecCategory_Decoder,
                           codecHstr, asyncResult.GetAddressOf());
  assert(SUCCEEDED(hr));

  Event asyncCompleted(CreateEventEx(nullptr, nullptr,
                                     CREATE_EVENT_MANUAL_RESET,
                                     WRITE_OWNER | EVENT_ALL_ACCESS));
  hr = asyncCompleted.IsValid() ? S_OK : HRESULT_FROM_WIN32(GetLastError());
  assert(SUCCEEDED(hr));

  bool supported = false;
  hr = asyncResult->put_Completed(
      Callback<IAsyncOperationCompletedHandler<
          IVectorView<ABI::Windows::Media::Core::CodecInfo*>*>>(
          [this, &asyncCompleted, &supported](
              IAsyncOperation<
                  IVectorView<ABI::Windows::Media::Core::CodecInfo*>*>* op,
              AsyncStatus status) {
            if (AsyncStatus::Completed == status) {
              ComPtr<IVectorView<CodecInfo*>> result;
              HRESULT hr = op->GetResults(result.GetAddressOf());
              assert(SUCCEEDED(hr));

              UINT size = 0;
              hr = result->get_Size(&size);
              assert(SUCCEEDED(hr));

              if (size > 0) {
                std::wcout << L"There are " << size << L" codecs" << std::endl;
                for (auto index = 0; index < size; index++) {
                  ComPtr<ICodecInfo> codecInfo;
                  hr = result->GetAt(index, codecInfo.GetAddressOf());
                  assert(SUCCEEDED(hr));

                  HString name;
                  hr = codecInfo->get_DisplayName(name.GetAddressOf());
                  assert(SUCCEEDED(hr));

                  UINT nameLength = 0;
                  std::wcout << L"    DisplayName:"
                             << name.GetRawBuffer(&nameLength) << std::endl;
                }
                SetEvent(asyncCompleted.Get());
              }
            }
            return S_OK;
          })
          .Get());
  assert(SUCCEEDED(hr));

  WaitForSingleObjectEx(asyncCompleted.Get(), INFINITE, FALSE);

  return false;
}

void WrlCodecQuery::_init() {
  RoInitializeWrapper initialize(RO_INIT_MULTITHREADED);
  // Get the activation factory for the IUriRuntimeClass interface.
  ComPtr<ICodecSubtypesStatics> codecSubtypeStatics;

  HRESULT hr = GetActivationFactory(
      HStringReference(RuntimeClass_Windows_Media_Core_CodecSubtypes).Get(),
      &codecSubtypeStatics);
  assert(SUCCEEDED(hr));

  HString str;
  UINT strLength;

  hr = codecSubtypeStatics->get_AudioFormatDolbyAC3(str.GetAddressOf());
  assert(SUCCEEDED(hr));
  _codecSubTypeMap[L"ac-3"] = str.GetRawBuffer(&strLength);

  hr = codecSubtypeStatics->get_AudioFormatDolbyDDPlus(str.GetAddressOf());
  assert(SUCCEEDED(hr));
  _codecSubTypeMap[L"ec-3"] = str.GetRawBuffer(&strLength);

  hr = str.Set(L"{0000AC40-0000-0010-8000-00aa00389b71}");
  assert(SUCCEEDED(hr));
  _codecSubTypeMap[L"ac-4"] = str.GetRawBuffer(&strLength);

  hr = codecSubtypeStatics->get_VideoFormatHevc(str.GetAddressOf());
  assert(SUCCEEDED(hr));
  _codecSubTypeMap[L"hevc"] = str.GetRawBuffer(&strLength);

  hr = codecSubtypeStatics->get_VideoFormatHevc(str.GetAddressOf());
  assert(SUCCEEDED(hr));
  _codecSubTypeMap[L"hvc1"] = str.GetRawBuffer(&strLength);
}

#include <windows.media.audio.h>
#include <windows.media.devices.h>

using namespace ABI::Windows::Media::Devices;
using namespace ABI::Windows::Media::Audio;

bool WrlCodecQuery::IsDolbySpatialSoundEnabled() {
  ComPtr<IMediaDeviceStatics> media_device;
  ComPtr<ISpatialAudioDeviceConfigurationStatics>
      spatial_audio_device_config_factory;
  ComPtr<ISpatialAudioDeviceConfiguration> spatial_audio_device_config;
  ComPtr<ISpatialAudioFormatSubtypeStatics>
      spatial_audio_format_subtype_factory;

  HRESULT hr = GetActivationFactory(
      HStringReference(RuntimeClass_Windows_Media_Devices_MediaDevice).Get(),
      &media_device);
  assert(SUCCEEDED(hr));

  HString str;
  UINT strLength;

  hr = media_device->GetDefaultAudioCaptureId(
      AudioDeviceRole::AudioDeviceRole_Default, str.GetAddressOf());
  assert(SUCCEEDED(hr));

  hr = GetActivationFactory(
      HStringReference(
          RuntimeClass_Windows_Media_Audio_SpatialAudioDeviceConfiguration)
          .Get(),
      &spatial_audio_device_config_factory);
  assert(SUCCEEDED(hr));

  hr = spatial_audio_device_config_factory->GetForDeviceId(
      str, spatial_audio_device_config.GetAddressOf());
  assert(SUCCEEDED(hr));

  hr = spatial_audio_device_config->get_ActiveSpatialAudioFormat(
      str.GetAddressOf());
  assert(SUCCEEDED(hr));

  /*hr = GetActivationFactory(
      HStringReference(
          RuntimeClass_Windows_Media_Audio_SpatialAudioFormatSubtype)
          .Get(),
      &spatial_audio_format_subtype_factory);
  assert(SUCCEEDED(hr));

  HString ;
  spatial_audio_format_subtype_factory->get_DolbyAtmosForHeadphones()*/

  return false;
}