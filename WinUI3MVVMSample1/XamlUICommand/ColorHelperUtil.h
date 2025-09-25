// ColorHelperUtil.h
#pragma once
#include <winrt/Windows.UI.h>
#include <winrt/Microsoft.UI.Xaml.Media.h>
#include <hstring.h>

namespace winrt::XamlUICommand::implementation
{
    struct ColorHelperUtil
    {
        static winrt::Windows::UI::Color FromString(winrt::hstring const& str);
        static winrt::hstring ToString(winrt::Windows::UI::Color const& color);
        static winrt::Microsoft::UI::Xaml::Media::SolidColorBrush ToBrush(winrt::Windows::UI::Color const& color);
    };
}
