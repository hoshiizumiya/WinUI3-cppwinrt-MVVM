#include "pch.h"
#include "MainPage.xaml.h"
#if __has_include("MainPage.g.cpp")
#include "MainPage.g.cpp"
#endif

#include <winrt/Windows.UI.h>
#include <winrt/Microsoft.UI.Xaml.Media.Animation.h>
#include <ColorHelperUtil.h>
#include "mvvm_framework_core.h"
#include <chrono> // 用于 std::chrono::milliseconds

using namespace winrt;
namespace wui = winrt::Windows::UI;
namespace mux = winrt::Microsoft::UI::Xaml;
namespace muxi = winrt::Microsoft::UI::Xaml::Input;
namespace muxc = winrt::Microsoft::UI::Xaml::Controls;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::XamlUICommand::implementation
{
#ifndef TIMESPAN_HELPER
#define TIMESPAN_HELPER
    constexpr int64_t MillisecondsToTimeSpan(int64_t ms)
    {
        return ms * 10'000; // 1 ms = 10,000 * 100ns
    }
#endif // TIMESPAN_HELPER

    MainPage::MainPage()
    {
        InitializeComponent();
        InitializeCommands();

        // Test ObservableProperties
#pragma region Test ObservableProperties
        auto obsInt8 = winrt::make<winrt::Mvvm::Framework::Core::implementation::ObservableInt8>();
        auto& obsInt8Impl = winrt::get_self_ref<winrt::Mvvm::Framework::Core::implementation::ObservableInt8>(obsInt8);
        obsInt8Impl.ValueSigned(-1);
        auto valueInt8 = obsInt8Impl.ValueSigned();
        auto valueInt8Raw = obsInt8.RawValue();
        auto valueInt8Projection = obsInt8.SignedValue();

        auto obsBoolean = winrt::make<winrt::Mvvm::Framework::Core::implementation::ObservableBoolean>();
        obsBoolean.Value(true);
        if (obsBoolean.IsObservable())
        {
            OutputDebugString(L"obsBoolean is observable\n");
        }
        else
        {
            OutputDebugString(L"obsBoolean is not observable\n");
        }

        if (obsBoolean.Value())
        {
            OutputDebugString(L"obsBoolean is true\n");
        } else {
            OutputDebugString(L"obsBoolean is false\n");
        }

        obsBoolean.Value(false);
        if (obsBoolean.Value())
        {
            OutputDebugString(L"obsBoolean is true\n");
        } else {
            OutputDebugString(L"obsBoolean is false\n");
        }
#pragma endregion

        //////////////////////////////////////////////////////

        // 在布局完成后初始化矩形位置
        CanvasMovableRect().LayoutUpdated([this](auto const&, auto const&)
            {
                // 防止多次触发
                static bool initialized = false;
                if (initialized) return;
                initialized = true;

                double centerX = (CanvasMovableRect().ActualWidth() - MovableRect().Width()) / 2;
                double centerY = (CanvasMovableRect().ActualHeight() - MovableRect().Height()) / 2;

                muxc::Canvas::SetLeft(MovableRect(), centerX);
                muxc::Canvas::SetTop(MovableRect(), centerY);

                // 通知按钮状态更新
                m_moveLeftCmd.NotifyCanExecuteChanged();
                m_moveRightCmd.NotifyCanExecuteChanged();
            });
    }

    void MainPage::InitializeCommands()
    {
        muxc::SymbolIconSource iconLeft;
        iconLeft.Symbol(muxc::Symbol::AlignLeft);
        muxc::SymbolIconSource iconRight;
        iconRight.Symbol(muxc::Symbol::AlignRight);

        // 创建 Left 命令
        m_moveLeftCmd = muxi::XamlUICommand();
        m_moveLeftCmd.Label(L"Move Left");
        m_moveLeftCmd.Description(L"Move rectangle to the left");
        m_moveLeftCmd.IconSource(iconLeft);
        m_moveLeftCmd.ExecuteRequested({ this, &MainPage::OnMoveLeftExecute });
        m_moveLeftCmd.CanExecuteRequested({ this, &MainPage::OnCanExecuteLeft });

        muxi::KeyboardAccelerator leftAccel;
        leftAccel.Key(Windows::System::VirtualKey::Left);
        leftAccel.Modifiers(Windows::System::VirtualKeyModifiers::Control);
        m_moveLeftCmd.KeyboardAccelerators().Append(leftAccel);

        // 创建 Right 命令
        m_moveRightCmd = muxi::XamlUICommand();
        m_moveRightCmd.Label(L"Move Right");
        m_moveRightCmd.Description(L"Move rectangle to the right");
        m_moveRightCmd.IconSource(iconRight);
        m_moveRightCmd.ExecuteRequested({ this, &MainPage::OnMoveRightExecute });
        m_moveRightCmd.CanExecuteRequested({ this, &MainPage::OnCanExecuteRight });

        muxi::KeyboardAccelerator rightAccel;
        rightAccel.Key(Windows::System::VirtualKey::Right);
        rightAccel.Modifiers(Windows::System::VirtualKeyModifiers::Control);
        m_moveRightCmd.KeyboardAccelerators().Append(rightAccel);

        // 填充颜色命令
        m_fillColor1Cmd = muxi::XamlUICommand();
        m_fillColor1Cmd.Label(L"Fuchsia");
        m_fillColor1Cmd.ExecuteRequested([this](auto&, auto&) {
            //MovableRect().Fill(mux::Media::SolidColorBrush(wui::Colors::Fuchsia()));
            RectangleColor(ColorHelperUtil::ToString(wui::Colors::Fuchsia()));
            });

        m_fillColor2Cmd = muxi::XamlUICommand();
        m_fillColor2Cmd.Label(L"Cornsilk");
        m_fillColor2Cmd.ExecuteRequested([this](auto&, auto&) {
            //MovableRect().Fill(mux::Media::SolidColorBrush(wui::Colors::Cornsilk()));
            RectangleColor(ColorHelperUtil::ToString(wui::Colors::Cornsilk()));
            });

        m_fillColor3Cmd = muxi::XamlUICommand();
        m_fillColor3Cmd.Label(L"CornflowerBlue");
        m_fillColor3Cmd.ExecuteRequested([this](auto&, auto&) {
            //MovableRect().Fill(mux::Media::SolidColorBrush(wui::Colors::CornflowerBlue()));
            RectangleColor(ColorHelperUtil::ToString(wui::Colors::CornflowerBlue()));
            });

        // 显示/隐藏命令
        m_toggleVisibilityCmd = muxi::XamlUICommand();
        m_toggleVisibilityCmd.Label(L"Show/Hide");
        m_toggleVisibilityCmd.ExecuteRequested([this](auto&, auto&) {
            auto current = MovableRect().Visibility();
            MovableRect().Visibility(
                current == mux::Visibility::Visible ? mux::Visibility::Collapsed : mux::Visibility::Visible
            );
            });

        // 绑定按钮命令
        BtnLeft().Command(m_moveLeftCmd);
        BtnRight().Command(m_moveRightCmd);

        // 菜单项和底部菜单绑定
        MenuFillRed().Command(m_fillColor1Cmd);
        MenuFillGreen().Command(m_fillColor2Cmd);
        MenuFillBlue().Command(m_fillColor3Cmd);
        MenuToggleVisibility().Command(m_toggleVisibilityCmd);

        MenuBottomFillRed().Command(m_fillColor1Cmd);
        MenuBottomFillGreen().Command(m_fillColor2Cmd);
        MenuBottomFillBlue().Command(m_fillColor3Cmd);
        MenuBottomToggleVisibility().Command(m_toggleVisibilityCmd);
    }

    void MainPage::OnMoveLeftExecute(
        IInspectable const&,
        muxi::ExecuteRequestedEventArgs const&)
    {
        double left = muxc::Canvas::GetLeft(MovableRect());
        if (std::isnan(left))
            left = (CanvasMovableRect().ActualWidth() - MovableRect().Width()) / 2;
        muxc::Canvas::SetLeft(MovableRect(), std::max(0.0, left - 20.0));
        m_moveLeftCmd.NotifyCanExecuteChanged();
        m_moveRightCmd.NotifyCanExecuteChanged();
    }

    void MainPage::OnMoveRightExecute(
        IInspectable const&,
        muxi::ExecuteRequestedEventArgs const&)
    {
        double left = muxc::Canvas::GetLeft(MovableRect());
        if (std::isnan(left))
            left = (CanvasMovableRect().ActualWidth() - MovableRect().Width()) / 2;

        double maxRight = CanvasMovableRect().ActualWidth() - MovableRect().Width();
        muxc::Canvas::SetLeft(MovableRect(), std::min(maxRight, left + 20.0));

        m_moveLeftCmd.NotifyCanExecuteChanged();
        m_moveRightCmd.NotifyCanExecuteChanged();
    }

    void MainPage::OnCanExecuteLeft(
        IInspectable const&,
        muxi::CanExecuteRequestedEventArgs const& args)
    {
        double left = muxc::Canvas::GetLeft(MovableRect());
        if (std::isnan(left))
            left = (CanvasMovableRect().ActualWidth() - MovableRect().Width()) / 2;

        args.CanExecute(left > 0);
    }

    void MainPage::OnCanExecuteRight(
        IInspectable const&,
        muxi::CanExecuteRequestedEventArgs const& args)
    {
        double left = muxc::Canvas::GetLeft(MovableRect());
        if (std::isnan(left))
            left = (CanvasMovableRect().ActualWidth() - MovableRect().Width()) / 2;

        double maxRight = CanvasMovableRect().ActualWidth() - MovableRect().Width();
        args.CanExecute(left < maxRight);
    }

    hstring MainPage::RectangleColor()
    {
        return m_rectangleColor;
    }

    void MainPage::RectangleColor(hstring const& value)
    {
        if (m_rectangleColor != value)
        {
            m_rectangleColor = value;
            RaisePropertyChanged(L"RectangleColor");
        }
    }

    void MainPage::OnShowColorPicker(IInspectable const&, mux::RoutedEventArgs const&)
    {
        if (ColorPickerPanel().Visibility() == mux::Visibility::Visible)
        {
            return OnHideColorPicker(nullptr, nullptr);
        }

        double rectLeft = muxc::Canvas::GetLeft(MovableRect());
        double canvasWidth = CanvasMovableRect().ActualWidth();

        bool showFromRight = rectLeft < (canvasWidth / 3);

        // 设置对齐和初始位置
        if (showFromRight)
        {
            ColorPickerPanel().HorizontalAlignment(mux::HorizontalAlignment::Right);
            ColorPickerTransform().X(250); // 从右滑入
        }
        else
        {
            ColorPickerPanel().HorizontalAlignment(mux::HorizontalAlignment::Left);
            ColorPickerTransform().X(-250); // 从左滑入
        }

        ColorPickerPanel().Visibility(mux::Visibility::Visible);

        // 动画
        auto sb = mux::Media::Animation::Storyboard();
        wf::TimeSpan duration{ MillisecondsToTimeSpan(1) }; // FIXME: 动画时长似乎不凑效，目前通过速率 SpeedRatio 调整

        auto slideAnim = mux::Media::Animation::DoubleAnimation();
        slideAnim.To(0.0);
        slideAnim.Duration(mux::Duration(duration));
        slideAnim.SpeedRatio(3.5);
        mux::Media::Animation::Storyboard::SetTarget(slideAnim, ColorPickerTransform());
        mux::Media::Animation::Storyboard::SetTargetProperty(slideAnim, L"X");

        auto fadeAnim = mux::Media::Animation::DoubleAnimation();
        fadeAnim.To(1.0);
        fadeAnim.Duration(mux::Duration(duration));
        fadeAnim.SpeedRatio(3.5);
        mux::Media::Animation::Storyboard::SetTarget(fadeAnim, ColorPickerPanel());
        mux::Media::Animation::Storyboard::SetTargetProperty(fadeAnim, L"Opacity");

        sb.Children().Append(slideAnim);
        sb.Children().Append(fadeAnim);
        sb.Begin();
    }

    void MainPage::OnHideColorPicker(IInspectable const&, mux::RoutedEventArgs const&)
    {
        bool hideToRight = (ColorPickerPanel().HorizontalAlignment() == mux::HorizontalAlignment::Right);

        double offScreenX = hideToRight ? 250 : -250;

        auto sb = mux::Media::Animation::Storyboard();
        wf::TimeSpan duration{ MillisecondsToTimeSpan(1) };

        // Slide out
        auto slideAnim = mux::Media::Animation::DoubleAnimation();
        slideAnim.To(offScreenX);
        slideAnim.Duration(mux::Duration(duration));
        slideAnim.SpeedRatio(3.5);
        mux::Media::Animation::Storyboard::SetTarget(slideAnim, ColorPickerTransform());
        mux::Media::Animation::Storyboard::SetTargetProperty(slideAnim, L"X");

        // Fade out
        auto fadeAnim = mux::Media::Animation::DoubleAnimation();
        fadeAnim.To(0.0);
        fadeAnim.Duration(mux::Duration(duration));
        fadeAnim.SpeedRatio(3.5);
        mux::Media::Animation::Storyboard::SetTarget(fadeAnim, ColorPickerPanel());
        mux::Media::Animation::Storyboard::SetTargetProperty(fadeAnim, L"Opacity");

        sb.Children().Append(slideAnim);
        sb.Children().Append(fadeAnim);

        // 动画完成后隐藏
        sb.Completed([this](auto const&, auto const&)
            {
                ColorPickerPanel().Visibility(mux::Visibility::Collapsed);
            });

        sb.Begin();
    }
}
