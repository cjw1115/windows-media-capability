#include "pch.h"

#include "WinrtCodeQuery.h"

bool WinrtCodecQuery::IsCodecSupportedOnPlatform(std::wstring mime) {
  CodecQuery query;
  auto codec = _codecSubTypeMap[mime];
  auto kind = _codecKindMap[mime];
  if (codec.empty()) {
    return false;
  }

  auto result =
      query.FindAllAsync(kind, CodecCategory::Decoder, codec.c_str()).get();
  if (result.Size() > 0) {
    std::wcout << L"There are " << result.Size() << L" codecs" << std::endl;
    for (auto iter = result.begin(); iter != result.end(); iter++) {
      std::wcout << L"    DisplayName:" << (*iter).DisplayName().c_str()
                 << std::endl;
    }
    return true;
  }
  return false;
}

#include <winrt/Windows.Media.Audio.h>
#include <winrt/Windows.Media.Devices.h>

using namespace winrt::Windows::Media::Devices;
using namespace winrt::Windows::Media::Audio;

bool IsDolbySpatialSound() {
  auto renderId =
      MediaDevice::GetDefaultAudioRenderId(AudioDeviceRole::Default);
  auto config = SpatialAudioDeviceConfiguration::GetForDeviceId(renderId);
  auto format = config.ActiveSpatialAudioFormat();
  if (format == SpatialAudioFormatSubtype::DolbyAtmosForHeadphones()) {
    return true;
  }
  return false;
}