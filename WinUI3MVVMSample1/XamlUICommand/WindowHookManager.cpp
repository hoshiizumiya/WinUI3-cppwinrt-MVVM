#include "pch.h"
#include "WindowHookManager.h"
#include <winternl.h>

// 静态成员初始化
WindowHookManager::WindowEventCallback WindowHookManager::s_windowCallback;
WindowHookManager::ErrorCallback WindowHookManager::s_errorCallback;
std::mutex WindowHookManager::s_dataMutex;
std::unordered_map<HWND, WindowHookManager::WindowInfo> WindowHookManager::s_trackedWindows;
std::unordered_map<DWORD, HHOOK> WindowHookManager::s_threadHooks;
RtlUserThreadStart WindowHookManager::s_originalRtlUserThreadStart = nullptr;
HHOOK WindowHookManager::s_mainThreadHook = nullptr;
std::atomic<bool> WindowHookManager::s_debugEnabled(false);

WindowHookManager& WindowHookManager::GetInstance() {
    static WindowHookManager instance;
    return instance;
}

void WindowHookManager::Initialize(WindowEventCallback windowCallback, ErrorCallback errorCallback) {
    s_windowCallback = windowCallback;
    s_errorCallback = errorCallback;

    // 安装主线程钩子
    InstallMainThreadHook();

    // 挂钩线程创建函数
    HMODULE ntdll = GetModuleHandleW(L"ntdll.dll");
    if (!ntdll) {
        LogError(L"Failed to get ntdll module handle");
        return;
    }

    s_originalRtlUserThreadStart = reinterpret_cast<RtlUserThreadStart>(
        GetProcAddress(ntdll, "RtlUserThreadStart"));

    if (!s_originalRtlUserThreadStart) {
        LogError(L"Failed to get RtlUserThreadStart address");
        return;
    }

    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    if (DetourAttach(&(PVOID&)s_originalRtlUserThreadStart, HookedRtlUserThreadStart) != NO_ERROR) {
        LogError(L"DetourAttach failed for RtlUserThreadStart");
        DetourTransactionAbort();
        return;
    }

    if (DetourTransactionCommit() != NO_ERROR) {
        LogError(L"DetourTransactionCommit failed");
    }
}

void WindowHookManager::Shutdown() {
    // 卸载Detour钩子
    if (s_originalRtlUserThreadStart) {
        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
        DetourDetach(&(PVOID&)s_originalRtlUserThreadStart, HookedRtlUserThreadStart);
        DetourTransactionCommit();
    }

    // 卸载主线程钩子
    if (s_mainThreadHook) {
        UnhookWindowsHookEx(s_mainThreadHook);
        s_mainThreadHook = nullptr;
    }

    // 卸载所有线程钩子
    std::lock_guard<std::mutex> lock(s_dataMutex);
    for (auto& pair : s_threadHooks) {
        if (pair.second) {
            UnhookWindowsHookEx(pair.second);
        }
    }
    s_threadHooks.clear();
    s_trackedWindows.clear();
}

void WindowHookManager::InstallMainThreadHook() {
    DWORD threadId = GetCurrentThreadId();
    s_mainThreadHook = SetWindowsHookExW(
        WH_CBT,
        CBTProc,
        nullptr,
        GetCurrentThreadId()
    );

    if (!s_mainThreadHook) {
        DWORD error = GetLastError();
        std::wstringstream ss;
        ss << L"SetWindowsHookExW failed for main thread (ID: "
            << threadId << L"), error: " << error;
        LogError(ss.str());
        return;
    }
    if (s_debugEnabled) {
        OutputDebugStringW(L"Main thread hook installed\n");
    }
}

void WindowHookManager::InstallThreadHook() {
    DWORD threadId = GetCurrentThreadId();

    // 检查是否已安装钩子
    {
        std::lock_guard<std::mutex> lock(s_dataMutex);
        if (s_threadHooks.find(threadId) != s_threadHooks.end()) {
            if (s_debugEnabled) {
                std::wstringstream ss;
                ss << L"Hook already installed for thread " << threadId << L"\n";
                OutputDebugStringW(ss.str().c_str());
            }
            return;
        }
    }

    HHOOK hook = SetWindowsHookExW(WH_CBT, CBTProc, nullptr, threadId);

    if (!hook) {
        DWORD error = GetLastError();
        std::wstringstream ss;
        ss << L"SetWindowsHookExW failed for thread "
            << threadId << L", error: " << error;
        LogError(ss.str());
        return;
    }

    {
        std::lock_guard<std::mutex> lock(s_dataMutex);
        s_threadHooks[threadId] = hook;
    }

    if (s_debugEnabled) {
        std::wstringstream ss;
        ss << L"Hook installed for thread " << threadId << L"\n";
        OutputDebugStringW(ss.str().c_str());
    }
}

void NTAPI WindowHookManager::HookedRtlUserThreadStart(PTHREAD_START_ROUTINE startAddr, PVOID arg) {
    // 在新线程中安装钩子
    GetInstance().InstallThreadHook();

    // 调用原始线程函数
    s_originalRtlUserThreadStart(startAddr, arg);
}

LRESULT CALLBACK WindowHookManager::CBTProc(int code, WPARAM wParam, LPARAM lParam) {
    if (code < 0) {
        return CallNextHookEx(nullptr, code, wParam, lParam);
    }

    HWND hwnd = reinterpret_cast<HWND>(wParam);
    auto& instance = GetInstance();

    switch (code) {
    case HCBT_CREATEWND: {
        // 在窗口创建完成前捕获信息
        instance.TrackWindow(hwnd, true);
        break;
    }
    case HCBT_DESTROYWND: {
        instance.TrackWindow(hwnd, false);
        instance.UntrackWindow(hwnd);
        break;
    }
    case HCBT_SETFOCUS:
    case HCBT_ACTIVATE: {
        instance.UpdateWindowInfo(hwnd);
        break;
    }
    }

    return CallNextHookEx(nullptr, code, wParam, lParam);
}

WindowHookManager::WindowInfo WindowHookManager::CaptureWindowInfo(HWND hwnd) const {
    WindowInfo info;
    info.hWnd = hwnd;
    info.threadId = GetWindowThreadProcessId(hwnd, nullptr);

    int classNameLen = 256;
    std::vector<wchar_t> classNameBuffer;

    while (true) {
        classNameBuffer.resize(classNameLen);
        int result = GetClassNameW(hwnd, classNameBuffer.data(), classNameLen);

        if (result == 0) {
            DWORD error = GetLastError();
            if (error == ERROR_INSUFFICIENT_BUFFER) {
                classNameLen *= 2; 
                continue;
            }
            info.classNameError = true;
            break;
        }
        info.className = classNameBuffer.data();
        break;
    }

    int titleLen = GetWindowTextLengthW(hwnd) + 1; // +1 for null terminator
    if (titleLen > 1) {
        std::vector<wchar_t> titleBuffer(titleLen);
        if (GetWindowTextW(hwnd, titleBuffer.data(), titleLen) > 0) {
            info.windowTitle = titleBuffer.data();
        }
        else {
            DWORD error = GetLastError();
            if (error != ERROR_SUCCESS) {
                info.titleError = true;
            }
        }
    }
    else if (titleLen == 0) {
        // 空标题是合法的
    }
    else {
        info.titleError = true;
    }

    info.isVisible = IsWindowVisible(hwnd) != FALSE;
    info.isTopLevel = (GetWindowLongPtr(hwnd, GWL_STYLE) & WS_CHILD) == 0;

    return info;
}

void WindowHookManager::UpdateWindowInfo(HWND hwnd) {
    if (!IsWindow(hwnd)) return;

    WindowInfo newInfo = CaptureWindowInfo(hwnd);
    bool updated = false;

    {
        std::lock_guard<std::mutex> lock(s_dataMutex);
        auto it = s_trackedWindows.find(hwnd);
        if (it != s_trackedWindows.end()) {
            if (it->second.className != newInfo.className ||
                it->second.windowTitle != newInfo.windowTitle ||
                it->second.isVisible != newInfo.isVisible)
            {
                it->second = newInfo;
                updated = true;
            }
        }
    }

    if (updated && s_windowCallback) {
        s_windowCallback(newInfo, false); // false 表示更新而非创建
    }
}

void WindowHookManager::TrackWindow(HWND hwnd, bool created) {
    if (!s_windowCallback) return;

    WindowInfo info = CaptureWindowInfo(hwnd);

    // 更新缓存
    {
        std::lock_guard<std::mutex> lock(s_dataMutex);
        auto it = s_trackedWindows.find(hwnd);
        if (it != s_trackedWindows.end()) {
            // 更新现有记录
            it->second = info;
        }
        else {
            // 新建记录
            s_trackedWindows[hwnd] = info;
        }
    }

    s_windowCallback(info, created);

    if (s_debugEnabled) {
        std::wstringstream ss;
        ss << L"Window " << (created ? L"created" : L"updated")
            << L": HWND=0x" << std::hex << reinterpret_cast<uintptr_t>(hwnd);

        if (info.classNameError) {
            ss << L", ClassName=ERROR";
        }
        else {
            ss << L", Class=" << info.className;
        }

        if (info.titleError) {
            ss << L", Title=ERROR";
        }
        else {
            ss << L", Title=" << info.windowTitle;
        }

        ss << L", Visible=" << (info.isVisible ? L"true" : L"false")
            << L", TopLevel=" << (info.isTopLevel ? L"true" : L"false")
            << L"\n";

        OutputDebugStringW(ss.str().c_str());
    }
}

void WindowHookManager::UntrackWindow(HWND hwnd) {
    std::lock_guard<std::mutex> lock(s_dataMutex);
    s_trackedWindows.erase(hwnd);
}

void WindowHookManager::LogError(const std::wstring& message) const {
    if (s_errorCallback) {
        s_errorCallback(message);
    }

    if (s_debugEnabled) {
        SYSTEMTIME st;
        GetLocalTime(&st);
        wchar_t timestamp[64];
        swprintf_s(timestamp, L"[%02d:%02d:%02d.%03d] ",
            st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);

        OutputDebugStringW((timestamp + std::wstring(L"ERROR: ") + message + L"\n").c_str());
    }
}

std::vector<WindowHookManager::WindowInfo> WindowHookManager::GetTrackedWindows() const {
    std::lock_guard<std::mutex> lock(s_dataMutex);
    std::vector<WindowInfo> windows;
    for (const auto& pair : s_trackedWindows) {
        windows.push_back(pair.second);
    }
    return windows;
}

bool WindowHookManager::IsWindowTracked(HWND hwnd) const {
    std::lock_guard<std::mutex> lock(s_dataMutex);
    return s_trackedWindows.find(hwnd) != s_trackedWindows.end();
}

void WindowHookManager::EnableDebugOutput(bool enable) {
    s_debugEnabled = enable;
}