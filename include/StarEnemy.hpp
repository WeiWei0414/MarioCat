//
// Created by eatba on 2026/6/1.
//

#ifndef REPLACE_WITH_YOUR_PROJECT_NAME_STARENEMY_HPP
#define REPLACE_WITH_YOUR_PROJECT_NAME_STARENEMY_HPP
#include "Enemy.hpp"
#include "Block.hpp"
#include <vector>
#include <memory>

class StarEnemy : public Enemy {
public:
    explicit StarEnemy(const std::string& imagePath);
    void Update(const std::vector<std::shared_ptr<Block>>& blocks) override;
    [[nodiscard]] bool IfCollidesWithBlock(const std::shared_ptr<Block>& block) const;

private:
    float m_Speed = 2.0f;     // 星星的移動速度通常比較快
    float m_Direction = 1.0f; // 預設往右彈跳
};
#endif //REPLACE_WITH_YOUR_PROJECT_NAME_STARENEMY_HPP