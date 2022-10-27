#include "Platform/PlanetWindows.h"
#include "PlanetLogging.h"

KeyCode Platform::GetKeyCode(unsigned char VirtualKey)
{
    if (VirtualKey >= 'A' && VirtualKey <= 'Z')
    {
        return (KeyCode)(static_cast<int>(KeyCode::A) + (VirtualKey - 'A'));
    }
    else if (VirtualKey >= '0' && VirtualKey <= '9')
    {
        return (KeyCode)(static_cast<int>(KeyCode::ZERO) + (VirtualKey - '0'));
    }

    switch (VirtualKey)
    {
    case VK_LBUTTON:
        return KeyCode::LEFT_MOUSE_BUTTON;
    case VK_RBUTTON:
        return KeyCode::RIGHT_MOUSE_BUTTON;
    case VK_MBUTTON:
        return KeyCode::MIDDLE_MOUSE_BUTTON;
    case VK_BACK:
        return KeyCode::BACKSPACE;
    case VK_TAB:
        return KeyCode::TAB;
    case VK_RETURN:
        return KeyCode::RETURN;
    case VK_LSHIFT:
    case VK_SHIFT:
        return KeyCode::LEFT_SHIFT;
    case VK_RSHIFT:
        return KeyCode::RIGHT_SHIFT;
    case VK_LCONTROL:
    case VK_CONTROL:
        return KeyCode::LEFT_CTRL;
    case VK_RCONTROL:
        return KeyCode::RIGHT_CTRL;
    case VK_MENU:
        return KeyCode::ALT;
    case VK_PAUSE:
        return KeyCode::PAUSE;
    case VK_CAPITAL:
        return KeyCode::CAPS_LOCK;
    case VK_ESCAPE:
        return KeyCode::ESC;
    case VK_SPACE:
        return KeyCode::SPACE;
    case VK_PRIOR:
        return KeyCode::PAGE_UP;
    case VK_NEXT:
        return KeyCode::PAGE_DOWN;
    case VK_END:
        return KeyCode::END;
    case VK_HOME:
        return KeyCode::HOME;
    case VK_LEFT:
        return KeyCode::LEFT_ARROW;
    case VK_UP:
        return KeyCode::UP_ARROW;
    case VK_RIGHT:
        return KeyCode::RIGHT_ARROW;
    case VK_DOWN:
        return KeyCode::DOWN_ARROW;
    case VK_INSERT:
        return KeyCode::INSERT;
    case VK_DELETE:
        return KeyCode::DEL;
    case VK_LWIN:
        return KeyCode::LEFT_SUPER;
    case VK_RWIN:
        return KeyCode::RIGHT_SUPER;
    }

    return KeyCode::INVALID;
}

void Platform::CreateDirectoryIfNotExists(const char* directory)
{
    CreateDirectory(directory, NULL);
    auto error = GetLastError();
    P_ASSERT(error == ERROR_ALREADY_EXISTS || error == 0, "Failed to create directory {} due to {}", directory, error);
}

namespace Wait
{
struct ActiveWait
{
    std::string directory_;
    std::function<void()> change_func_;
};
std::vector<ActiveWait*> active_waits_;

ActiveWait* CreateWait(const char* directory, const std::function<void()>& change_func)
{
    ActiveWait* wait = new ActiveWait{};
    wait->directory_ = directory;
    wait-> change_func_ = change_func;

    active_waits_.push_back(wait);
    return wait;
}

void RegisterChangeCallback(ActiveWait* wait);

void CALLBACK DirectoryChangeCallback(PVOID lpParameter, BOOLEAN TimerOrWaitFired)
{
    ActiveWait* wait = reinterpret_cast<ActiveWait*>(lpParameter);
    wait->change_func_();
    // after each trigger the change callback must be reregistered
    RegisterChangeCallback(wait);
}

void RegisterChangeCallback(ActiveWait* wait)
{
    auto flags = FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME | FILE_NOTIFY_CHANGE_LAST_WRITE;
    HANDLE h = FindFirstChangeNotificationA(wait->directory_.c_str(), true, flags);
    P_ASSERT(h != nullptr, "Failed to register change notification on {}", wait->directory_);
    HANDLE ph;
    bool result = RegisterWaitForSingleObject(&ph, h, &Wait::DirectoryChangeCallback, wait, INFINITE, WT_EXECUTEONLYONCE);
    P_ASSERT(result, "Failed to wait for change notification on {}", wait->directory_);
}

}  // namespace Wait

void Platform::AddDirectoryChangeListener(const char* directory, const std::function<void()>& change_func)
{
    Wait::ActiveWait* wait = Wait::CreateWait(directory, change_func);
    Wait::RegisterChangeCallback(wait);
}

std::vector<std::string> Platform::ListFiles(const char* directory)
{
    std::vector<std::string> files;

    char search[MAX_PATH];
    std::snprintf(search, MAX_PATH * sizeof(char), "%s\\*", directory);

    WIN32_FIND_DATA ffd;
    HANDLE find_result = FindFirstFile(search, &ffd);
    do
    {
        const bool is_dir = ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
        if (!is_dir)
        {
            char full_path[MAX_PATH];
            std::snprintf(full_path, MAX_PATH * sizeof(char), "%s\\%s", directory, ffd.cFileName);
            files.push_back(std::string{full_path});
        }
    }
    while (FindNextFile(find_result, &ffd) != 0);  // NOLINT

    FindClose(find_result);
    return files;
}

std::vector<std::string> Platform::ListDirectories(const char* directory)
{
    std::vector<std::string> files;

    char search[MAX_PATH];
    std::snprintf(search, MAX_PATH * sizeof(char), "%s\\*", directory);

    WIN32_FIND_DATA ffd;
    HANDLE find_result = FindFirstFile(search, &ffd);
    do
    {
        const bool is_dir = ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
        if (is_dir && ffd.cFileName[0] != '.')
        {
            char full_path[MAX_PATH];
            std::snprintf(full_path, MAX_PATH * sizeof(char), "%s\\%s", directory, ffd.cFileName);
            files.push_back(std::string{full_path});
        }
    }
    while (FindNextFile(find_result, &ffd) != 0);    // NOLINT

    FindClose(find_result);
    return files;
}