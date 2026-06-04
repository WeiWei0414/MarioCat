#include "StarEnemy.hpp"

StarEnemy::StarEnemy(const std::string& imagePath) : Enemy(imagePath) {
    m_Velocity = glm::vec2(0.0f, 10.0f); // 剛從方塊噴出來時，給它一個強烈的初速度
}

bool StarEnemy::IfCollidesWithBlock(const std::shared_ptr<Block>& block) const {
    if (!block) return false;
    glm::vec2 m_pos = this->GetPosition();
    glm::vec2 b_pos = block->GetPosition();
    glm::vec2 m_size = this->GetScaledSize();
    glm::vec2 b_size = block->GetScaledSize();

    bool collisionX = std::abs(m_pos.x - b_pos.x) < ((m_size.x + b_size.x) / 2.0f) - 3.0f;
    bool collisionY = std::abs(m_pos.y - b_pos.y) < ((m_size.y + b_size.y) / 2.0f) - 0.1f;
    return collisionX && collisionY;
}

void StarEnemy::Update(const std::vector<std::shared_ptr<Block>>& blocks) {
    if (m_IsDead) return;

    glm::vec2 currentPos = GetPosition();

    // --- 1. X 軸移動 ---
    currentPos.x += m_Speed * m_Direction;
    SetPosition(currentPos);

    for (const auto& block : blocks) {
        if (IfCollidesWithBlock(block)) {
            currentPos.x -= m_Speed * m_Direction;
            m_Direction *= -1.0f; // 撞到牆壁反彈
            SetPosition(currentPos);
            break;
        }
    }

    // --- 2. Y 軸重力與彈跳 ---
    m_Velocity.y -= 0.5f; // 重力往下墜
    currentPos.y += m_Velocity.y;
    SetPosition(currentPos);

    for (const auto& block : blocks) {
        if (IfCollidesWithBlock(block)) {
            if (m_Velocity.y < 0.0f) {
                // 🌟 關鍵邏輯：落地時不歸零，而是強烈反彈！
                currentPos.y = block->GetPosition().y + (block->GetScaledSize().y / 2.0f) + (this->GetScaledSize().y / 2.0f);
                m_Velocity.y = 12.0f; // 彈跳高度 (數字越大彈越高)
            } else {
                // 撞到天花板
                currentPos.y = block->GetPosition().y - (block->GetScaledSize().y / 2.0f) - (this->GetScaledSize().y / 2.0f);
                m_Velocity.y = -2.0f; // 稍微往下彈
            }
            SetPosition(currentPos);
            break;
        }
    }
}