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

namespace winrt::TheRealTree::implementation
{

    std::map<int, std::list<Dot>> roundsToDots{};
    std::map<int, std::list<winrt::Microsoft::UI::Xaml::Shapes::Line>> roundToLineCollection{};
    int currentRound{ 1 };
    int numberOfRounds{ 60 };


    void MainWindow::DrawCurrentRound()
    {
        for (auto&& line : roundToLineCollection[currentRound])
        {
            line.Visibility(winrt::Microsoft::UI::Xaml::Visibility::Visible);
        }

        currentRound++;
    }

    void MainWindow::RemoveCurrentRound()
    {
        currentRound--;
        for (auto&& line : roundToLineCollection[currentRound])
        {
            line.Visibility(winrt::Microsoft::UI::Xaml::Visibility::Collapsed);
        }
    }

    MainWindow::MainWindow()
    {
        InitializeComponent();
        auto dots{ MakeDots(static_cast<int>(MyStackPanel().ActualWidth()), static_cast<int>(MyStackPanel().ActualHeight()), numberOfRounds) };

        winrt::Microsoft::UI::Xaml::Media::SolidColorBrush myBrush(winrt::Windows::UI::ColorHelper::FromArgb(255, 0, 0, 0));
        for (auto& dot : dots)
        {
            if (dot.previous.get() != nullptr)
            {
                winrt::Microsoft::UI::Xaml::Shapes::Line myLine{};
                myLine.Stroke(myBrush);
                myLine.X1(dot.x);
                myLine.X2(dot.previous->x);
                myLine.Y1(dot.y);
                myLine.Y2(dot.previous->y);
                myLine.Visibility(winrt::Microsoft::UI::Xaml::Visibility::Collapsed);
                roundToLineCollection[dot.round].push_back(myLine);
                MyStackPanel().Children().Append(myLine);
            }
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

            DrawCurrentRound();
        }
        else if (keyEventArgs.Key() == winrt::Windows::System::VirtualKey::Down)
        {
            if (currentRound <= 1)
            {
                currentRound = 1;
                return;
            }

            RemoveCurrentRound();
        }
    }

}
