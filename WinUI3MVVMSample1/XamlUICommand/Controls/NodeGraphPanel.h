#pragma once
#include "NodeGraphPanel.g.h"
#include <unordered_map>

namespace winrt::XamlUICommand::implementation
{
    struct NodeGraphPanel : NodeGraphPanelT<NodeGraphPanel>
    {
        NodeGraphPanel();

        // DP-like properties exposed in IDL
        Windows::Foundation::Collections::IObservableVector<XamlUICommand::NodeViewModel> Nodes() const { return m_nodes; }
        void Nodes(Windows::Foundation::Collections::IObservableVector<XamlUICommand::NodeViewModel> const& value);

        Windows::Foundation::Collections::IObservableVector<XamlUICommand::EdgeViewModel> Edges() const { return m_edges; }
        void Edges(Windows::Foundation::Collections::IObservableVector<XamlUICommand::EdgeViewModel> const& value);

        XamlUICommand::NodeViewModel SelectedNode() const { return m_selected; }
        void SelectedNode(XamlUICommand::NodeViewModel const& value);

        XamlUICommand::NodeShape DefaultNodeShape() const { return m_defaultShape; }
        void DefaultNodeShape(XamlUICommand::NodeShape const& value);

        hstring DefaultTooltipMetaKey() const { return m_defaultTipKey; }
        void DefaultTooltipMetaKey(hstring const& v);

        hstring DefaultLabelMetaKey() const { return m_defaultLabelKey; }
        void DefaultLabelMetaKey(hstring const& v);

        // Public API
        XamlUICommand::NodeViewModel AddNode(int64_t id, hstring const& label, Windows::Foundation::Point const& position);
        void RemoveNode(int64_t id);
        XamlUICommand::NodeViewModel GetNode(int64_t id);
        void AddOrUpdateMeta(int64_t id, hstring const& key, IInspectable const& value);
        void RemoveMeta(int64_t id, hstring const& key);

        XamlUICommand::EdgeViewModel AddEdge(int64_t fromId, int64_t toId, hstring const& label);
        void RemoveEdge(int64_t fromId, int64_t toId);

        // Events
        winrt::event<XamlUICommand::NodeInvokedEventHandler> m_NodeInvoked;
        winrt::event_token NodeInvoked(XamlUICommand::NodeInvokedEventHandler const& handler)
        {
            return m_NodeInvoked.add(handler);
        }
        void NodeInvoked(winrt::event_token const& token) noexcept { m_NodeInvoked.remove(token); }

        // Dependency Properties (appearance)
        static winrt::Microsoft::UI::Xaml::DependencyProperty NodeFillProperty();
        static winrt::Microsoft::UI::Xaml::DependencyProperty NodeStrokeProperty();
        static winrt::Microsoft::UI::Xaml::DependencyProperty NodeTextBrushProperty();
        static winrt::Microsoft::UI::Xaml::DependencyProperty EdgeBrushProperty();
        static winrt::Microsoft::UI::Xaml::DependencyProperty EdgeThicknessProperty();
        static winrt::Microsoft::UI::Xaml::DependencyProperty NodeCornerRadiusProperty();
        static winrt::Microsoft::UI::Xaml::DependencyProperty AutoContrastProperty();
        static winrt::Microsoft::UI::Xaml::DependencyProperty ContrastThresholdProperty();

        Microsoft::UI::Xaml::Media::Brush NodeFill();
        void NodeFill(Microsoft::UI::Xaml::Media::Brush const& v);

        Microsoft::UI::Xaml::Media::Brush NodeStroke();
        void NodeStroke(Microsoft::UI::Xaml::Media::Brush const& v);

        Microsoft::UI::Xaml::Media::Brush NodeTextBrush();
        void NodeTextBrush(Microsoft::UI::Xaml::Media::Brush const& v);

        Microsoft::UI::Xaml::Media::Brush EdgeBrush();
        void EdgeBrush(Microsoft::UI::Xaml::Media::Brush const& v);

        double EdgeThickness();
        void EdgeThickness(double v);

        double NodeCornerRadius();
        void NodeCornerRadius(double v);

        bool AutoContrast();
        void AutoContrast(bool v);

        double ContrastThreshold();
        void ContrastThreshold(double v);

        // Templating
        void OnApplyTemplate();

    private:
        struct InnerAppearance
        {
            Microsoft::UI::Xaml::Media::Brush fill;
            Microsoft::UI::Xaml::Media::Brush stroke;
            Microsoft::UI::Xaml::Media::Brush text;
            double cornerRadius;
        };
        InnerAppearance ComputeAppearance();

        // Helpers
        void RebuildAll();
        void RedrawEdges();
        void RedrawNodes();
        void ConnectCollectionEvents();
        void DisconnectCollectionEvents();

        void AttachNodeElement(XamlUICommand::NodeViewModel const& node);
        void UpdateNodeElement(XamlUICommand::NodeViewModel const& node);
        void RemoveNodeElement(int64_t id);

        hstring ResolveLabel(XamlUICommand::NodeViewModel const& node) const;
        hstring ResolveTooltip(XamlUICommand::NodeViewModel const& node) const;

        // Create flyout for a node on demand
        Microsoft::UI::Xaml::Controls::Flyout CreateDetailsFlyout(XamlUICommand::NodeViewModel const& node);

        // Attribute change callback, triggers re-rendering when style extensions are modified.
        static void OnAppearanceChanged(Microsoft::UI::Xaml::DependencyObject const& d,
            Microsoft::UI::Xaml::DependencyPropertyChangedEventArgs const&);

        Microsoft::UI::Xaml::Controls::Canvas m_canvas{ nullptr };
        Windows::Foundation::Collections::IObservableVector<XamlUICommand::NodeViewModel> m_nodes{ winrt::single_threaded_observable_vector<XamlUICommand::NodeViewModel>() };
        Windows::Foundation::Collections::IObservableVector<XamlUICommand::EdgeViewModel> m_edges{ winrt::single_threaded_observable_vector<XamlUICommand::EdgeViewModel>() };
        XamlUICommand::NodeViewModel m_selected{ nullptr };
        XamlUICommand::NodeShape m_defaultShape{ XamlUICommand::NodeShape::Circle };
        hstring m_defaultTipKey{};
        hstring m_defaultLabelKey{};

        winrt::event_token m_nodesToken{};
        winrt::event_token m_edgesToken{};
        winrt::event_token m_themeToken{};

        // Map nodeId -> container element (Grid) for quick update
        std::unordered_map<int64_t, Microsoft::UI::Xaml::FrameworkElement> m_nodeElements;

        static Microsoft::UI::Xaml::DependencyProperty s_NodeFillProperty;
        static Microsoft::UI::Xaml::DependencyProperty s_NodeStrokeProperty;
        static Microsoft::UI::Xaml::DependencyProperty s_NodeTextBrushProperty;
        static Microsoft::UI::Xaml::DependencyProperty s_EdgeBrushProperty;
        static Microsoft::UI::Xaml::DependencyProperty s_EdgeThicknessProperty;
        static Microsoft::UI::Xaml::DependencyProperty s_NodeCornerRadiusProperty;
        static Microsoft::UI::Xaml::DependencyProperty s_AutoContrastProperty;
        static Microsoft::UI::Xaml::DependencyProperty s_ContrastThresholdProperty;
    };
}

namespace winrt::XamlUICommand::factory_implementation
{
    struct NodeGraphPanel : NodeGraphPanelT<NodeGraphPanel, implementation::NodeGraphPanel> {};
}
