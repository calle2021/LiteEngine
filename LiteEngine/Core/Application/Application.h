#pragma once
#include <memory>
#include "Core/Vulkan/Context.h"
#include "iLiteBox.h"

namespace LiteEngine
{
class Application
{
public:
    int Launch(iLiteBox& app);
};
}