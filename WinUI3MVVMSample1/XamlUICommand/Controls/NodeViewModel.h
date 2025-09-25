#pragma once
#include "NodeViewModel.g.h"

namespace winrt::XamlUICommand::implementation
{
    struct NodeViewModel : NodeViewModelT<NodeViewModel>
    {
        NodeViewModel() = default;

        int64_t Id() const noexcept { return m_id; }
        void Id(int64_t value) noexcept { if (m_id != value) { m_id = value; Raise(L"Id"); } }

        hstring Label() const noexcept { return m_label; }
        void Label(hstring const& value) { if (m_label != value) { m_label = value; Raise(L"Label"); } }

        Windows::Foundation::Point Position() const noexcept { return m_pos; }
        void Position(Windows::Foundation::Point const& value) { if (m_pos != value) { m_pos = value; Raise(L"Position"); } }

        Windows::Foundation::Size Size() const noexcept { return m_size; }
        void Size(Windows::Foundation::Size const& value) { if (m_size != value) { m_size = value; Raise(L"Size"); } }

        bool IsSelected() const noexcept { return m_selected; }
        void IsSelected(bool value) { if (m_selected != value) { m_selected = value; Raise(L"IsSelected"); } }

        XamlUICommand::NodeShape Shape() const noexcept { return m_shape; }
        void Shape(XamlUICommand::NodeShape value) { if (m_shape != value) { m_shape = value; Raise(L"Shape"); } }

        Windows::Foundation::Collections::IMap<hstring, winrt::Windows::Foundation::IInspectable> Meta() const noexcept { return m_meta; }
        void Meta(Windows::Foundation::Collections::IMap<hstring, winrt::Windows::Foundation::IInspectable> const& value)
        {
            if (m_meta != value) { m_meta = value; Raise(L"Meta"); }
        }

        hstring LabelMetaKey() const noexcept { return m_labelKey; }
        void LabelMetaKey(hstring const& v) { if (m_labelKey != v) { m_labelKey = v; Raise(L"LabelMetaKey"); } }

        hstring TooltipMetaKey() const noexcept { return m_tipKey; }
        void TooltipMetaKey(hstring const& v) { if (m_tipKey != v) { m_tipKey = v; Raise(L"TooltipMetaKey"); } }

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

        int64_t m_id{};
        hstring m_label{};
        Windows::Foundation::Point m_pos{ 0, 0 };
        Windows::Foundation::Size m_size{ 64, 64 };
        bool m_selected{};
        XamlUICommand::NodeShape m_shape{ XamlUICommand::NodeShape::Circle };
        Windows::Foundation::Collections::IMap<hstring, winrt::Windows::Foundation::IInspectable> m_meta{ winrt::single_threaded_map<hstring, winrt::Windows::Foundation::IInspectable>() };
        hstring m_labelKey{};
        hstring m_tipKey{};
    };
}

namespace winrt::XamlUICommand::factory_implementation
{
    struct NodeViewModel : NodeViewModelT<NodeViewModel, implementation::NodeViewModel> {};
}