//
// Created by eatba on 2026/6/2.
//

#ifndef REPLACE_WITH_YOUR_PROJECT_NAME_TURTLEENEMY_HPP
#define REPLACE_WITH_YOUR_PROJECT_NAME_TURTLEENEMY_HPP
#include "Enemy.hpp"
#include "Block.hpp"
#include <vector>
#include <memory>

// 烏龜的三種狀態
#include "Enemy.hpp"
#include "Block.hpp"
#include <vector>
#include <memory>

// 烏龜的三種狀態
enum class TurtleState { WALKING, SHELL_IDLE, SHELL_MOVING };

class TurtleEnemy : public Enemy {
public:
    // 🌟 修正：加入 avoidCliff 參數，並預設為 true
    explicit TurtleEnemy(const std::string& imagePath, bool avoidCliff = true);

    void Update(const std::vector<std::shared_ptr<Block>>& blocks) override;
    [[nodiscard]] bool IfCollidesWithBlock(const std::shared_ptr<Block>& block) const;

    // 狀態相關操作
    TurtleState GetState() const { return m_State; }
    void TurnIntoShell();
    void KickShell(float dir);
    void StopShell();

    bool CanKillPlayer() const { return m_SafeTimer <= 0.0f; }

private:
    TurtleState m_State = TurtleState::WALKING;
    float m_Speed = 1.0f;
    float m_ShellSpeed = 9.0f;
    float m_Direction = -1.0f;
    float m_SafeTimer = 0.0f;
    bool m_AvoidCliff; // 🌟 宣告成員變數
};
#endif //REPLACE_WITH_YOUR_PROJECT_NAME_TURTLEENEMY_HPP