#include "pch.h"
#include "MainWindow.xaml.h"
#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"
#endif

#include "HomePage.xaml.h"
#include "NodeGraphPage.xaml.h"

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml::Controls;

namespace winrt::XamlUICommand::implementation
{
    MainWindow::MainWindow()
    {
        InitializeComponent();

        // 默认进到 Home
        if (auto items = Nav().MenuItems(); items && items.Size() > 0)
        {
            Nav().SelectedItem(items.GetAt(0));
            NavigateTo(L"home");
        }
    }

    void MainWindow::Nav_SelectionChanged(NavigationView const&, NavigationViewSelectionChangedEventArgs const& args)
    {
        if (auto item = args.SelectedItem().try_as<NavigationViewItem>())
        {
            auto tag = unbox_value_or<hstring>(item.Tag(), L"");
            NavigateTo(tag);
        }
    }

    void MainWindow::NavigateTo(hstring const& tag)
    {
        if (tag == L"home")
        {
            RootFrame().Navigate(xaml_typename<XamlUICommand::HomePage>());
        }
        else if (tag == L"graph")
        {
            RootFrame().Navigate(xaml_typename<XamlUICommand::NodeGraphPage>());
        }
    }
}
