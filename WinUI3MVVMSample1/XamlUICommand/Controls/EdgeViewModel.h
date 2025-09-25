#pragma once
#include "EdgeViewModel.g.h"

namespace winrt::XamlUICommand::implementation
{
    struct EdgeViewModel : EdgeViewModelT<EdgeViewModel>
    {
        EdgeViewModel() = default;

        int64_t FromId() const noexcept { return m_from; }
        void FromId(int64_t v) noexcept { if (m_from != v) { m_from = v; Raise(L"FromId"); } }

        int64_t ToId() const noexcept { return m_to; }
        void ToId(int64_t v) noexcept { if (m_to != v) { m_to = v; Raise(L"ToId"); } }

        hstring Label() const noexcept { return m_label; }
        void Label(hstring const& v) { if (m_label != v) { m_label = v; Raise(L"Label"); } }

        bool IsDirected() const noexcept { return m_directed; }
        void IsDirected(bool v) { if (m_directed != v) { m_directed = v; Raise(L"IsDirected"); } }

        double Weight() const noexcept { return m_weight; }
        void Weight(double v) { if (m_weight != v) { m_weight = v; Raise(L"Weight"); } }

        // INotifyPropertyChanged
        winrt::event<Microsoft::UI::Xaml::Data::PropertyChangedEventHandler> m_propertyChanged;
        winrt::event_token PropertyChanged(Microsoft::UI::Xaml::Data::PropertyChangedEventHandler const& handler)
        {
            return m_propertyChanged.add(handler);
        }
        void PropertyChanged(winrt::event_token const& token) noexcept
        {
            m_propertyChanged.remove(token);
        }

    private:
        void Raise(std::wstring_view const& name)
        {
            m_propertyChanged(*this, Microsoft::UI::Xaml::Data::PropertyChangedEventArgs{ winrt::hstring{ name } });
        }

        int64_t m_from{};
        int64_t m_to{};
        hstring m_label{};
        bool m_directed{};
        double m_weight{};
    };
}

namespace winrt::XamlUICommand::factory_implementation
{
    struct EdgeViewModel : EdgeViewModelT<EdgeViewModel, implementation::EdgeViewModel> {};
}
