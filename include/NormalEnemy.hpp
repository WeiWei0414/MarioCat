//
// Created by eatba on 2026/5/22.
//

#ifndef REPLACE_WITH_YOUR_PROJECT_NAME_NORMALENEMY_HPP
#define REPLACE_WITH_YOUR_PROJECT_NAME_NORMALENEMY_HPP
#include "Enemy.hpp"



class NormalEnemy : public Enemy {
public:
    // 🌟 修正：在建構子的冒號後面，把參數避開懸崖的值存進 m_AvoidCliff 裡面
    explicit NormalEnemy(const std::string& imagePath, bool avoidCliff = false)
        : Enemy(imagePath), m_AvoidCliff(avoidCliff) {}

    void Update(const std::vector<std::shared_ptr<Block>>& blocks) override;
    void PowerUp() override;
private:
    bool m_AvoidCliff; // 🌟 宣告成員變數 (加上 m_ 區分)
};

#endif //REPLACE_WITH_YOUR_PROJECT_NAME_NORMALENEMY_HPP