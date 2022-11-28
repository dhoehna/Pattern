// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE in the project root for license information.

#pragma once

#include "MainWindow.g.h"
#include <winrt/Microsoft.UI.Xaml.Input.h>
#include <winrt/windows.ui.xaml.input.h>

namespace winrt::TheRealTree::implementation
{
    struct MainWindow : MainWindowT<MainWindow>
    {
        MainWindow();
        void OnKeyDownHandler(Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Input::KeyRoutedEventArgs const& KeyEventArgs);
        void DrawCurrentRound();
        void RemoveCurrentRound();
    };
}

namespace winrt::TheRealTree::factory_implementation
{
    struct MainWindow : MainWindowT<MainWindow, implementation::MainWindow>
    {
    };
}
