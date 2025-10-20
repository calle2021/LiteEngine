#include <iostream>
#include <memory>
#include <stdexcept>

#include "LiteBox.h"
#include "LiteEngine.h"

int main()
{
    try {
        LiteBox liteBox;
        LiteEngine::Application().Launch(liteBox);
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}