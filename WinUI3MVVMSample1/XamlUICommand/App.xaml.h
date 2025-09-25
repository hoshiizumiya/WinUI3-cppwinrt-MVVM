#pragma once

#include "App.xaml.g.h"
#include "ColorConverter.h"
#include "Controls\AcrylicVisual.h"
#include "Controls\FlyoutAcrylicBackdropHelper.h"
//#include "Controls\NodeGraphPanel.h"
//#include "Controls\NodeGraphNodeControl.h"

namespace winrt::XamlUICommand::implementation
{
    struct App : AppT<App>
    {
        App();

        void OnLaunched(Microsoft::UI::Xaml::LaunchActivatedEventArgs const&);

    private:
        winrt::Microsoft::UI::Xaml::Window window{ nullptr };
    };
}
