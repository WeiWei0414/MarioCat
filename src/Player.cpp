//
// Created by eatba on 2026/3/18.
//
#include "Player.hpp"

#include "Util/Input.hpp"
#include "Util/Keycode.hpp"

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
    return collisionX && collisionY;
}

void Player::Update(const std::vector<std::shared_ptr<Block>>& blocks)
{
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
                m_IsGrounded=true;
                currentPos.y=block->GetPosition().y+(block->GetScaledSize().y/2.0f)+(this->GetScaledSize().y/2.0f);

            }else if (m_Velocity.y>0.0f){
                currentPos.y = block->GetPosition().y - (block->GetScaledSize().y/2.0f) - (this->GetScaledSize().y/2.0f);
            }
            m_Velocity.y=0.0f;
            SetPosition(currentPos);
            break;
        }
    }
    if (Util::Input::IsKeyPressed(Util::Keycode::SPACE)&& m_IsGrounded)
    {
        m_Velocity.y=12.0f;
        m_IsGrounded=false;
    }

}