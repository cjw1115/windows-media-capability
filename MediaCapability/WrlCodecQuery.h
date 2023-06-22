#pragma once
#include <Windows.Foundation.h>
#include <windows.foundation.collections.h>
#include <wrl.h>
#include <wrl\client.h>
#include <wrl\wrappers\corewrappers.h>

#include <windows.media.core.h>

#include <map>
#include <iostream>
#include <string>

using namespace Microsoft::WRL;
using namespace Microsoft::WRL::Wrappers;
using namespace ABI::Windows::Foundation;
using namespace ABI::Windows::Foundation::Collections;
using namespace ABI::Windows::Media::Core;

class WrlCodecQuery {
 public:
  WrlCodecQuery(){_init();};
  bool IsCodecSupportedOnPlatform(std::wstring mime);
  bool IsDolbySpatialSoundEnabled();

 private:
  void _init();

 private:
  std::map<std::wstring, std::wstring> _codecSubTypeMap;
  std::map<std::wstring, CodecKind> _codecKindMap{
      {L"ac-3", CodecKind::CodecKind_Audio},
      {L"ec-3", CodecKind::CodecKind_Audio},
      {L"ac-4", CodecKind::CodecKind_Audio},
      {L"hevc", CodecKind::CodecKind_Video},
      {L"hvc1", CodecKind::CodecKind_Video},
  };
};
