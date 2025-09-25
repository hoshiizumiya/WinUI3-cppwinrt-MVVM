// ColorHelperUtil.cpp
#include "pch.h"
#include "ColorHelperUtil.h"
#include <format>

using namespace winrt;
namespace wui = winrt::Windows::UI;
namespace muxm = winrt::Microsoft::UI::Xaml::Media;

namespace winrt::XamlUICommand::implementation
{
    wui::Color ColorHelperUtil::FromString(winrt::hstring const& str)
    {
        uint8_t r = 0, g = 0, b = 0, a = 255;
        if (swscanf_s(str.c_str(), L"%hhu,%hhu,%hhu,%hhu", &r, &g, &b, &a) >= 3)
        {
            return wui::ColorHelper::FromArgb(a, r, g, b);
        }
        return wui::Colors::Transparent();
    }

    winrt::hstring ColorHelperUtil::ToString(wui::Color const& color)
    {
        return winrt::hstring{ std::format(L"{},{},{},{}", color.R, color.G, color.B, color.A) };
    }

    muxm::SolidColorBrush ColorHelperUtil::ToBrush(wui::Color const& color)
    {
        return muxm::SolidColorBrush(color);
    }
}
