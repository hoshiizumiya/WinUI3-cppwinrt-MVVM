#pragma once
#include "NodeGraphPage.g.h"

namespace winrt::XamlUICommand::implementation
{
    struct NodeGraphPage : NodeGraphPageT<NodeGraphPage>
    {
        NodeGraphPage();
        void BuildSample();
        void OnResetViewClick(Windows::Foundation::IInspectable const&, Microsoft::UI::Xaml::RoutedEventArgs const&);
        void OnRandomizeClick(Windows::Foundation::IInspectable const&, Microsoft::UI::Xaml::RoutedEventArgs const&);
        void OnGraphDoubleTapped(Windows::Foundation::IInspectable const&, Microsoft::UI::Xaml::Input::DoubleTappedRoutedEventArgs const&);
    };
}

namespace winrt::XamlUICommand::factory_implementation
{
    struct NodeGraphPage : NodeGraphPageT<NodeGraphPage, implementation::NodeGraphPage> {};
}
