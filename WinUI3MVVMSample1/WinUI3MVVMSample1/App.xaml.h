#pragma once

#include "App.xaml.g.h"
#include "ViewModel\MyEntityViewModel.h"

namespace winrt::WinUI3MVVMSample1::implementation
{
    struct App : AppT<App>
    {
        App();

        void OnLaunched(Microsoft::UI::Xaml::LaunchActivatedEventArgs const&);

    private:
        winrt::Microsoft::UI::Xaml::Window window{ nullptr };
    };
}
