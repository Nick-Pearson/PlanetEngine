#pragma once

#include <thread>

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

class GameCanvas : public wxPanel
{
 public:
    explicit GameCanvas(wxWindow *parent);
    virtual ~GameCanvas();

    bool AcceptsFocus() const final;

#if PLATFORM_WIN
    WXLRESULT MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam) override;

    LRESULT SendMSMessageToEngine(UINT msg, WPARAM wParam, LPARAM lParam);
#endif

 private:
    class D3DRenderSystem* renderer_;
    class PlanetEngine* engine_;
    std::thread eng_thread_;
    bool focused_ = false;

    void OnSetFocus(wxFocusEvent& event);
    void OnKillFocus(wxFocusEvent& event);
    wxDECLARE_EVENT_TABLE();
};
