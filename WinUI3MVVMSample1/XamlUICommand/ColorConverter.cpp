#include "pch.h"
#include "ColorConverter.h"
#if __has_include("ColorConverter.g.cpp")
#include "ColorConverter.g.cpp"
#endif

#include <winrt/Windows.UI.h>
#include <winrt/Windows.UI.Xaml.Interop.h>
#include <ColorHelperUtil.h>

using namespace winrt;
namespace mux = winrt::Microsoft::UI::Xaml;
namespace muxm = winrt::Microsoft::UI::Xaml::Media;
namespace wui = winrt::Windows::UI;
namespace wf = winrt::Windows::Foundation;
namespace wuxiop = winrt::Windows::UI::Xaml::Interop;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::XamlUICommand::implementation
{

    wf::IInspectable ColorConverter::Convert(
        wf::IInspectable const& value,
        wuxiop::TypeName const& targetType,
        [[maybe_unused]] wf::IInspectable const& parameter,
        [[maybe_unused]] hstring const& language
    )
    {
        if (auto str = value.try_as<hstring>())
        {
            auto color = ColorHelperUtil::FromString(str.value());

            if (targetType == xaml_typename<wui::Color>())  // 类型检查，L"Windows.UI.Color"
            {
                return winrt::box_value(color);
            }
            else if (targetType == xaml_typename<muxm::Brush>())
            {
                return ColorHelperUtil::ToBrush(color);  // L"Microsoft.UI.Xaml.Media.Brush"
            }
        }

        // 默认透明
        if (targetType.Name == L"Windows.UI.Color")
        {
            return winrt::box_value(wui::Colors::Transparent());
        }
        else
        {
            return muxm::SolidColorBrush(wui::Colors::Transparent());
        }
    }

    wf::IInspectable ColorConverter::ConvertBack(
        wf::IInspectable const& value,
        [[maybe_unused]] wuxiop::TypeName const& targetType,
        [[maybe_unused]] wf::IInspectable const& parameter,
        [[maybe_unused]] hstring const& language
    )
    {
        if (auto brush = value.try_as<muxm::SolidColorBrush>())
        {
            return winrt::box_value(ColorHelperUtil::ToString(brush.Color()));
        }
        else if (auto color = value.try_as<wui::Color>())
        {
            return winrt::box_value(ColorHelperUtil::ToString(color.value()));
        }
        return wf::IInspectable{};
    }
}
