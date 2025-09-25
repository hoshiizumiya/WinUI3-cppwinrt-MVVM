#include "pch.h"
#include "App.xaml.h"
#include "WindowHookManager.h"

int __stdcall wWinMain(HINSTANCE, HINSTANCE, PWSTR, int)
{
    // ���õ������
    WindowHookManager::GetInstance().EnableDebugOutput(true);

    // ��ʼ�����ڹ��ӹ�����
    WindowHookManager::GetInstance().Initialize(
        [](const WindowHookManager::WindowInfo& info, bool created) {
            // �����¼�����
            std::wstring eventType = created ? L"Created" : L"Destroyed";
            std::wstring msg = eventType + L": " + info.className;
            OutputDebugStringW(msg.c_str());
        },
        [](const std::wstring& error) {
            // ������
            OutputDebugStringW((L"WindowHook Error: " + error).c_str());
        }
    );

    winrt::init_apartment(winrt::apartment_type::single_threaded);
    ::winrt::Microsoft::UI::Xaml::Application::Start(
        [](auto&&) {
            ::winrt::make<::winrt::XamlUICommand::implementation::App>();
        });

    WindowHookManager::GetInstance().Shutdown();
    return 0;
}