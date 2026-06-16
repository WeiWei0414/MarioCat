//
// Created by eatba on 2026/4/10.
//

#ifndef REPLACE_WITH_YOUR_PROJECT_NAME_ENEMY_HPP
#define REPLACE_WITH_YOUR_PROJECT_NAME_ENEMY_HPP
#include "Character.hpp"
class Enemy : public Character {
public:
    explicit Enemy(const std::string& imagePath);

    // 加上 virtual，讓子類別能覆寫
    virtual void Update(const std::vector<std::shared_ptr<Block>>& blocks) override;

    [[nodiscard]] bool IfCollidesWithBlock(const std::shared_ptr<Block>& block) const;
    [[nodiscard]] bool IsDead() const { return m_IsDead; }

    // 加上 virtual，未來有怪物死掉想爆炸也能自己寫
    virtual void Die();

    // 🌟 新增：讓外部可以設定怪物的速度 (水管噴怪物會用到！)
    void SetVelocity(const glm::vec2& velocity) { m_Velocity = velocity; }
    void SetDirection(float direction) { m_Direction = direction; }
    [[nodiscard]] bool IsPoweredUp() const { return m_IsPoweredUp; }

    // 🌟 新增 2：加上 virtual，讓子類別 (如 NormalEnemy) 可以自己決定變大長什麼樣子
    virtual void PowerUp() {}
protected: // 🌟 關鍵：改成 protected！這樣兒子們才拿得到這些變數
    float m_Speed = 1.0f;
    float m_Direction = -1.0f;
    bool m_IsDead = false;
    bool m_IsPoweredUp = false;
    // 探測懸崖的工具留給所有走地怪物共用
    [[nodiscard]] bool IsGroundAhead(const std::vector<std::shared_ptr<Block>>& blocks) const;
};
#endif //REPLACE_WITH_YOUR_PROJECT_NAME_ENEMY_HPP