//
// Created by eatba on 2026/3/18.
//

#ifndef REPLACE_WITH_YOUR_PROJECT_NAME_PLAYER_HPP
#define REPLACE_WITH_YOUR_PROJECT_NAME_PLAYER_HPP
#include <string>
#include <memory>
#include "Util/GameObject.hpp" // 引入 PTSD 的核心物件類別
#include "glm/glm.hpp"
#include "Character.hpp"

class Player :public Character
{
public:
    explicit Player();
    void Update(const std::vector<std::shared_ptr<Block>>& blocks) override;
    [[nodiscard]] bool IfCollidesWithBlock(const std::shared_ptr<Block>& block) const;
private:
    float m_speed=5.0f;

};

#endif //REPLACE_WITH_YOUR_PROJECT_NAME_PLAYER_HPP