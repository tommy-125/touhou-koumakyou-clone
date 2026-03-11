#include "App.hpp"
#include "Core/Context.hpp"

int main(int, char**) {
    auto context = Core::Context::GetInstance();
    App app;

    app.Start();

    while (!context->GetExit() && !app.IsDone()) {
        context->Setup();
        app.Update();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        context->Update();
    }

    app.End();
    return 0;
}
