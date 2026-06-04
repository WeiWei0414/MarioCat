//
// Created by eatba on 2026/3/18.
//
#include "Player.hpp"
#include "HiddenBlock.hpp"
#include "EventBlock.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"
#include <vector>
#include <algorithm>
Player::Player():Character("player_idle")
{
    m_Velocity=glm::vec2(0.0f, 0.0f);
    m_IsGrounded=false; //跳躍狀態


}

bool Player::IfCollidesWithBlock(const std::shared_ptr<Block>& block) const
{
    if (!block) return false;
    glm::vec2 m_pos=this->GetPosition();
    glm::vec2 b_pos=block->GetPosition();
    glm::vec2 m_size=this->GetScaledSize();
    glm::vec2 b_size=block->GetScaledSize();

    bool collisionX=std::abs(m_pos.x-b_pos.x)<(m_size.x+b_size.x)/2.0f-0.1f;
    bool collisionY=std::abs(m_pos.y-b_pos.y)<(m_size.y+b_size.y)/2.0f-0.1f;
    bool isOverlapping = collisionX && collisionY;

    if (!isOverlapping) return false; // 如果連 AABB 都沒碰到，直接回傳 false
    auto eventBlock = std::dynamic_pointer_cast<EventBlock>(block);
    if (eventBlock) {
        std::vector<int> ghostIDs = {84,85}; // 如果未來加了新幽靈陷阱，直接改成 {101, 102, 105} 就可以了！

        // 2. 去陣列裡面找，看看當前碰到的方塊 ID 有沒有在名單裡
        int currentID = eventBlock->GetEventID();

        if (std::find(ghostIDs.begin(), ghostIDs.end(), currentID) != ghostIDs.end()) {
            return false; // 在名單內！直接當作沒碰到，穿過去！
        }
        if (currentID == 86) {
            if (m_pos.x < b_pos.x) {
                return true;
            } else {
                return false;  // 貓在右邊：物理引擎請把它擋下來！
            }
        }
    }
    // ==========================================
    // 🌟 隱藏方塊的特殊穿透邏輯
    // ==========================================
    auto hiddenBlock = std::dynamic_pointer_cast<HiddenBlock>(block);
    if (hiddenBlock && hiddenBlock->IsHidden()) {
        // 只有當貓咪「正在往上跳」且「人在方塊下方」時，才算撞到！
        if (m_Velocity.y > 0.0f && m_pos.y < b_pos.y) {
            hiddenBlock->Reveal(); // 方塊現形！
            return true;           // 發生碰撞，會觸發你原本的物理推回邏輯
        }
        return false; // 其他情況(左右走、掉落)一律當作沒撞到，直接穿過去！
    }

    // 如果是普通方塊，重疊了就是撞到
    return true;
}
void Player::Die()
{
    if (IsInvincible()) return;
    if (m_IsSuper)
    {
        m_IsSuper = false;
        SetImage("player_idle");
        m_LastSafePos = {700.0f, -120.0f};
    }
    m_lives--;
    if (m_lives>0)
    {
        SetPosition(m_LastSafePos);
        m_Velocity={0.0f, 0.0f};
        m_InvincibleTimer=2.0f;
    }
    else
    {
        LOG_INFO("Game over");
    }
}
void Player::Bounce()
{
    m_Velocity.y=8.0f;
}

void Player::PowerUp()
{

    m_IsSuper=true;
    SetImage("player_powerup");
}

void Player::Update(const std::vector<std::shared_ptr<Block>>& blocks)
{
    if (m_InvincibleTimer>0.0f)
    {
        m_InvincibleTimer-=1.0f/60.0f;
        int blink=static_cast<int>(m_InvincibleTimer*10);
        SetVisible(blink%2==0);
    }
    else
    {
        SetVisible(true);
    }
    glm::vec2 currentPos = GetPosition();
    m_IsGrounded=false;
    //x軸判斷
    float moveX=0.0f;
    if (Util::Input::IsKeyPressed(Util::Keycode::A) || Util::Input::IsKeyPressed(Util::Keycode::LEFT))
    {
        moveX-=m_speed;
    }
    if (Util::Input::IsKeyPressed(Util::Keycode::D) || Util::Input::IsKeyPressed(Util::Keycode::RIGHT))
    {
        moveX+=m_speed;
    }
    currentPos.x+=moveX;
    SetPosition(currentPos);

    for (const auto& block : blocks)
    {
        if (IfCollidesWithBlock(block))
        {
            currentPos.x-=moveX;
            SetPosition(currentPos);
            break;
        }
    }

    m_Velocity.y-=0.5f;
    currentPos.y+=m_Velocity.y;
    SetPosition(currentPos);

    for (const auto& block:blocks)
    {
        if (IfCollidesWithBlock(block))
        {
            if (m_Velocity.y<0.0f)
            {
                if (m_IsSuper)
                {
                    m_LastSafePos-=100.0f;
                    block->SetDestoryed(true);
                    m_Velocity.y *= 0.8f; // 稍微減緩速度製造打擊感
                    continue;
                }
                else
                {
                    m_IsGrounded=true;
                    currentPos.y=block->GetPosition().y+(block->GetScaledSize().y/2.0f)+(this->GetScaledSize().y/2.0f);
                    m_Velocity.y=0.0f;
                    m_LastSafePos=currentPos;
                    auto eventBlock = std::dynamic_pointer_cast<EventBlock>(block);
                    if (eventBlock && (eventBlock->GetEventID() == 97 || eventBlock->GetEventID() == 98)) {
                        eventBlock->Activate();
                    }

                }

            }else if (m_Velocity.y>0.0f){

                auto eventBlock=std::dynamic_pointer_cast<EventBlock>(block);

                if (eventBlock)
                {   //這段有問題

                    if (!eventBlock->IsActivated())
                    {
                        std::vector<int> ignoreBumpIDs = {83, 97, 98,85,86};
                        int currentID = eventBlock->GetEventID();

                        if (std::find(ignoreBumpIDs.begin(), ignoreBumpIDs.end(), currentID) == ignoreBumpIDs.end())
                        {
                            eventBlock->Activate();
                        }

                    }
                    else
                    {
                        if (block->IsBreakable())
                        {
                            block->SetDestoryed(true);
                        }
                    }
                }
                if (block->IsBreakable())
                {
                    block->SetDestoryed(true);
                }

                currentPos.y = block->GetPosition().y - (block->GetScaledSize().y/2.0f) - (this->GetScaledSize().y/2.0f);
            }
            m_Velocity.y=0.0f;
            SetPosition(currentPos);
            break;
        }
    }

    if (Util::Input::IsKeyPressed(Util::Keycode::U))
    {
        m_InvincibleTimer=999999.0f;
    }
    if (Util::Input::IsKeyPressed(Util::Keycode::SPACE)&& m_IsGrounded)
    {
        m_Velocity.y=12.0f;
        m_IsGrounded=false;
        // SetImage("player_jump");

    }

}