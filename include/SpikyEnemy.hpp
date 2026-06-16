//
// Created by eatba on 2026/6/12.
//

#ifndef REPLACE_WITH_YOUR_PROJECT_NAME_SPIKYENEMY_HPP
#define REPLACE_WITH_YOUR_PROJECT_NAME_SPIKYENEMY_HPP
#include "NormalEnemy.hpp"
class SpikyEnemy : public NormalEnemy {
public:
    explicit SpikyEnemy(const std::string& imagePath) : NormalEnemy(imagePath) {
        // 這裡不用多寫什麼，圖片名稱會直接傳給老爸 NormalEnemy 去處理
    }

    // 未來如果你想讓刺蝟怪有不同的行為 (例如走比較快)，可以在這裡覆寫 Update
};
#endif //REPLACE_WITH_YOUR_PROJECT_NAME_SPIKYENEMY_HPP