#include "App.hpp"
#include "Core/Context.hpp"

#include <algorithm>
#include <cmath>
#include <fstream>

namespace {
constexpr const char* WINDOW_TITLE = "Touhou Koumakyou";

float LoadWindowScale() {
    for (const char* path : {"config.json", "../config.json"}) {
        std::ifstream file(path);
        if (!file) continue;

        try {
            const auto config = nlohmann::json::parse(file);
            return std::clamp(config.value("window_scale", 1.0f), 0.5f, 4.0f);
        } catch (...) {
            return 1.0f;
        }
    }
    return 1.0f;
}

void ApplyWindowScale(Core::Context& context) {
    const float scale = LoadWindowScale();

    if (SDL_Window* window = SDL_GL_GetCurrentWindow()) {
        SDL_SetWindowTitle(window, WINDOW_TITLE);

        if (scale == 1.0f) return;

        const int width =
            static_cast<int>(std::round(static_cast<float>(PTSD_Config::WINDOW_WIDTH) * scale));
        const int height =
            static_cast<int>(std::round(static_cast<float>(PTSD_Config::WINDOW_HEIGHT) * scale));

        SDL_SetWindowSize(window, width, height);
        glViewport(0, 0, width, height);
        context.SetWindowWidth(static_cast<unsigned int>(width));
        context.SetWindowHeight(static_cast<unsigned int>(height));
    }
}
}  // namespace

int main(int, char**) {
    auto context = Core::Context::GetInstance();
    ApplyWindowScale(*context);
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
