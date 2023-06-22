#pragma once
#include <functional>


using ErrorCB = std::function<void(MF_MEDIA_ENGINE_ERR, HRESULT)>;
enum class BufferingState { HAVE_NOTHING = 0, HAVE_ENOUGH = 1 };
using BufferingStateChangeCB = std::function<void(BufferingState)>;

class MediaEngineCallbackHelper
    : public winrt::implements<MediaEngineCallbackHelper,
                               IMFMediaEngineNotify> {
 public:
  MediaEngineCallbackHelper(std::function<void()> onLoadedCB,
                            ErrorCB errorCB,
                            BufferingStateChangeCB bufferingStateChangeCB,
                            std::function<void()> playbackEndedCB,
                            std::function<void()> timeUpdateCB)
      : m_onLoadedCB(onLoadedCB),
        m_errorCB(errorCB),
        m_bufferingStateChangeCB(bufferingStateChangeCB),
        m_playbackEndedCB(playbackEndedCB),
        m_timeUpdateCB(timeUpdateCB) {
    // Ensure that callbacks are valid
    THROW_HR_IF(E_INVALIDARG, !m_onLoadedCB);
    THROW_HR_IF(E_INVALIDARG, !m_errorCB);
    THROW_HR_IF(E_INVALIDARG, !m_bufferingStateChangeCB);
    THROW_HR_IF(E_INVALIDARG, !m_playbackEndedCB);
    THROW_HR_IF(E_INVALIDARG, !m_timeUpdateCB);
  }
  virtual ~MediaEngineCallbackHelper() = default;

  void DetachParent() {
    auto lock = m_lock.lock();
    m_detached = true;
    m_onLoadedCB = nullptr;
    m_errorCB = nullptr;
    m_bufferingStateChangeCB = nullptr;
    m_playbackEndedCB = nullptr;
    m_timeUpdateCB = nullptr;
  }

  // IMFMediaEngineNotify
  IFACEMETHODIMP EventNotify(DWORD eventCode,
                             DWORD_PTR param1,
                             DWORD param2) noexcept override try {
    auto lock = m_lock.lock();
    THROW_HR_IF(MF_E_SHUTDOWN, m_detached);

    switch ((MF_MEDIA_ENGINE_EVENT)eventCode) {
      case MF_MEDIA_ENGINE_EVENT_LOADEDMETADATA:
        m_onLoadedCB();
        break;
      case MF_MEDIA_ENGINE_EVENT_ERROR:
        m_errorCB((MF_MEDIA_ENGINE_ERR)param1, (HRESULT)param2);
        break;
      case MF_MEDIA_ENGINE_EVENT_PLAYING:
        m_bufferingStateChangeCB(
            BufferingState::HAVE_ENOUGH);
        break;
      case MF_MEDIA_ENGINE_EVENT_WAITING:
        m_bufferingStateChangeCB(
            BufferingState::HAVE_NOTHING);
        break;
      case MF_MEDIA_ENGINE_EVENT_ENDED:
        m_playbackEndedCB();
        break;
      case MF_MEDIA_ENGINE_EVENT_TIMEUPDATE:
        m_timeUpdateCB();
        break;
      default:
        break;
    }

    return S_OK;
  }
  CATCH_RETURN();

 private:
  wil::critical_section m_lock;
  std::function<void()> m_onLoadedCB;
  ErrorCB m_errorCB;
  BufferingStateChangeCB m_bufferingStateChangeCB;
  std::function<void()> m_playbackEndedCB;
  std::function<void()> m_timeUpdateCB;
  bool m_detached = false;
};