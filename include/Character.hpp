//
// Created by eatba on 2026/3/18.
//

#ifndef REPLACE_WITH_YOUR_PROJECT_NAME_CHARACTER_HPP
#define REPLACE_WITH_YOUR_PROJECT_NAME_CHARACTER_HPP


#include <string>
#include <memory>
#include "Util/GameObject.hpp" // 引入 PTSD 的核心物件類別
#include "glm/glm.hpp"
#include "Block.hpp"
class Character : public Util::GameObject
{
public:
    explicit Character(const std::string& ImagePath);
    Character(const Character&) = delete;
    Character(Character&&) = delete;
    Character& operator=(const Character&) = delete;
    Character& operator=(Character&&) = delete;
    virtual ~Character() = default;
    [[nodiscard]] const std::string& GetImagePath() const { return m_ImagePath; }

    [[nodiscard]] const glm::vec2& GetPosition() const { return m_Transform.translation; }

    [[nodiscard]] bool GetVisibility() const { return m_Visible; }

    void SetImage(const std::string& ImagePath);

    void SetPosition(const glm::vec2& Position) { m_Transform.translation = Position; }
    virtual void Update(const std::vector<std::shared_ptr<Block>>& blocks) = 0;
    //碰撞機制
    [[nodiscard]] virtual bool IfCollides(const std::shared_ptr<Character>& other) const {
        if (!other) return false;

        glm::vec2 g_pos = this->GetPosition();
        glm::vec2 other_pos = other->GetPosition();
        glm::vec2 g_size = this->GetScaledSize();
        glm::vec2 other_size = other->GetScaledSize();

        // AABB 矩形碰撞判定 (假設座標點位於圖片正中心)
        bool collisionX = std::abs(g_pos.x - other_pos.x) < (g_size.x + other_size.x) / 2.0f;
        bool collisionY = std::abs(g_pos.y - other_pos.y) < (g_size.y + other_size.y) / 2.0f;

        return collisionX && collisionY;
    }
protected:
    std::string m_ImagePath;
    glm::vec2 m_Velocity = {0.0f, 0.0f}; //
    bool m_IsGrounded = false; //

};

#endif //REPLACE_WITH_YOUR_PROJECT_NAME_CHARACTER_HPP