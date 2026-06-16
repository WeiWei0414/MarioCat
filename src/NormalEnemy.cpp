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
            if (m_Velocity.y <= 0.0f) {
                if (m_IsPoweredUp) {
                    block->SetBreakable(true);
                    block->SetDestoryed(true); // 無情壓碎！
                    m_Velocity.y *= 0.8f;      // 稍微減速，營造壓碎磚塊的「重量感」與打擊感
                    continue;                  // 🌟 核心：用 continue 放棄這次的阻擋判定，讓怪物繼續往下掉！
                }
                currentPos.y = block->GetPosition().y + (block->GetScaledSize().y / 2.0f) + (this->GetScaledSize().y / 2.0f);
            }
            m_Velocity.y = 0.0f;
            SetPosition(currentPos);
            break;
        }
    }
}
void NormalEnemy::PowerUp() {
    if (!m_IsPoweredUp) {
        m_IsPoweredUp = true;

        // 🌟 換成巨大怪物的圖片 (請把 "teki_powerup" 換成你實際放進 Image 資料夾的檔名！)
        SetImage("teki_powerup");

        // (選用) 如果你想讓怪物變大後跑得更快，可以在這裡加這行：
        // m_Speed = 2.0f;
    }
}