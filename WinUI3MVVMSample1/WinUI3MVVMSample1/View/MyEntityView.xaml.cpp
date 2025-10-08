#include "pch.h"
#include "View/MyEntityView.xaml.h"

#include "ViewModels/Locator.h"

#if __has_include("View/MyEntityView.g.cpp")
#include "View/MyEntityView.g.cpp"
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::WinUI3MVVMSample1::View::implementation
{
    MyEntityView::MyEntityView()
        //: MyEntityView(winrt::WinUI3MVVMSample1::implementation::MyEntityViewModel::class_type{})
        : MyEntityView(WinUI3MVVMSample1::ViewModels::Locator::MyEntity())
    {
    }

    MyEntityView::MyEntityView(winrt::WinUI3MVVMSample1::implementation::MyEntityViewModel::class_type const& viewModel)
        : view_base(viewModel)
    {
        InitializeComponent();

        auto vm = MainViewModel();

        auto vm_ptr = winrt::get_self<winrt::WinUI3MVVMSample1::implementation::MyEntityViewModel>(vm);
        vm_ptr->SetDispatcherQueue(winrt::Microsoft::UI::Dispatching::DispatcherQueue::GetForCurrentThread());
    }

    void MyEntityView::MyEntityViewClickUnBind(
        winrt::Windows::Foundation::IInspectable const& /*sender*/, 
        winrt::Microsoft::UI::Xaml::RoutedEventArgs const& /*args*/)
    {
        WinUI3MVVMSample1::ViewModels::Locator::ResetViewModel(MainViewModel());
    }

    WinUI3MVVMSample1::MyEntityViewModel implementation::MyEntityView::MainViewModel()
    {
        return view_base::m_viewModel;
    }
}
