#pragma once

#include "MainPage.g.h"

namespace mux   =   winrt::Microsoft::UI::Xaml;
namespace muxi  =   winrt::Microsoft::UI::Xaml::Input;
namespace muxd  =   winrt::Microsoft::UI::Xaml::Data;
namespace wf    =   winrt::Windows::Foundation;

namespace winrt::XamlUICommand::implementation
{
    struct MainPage : MainPageT<MainPage>
    {
        MainPage();

        hstring RectangleColor();
        void RectangleColor(hstring const& value);
        void OnShowColorPicker(wf::IInspectable const&, mux::RoutedEventArgs const&);
        void OnHideColorPicker(wf::IInspectable const&, mux::RoutedEventArgs const&);

        winrt::event_token PropertyChanged(muxd::PropertyChangedEventHandler const& handler)
        {
            return m_propertyChanged.add(handler);
        }
        void PropertyChanged(winrt::event_token const& token) noexcept
        {
            m_propertyChanged.remove(token);
        }

    protected:
        void RaisePropertyChanged(std::wstring const& propertyName)
        {
            m_propertyChanged(*this,
                muxd::PropertyChangedEventArgs{ winrt::hstring(propertyName) });
        }

    private:
        void InitializeCommands();

        void OnMoveLeftExecute(
            wf::IInspectable const& sender,
            muxi::ExecuteRequestedEventArgs const& args);

        void OnMoveRightExecute(
            wf::IInspectable const& sender,
            muxi::ExecuteRequestedEventArgs const& args);

        void OnCanExecuteLeft(
            wf::IInspectable const& sender,
            muxi::CanExecuteRequestedEventArgs const& args);

        void OnCanExecuteRight(
            wf::IInspectable const& sender,
            muxi::CanExecuteRequestedEventArgs const& args);

    private:
        hstring m_rectangleColor = L"100,149,237,255"; // CornflowerBlue
        muxi::XamlUICommand m_moveLeftCmd{ nullptr };
        muxi::XamlUICommand m_moveRightCmd{ nullptr };
        muxi::XamlUICommand m_fillColor1Cmd{ nullptr };
        muxi::XamlUICommand m_fillColor2Cmd{ nullptr };
        muxi::XamlUICommand m_fillColor3Cmd{ nullptr };
        muxi::XamlUICommand m_toggleVisibilityCmd{ nullptr };
        winrt::event<muxd::PropertyChangedEventHandler> m_propertyChanged;
    };
}

namespace winrt::XamlUICommand::factory_implementation
{
    struct MainPage : MainPageT<MainPage, implementation::MainPage>
    {
    };
}
