mod renderer;

use renderer::Renderer;
use windows::{
    core::*,
    Win32::Foundation::*,
    Win32::System::LibraryLoader::*,
    Win32::UI::WindowsAndMessaging::*,
};

use std::mem::transmute;

fn run_engine() -> Result<()> {
    let instance = unsafe { GetModuleHandleA(None)? };

    let wc = WNDCLASSEXA {
        cbSize: std::mem::size_of::<WNDCLASSEXA>() as u32,
        style: CS_OWNDC,
        lpfnWndProc: Some(wndproc),
        hInstance: instance,
        lpszClassName: s!("EngWindowClass"),
        ..Default::default()
    };

    let atom = unsafe { RegisterClassExA(&wc) };
    debug_assert_ne!(atom, 0);

    let hwnd = unsafe {
        CreateWindowExA(
            WINDOW_EX_STYLE::default(),
            s!("EngWindowClass"),
            s!("Engine"),
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            1280,
            720,
            None, // no parent window
            None, // no menus
            instance,
            None,
        )
    };
    unsafe { ShowWindow(hwnd, SW_SHOW) };

    loop {
        let mut message = MSG::default();

        if unsafe { PeekMessageA(&mut message, None, 0, 0, PM_REMOVE) }.into() {
            unsafe {
                TranslateMessage(&message);
                DispatchMessageA(&message);
            }

            if message.message == WM_QUIT {
                break;
            }
        }
    }

    Ok(())

}

extern "system" fn wndproc(
    window: HWND,
    message: u32,
    wparam: WPARAM,
    lparam: LPARAM,
) -> LRESULT {
    match message {
        WM_CREATE => {
            unsafe {
                let create_struct: &CREATESTRUCTA = transmute(lparam);
                SetWindowLongPtrA(window, GWLP_USERDATA, create_struct.lpCreateParams as _);
            }
            LRESULT::default()
        }
        WM_DESTROY => {
            unsafe { PostQuitMessage(0) };
            LRESULT::default()
        }
        _ => {
            unsafe { DefWindowProcA(window, message, wparam, lparam) }
        }
    }
}

fn main() {
    let r = renderer::default_renderer();
    r.render_frame();
    
    run_engine().unwrap();
}