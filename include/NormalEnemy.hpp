//
// Created by eatba on 2026/5/22.
//

#ifndef REPLACE_WITH_YOUR_PROJECT_NAME_NORMALENEMY_HPP
#define REPLACE_WITH_YOUR_PROJECT_NAME_NORMALENEMY_HPP
#include "Enemy.hpp"

// 繼承老爸 Enemy
class NormalEnemy : public Enemy {
public:
    explicit NormalEnemy(const std::string& imagePath,bool avoidCliff = false) : Enemy(imagePath) {}

    // 只覆寫 Update 邏輯
    void Update(const std::vector<std::shared_ptr<Block>>& blocks) override;
    private:
    bool avoidCliff;
};

#endif //REPLACE_WITH_YOUR_PROJECT_NAME_NORMALENEMY_HPP