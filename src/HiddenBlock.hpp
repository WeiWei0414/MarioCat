//
// Created by eatba on 2026/5/15.
//

#ifndef REPLACE_WITH_YOUR_PROJECT_NAME_HIDDENBLOCK_HPP
#define REPLACE_WITH_YOUR_PROJECT_NAME_HIDDENBLOCK_HPP

#include "Block.hpp"
#include "ImageManager.hpp"
class HiddenBlock : public Block
{
public:
    explicit HiddenBlock() : Block("brick_3") {
        SetVisible(false); // 🌟 預設為隱形
    }
    [[nodiscard]] bool IsHidden() const { return m_IsHidden; }
    void Reveal()
    {
        if (m_IsHidden)
        {
            m_IsHidden=false;
            SetVisible(true);
            SetDrawable(ImageManager::Get("brick_3"));
        }
    }
private:
    bool m_IsHidden = true; // 狀態標記

};
#endif //REPLACE_WITH_YOUR_PROJECT_NAME_HIDDENBLOCK_HPP