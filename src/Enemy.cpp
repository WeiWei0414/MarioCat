#include "Enemy.hpp"

Enemy::Enemy(const std::string& imagePath) : Character(imagePath) {
    m_Velocity = glm::vec2(0.0f, 0.0f);
}

// 碰撞判定 (直接借用你 Player 寫好的完美邏輯)
bool Enemy::IfCollidesWithBlock(const std::shared_ptr<Block>& block) const {
    if (!block) return false;
    glm::vec2 m_pos = this->GetPosition();
    glm::vec2 b_pos = block->GetPosition();
    glm::vec2 m_size = this->GetScaledSize();
    glm::vec2 b_size = block->GetScaledSize(); 

    bool collisionX = std::abs(m_pos.x - b_pos.x) < ((m_size.x + b_size.x) / 2.0f) - 3.0f;
    bool collisionY = std::abs(m_pos.y - b_pos.y) < ((m_size.y + b_size.y) / 2.0f) - 0.1f;
    return collisionX && collisionY;
}

void Enemy::Update(const std::vector<std::shared_ptr<Block>>& blocks) {
    glm::vec2 currentPos = GetPosition();
    
    // === 1. X 軸移動與碰撞 (左右巡邏) ===
    currentPos.x += m_Speed * m_Direction;
    SetPosition(currentPos);

    for (const auto& block : blocks) {
        if (IfCollidesWithBlock(block)) {
            // 如果撞到牆壁了，就退回原位，並且「反轉方向」！
            currentPos.x -= m_Speed * m_Direction;
            m_Direction *= -1.0f; // 往左撞到就變往右，往右撞到就變往左
            SetPosition(currentPos);
            break;
        }
    }

    // === 2. Y 軸重力與碰撞 (讓怪物能踩在地板上) ===
    m_Velocity.y -= 0.5f;
    currentPos.y += m_Velocity.y;
    SetPosition(currentPos);

    for (const auto& block : blocks) {
        if (IfCollidesWithBlock(block)) {
            if (m_Velocity.y < 0.0f) { // 踩到地板
                currentPos.y = block->GetPosition().y + (block->GetScaledSize().y / 2.0f) + (this->GetScaledSize().y / 2.0f);
            }
            m_Velocity.y = 0.0f;
            SetPosition(currentPos);
            break;
        }
    }
}

void Enemy::Die()
{
    if (m_IsDead) return;
    m_IsDead=true;
    m_Velocity = glm::vec2(0.0f, 0.0f);

}
bool Enemy::IsGroundAhead(const std::vector<std::shared_ptr<Block>>& blocks) const {
    glm::vec2 pos = GetPosition();
    glm::vec2 size = GetScaledSize();

    // 決定探測點：X 軸往當前方向凸出半個身位，Y 軸在腳底下一點點的位置
    float probeX = pos.x + (size.x / 2.0f + 5.0f) * m_Direction;
    float probeY = pos.y - (size.y / 2.0f + 5.0f);

    for (const auto& block : blocks) {
        if (!block) continue;

        glm::vec2 b_pos = block->GetPosition();
        glm::vec2 b_size = block->GetScaledSize();

        // 檢查探測點 (probeX, probeY) 是不是落在這塊磚塊的範圍內
        bool inX = (probeX > b_pos.x - b_size.x / 2.0f) && (probeX < b_pos.x + b_size.x / 2.0f);
        bool inY = (probeY > b_pos.y - b_size.y / 2.0f) && (probeY < b_pos.y + b_size.y / 2.0f);

        if (inX && inY) {
            return true; // 探測點有碰到磚塊，代表前面有路！
        }
    }
    return false; // 所有磚塊都巡完了還是沒碰到，代表前面是懸崖！
}