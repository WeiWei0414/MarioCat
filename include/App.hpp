#ifndef APP_HPP
#define APP_HPP
#include "Player.hpp"
#include "pch.hpp" // IWYU pragma: export
#include "Block.hpp"
#include "Enemy.hpp"
#include "Decoration.hpp"
#include "Coin.hpp"
#include "Mushroom.hpp"
#include <map>
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
    int m_CurrentLevel = 2;
    void LoadLevel(int level);

    State m_CurrentState = State::START;
    std::shared_ptr<Player> m_Player;
    std::map<std::shared_ptr<Block>, std::weak_ptr<Enemy>> m_SpawnerTracker;
    std::vector<std::shared_ptr<Block>> m_Blocks;
    std::shared_ptr<Util::GameObject> m_Background;
    std::vector<std::shared_ptr<Enemy>> m_Enemies;
    std::vector<std::shared_ptr<Decoration>> m_Decorations;
    std::vector<std::shared_ptr<Coin>> m_Coins;
    std::vector<std::shared_ptr<Mushroom>> m_Mushrooms;
    std::map<std::shared_ptr<Block>, int> m_SpawnCounts;
    std::map<std::shared_ptr<Block>, int> m_FireballTimers;
    std::map<std::shared_ptr<Block>, glm::vec2> m_PlatformInitialPos;
    std::map<std::shared_ptr<Block>, float> m_PlatformAngles;
    int m_FireballInterval = 120;
    int m_PipeAnimPhase = 0;
    int m_PipeAnimationTimer = 0;
    float m_OriginalPipeX = 0.0f;
    int m_FlagAnimPhase = 0;
    int m_FlagAnimTimer = 0;
    bool m_IsTrollFlagDeath = false;
    float m_FlagBottomY = 0.0f;
    int m_HorizPipePhase = 0;  // 0:未觸發, 1:走進去, 2:向左噴射
    int m_HorizPipeTimer = 0;
    float m_MaxCameraX = -213.0f;
    int m_ClearPipePhase = 0;
    int m_ClearPipeTimer = 0;
    int m_DeathPipePhase = 0;   // 🌟 0: 未觸發, 1: 下沉中
    int m_DeathPipeTimer = 0;
    int m_HorizClearPipePhase = 0; // 🌟 0: 未觸發, 1: 走入水管中
    int m_HorizClearPipeTimer = 0;
    int m_DeathTimer = 0;
    bool m_IsFakeFlagTriggered = false;
    std::map<std::shared_ptr<Block>, float> m_BlockVelocityY;
    std::shared_ptr<Util::GameObject> m_DeathTextUI;
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
