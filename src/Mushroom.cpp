#include "Mushroom.hpp"

Mushroom::Mushroom(const std::string& imagePath,int typeID) : Character(imagePath),m_TypeID(typeID) {
    m_Velocity = glm::vec2(0.0f, 0.0f);
}

// 完美的碰撞判定 (跟你原本寫的一模一樣)
bool Mushroom::IfCollidesWithBlock(const std::shared_ptr<Block>& block) const {
    if (!block) return false;
    glm::vec2 m_pos = this->GetPosition();
    glm::vec2 b_pos = block->GetPosition();
    glm::vec2 m_size = this->GetScaledSize();
    glm::vec2 b_size = block->GetScaledSize();

    bool collisionX = std::abs(m_pos.x - b_pos.x) < ((m_size.x + b_size.x) / 2.0f) - 3.0f;
    bool collisionY = std::abs(m_pos.y - b_pos.y) < ((m_size.y + b_size.y) / 2.0f) - 0.1f;
    return collisionX && collisionY;
}

void Mushroom::Update(const std::vector<std::shared_ptr<Block>>& blocks) {
    if (m_IsEaten) return;

    glm::vec2 currentPos = GetPosition();

    // 1. X 軸移動 (左右滑行)
    currentPos.x += m_Speed * m_Direction;
    SetPosition(currentPos);

    for (const auto& block : blocks) {
        if (IfCollidesWithBlock(block)) {
            currentPos.x -= m_Speed * m_Direction;
            m_Direction *= -1.0f; // 撞到牆壁就反彈
            SetPosition(currentPos);
            break;
        }
    }

    // 2. Y 軸重力 (會往下掉)
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