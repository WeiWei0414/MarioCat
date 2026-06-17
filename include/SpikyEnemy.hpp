#ifndef SPIKYENEMY_HPP
#define SPIKYENEMY_HPP

#include "NormalEnemy.hpp"

class SpikyEnemy : public NormalEnemy {
public:
    explicit SpikyEnemy(const std::string& imagePath) : NormalEnemy(imagePath) {}

    // 🌟 新增：標記這隻怪物到底是刺蝟還是火球
    void SetAsFireball(bool isFireball) { m_IsFireball = isFireball; }
    bool IsFireball() const { return m_IsFireball; }

    // 🌟 核心修改：覆寫物理更新，實作「撞地即死」
    void Update(const std::vector<std::shared_ptr<Block>>& blocks) override {
        if (m_IsDead) return;

        if (m_IsFireball) {
            glm::vec2 currentPos = GetPosition();

            // 1. X 軸移動與碰撞偵測
            currentPos.x += m_Speed * m_Direction;
            SetPosition(currentPos);
            for (const auto& block : blocks) {
                if (IfCollidesWithBlock(block)) {
                    Die(); // 碰到底部側邊或牆壁 -> 瞬間消滅！
                    return;
                }
            }

            // 2. Y 軸重力與下墜碰撞偵測
            m_Velocity.y -= 0.5f;
            currentPos.y += m_Velocity.y;
            SetPosition(currentPos);
            for (const auto& block : blocks) {
                if (IfCollidesWithBlock(block)) {
                    Die(); // 砸到地板 -> 瞬間消滅！
                    return;
                }
            }
        } else {
            // 如果是一般的刺蝟怪，維持普通走地怪物的行為
            NormalEnemy::Update(blocks);
        }
    }

private:
    bool m_IsFireball = false; // 預設是一般怪
};

#endif