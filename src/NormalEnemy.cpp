//
// Created by eatba on 2026/5/22.
//
#include "NormalEnemy.hpp"
void NormalEnemy::Update(const std::vector<std::shared_ptr<Block>>& blocks) {
    if (m_IsDead) return; // 🌟 如果死了就不動了

    glm::vec2 currentPos = GetPosition();
    if (m_AvoidCliff)
    {
        // 懸崖防呆機制 (呼叫老爸的函式)
        if (!IsGroundAhead(blocks)) {
            m_Direction *= -1.0f;
        }
    }

    // === 1. X 軸移動與碰撞 ===
    currentPos.x += m_Speed * m_Direction;
    SetPosition(currentPos);

    for (const auto& block : blocks) {
        if (IfCollidesWithBlock(block)) {
            currentPos.x -= m_Speed * m_Direction;
            m_Direction *= -1.0f;
            SetPosition(currentPos);
            break;
        }
    }

    // === 2. Y 軸重力與碰撞 ===
    m_Velocity.y -= 0.5f;
    currentPos.y += m_Velocity.y;
    SetPosition(currentPos);

    for (const auto& block : blocks) {
        if (IfCollidesWithBlock(block)) {
            if (m_Velocity.y < 0.0f) {
                currentPos.y = block->GetPosition().y + (block->GetScaledSize().y / 2.0f) + (this->GetScaledSize().y / 2.0f);
            }
            m_Velocity.y = 0.0f;
            SetPosition(currentPos);
            break;
        }
    }
}