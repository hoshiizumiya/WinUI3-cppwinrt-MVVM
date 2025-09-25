#include "pch.h"

#include "EdgeViewModel.h"
#include "NodeGraphPanel.h"
#include "NodeInvokedEventArgs.h"
#include "NodeViewModel.h"

#if __has_include("NodeGraphPanel.g.cpp")
#include "NodeGraphPanel.g.cpp"
#endif

#include <winrt/Windows.UI.Text.h>
#include <Helpers/VisualTreeHelper.hpp>
#include <winrt/Microsoft.UI.Content.h>
#include <winrt/Microsoft.UI.Xaml.Hosting.h>


using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml::Controls;
using namespace Microsoft::UI::Xaml::Input;
using namespace Microsoft::UI::Xaml::Media;
using namespace Microsoft::UI::Xaml::Shapes;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;

namespace winrt::XamlUICommand::implementation
{
    // Helper functions, use macros to avoid polluting global namespace and avoid redefinition errors.
#ifndef NODEGRAPHPANEL_HELPERS_API
#define NODEGRAPHPANEL_HELPERS_API
    // 相对亮度 & 对比度（WCAG）
    static double Luminance(Windows::UI::Color c)
    {
        auto toLin = [](double u)
            {
                u /= 255.0;
                return (u <= 0.03928) ? (u / 12.92) : std::pow((u + 0.055) / 1.055, 2.4);
            };
        return 0.2126 * toLin(c.R) + 0.7152 * toLin(c.G) + 0.0722 * toLin(c.B);
    }
    static double Contrast(Windows::UI::Color a, Windows::UI::Color b)
    {
        double la = Luminance(a), lb = Luminance(b);
        double L1 = std::max(la, lb) + 0.05;
        double L2 = std::min(la, lb) + 0.05;
        return L1 / L2;
    }
    static Windows::UI::Color MakeLighterDarker(Windows::UI::Color c, double delta) // delta: [-1,1]
    {
        auto clamp = [](int v) { return std::min(255, std::max(0, v)); };
        int d = int(delta * 255.0);
        return Windows::UI::Color{ c.A, (uint8_t)clamp(c.R + d), (uint8_t)clamp(c.G + d), (uint8_t)clamp(c.B + d) };
    }
    static bool TryGetSolid(Brush const& b, Windows::UI::Color& out)
    {
        if (auto sb = b.try_as<SolidColorBrush>()) { out = sb.Color(); return true; }
        return false;
    }
#endif

    // Register dependency properties.
#pragma region NodeGraphPanel_DP_Registers
    DependencyProperty NodeGraphPanel::s_NodeFillProperty =
        DependencyProperty::Register(L"NodeFill",
            xaml_typename<Brush>(),
            xaml_typename<XamlUICommand::NodeGraphPanel>(),
            PropertyMetadata{ nullptr, PropertyChangedCallback{ &NodeGraphPanel::OnAppearanceChanged } });

    DependencyProperty NodeGraphPanel::s_NodeStrokeProperty =
        DependencyProperty::Register(L"NodeStroke", xaml_typename<Brush>(),
            xaml_typename<XamlUICommand::NodeGraphPanel>(),
            PropertyMetadata{ nullptr, PropertyChangedCallback{ &NodeGraphPanel::OnAppearanceChanged } });

    DependencyProperty NodeGraphPanel::s_NodeTextBrushProperty =
        DependencyProperty::Register(L"NodeTextBrush", xaml_typename<Brush>(),
            xaml_typename<XamlUICommand::NodeGraphPanel>(),
            PropertyMetadata{ nullptr, PropertyChangedCallback{ &NodeGraphPanel::OnAppearanceChanged } });

    DependencyProperty NodeGraphPanel::s_EdgeBrushProperty =
        DependencyProperty::Register(L"EdgeBrush", xaml_typename<Brush>(),
            xaml_typename<XamlUICommand::NodeGraphPanel>(),
            PropertyMetadata{ nullptr, PropertyChangedCallback{ &NodeGraphPanel::OnAppearanceChanged } });

    DependencyProperty NodeGraphPanel::s_EdgeThicknessProperty =
        DependencyProperty::Register(L"EdgeThickness", xaml_typename<double>(),
            xaml_typename<XamlUICommand::NodeGraphPanel>(),
            PropertyMetadata{ winrt::box_value(2.5), PropertyChangedCallback{ &NodeGraphPanel::OnAppearanceChanged } });

    DependencyProperty NodeGraphPanel::s_NodeCornerRadiusProperty =
        DependencyProperty::Register(L"NodeCornerRadius", xaml_typename<double>(),
            xaml_typename<XamlUICommand::NodeGraphPanel>(),
            PropertyMetadata{ winrt::box_value(10.0), PropertyChangedCallback{ &NodeGraphPanel::OnAppearanceChanged } });

    DependencyProperty NodeGraphPanel::s_AutoContrastProperty =
        DependencyProperty::Register(L"AutoContrast", xaml_typename<bool>(),
            xaml_typename<XamlUICommand::NodeGraphPanel>(),
            PropertyMetadata{ winrt::box_value(true), PropertyChangedCallback{ &NodeGraphPanel::OnAppearanceChanged } });

    DependencyProperty NodeGraphPanel::s_ContrastThresholdProperty =
        DependencyProperty::Register(L"ContrastThreshold", xaml_typename<double>(),
            xaml_typename<XamlUICommand::NodeGraphPanel>(),
            PropertyMetadata{ winrt::box_value(4.5), PropertyChangedCallback{ &NodeGraphPanel::OnAppearanceChanged } });


    // 注册（保持你原有的 Register 代码，但改用 s_ 前缀）
    DependencyProperty NodeGraphPanel::NodeFillProperty()
    {
        return s_NodeFillProperty;
    }
    DependencyProperty NodeGraphPanel::NodeStrokeProperty()
    {
        return s_NodeStrokeProperty;
    }
    DependencyProperty NodeGraphPanel::NodeTextBrushProperty()
    {
        return s_NodeTextBrushProperty;
    }
    DependencyProperty NodeGraphPanel::EdgeBrushProperty()
    {
        return s_EdgeBrushProperty;
    }
    DependencyProperty NodeGraphPanel::EdgeThicknessProperty()
    {
        return s_EdgeThicknessProperty;
    }
    DependencyProperty NodeGraphPanel::NodeCornerRadiusProperty()
    {
        return s_NodeCornerRadiusProperty;
    }
    DependencyProperty NodeGraphPanel::AutoContrastProperty()
    {
        return s_AutoContrastProperty;
    }
    DependencyProperty NodeGraphPanel::ContrastThresholdProperty()
    {
        return s_ContrastThresholdProperty;
    }

    Microsoft::UI::Xaml::Media::Brush NodeGraphPanel::NodeFill()
    {
        return GetValue(NodeFillProperty()).as<Microsoft::UI::Xaml::Media::Brush>();
    }
    void NodeGraphPanel::NodeFill(Microsoft::UI::Xaml::Media::Brush const& v)
    {
        SetValue(NodeFillProperty(), v);
    }

    Microsoft::UI::Xaml::Media::Brush NodeGraphPanel::NodeStroke()
    {
        return GetValue(NodeStrokeProperty()).as<Microsoft::UI::Xaml::Media::Brush>();
    }
    void NodeGraphPanel::NodeStroke(Microsoft::UI::Xaml::Media::Brush const& v)
    {
        SetValue(NodeStrokeProperty(), v);
    }

    Microsoft::UI::Xaml::Media::Brush NodeGraphPanel::NodeTextBrush()
    {
        return GetValue(NodeTextBrushProperty()).as<Microsoft::UI::Xaml::Media::Brush>();
    }
    void NodeGraphPanel::NodeTextBrush(Microsoft::UI::Xaml::Media::Brush const& v)
    {
        SetValue(NodeTextBrushProperty(), v);
    }

    Microsoft::UI::Xaml::Media::Brush NodeGraphPanel::EdgeBrush()
    {
        return GetValue(EdgeBrushProperty()).as<Microsoft::UI::Xaml::Media::Brush>();
    }
    void NodeGraphPanel::EdgeBrush(Microsoft::UI::Xaml::Media::Brush const& v)
    {
        SetValue(EdgeBrushProperty(), v);
    }

    double NodeGraphPanel::EdgeThickness()
    {
        return winrt::unbox_value_or<double>(GetValue(EdgeThicknessProperty()), 2.5);
    }
    void NodeGraphPanel::EdgeThickness(double v)
    {
        SetValue(EdgeThicknessProperty(), winrt::box_value(v));
    }

    double NodeGraphPanel::NodeCornerRadius()
    {
        return winrt::unbox_value_or<double>(GetValue(NodeCornerRadiusProperty()), 10.0);
    }
    void NodeGraphPanel::NodeCornerRadius(double v)
    {
        SetValue(NodeCornerRadiusProperty(), winrt::box_value(v));
    }

    bool NodeGraphPanel::AutoContrast()
    {
        return winrt::unbox_value_or<bool>(GetValue(AutoContrastProperty()), true);
    }
    void NodeGraphPanel::AutoContrast(bool v)
    {
        SetValue(AutoContrastProperty(), winrt::box_value(v));
    }

    double NodeGraphPanel::ContrastThreshold()
    {
        return winrt::unbox_value_or<double>(GetValue(ContrastThresholdProperty()), 4.5);
    }
    void NodeGraphPanel::ContrastThreshold(double v)
    {
        SetValue(ContrastThresholdProperty(), winrt::box_value(v));
    }

#pragma endregion

    NodeGraphPanel::NodeGraphPanel()
    {
        DefaultStyleKey(box_value(L"XamlUICommand.NodeGraphPanel"));
    }

    void NodeGraphPanel::OnApplyTemplate()
    {
        NodeGraphPanelT<NodeGraphPanel>::OnApplyTemplate();
        m_canvas = GetTemplateChild(L"NodeGraphPanelCanvas").as<Canvas>();
        RebuildAll();
        ConnectCollectionEvents();

        // 主题变化 -> 刷新
        m_themeToken = ActualThemeChanged(
            [weak = get_weak()]
            (
                FrameworkElement const& /* sender */, 
                IInspectable const&     /* args */
            )
            {
                if (auto self = weak.get())
                {
                    self->RedrawEdges();
                    self->RedrawNodes();
                }
            });
    }

    void NodeGraphPanel::Nodes(IObservableVector<XamlUICommand::NodeViewModel> const& value)
    {
        if (m_nodes != value)
        {
            DisconnectCollectionEvents();
            m_nodes = value ? value : single_threaded_observable_vector<XamlUICommand::NodeViewModel>();
            ConnectCollectionEvents();
            RebuildAll();
        }
    }

    void NodeGraphPanel::Edges(IObservableVector<XamlUICommand::EdgeViewModel> const& value)
    {
        if (m_edges != value)
        {
            DisconnectCollectionEvents();
            m_edges = value ? value : single_threaded_observable_vector<XamlUICommand::EdgeViewModel>();
            ConnectCollectionEvents();
            RedrawEdges();
        }
    }

    void NodeGraphPanel::SelectedNode(XamlUICommand::NodeViewModel const& value)
    {
        if (m_selected != value)
        {
            m_selected = value;
            // Update visuals: bold border for selected, etc.
            for (auto& kv : m_nodeElements)
            {
                auto fe = kv.second;
                auto border = fe.as<Controls::Grid>().Children().GetAt(0).as<Shapes::Shape>();
                border.StrokeThickness(kv.first == (m_selected ? m_selected.Id() : INT64_MIN) ? 3.0 : 1.0);
            }
        }
    }

    void NodeGraphPanel::DefaultNodeShape(XamlUICommand::NodeShape const& value)
    {
        if (m_defaultShape != value)
        {
            m_defaultShape = value;
            RedrawNodes();
        }
    }

    void NodeGraphPanel::DefaultTooltipMetaKey(hstring const& v)
    {
        if (m_defaultTipKey != v)
        {
            m_defaultTipKey = v;
            RedrawNodes();
        }
    }

    void NodeGraphPanel::DefaultLabelMetaKey(hstring const& v)
    {
        if (m_defaultLabelKey != v)
        {
            m_defaultLabelKey = v;
            RedrawNodes();
        }
    }

    XamlUICommand::NodeViewModel NodeGraphPanel::AddNode(int64_t id, hstring const& label, Point const& position)
    {
        auto node = winrt::make<XamlUICommand::implementation::NodeViewModel>();
        node.Id(id);
        node.Label(label);
        node.Position(position);
        node.Shape(m_defaultShape);
        if (!m_defaultLabelKey.empty()) node.LabelMetaKey(m_defaultLabelKey);
        if (!m_defaultTipKey.empty()) node.TooltipMetaKey(m_defaultTipKey);
        m_nodes.Append(node);
        return node;
    }

    void NodeGraphPanel::RemoveNode(int64_t id)
    {
        for (uint32_t i = 0; i < m_nodes.Size(); ++i)
        {
            if (m_nodes.GetAt(i).Id() == id)
            {
                m_nodes.RemoveAt(i);
                RemoveNodeElement(id);
                break;
            }
        }
        // Remove edges referencing this node
        for (int32_t i = (int32_t)m_edges.Size() - 1; i >= 0; --i)
        {
            auto e = m_edges.GetAt(i);
            if (e.FromId() == id || e.ToId() == id)
            {
                m_edges.RemoveAt(i);
            }
        }
        RedrawEdges();
    }

    XamlUICommand::NodeViewModel NodeGraphPanel::GetNode(int64_t id)
    {
        for (auto const& n : m_nodes) if (n.Id() == id) return n;
        return nullptr;
    }

    void NodeGraphPanel::AddOrUpdateMeta(int64_t id, hstring const& key, IInspectable const& value)
    {
        auto node = GetNode(id);
        if (node)
        {
            node.Meta().Insert(key, value);
            UpdateNodeElement(node);
        }
    }

    void NodeGraphPanel::RemoveMeta(int64_t id, hstring const& key)
    {
        auto node = GetNode(id);
        if (node && node.Meta().HasKey(key))
        {
            node.Meta().Remove(key);
            UpdateNodeElement(node);
        }
    }

    XamlUICommand::EdgeViewModel NodeGraphPanel::AddEdge(int64_t fromId, int64_t toId, hstring const& label)
    {
        auto edge = winrt::make<XamlUICommand::implementation::EdgeViewModel>();
        edge.FromId(fromId);
        edge.ToId(toId);
        edge.Label(label);
        m_edges.Append(edge);
        RedrawEdges();
        return edge;
    }

    void NodeGraphPanel::RemoveEdge(int64_t fromId, int64_t toId)
    {
        for (int32_t i = (int32_t)m_edges.Size() - 1; i >= 0; --i)
        {
            auto e = m_edges.GetAt(i);
            if (e.FromId() == fromId && e.ToId() == toId)
            {
                m_edges.RemoveAt(i);
            }
        }
        RedrawEdges();
    }

    void NodeGraphPanel::ConnectCollectionEvents()
    {
        if (m_nodes)
        {
            m_nodesToken = m_nodes.VectorChanged(
                [this](Windows::Foundation::Collections::IObservableVector<XamlUICommand::NodeViewModel> const&,
                    Windows::Foundation::Collections::IVectorChangedEventArgs const& args)
                {
                    switch (args.CollectionChange())
                    {
                    case Windows::Foundation::Collections::CollectionChange::ItemInserted:
                    case Windows::Foundation::Collections::CollectionChange::ItemChanged:
                    case Windows::Foundation::Collections::CollectionChange::ItemRemoved:
                    case Windows::Foundation::Collections::CollectionChange::Reset:
                        RedrawNodes();
                        RedrawEdges();
                        break;
                    }
                });
        }

        if (m_edges)
        {
            m_edgesToken = m_edges.VectorChanged(
                [this](Windows::Foundation::Collections::IObservableVector<XamlUICommand::EdgeViewModel> const&,
                    Windows::Foundation::Collections::IVectorChangedEventArgs const&)
                {
                    RedrawEdges();
                });
        }
    }

    void NodeGraphPanel::DisconnectCollectionEvents()
    {
        if (m_nodes && m_nodesToken.value) m_nodes.VectorChanged(m_nodesToken);
        if (m_edges && m_edgesToken.value) m_edges.VectorChanged(m_edgesToken);
    }

    NodeGraphPanel::InnerAppearance NodeGraphPanel::ComputeAppearance()
    {
        bool isDark = (ActualTheme() == Microsoft::UI::Xaml::ElementTheme::Dark);

        auto defFill = SolidColorBrush{ isDark ? Windows::UI::Colors::DimGray() : Windows::UI::Colors::White() };
        auto defStroke = SolidColorBrush{ isDark ? Windows::UI::Colors::LightSteelBlue() : Windows::UI::Colors::SteelBlue() };
        auto defText = SolidColorBrush{ isDark ? Windows::UI::Colors::White() : Windows::UI::Colors::Black() };

        NodeGraphPanel::InnerAppearance a{
            NodeFill() ? NodeFill() : defFill,
            NodeStroke() ? NodeStroke() : defStroke,
            NodeTextBrush() ? NodeTextBrush() : defText,
            NodeCornerRadius()
        };

        // 自动对比度（与你 AttachNodeElement 里保持一致）
        if (AutoContrast())
        {
            Windows::UI::Color cf{}, cs{}, ct{};
            if (TryGetSolid(a.fill, cf))
            {
                if (!TryGetSolid(a.text, ct) || Contrast(cf, ct) < ContrastThreshold())
                {
                    Windows::UI::Color black{ 255,0,0,0 }, white{ 255,255,255,255 };
                    a.text = SolidColorBrush{ (Contrast(cf, black) >= Contrast(cf, white)) ? black : white };
                }
                if (TryGetSolid(a.stroke, cs) && Contrast(cf, cs) < 2.5)
                {
                    bool fillIsLight = Luminance(cf) > 0.5;
                    auto adj = MakeLighterDarker(cf, fillIsLight ? -0.35 : +0.35);
                    a.stroke = SolidColorBrush{ adj };
                }
            }
        }
        return a;
    }

    void NodeGraphPanel::RebuildAll()
    {
        if (!m_canvas) return;
        m_canvas.Children().Clear();
        m_nodeElements.clear();
        // Edges first so they render behind
        RedrawEdges();
        RedrawNodes();
    }

    void NodeGraphPanel::RedrawEdges()
    {
        if (!m_canvas) return;
        // Remove existing lines (tagged as L"Edge")
        for (int32_t i = (int32_t)m_canvas.Children().Size() - 1; i >= 0; --i)
        {
            if (auto fe = m_canvas.Children().GetAt(i).try_as<FrameworkElement>())
            {
                if (fe.Tag() && unbox_value<hstring>(fe.Tag()) == L"Edge")
                    m_canvas.Children().RemoveAt(i);
            }
        }

        for (auto const& e : m_edges)
        {
            auto from = GetNode(e.FromId());
            auto to = GetNode(e.ToId());
            if (!from || !to) continue;

            // Draw straight line (center-to-center)
            auto line = Shapes::Line{};
            auto eb = EdgeBrush();
            line.Stroke(eb ? eb : SolidColorBrush{ Windows::UI::Colors::Gray() });
            line.StrokeThickness(EdgeThickness());         // 默认线条粗细为 2.5
            line.X1(from.Position().X + from.Size().Width / 2.0);
            line.Y1(from.Position().Y + from.Size().Height / 2.0);
            line.X2(to.Position().X + to.Size().Width / 2.0);
            line.Y2(to.Position().Y + to.Size().Height / 2.0);
            line.Tag(box_value(L"Edge"));
            Canvas::SetZIndex(line, 0);
            m_canvas.Children().Append(line);
        }
    }

    hstring NodeGraphPanel::ResolveLabel(XamlUICommand::NodeViewModel const& node) const
    {
        hstring label = node.Label();
        auto key = node.LabelMetaKey();
        if (key.empty()) key = m_defaultLabelKey;
        if (!key.empty() && node.Meta() && node.Meta().HasKey(key))
        {
            auto v = node.Meta().Lookup(key);
            try
            {
                label = unbox_value<hstring>(v);
            }
            catch (...) { /* Non-string values will be ToString() */
                label = hstring{};
            }
        }
        return label;
    }

    hstring NodeGraphPanel::ResolveTooltip(XamlUICommand::NodeViewModel const& node) const
    {
        auto key = node.TooltipMetaKey();
        if (key.empty()) key = m_defaultTipKey;
        if (!key.empty() && node.Meta() && node.Meta().HasKey(key))
        {
            auto v = node.Meta().Lookup(key);
            return v ? unbox_value<hstring>(v) : hstring{};
        }
        return node.Label();
    }

    void NodeGraphPanel::AttachNodeElement(XamlUICommand::NodeViewModel const& node)
    {
        auto grid = Controls::Grid{};
        grid.Tag(box_value(L"Node"));
        grid.Width(node.Size().Width);
        grid.Height(node.Size().Height);

        auto ap = ComputeAppearance();

        Shapes::Shape shape{ nullptr };
        if (node.Shape() == XamlUICommand::NodeShape::Circle)
        {
            auto ellipse = Shapes::Ellipse{};
            ellipse.Fill(ap.fill);
            ellipse.Stroke(ap.stroke);
            ellipse.StrokeThickness(node.IsSelected() ? 3.0 : 1.0);
            shape = ellipse;
        }
        else if(node.Shape() == XamlUICommand::NodeShape::RoundedRect)
        {
            auto rect = Shapes::Rectangle{};
            rect.RadiusX(ap.cornerRadius);
            rect.RadiusY(ap.cornerRadius);
            rect.Fill(ap.fill);
            rect.Stroke(ap.stroke);
            rect.StrokeThickness(node.IsSelected() ? 3.0 : 1.0);
            shape = rect;
        }
        else {
            // TODO... for other shapes
            return;
        }
        grid.Children().Append(shape);

        auto text = Controls::TextBlock{};
        text.Text(ResolveLabel(node));
        text.HorizontalAlignment(HorizontalAlignment::Center);
        text.VerticalAlignment(VerticalAlignment::Center);
        text.TextTrimming(TextTrimming::CharacterEllipsis);
        text.Margin(Thickness{ 6 });
        text.Foreground(ap.text);                  // 使用对比度适应算法调整后的前景
        grid.Children().Append(text);

        // Tooltip (on hover)
        Controls::ToolTipService::SetToolTip(grid, box_value(ResolveTooltip(node)));

        // Input: click to select + flyout
        grid.Tapped([this, node, grid](IInspectable const&, TappedRoutedEventArgs const&)
            {
                SelectedNode(node);

                XamlUICommand::NodeInvokedEventArgs args{};
                args.Node(node);
                m_NodeInvoked(*this, args); // 触发事件（注意：用私有字段 m_NodeInvoked）

                auto fly = CreateDetailsFlyout(node);
                fly.ShowAt(grid);
            });

        // Position on canvas
        Canvas::SetLeft(grid, node.Position().X);
        Canvas::SetTop(grid, node.Position().Y);
        Canvas::SetZIndex(grid, 1);
        m_canvas.Children().Append(grid);

        // m_nodeElements[node.Id()] = grid; 
        // std::unordered_map::operator[] 的坑：operator[] 在 键不存在 时会使用默认构造。
        // 映射值类型是 Microsoft::UI::Xaml::FrameworkElement，在当前编译单元里它不是可默认构造的，
        // 于是会触发模板实例化错误。采用带值插入将避免此问题。
        m_nodeElements.insert_or_assign(node.Id(), grid.as<Microsoft::UI::Xaml::FrameworkElement>());  

        // Listen for VM changes to live-update visuals
        auto weak = get_weak();
        node.PropertyChanged([weak, id = node.Id()](IInspectable const& sender, Microsoft::UI::Xaml::Data::PropertyChangedEventArgs const& e)
            {
                if (auto self = weak.get())
                {
                    auto vm = sender.as<XamlUICommand::NodeViewModel>();
                    self->UpdateNodeElement(vm);
                    if (e.PropertyName() == L"Position" || e.PropertyName() == L"Size")
                    {
                        self->RedrawEdges();
                    }
                }
            });
    }

    void NodeGraphPanel::UpdateNodeElement(XamlUICommand::NodeViewModel const& node)
    {
        auto it = m_nodeElements.find(node.Id());
        if (it == m_nodeElements.end()) return;

        auto grid = it->second.as<Controls::Grid>();
        if (!grid) return;

        // 尺寸、位置
        grid.Width(node.Size().Width);
        grid.Height(node.Size().Height);
        Canvas::SetLeft(grid, node.Position().X);
        Canvas::SetTop(grid, node.Position().Y);

        // 形状、文本
        auto shape = grid.Children().GetAt(0).as<Shapes::Shape>();
        auto text = grid.Children().GetAt(1).as<Controls::TextBlock>();

        // 选中态
        shape.StrokeThickness(node.IsSelected() ? 3.0 : 1.0);

        // 如果形状类型变了，则重建元素
        if ((node.Shape() == NodeShape::Circle && !shape.try_as<Shapes::Ellipse>()) ||
            (node.Shape() == NodeShape::RoundedRect && !shape.try_as<Shapes::Rectangle>()))
        {
            uint32_t index = 0;
            m_canvas.Children().IndexOf(grid, index);
            m_canvas.Children().RemoveAt(index);
            m_nodeElements.erase(it);
            AttachNodeElement(node);
            return;
        }

        // 增量更新外观
        auto ap = ComputeAppearance();

        if (auto rect = shape.try_as<Shapes::Rectangle>())
        {
            rect.RadiusX(ap.cornerRadius);
            rect.RadiusY(ap.cornerRadius);
        }
        shape.Fill(ap.fill);
        shape.Stroke(ap.stroke);
        text.Foreground(ap.text);

        // 文本/Tooltip
        text.Text(ResolveLabel(node));
        Controls::ToolTipService::SetToolTip(grid, box_value(ResolveTooltip(node)));
    }

    void NodeGraphPanel::RemoveNodeElement(int64_t id)
    {
        auto it = m_nodeElements.find(id);
        if (it == m_nodeElements.end()) return;
        auto fe = it->second;
        // Remove child from canvas
        for (uint32_t i = 0; i < m_canvas.Children().Size(); ++i)
        {
            if (m_canvas.Children().GetAt(i) == fe)
            {
                m_canvas.Children().RemoveAt(i);
                break;
            }
        }
        m_nodeElements.erase(it);
    }

    void NodeGraphPanel::RedrawNodes()
    {
        if (!m_canvas) return;
        // Remove all existing node visuals
        for (int32_t i = (int32_t)m_canvas.Children().Size() - 1; i >= 0; --i)
        {
            if (auto fe = m_canvas.Children().GetAt(i).try_as<FrameworkElement>())
            {
                if (fe.Tag() && unbox_value<hstring>(fe.Tag()) == L"Node")
                {
                    m_canvas.Children().RemoveAt(i);
                }
            }
        }
        m_nodeElements.clear();

        for (auto const& n : m_nodes)
        {
            // Default size if unset
            auto size = n.Size();
            if (size.Width == 0 || size.Height == 0)
            {
                n.Size(Size{ 64, 64 });
            }
            // Default shape if unset
            if (n.Shape() != XamlUICommand::NodeShape::Circle && n.Shape() != XamlUICommand::NodeShape::RoundedRect)
            {
                n.Shape(m_defaultShape);
            }
            AttachNodeElement(n);
        }
    }

    static void SetupAcrylicPresenter(Flyout const& fly, double corner = 12.0)
    {
        Style s{};
        s.TargetType(xaml_typename<FlyoutPresenter>());
        CornerRadius cr{ corner, corner, corner, corner };
        s.Setters().Append(Setter(Control::CornerRadiusProperty(), box_value(cr)));
        s.Setters().Append(Setter(Control::BackgroundProperty(), SolidColorBrush{ Windows::UI::Colors::Transparent() }));
        s.Setters().Append(Setter(Controls::FlyoutPresenter::IsDefaultShadowEnabledProperty(), box_value(true)));
        fly.FlyoutPresenterStyle(s);
        fly.ShouldConstrainToRootBounds(false);
    }

    Controls::Flyout NodeGraphPanel::CreateDetailsFlyout(XamlUICommand::NodeViewModel const& node)
    {
        Controls::Flyout fly{};

        auto border = Controls::Border{};
        border.Name(L"NodeDetailsFlyoutBorder");
        border.BorderBrush(SolidColorBrush{ Windows::UI::Colors::Transparent() });
        border.BorderThickness(Thickness{ 0.5 });
        border.Padding({});

        auto panel = Controls::StackPanel{};
        panel.Name(L"FlyoutContentStackPanel");
        panel.Padding(Thickness{ 8 });
        panel.Spacing(6);
        panel.Background(SolidColorBrush{ Windows::UI::Colors::Transparent() });

        auto title = Controls::TextBlock{};
        title.Name(L"FlyoutContentTitle");
        title.Text(std::format(L"节点 {}", node.Id()));
        title.FontSize(16);
        title.FontWeight(Windows::UI::Text::FontWeights::SemiBold());
        panel.Children().Append(title);

        // label
        auto lbl = Controls::TextBlock{};
        lbl.Name(L"FlyoutContentLabel");
        lbl.Text(ResolveLabel(node));
        lbl.TextWrapping(TextWrapping::Wrap);
        panel.Children().Append(lbl);

        // meta list
        if (node.Meta() && node.Meta().Size() > 0)
        {
            auto grid = Controls::Grid{};
            auto row = RowDefinition{};
            grid.RowDefinitions().Append(row);

            auto colAuto = ColumnDefinition{};
            colAuto.Width(GridLengthHelper::Auto());
            grid.ColumnDefinitions().Append(colAuto);

            auto colStar = ColumnDefinition{};
            colStar.Width(GridLengthHelper::FromValueAndType(1.0, GridUnitType::Star));
            grid.ColumnDefinitions().Append(colStar);

            for (auto const& kvp : node.Meta())
            {
                uint32_t r = grid.RowDefinitions().Size();
                grid.RowDefinitions().Append(RowDefinition{});

                auto k = Controls::TextBlock{}; k.Text(kvp.Key()); k.Margin(Thickness{ 0,2,8,2 });
                auto v = Controls::TextBlock{}; v.Text(kvp.Value() ? unbox_value<hstring>(kvp.Value()) : L"<null>"); v.Margin(Thickness{ 0,2,0,2 });

                Grid::SetRow(k, r); Grid::SetColumn(k, 0);
                Grid::SetRow(v, r); Grid::SetColumn(v, 1);
                grid.Children().Append(k);
                grid.Children().Append(v);
            }
            grid.Background(SolidColorBrush{ Windows::UI::Colors::Transparent() });
            panel.Children().Append(grid);
        }

        panel.Background(SolidColorBrush{ Windows::UI::Colors::Transparent() });
        border.Child(panel);
        fly.Content(border);

        // FIXME: 系统背景材质可能存在问题，在浅色模式下几乎无效果
        fly.SystemBackdrop(Microsoft::UI::Xaml::Media::DesktopAcrylicBackdrop());
        SetupAcrylicPresenter(fly, 12.0);
        return fly;
    }

    void NodeGraphPanel::OnAppearanceChanged(DependencyObject const& d,
        DependencyPropertyChangedEventArgs const& e)
    {
        if (auto self = d.try_as<NodeGraphPanel>())
        {
            auto p = e.Property();

            // 边线外观
            if (p == s_EdgeBrushProperty || p == s_EdgeThicknessProperty)
            {
                self->RedrawEdges();
            }

            // 节点外观
            if (p == s_NodeFillProperty || p == s_NodeStrokeProperty ||
                p == s_NodeTextBrushProperty || p == s_NodeCornerRadiusProperty ||
                p == s_AutoContrastProperty || p == s_ContrastThresholdProperty)
            {
                // 增量更新样式
                for (auto const& kv : self->m_nodeElements)
                {
                    if (auto node = self->GetNode(kv.first))
                    {
                        self->UpdateNodeElement(node);
                    }
                }
            }
        }
    }

}