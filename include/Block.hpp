//
// Created by eatba on 2026/3/20.
//

#ifndef REPLACE_WITH_YOUR_PROJECT_NAME_BLOCK_HPP
#define REPLACE_WITH_YOUR_PROJECT_NAME_BLOCK_HPP
#include "Util/GameObject.hpp"
#include <memory>

class Block : public Util::GameObject
{
    public:
    Block(const std::string& imageName);
    [[nodiscard]] glm::vec2 GetPosition() const {return m_Transform.translation;}
    [[nodiscard]] glm::vec2 GetSie() const {return GetScaledSize();}

    void SetPosition(const glm::vec2& pos){m_Transform.translation=pos;}
};
#endif //REPLACE_WITH_YOUR_PROJECT_NAME_BLOCK_HPP