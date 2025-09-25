#include "pch.h"
#include "NodeGraphPage.xaml.h"
#if __has_include("NodeGraphPage.g.cpp")
#include "NodeGraphPage.g.cpp"
#endif

#include "Controls/NodeGraphPanel.h"
#include "Controls/NodeViewModel.h"
#include "Controls/EdgeViewModel.h"
#include "Controls/NodeInvokedEventArgs.h"

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml::Controls;
using namespace Microsoft::UI::Xaml::Input;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace winrt::Windows::Foundation::Numerics;

namespace winrt::XamlUICommand::implementation
{
    NodeGraphPage::NodeGraphPage()
    {
        InitializeComponent();
        BuildSample();
    }

    void NodeGraphPage::BuildSample()
    {
        auto nodes = single_threaded_observable_vector<XamlUICommand::NodeViewModel>();
        auto edges = single_threaded_observable_vector<XamlUICommand::EdgeViewModel>();
        Graph().Nodes(nodes);
        Graph().Edges(edges);

        Graph().DefaultLabelMetaKey(L"Name");
        Graph().DefaultTooltipMetaKey(L"Desc");

        auto n1 = Graph().AddNode(1, L"A", Point{ 100,100 });
        n1.Size(Size{ 64,64 });
        n1.Meta().Insert(L"Name", box_value(L"Alpha"));
        n1.Meta().Insert(L"Desc", box_value(L"First node"));

        auto n2 = Graph().AddNode(2, L"B", Point{ 320,160 });
        n2.Size(Size{ 64,64 });
        n2.Meta().Insert(L"Name", box_value(L"Beta"));
        n2.Meta().Insert(L"Desc", box_value(L"Second node"));

        auto n3 = Graph().AddNode(3, L"C", Point{ 200,260 });
        n3.Shape(XamlUICommand::NodeShape::RoundedRect);
        n3.Size(Size{ 120,64 });
        n3.Meta().Insert(L"Name", box_value(L"Gamma (rect)"));
        n3.Meta().Insert(L"Desc", box_value(L"A longer description here"));

        Graph().AddEdge(1, 2, L"e12");
        Graph().AddEdge(2, 3, L"e23");
        Graph().AddEdge(1, 3, L"e13");

        Graph().NodeInvoked([this](IInspectable const&, XamlUICommand::NodeInvokedEventArgs const& e)
            {
                auto node = e.Node();
                node.IsSelected(true);
            });

        // ��ʼ�����ӿ�
        OnResetViewClick(nullptr, nullptr);
    }

    // ���� ������������ͼ��������������ʾ��Ƭ�� ����
    void NodeGraphPage::OnResetViewClick(IInspectable const&, Microsoft::UI::Xaml::RoutedEventArgs const&)
    {
        double vw = Zoomer().ViewportWidth();
        double vh = Zoomer().ViewportHeight();
        double cw = Graph().Width();   // �� ActualWidth()
        double ch = Graph().Height();
        if (vw <= 0 || vh <= 0 || cw <= 0 || ch <= 0) return;

        double target = std::min(vw / cw, vh / ch);
        target = std::clamp(target, Zoomer().MinZoomFactor(), Zoomer().MaxZoomFactor());

        // ��ָ�����ĵ� �� ���� IReference<float2>
        winrt::Windows::Foundation::IReference<float2> center = nullptr;

        using namespace Microsoft::UI::Xaml::Controls;
        ScrollingZoomOptions opt{ ScrollingAnimationMode::Enabled, ScrollingSnapPointsMode::Ignore };

        Zoomer().ZoomTo(target, center, opt);  // ���� correlationId���������¼�ƥ��
    }

    // ���� ������˫���л� 1x/2x������Ϊ˫���� ����
    void NodeGraphPage::OnGraphDoubleTapped(IInspectable const&, Microsoft::UI::Xaml::Input::DoubleTappedRoutedEventArgs const& e)
    {
        float target = (Zoomer().ZoomFactor() < 1.5f) ? 2.0f : 1.0f;

        auto p = e.GetPosition(Graph()); // �������Ԫ�ص�λ��
        auto cp = winrt::box_value(float2{ (float)p.X, (float)p.Y })
            .as<winrt::Windows::Foundation::IReference<float2>>(); // IReference<float2>
        using namespace Microsoft::UI::Xaml::Controls;
        ScrollingZoomOptions opt{ ScrollingAnimationMode::Enabled, ScrollingSnapPointsMode::Ignore };

        Zoomer().ZoomTo(target, cp, opt);
    }

    // ���� ʾ��������Ŷ����֣�������ʾ���ӱ仯�� ����
    void NodeGraphPage::OnRandomizeClick(IInspectable const&, RoutedEventArgs const&)
    {
        // ������������нڵ�λ�ô�ɢ
        std::mt19937 rng{ std::random_device{}() };
        std::uniform_real_distribution<float> dx(60.f, (float)Graph().Width() - 120.f);
        std::uniform_real_distribution<float> dy(60.f, (float)Graph().Height() - 120.f);

        for (auto const& node : Graph().Nodes())
        {
            node.Position(Point{ dx(rng), dy(rng) });
        }
    }
}