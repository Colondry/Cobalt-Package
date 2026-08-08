#include "matplotlibcpp.h"
#include "plot.hpp"
#include <string>
#include <vector>
#include "../Python314/include/Python.h"
#include <mutex>

namespace plt = matplotlibcpp;
__Plot__ Plot;

#include <windows.h>
#include <Python.h>
#include <filesystem>
#include <iostream>

static void initializePython()
{
    wchar_t buffer[MAX_PATH];

    DWORD length = GetModuleFileNameW(
        nullptr,
        buffer,
        MAX_PATH
    );

    if (length == 0 || length == MAX_PATH) {
        std::cerr << "Could not determine executable path.\n";
        return;
    }

    std::filesystem::path current =
        std::filesystem::path(buffer).parent_path();

    std::filesystem::path pythonHome;

    // Search the executable directory and its parents.
    while (!current.empty()) {

        std::filesystem::path candidate =
            current / "lib" / "Python314";

        if (std::filesystem::exists(candidate)) {
            pythonHome = candidate;
            break;
        }

        std::filesystem::path parent = current.parent_path();

        if (parent == current)
            break;

        current = parent;
    }

    if (pythonHome.empty()) {
        std::cerr
            << "Could not find bundled Python 3.14.\n"
            << "Expected: lib\\Python314\n";

        return;
    }

    std::wstring home = pythonHome.wstring();

    std::wcerr
        << L"Python home: "
        << home
        << L"\n";

    Py_SetPythonHome(home.c_str());

    Py_Initialize();

    if (!Py_IsInitialized()) {
        std::cerr << "Failed to initialize Python.\n";
    }
}

void __EnsurePlotReady__()
{
    static std::once_flag flag;
    std::call_once(flag, []() {
        initializePython();
        plt::backend("TkAgg");
        });
}

void __Plot__::Show()
{
    __EnsurePlotReady__();
    plt::title(Plot.Title);
    plt::xlabel(Plot.XLabel);
    plt::ylabel(Plot.YLabel);
    plt::grid(Plot.Grid);
    plt::show();
}