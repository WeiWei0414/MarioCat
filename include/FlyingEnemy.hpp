//
// Created by eatba on 2026/6/1.
//

#ifndef REPLACE_WITH_YOUR_PROJECT_NAME_FLYINGENEMY_HPP
#define REPLACE_WITH_YOUR_PROJECT_NAME_FLYINGENEMY_HPP
#include "Enemy.hpp"

// 專屬的飛天怪類別
class FlyingEnemy : public Enemy {
public:
    explicit FlyingEnemy(const std::string& imagePath) : Enemy(imagePath) {}

    // 飛天怪不需要原本的重力跟走路邏輯，所以我們覆寫一個空的（或專屬的）給它
    void Update(const std::vector<std::shared_ptr<Block>>& blocks) override {
        // 什麼都不做，因為我們等一下會讓 App.cpp 來遙控它升空
    }
};
#endif //REPLACE_WITH_YOUR_PROJECT_NAME_FLYINGENEMY_HPP