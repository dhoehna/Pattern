// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE in the project root for license information.

#include "pch.h"
#include "MainWindow.xaml.h"
#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"
#endif
#include <winrt/Windows.UI.h>
#include <list>
#include <Tree.h>
#include <map>
#include <winrt/windows.ui.xaml.input.h>
#include <winrt/Microsoft.UI.Xaml.Input.h>

using namespace winrt;
using namespace Microsoft::UI::Xaml;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

			/* winrt::Microsoft::UI::Xaml::Shapes::Line horizontalPlus{};
			 horizontalPlus.Stroke(myBrush);
			 horizontalPlus.X1(dot.x - plusLength);
			 horizontalPlus.X2(dot.x + plusLength);
			 horizontalPlus.Y1(dot.y);
			 horizontalPlus.Y2(dot.y);
			 MyStackPanel().Children().Append(horizontalPlus);

			 winrt::Microsoft::UI::Xaml::Shapes::Line verticalPlus{};
			 verticalPlus.Stroke(myBrush);
			 verticalPlus.X1(dot.x);
			 verticalPlus.X2(dot.x);
			 verticalPlus.Y1(dot.y + plusLength);
			 verticalPlus.Y2(dot.y - plusLength);
			 MyStackPanel().Children().Append(verticalPlus);*/
			 //}

namespace winrt::TheRealTree::implementation
{

	std::map<int, std::list<Dot>> roundsToDots{};
	int currentRound{ 1 };
	int numberOfRounds{ 30 };


	void MainWindow::DrawCurrentRound()
	{
		winrt::Microsoft::UI::Xaml::Media::SolidColorBrush myBrush(winrt::Windows::UI::ColorHelper::FromArgb(255, 0, 0, 0));
		auto dotsToDraw{ roundsToDots[currentRound] };
		for (auto dot : dotsToDraw)
		{
			if (dot.previous.get() != nullptr)
			{
				winrt::Microsoft::UI::Xaml::Shapes::Line myLine{};
				myLine.Stroke(myBrush);
				myLine.X1(dot.x);
				myLine.X2(dot.previous->x);
				myLine.Y1(dot.y);
				myLine.Y2(dot.previous->y);
				MyStackPanel().Children().Append(myLine);
			}
		}
	}

	void MainWindow::DrawCurrentRoundAsTransparent()
	{
		winrt::Microsoft::UI::Xaml::Media::SolidColorBrush myBrush(winrt::Windows::UI::ColorHelper::FromArgb(100, 0, 0, 0));
		auto dotsToDraw{ roundsToDots[currentRound] };
		for (auto dot : dotsToDraw)
		{
			if (dot.previous.get() != nullptr)
			{
				winrt::Microsoft::UI::Xaml::Shapes::Line myLine{};
				myLine.Stroke(myBrush);
				myLine.X1(dot.x);
				myLine.X2(dot.previous->x);
				myLine.Y1(dot.y);
				myLine.Y2(dot.previous->y);
				MyStackPanel().Children().Append(myLine);
			}
		}
	}

	void MainWindow::RemoveCurrentRound()
	{
		winrt::Microsoft::UI::Xaml::Media::SolidColorBrush myBrush(winrt::Windows::UI::ColorHelper::FromArgb(255, 0, 0, 0));

		size_t numberOfLinesToRemove{ roundsToDots[currentRound].size() };

		for (size_t counter = 0; counter < numberOfLinesToRemove; counter++)
		{
			MyStackPanel().Children().RemoveAtEnd();
		}
	}

	MainWindow::MainWindow()
	{
		InitializeComponent();
		[[maybe_unused]] int plusLength{ 5 };
		auto dots{ MakeDots(static_cast<int>(MyStackPanel().ActualWidth()), static_cast<int>(MyStackPanel().ActualHeight()), numberOfRounds) };
		winrt::Microsoft::UI::Xaml::Media::SolidColorBrush myBrush(winrt::Windows::UI::ColorHelper::FromArgb(255, 0, 0, 0));
		for (auto& dot : dots)
		{
			roundsToDots[dot.round].push_back(dot);
		}
	}

	void MainWindow::OnKeyDownHandler([[maybe_unused]] winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Input::KeyRoutedEventArgs const& keyEventArgs)
	{
		if (keyEventArgs.Key() == winrt::Windows::System::VirtualKey::Up)
		{
			if (currentRound == numberOfRounds)
			{
				return;
			}
			
			// Remove transparent preview
			if (currentRound > 1)
			{
				currentRound--;
				RemoveCurrentRound();
			}

			DrawCurrentRound();
			currentRound++;

			// show a "Preview"
			DrawCurrentRoundAsTransparent();
			currentRound++;
		}
		else if (keyEventArgs.Key() == winrt::Windows::System::VirtualKey::Down)
		{
			if (currentRound <= 1)
			{
				currentRound = 1;
				return;
			}

			// Remove transparent preview.
			currentRound--;
			RemoveCurrentRound();

			if (currentRound == 1)
			{
				return;
			}

			// Remove current row in the UI.
			currentRound--;
			RemoveCurrentRound();

			// re-add preview
			DrawCurrentRoundAsTransparent();
			currentRound++;
		}
	}

}
