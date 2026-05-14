//
// Created by eatba on 2026/4/10.
//

#ifndef REPLACE_WITH_YOUR_PROJECT_NAME_ENEMY_HPP
#define REPLACE_WITH_YOUR_PROJECT_NAME_ENEMY_HPP

#include "Character.hpp"

class Enemy : public Character{
public:
    explicit Enemy(const std::string& imagePath);
    void Update(const std::vector<std::shared_ptr<Block>>& blocks) override;
    [[nodiscard]] bool IfCollidesWithBlock(const std::shared_ptr<Block>& block) const;
    [[nodiscard]] bool IsDead() const{return m_IsDead;}
    [[nodiscard]] bool IsGroundAhead(const std::vector<std::shared_ptr<Block>>& blocks) const;
    void Die();
private:
    float m_Speed=1.0f;
    float m_Direction=-1.0f;
    bool m_IsDead=false;

};

#endif //REPLACE_WITH_YOUR_PROJECT_NAME_ENEMY_HPP