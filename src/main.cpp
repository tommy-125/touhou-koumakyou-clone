#include "Core/Context.hpp"

int main(int, char**) {
    auto context = Core::Context::GetInstance();
    

    while (!context->GetExit()) {
        context->Setup();
        context->Update();
    }

    return 0;
}
