#include "GameCanvas.h"

#include "PlanetEngine.h"
#include "D3DRenderSystem.h"

wxBEGIN_EVENT_TABLE(GameCanvas, wxPanel)
    EVT_SET_FOCUS(GameCanvas::OnSetFocus)
    EVT_KILL_FOCUS(GameCanvas::OnKillFocus)
wxEND_EVENT_TABLE()

GameCanvas::GameCanvas(wxWindow *parent)
    : wxPanel(parent, wxID_ANY, wxPoint(0, 0), wxSize(1280, 720))
{
    SetMaxClientSize(wxSize(1280, 720));

    renderer_ = new D3DRenderSystem{HWND(GetHWND())};
    engine_ = new PlanetEngine{renderer_};
    eng_thread_ = std::thread(&PlanetEngine::Run, engine_);
}

GameCanvas::~GameCanvas()
{
    engine_->Close();
    eng_thread_.join();
    delete engine_;
    delete renderer_;
}

#if PLATFORM_WIN
WXLRESULT GameCanvas::MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam)
{
    auto result = SendMSMessageToEngine(nMsg, wParam, lParam);

    if (result == 0)
    {
        return wxPanel::MSWWindowProc(nMsg, wParam, lParam);
    }
    else
    {
        return result;
    }
}

LRESULT GameCanvas::SendMSMessageToEngine(UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (!focused_)
    {
        return false;
    }

    if (msg == WM_GETDLGCODE)
    {
        LPMSG lpmsg = (LPMSG)lParam;
        if (lpmsg == nullptr)
        {
            // system is querying the DLG code state
            return DLGC_WANTALLKEYS;
        }

        return engine_->ProcessWindowMessage(lpmsg->hwnd, lpmsg->message, lpmsg->wParam, lpmsg->lParam);
    }

    return engine_->ProcessWindowMessage(GetHWND(), msg, wParam, lParam);
}
#endif

bool GameCanvas::AcceptsFocus() const
{
    return true;
}

void GameCanvas::OnSetFocus(wxFocusEvent& event)
{
    P_LOG("Game got focus");
    focused_ = true;
}

void GameCanvas::OnKillFocus(wxFocusEvent& event)
{
    P_LOG("Game lost focus");
    focused_ = false;
}