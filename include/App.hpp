#ifndef APP_HPP
#define APP_HPP

#include <memory>

#include "Scene/Scene.hpp"

class App {
public:
    void Start();
    void Update();
    void End(); // NOLINT(readability-convert-member-functions-to-static)

    bool IsDone() const { return m_Done; }

private:
    std::unique_ptr<Scene> m_Scene;
    bool m_Done = false;
};

#endif // APP_HPP