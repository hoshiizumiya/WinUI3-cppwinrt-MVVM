#pragma once

#include "FlyoutAcrylicBackdropHelper.g.h"

namespace winrt::XamlUICommand::implementation
{
    struct FlyoutAcrylicBackdropHelper : FlyoutAcrylicBackdropHelperT<FlyoutAcrylicBackdropHelper>
    {
        FlyoutAcrylicBackdropHelper() = default;

        static winrt::Microsoft::UI::Xaml::DependencyProperty HostBackdropAcrylicProperty();
        static bool GetHostBackdropAcrylicState(winrt::Microsoft::UI::Xaml::Controls::Flyout const& flyout);
        static void SetHostBackdropAcrylicState(
            winrt::Microsoft::UI::Xaml::Controls::Flyout const& flyout,
            bool value
        );

    private:
        static winrt::Microsoft::UI::Xaml::DependencyProperty s_hostBackdropAcrylicProperty;

        static void HostBackdropAcrylicChanged(
            winrt::Microsoft::UI::Xaml::DependencyObject const& object,
            winrt::Microsoft::UI::Xaml::DependencyPropertyChangedEventArgs const& arg
        );
    };
}

namespace winrt::XamlUICommand::factory_implementation
{
    struct FlyoutAcrylicBackdropHelper : FlyoutAcrylicBackdropHelperT<FlyoutAcrylicBackdropHelper, implementation::FlyoutAcrylicBackdropHelper>
    {
    };
}
