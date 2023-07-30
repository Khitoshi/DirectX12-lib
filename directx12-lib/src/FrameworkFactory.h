#pragma once

#include "Framework.h"
#include <memory>

class FrameworkFactory
{
public:
    static std::shared_ptr<Framework> create(const HWND& hWnd)
    {
        std::shared_ptr<Framework> framework(new Framework(hWnd));
        framework->init();
        return framework;
    }
};
