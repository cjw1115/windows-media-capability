#pragma once

#include <winrt/base.h>
#include <winrt/Windows.Media.Core.h>

using namespace winrt::Windows::Media::Core;

class WinrtCodecQuery {
 public:
  bool IsCodecSupportedOnPlatform(std::wstring mime);

 private:
  std::map<std::wstring, std::wstring> _codecSubTypeMap{
      {L"ac-3", CodecSubtypes::AudioFormatDolbyAC3().c_str()},
      {L"ec-3", CodecSubtypes::AudioFormatDolbyDDPlus().c_str()},
      {L"ac-4", L"{0000AC40-0000-0010-8000-00aa00389b71}"},
      {L"hevc", CodecSubtypes::VideoFormatHevc().c_str()},
      {L"hvc1", CodecSubtypes::VideoFormatHevc().c_str()},
  };
  std::map<std::wstring, CodecKind> _codecKindMap{
      {L"ac-3", CodecKind::Audio},
      {L"ec-3", CodecKind::Audio},
      {L"ac-4", CodecKind::Audio},
      {L"hevc", CodecKind::Video},
      {L"hvc1", CodecKind::Video},
  };
};
