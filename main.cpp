#include <iostream>
#include "application.h"
#include "glfwcontext.h"
#include "settings.h"

int main()
{
    try {
        GlfwContext glfwCtx{"Voxel world",
                            Settings::get().rendering().width,
                            Settings::get().rendering().height};

        Application{glfwCtx.getWindow()}.run();
    }
    catch(std::exception& ex) {
        std::cerr << ex.what();
    }
    catch(...) {
        std::cerr << "Unknown exception\n";
    }
    return 0;
}
