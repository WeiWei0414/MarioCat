//
// Created by eatba on 2026/5/29.
//

#ifndef REPLACE_WITH_YOUR_PROJECT_NAME_MUSHROOM_HPP
#define REPLACE_WITH_YOUR_PROJECT_NAME_MUSHROOM_HPP
#include "Character.hpp"
#include "Block.hpp"
#include <vector>
#include <memory>

class Mushroom : public Character
{
public:
    explicit Mushroom(const std::string& imagePath,int typeID);

    void Update(const std::vector<std::shared_ptr<Block>>& blocks);
    [[nodiscard]] bool IfCollidesWithBlock(const std::shared_ptr<Block>& block) const;

    // 蘑菇被吃掉的狀態
    [[nodiscard]] bool IsEaten() const { return m_IsEaten; }
    void Eat() { m_IsEaten = true; }
    void SetVelocity(const glm::vec2& velocity) { m_Velocity = velocity; }
    [[nodiscard]] int GetTypeID() const { return m_TypeID; }
private:
    float m_Speed = 0.5f;       // 蘑菇滑行的速度通常比怪物快一點
    float m_Direction = 1.0f;   // 預設往右滑
    bool m_IsEaten = false;
    int m_TypeID=1;
};
#endif //REPLACE_WITH_YOUR_PROJECT_NAME_MUSHROOM_HPP