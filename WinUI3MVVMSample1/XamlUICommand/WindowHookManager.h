#pragma once
#include <Windows.h>
#include <detours/detours.h>
#include <unordered_set>
#include <mutex>
#include <functional>

typedef _Return_type_success_(return >= 0) long NTSTATUS;

#ifndef _NTRTL_H

typedef
VOID
(NTAPI*
RtlUserThreadStart)(
    _In_ PTHREAD_START_ROUTINE Function,
    _In_ PVOID Parameter
);

#endif


class WindowHookManager {
public:
    struct WindowInfo {
        HWND hWnd = nullptr;
        DWORD threadId = 0;
        std::wstring className;
        std::wstring windowTitle;
        bool isVisible = false;
        bool isTopLevel = false;
        bool classNameError = false;
        bool titleError = false;
    };

    using WindowEventCallback = std::function<void(const WindowInfo&, bool)>;
    using ErrorCallback = std::function<void(const std::wstring&)>;

    static WindowHookManager& GetInstance();
    void Initialize(
        WindowEventCallback windowCallback,
        ErrorCallback errorCallback = nullptr
    );
    void Shutdown();

    std::vector<WindowInfo> GetTrackedWindows() const;
    bool IsWindowTracked(HWND hwnd) const;
    void EnableDebugOutput(bool enable);

private:
    WindowHookManager() = default;
    ~WindowHookManager() = default;

    void InstallMainThreadHook();
    void InstallThreadHook();
    static void NTAPI HookedRtlUserThreadStart(PTHREAD_START_ROUTINE startAddr, PVOID arg);
    static LRESULT CALLBACK CBTProc(int code, WPARAM wParam, LPARAM lParam);

    WindowInfo CaptureWindowInfo(HWND hwnd) const;
    void UpdateWindowInfo(HWND hwnd);
    void TrackWindow(HWND hwnd, bool created);
    void UntrackWindow(HWND hwnd);
    void LogError(const std::wstring& message) const;

    static WindowEventCallback s_windowCallback;
    static ErrorCallback s_errorCallback;
    static std::mutex s_dataMutex;
    static std::unordered_map<HWND, WindowInfo> s_trackedWindows;
    static std::unordered_map<DWORD, HHOOK> s_threadHooks;
    static RtlUserThreadStart s_originalRtlUserThreadStart;
    static HHOOK s_mainThreadHook;
    static std::atomic<bool> s_debugEnabled;
};