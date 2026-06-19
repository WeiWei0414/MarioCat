//
// Created by eatba on 2026/3/18.
//

#ifndef REPLACE_WITH_YOUR_PROJECT_NAME_PLAYER_HPP
#define REPLACE_WITH_YOUR_PROJECT_NAME_PLAYER_HPP
#include <string>
#include <memory>
#include "Util/GameObject.hpp" // 引入 PTSD 的核心物件類別
#include "glm/glm.hpp"
#include "Character.hpp"

class Player :public Character
{
public:
    explicit Player();
    void Update(const std::vector<std::shared_ptr<Block>>& blocks) override;
    [[nodiscard]] bool IfCollidesWithBlock(const std::shared_ptr<Block>& block) const;
    [[nodiscard]] int GetLives() const{ return m_lives; }
    [[nodiscard]] bool IsInvincible() const { return m_InvincibleTimer > 0.0f; }
    [[nodiscard]] glm::vec2 GetVelocity() const { return m_Velocity; }
    void SetVelocity(const glm::vec2& velocity) { m_Velocity = velocity; }
    void Bounce();
    void Die();
    void PowerUp();
    [[nodiscard]] bool IsSuper() const { return m_IsSuper; }
    bool IsDying() const { return m_IsDying; }
    int GetDeathCount() const { return m_DeathCount; }
    // 🌟 新增：把原本 Die 裡面的「復活傳送」邏輯獨立出來
    void Respawn();
private:
    bool m_IsDying = false;
    int m_DeathCount = 0;
    float m_speed=5.0f;
    int m_lives=5;
    float m_InvincibleTimer = 0.0f; // 無敵時間計時器
    glm::vec2 m_LastSafePos = {-500.0f, 0.0f}; //最後安全點
    bool m_IsSuper=false;
};

#endif //REPLACE_WITH_YOUR_PROJECT_NAME_PLAYER_HPP