#include "pch.h"
#include "HomePage.xaml.h"
#if __has_include("HomePage.g.cpp")
#include "HomePage.g.cpp"
#endif
#include "NodeGraphPage.xaml.h"

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::XamlUICommand::implementation
{
    void HomePage::OpenGraph_Click(IInspectable const&, RoutedEventArgs const&)
    {
        if (auto f = Frame()) // Page вт╢Ь Frame()
        {
            f.Navigate(xaml_typename<XamlUICommand::NodeGraphPage>());
        }
    }
}
