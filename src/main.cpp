#include "App.hpp"
#include "Core/Context.hpp"

int main(int, char**) {
    auto context = Core::Context::GetInstance();
    App  app;

    app.Start();

    while (!context->GetExit() && !app.IsDone()) {
        context->Update();
        context->Setup();
        app.Update();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    app.End();
    return 0;
}
