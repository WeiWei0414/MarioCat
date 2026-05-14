//
// Created by eatba on 2026/4/17.
//

#ifndef REPLACE_WITH_YOUR_PROJECT_NAME_EVENTBLOCK_HPP
#define REPLACE_WITH_YOUR_PROJECT_NAME_EVENTBLOCK_HPP
#ifndef EVENT_BLOCK_HPP
#define EVENT_BLOCK_HPP

#include "Block.hpp"
#include "ImageManager.hpp" // 🌟 引入 ImageManager，因為我們要在這裡直接換圖

// 事件方塊：專門用來處理「頂了會發生特定事件(依據地圖數字)」的方塊
class EventBlock : public Block {
public:
    // 建構子：接收圖片並傳給父類別 Block，同時記下地圖數字
    EventBlock(const std::string& imageName, int eventID)
        : Block(imageName), m_EventID(eventID) {}

    // 取得這塊方塊的地圖數字
    [[nodiscard]] int GetEventID() const { return m_EventID; }

    [[nodiscard]] bool IsActivated() const { return m_IsActivated; }

    // 貓咪撞到它時呼叫
    void Activate() {
        if (!m_IsActivated) {
            m_IsActivated = true;

            SetDrawable(ImageManager::Get("block_empty"));
        }
    }

    [[nodiscard]] bool HasSpawned() const { return m_HasSpawned; }
    void SetSpawned(bool spawned) { m_HasSpawned = spawned; }

private:
    int m_EventID;
    bool m_IsActivated = false;
    bool m_HasSpawned = false;
};

#endif
#endif //REPLACE_WITH_YOUR_PROJECT_NAME_EVENTBLOCK_HPP