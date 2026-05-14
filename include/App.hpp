#ifndef APP_HPP
#define APP_HPP
#include "Player.hpp"
#include "pch.hpp" // IWYU pragma: export
#include "Block.hpp"
#include "Enemy.hpp"
class App {
public:
    enum class State {
        START,
        UPDATE,
        END,
    };

    State GetCurrentState() const { return m_CurrentState; }

    void Start();

    void Update();

    void End(); // NOLINT(readability-convert-member-functions-to-static)

private:
    void ValidTask();
    State m_CurrentState = State::START;
    std::shared_ptr<Player> m_Player;
    std::vector<std::shared_ptr<Block>> m_Blocks;
    std::shared_ptr<Util::GameObject> m_Background;
    std::vector<std::shared_ptr<Enemy>> m_Enemies;

    template<typename T>
    void RenderWithCamera(const std::vector<std::shared_ptr<T>>& objects, float cameraX, float zoom) {
        for (auto& obj : objects) {
            glm::vec2 realPos = obj->GetPosition();

            obj->SetPosition({(realPos.x - cameraX) * zoom, realPos.y * zoom});
            obj->SetScale({zoom, zoom});

            obj->Draw();

            obj->SetPosition(realPos);
            obj->SetScale({1.0f, 1.0f});
        }
    }


};

#endif
