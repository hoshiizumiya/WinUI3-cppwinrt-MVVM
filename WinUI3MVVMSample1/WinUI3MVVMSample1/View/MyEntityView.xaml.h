#pragma once

#include "ViewModels/MyEntityViewModel.h"
#include "View/MyEntityView.g.h"

namespace winrt::WinUI3MVVMSample1::View::implementation
{
    struct MyEntityView
        : MyEntityViewT<MyEntityView>
        , ::mvvm::view<MyEntityView, winrt::WinUI3MVVMSample1::implementation::MyEntityViewModel::class_type>
    {
        MyEntityView();
        MyEntityView(winrt::WinUI3MVVMSample1::implementation::MyEntityViewModel::class_type const& viewModel);

        /*
            This is the getter method for getting the view-model.
            REMEMBER to use the one from "<Your App Name> namespace" instead of "<Your App Name>::implementation namespace"!
            The one in the implementation namespace is your implementation written in your view-model header file and cpp file and they are NOT complete! (has virtual functions... etc)
            This one is the class generated from the view-model idl file, and has only methods declared in the idl file and is the correct one to use.
            Same as the view-model private member.
        */
        WinUI3MVVMSample1::MyEntityViewModel MainViewModel();

        /*void MyEntityView::OnUnloaded(
            winrt::Windows::Foundation::IInspectable const& sender, 
            winrt::Microsoft::UI::Xaml::RoutedEventArgs const& args
        )
        {
        }*/

    private:
        using view_base = typename ::mvvm::view<MyEntityView, winrt::WinUI3MVVMSample1::implementation::MyEntityViewModel::class_type>;
    };
}

namespace winrt::WinUI3MVVMSample1::View::factory_implementation
{
    struct MyEntityView
        : MyEntityViewT<MyEntityView, implementation::MyEntityView>
    {
    };
}
