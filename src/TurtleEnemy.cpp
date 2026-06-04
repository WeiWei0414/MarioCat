#include "TurtleEnemy.hpp"

TurtleEnemy::TurtleEnemy(const std::string& imagePath, bool avoidCliff)
    : Enemy(imagePath), m_AvoidCliff(avoidCliff) {
    m_Velocity = glm::vec2(0.0f, 0.0f);
}

void TurtleEnemy::TurnIntoShell() {
    m_State = TurtleState::SHELL_IDLE;
    // ⚠️ 請換成你實際的烏龜殼圖片名稱
    SetImage("turtle_shell"); 
}

void TurtleEnemy::KickShell(float dir) {
    m_State = TurtleState::SHELL_MOVING;
    m_Direction = dir;
    m_SafeTimer = 0.2f; // 🌟 關鍵：給予 0.2 秒的安全時間，避免一踢就判定撞死自己
}

void TurtleEnemy::StopShell() {
    m_State = TurtleState::SHELL_IDLE;
}

bool TurtleEnemy::IfCollidesWithBlock(const std::shared_ptr<Block>& block) const {
    if (!block) return false;
    glm::vec2 m_pos = this->GetPosition();
    glm::vec2 b_pos = block->GetPosition();
    glm::vec2 m_size = this->GetScaledSize();
    glm::vec2 b_size = block->GetScaledSize();

    bool collisionX = std::abs(m_pos.x - b_pos.x) < ((m_size.x + b_size.x) / 2.0f) - 2.0f;
    bool collisionY = std::abs(m_pos.y - b_pos.y) < ((m_size.y + b_size.y) / 2.0f) - 0.1f;
    return collisionX && collisionY;
}

void TurtleEnemy::Update(const std::vector<std::shared_ptr<Block>>& blocks) {
    if (m_IsDead) return;

    if (m_SafeTimer > 0.0f) {
        m_SafeTimer -= 1.0f / 60.0f; // 安全時間倒數
    }

    glm::vec2 currentPos = GetPosition();

    // ==========================================
    // 🌟 智能烏龜：呼叫老爸的懸崖防墜雷達
    // ==========================================
    if (m_AvoidCliff && m_State == TurtleState::WALKING && m_Velocity.y == 0.0f) {
        // 直接使用 Enemy 繼承下來的 IsGroundAhead 函式！
        if (!IsGroundAhead(blocks)) {
            m_Direction *= -1.0f; // 前面沒路了，立刻轉頭！
        }
    }

    // --- 1. X 軸移動 ---
    if (m_State == TurtleState::WALKING) {
        currentPos.x += m_Speed * m_Direction;
    } else if (m_State == TurtleState::SHELL_MOVING) {
        currentPos.x += m_ShellSpeed * m_Direction;
    }
    SetPosition(currentPos);

    // --- X 軸碰撞 (牆壁反彈) ---
    if (m_State != TurtleState::SHELL_IDLE) {
        for (const auto& block : blocks) {
            if (IfCollidesWithBlock(block)) {
                // 恢復上一幀的位置並反轉方向
                if (m_State == TurtleState::WALKING) {
                    currentPos.x -= m_Speed * m_Direction;
                } else {
                    currentPos.x -= m_ShellSpeed * m_Direction;
                }
                m_Direction *= -1.0f; 
                SetPosition(currentPos);
                break;
            }
        }
    }

    // --- 2. Y 軸重力 ---
    m_Velocity.y -= 0.5f; 
    currentPos.y += m_Velocity.y;
    SetPosition(currentPos);

    for (const auto& block : blocks) {
        if (IfCollidesWithBlock(block)) {
            if (m_Velocity.y < 0.0f) {
                currentPos.y = block->GetPosition().y + (block->GetScaledSize().y / 2.0f) + (this->GetScaledSize().y / 2.0f);
            } else {
                currentPos.y = block->GetPosition().y - (block->GetScaledSize().y / 2.0f) - (this->GetScaledSize().y / 2.0f);
            }
            m_Velocity.y = 0.0f;
            SetPosition(currentPos);
            break;
        }
    }

    // 掉出地圖死亡 (呼叫 Enemy 基底的 Die() 讓大管家回收)
    if (currentPos.y < -400.0f) {
        Die(); 
    }
}