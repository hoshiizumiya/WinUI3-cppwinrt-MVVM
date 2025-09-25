#pragma once
#include "HomePage.g.h"

namespace winrt::XamlUICommand::implementation
{
    struct HomePage : HomePageT<HomePage>
    {
        HomePage() { InitializeComponent(); }

        void OpenGraph_Click(winrt::Windows::Foundation::IInspectable const&,
            winrt::Microsoft::UI::Xaml::RoutedEventArgs const&);
    };
}

namespace winrt::XamlUICommand::factory_implementation
{
    struct HomePage : HomePageT<HomePage, implementation::HomePage> {};
}
