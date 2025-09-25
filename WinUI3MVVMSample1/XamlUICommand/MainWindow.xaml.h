#pragma once
#include "MainWindow.g.h"

namespace winrt::XamlUICommand::implementation
{
    struct MainWindow : MainWindowT<MainWindow>
    {
        MainWindow();

        void Nav_SelectionChanged(
            Microsoft::UI::Xaml::Controls::NavigationView const& sender,
            Microsoft::UI::Xaml::Controls::NavigationViewSelectionChangedEventArgs const& args);

    private:
        void NavigateTo(winrt::hstring const& tag);
    };
}

namespace winrt::XamlUICommand::factory_implementation
{
    struct MainWindow : MainWindowT<MainWindow, implementation::MainWindow> {};
}
