#include "pch.h"
#include "FlyoutAcrylicBackdropHelper.h"
#if __has_include("FlyoutAcrylicBackdropHelper.g.cpp")
#include "FlyoutAcrylicBackdropHelper.g.cpp"
#endif

#include "Helpers/VisualTreeHelper.hpp"
#include "AcrylicVisual.h"

using namespace winrt;
using namespace Microsoft::UI::Xaml;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::XamlUICommand::implementation
{
	winrt::Microsoft::UI::Xaml::DependencyProperty FlyoutAcrylicBackdropHelper::s_hostBackdropAcrylicProperty =
		winrt::Microsoft::UI::Xaml::DependencyProperty::RegisterAttached(
			L"HostBackdropAcrylic",
			winrt::xaml_typename<bool>(),
			winrt::xaml_typename<class_type>(),
			winrt::Microsoft::UI::Xaml::PropertyMetadata{
				nullptr,
				&FlyoutAcrylicBackdropHelper::HostBackdropAcrylicChanged
			}
		);

	winrt::Microsoft::UI::Xaml::DependencyProperty FlyoutAcrylicBackdropHelper::HostBackdropAcrylicProperty()
	{
		return s_hostBackdropAcrylicProperty;
	}

	bool FlyoutAcrylicBackdropHelper::GetHostBackdropAcrylicState(winrt::Microsoft::UI::Xaml::Controls::Flyout const& flyout)
	{
		return winrt::unbox_value<bool>(flyout.GetValue(HostBackdropAcrylicProperty()));
	}

	void FlyoutAcrylicBackdropHelper::SetHostBackdropAcrylicState(
		winrt::Microsoft::UI::Xaml::Controls::Flyout const& flyout,
		bool value
	)
	{
		flyout.SetValue(HostBackdropAcrylicProperty(), winrt::box_value(value));
	}

	static void adjustVisual(winrt::XamlUICommand::AcrylicVisual const& visual, winrt::Microsoft::UI::Xaml::CornerRadius cornerRadius)
	{
		visual.CornerRadius(cornerRadius);
		//OutputDebugString(std::format(L"CornerRadius: {}, {}, {}, {}\n", cornerRadius.TopLeft, cornerRadius.TopRight, cornerRadius.BottomRight, cornerRadius.BottomLeft).data());
	}

	void FlyoutAcrylicBackdropHelper::HostBackdropAcrylicChanged(
		winrt::Microsoft::UI::Xaml::DependencyObject const& object,
		winrt::Microsoft::UI::Xaml::DependencyPropertyChangedEventArgs const& arg)
	{
		auto const acrylicWorkaround = winrt::unbox_value<bool>(arg.NewValue());
		if (!acrylicWorkaround)
			return;

		/*auto flyout = object.as<winrt::Microsoft::UI::Xaml::Controls::Flyout>();
		flyout.LayoutUpdated([flyoutRef = winrt::make_weak(flyout), called = false](auto&&...) mutable
			{
				if (called) return;

				auto popup = VisualTreeHelper::FindVisualChildByName<winrt::Microsoft::UI::Xaml::Controls::Primitives::Popup>(
					flyoutRef.get().as<winrt::Microsoft::UI::Xaml::Controls::Flyout>(),
					L"SuggestionsPopup"
				);
				if (!popup) return;

				called = true;
				auto border = popup.FindName(L"SuggestionsContainer").as<winrt::Microsoft::UI::Xaml::Controls::Border>();
				border.Padding({});
				border.Background(winrt::Microsoft::UI::Xaml::Media::SolidColorBrush{ winrt::Windows::UI::Colors::Transparent() });
				border.SizeChanged([called = false, popup = winrt::make_weak(popup), visualRef = winrt::weak_ref<winrt::XamlUICommand::AcrylicVisual>{}](auto const& borderRef, winrt::Microsoft::UI::Xaml::SizeChangedEventArgs const& args) mutable
					{
						if (called)
						{
							if (auto visual = visualRef.get())
								adjustVisual(visual, borderRef.as<winrt::Microsoft::UI::Xaml::Controls::Border>().CornerRadius());
							return;
						}

						auto border = borderRef.as<winrt::Microsoft::UI::Xaml::Controls::Border>();
						if (auto const newSize = args.NewSize(); newSize.Width == 0 || newSize.Height == 0)
							return;
						called = true;

						auto originalChild = border.Child();
						winrt::Microsoft::UI::Xaml::Controls::Grid scrollGrid;
						border.Child(scrollGrid);

						winrt::WinUI3Package::AcrylicVisual visual;
						adjustVisual(visual, border.CornerRadius());
						visualRef = visual;
						scrollGrid.Children().ReplaceAll({ visual, originalChild });
					});
			});*/

	}
}
