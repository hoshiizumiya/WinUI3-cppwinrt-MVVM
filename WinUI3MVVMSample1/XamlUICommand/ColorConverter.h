#pragma once

#include "ColorConverter.g.h"

namespace wf = winrt::Windows::Foundation;
namespace wuxiop = winrt::Windows::UI::Xaml::Interop;

namespace winrt::XamlUICommand::implementation
{
    struct ColorConverter : ColorConverterT<ColorConverter>
    {
        ColorConverter() = default;

        wf::IInspectable Convert(
            wf::IInspectable const& value, 
            wuxiop::TypeName const& targetType,
            wf::IInspectable const& parameter,
            hstring const& language);

        wf::IInspectable ConvertBack(
            wf::IInspectable const& value,
            wuxiop::TypeName const& targetType,
            wf::IInspectable const& parameter,
            hstring const& language);
    };
}

namespace winrt::XamlUICommand::factory_implementation
{
    struct ColorConverter : ColorConverterT<ColorConverter, implementation::ColorConverter>
    {
    };
}
