//
// Created by eatba on 2026/5/22.
//

#ifndef REPLACE_WITH_YOUR_PROJECT_NAME_COIN_HPP
#define REPLACE_WITH_YOUR_PROJECT_NAME_COIN_HPP
#include "Util/GameObject.hpp"
#include "ImageManager.hpp"

class Coin : public Util::GameObject {
public:
    // 給定初始位置，並給予向上的初速度
    explicit Coin(const glm::vec2& startPos) {
        SetDrawable(ImageManager::Get("coin")); // 🌟 假設你有一張 coin 圖片
        SetPosition(startPos);
        SetZIndex(5); // 畫在磚塊前面
        m_VelocityY = 8.0f; // 往上噴發的力量
    }
    [[nodiscard]] glm::vec2 GetPosition() const { return m_Transform.translation; }
    void SetPosition(const glm::vec2& pos) { m_Transform.translation = pos; }
    void SetScale(const glm::vec2& scale) { m_Transform.scale = scale; }

    // 回傳 true 代表這個金幣的動畫跑完了，可以從陣列裡刪除了
    bool Update() {
        m_VelocityY -= 0.5f; // 重力

        glm::vec2 pos = m_Transform.translation;
        pos.y += m_VelocityY;
        SetPosition(pos);

        // 如果往下掉超過一定距離 (例如掉回原本方塊的位置)，就讓它消失
        m_DistanceTimer++;
        return m_DistanceTimer > 30; // 假設 30 幀後消失 (大約半秒)
    }

private:
    float m_VelocityY;
    int m_DistanceTimer = 0;
};

#endif
