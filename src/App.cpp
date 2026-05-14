#include "App.hpp"

#include "Util/Image.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"
#include "ImageManager.hpp"
#include "Player.hpp"
#include "MapManager.hpp"
#include <algorithm>
#include "Block.hpp"
#include "EventBlock.hpp"

void App::Start() {
    LOG_TRACE("Start");

    ImageManager::LoadAll();
    m_Player=std::make_shared<Player>();
    m_Player->SetPosition({-500.0f, 0.0f});

    LevelDate mydata=MapManager::LoadMap(RESOURCE_DIR "/Map/level1.txt");
    m_Blocks=mydata.blocks;
    m_Enemies=mydata.enemies;

    m_Background=std::make_shared<Util::GameObject>();
    m_Background->SetDrawable(ImageManager::Get("bg_blue"));
    m_Background->SetZIndex(-10);

    m_CurrentState = State::UPDATE;
}

void App::Update() {
    // 1. 畫背景 (背景不受縮放與攝影機影響，直接填滿最底層)
    m_Background->Draw();

    // 2. 物理運算 (維持在真實比例下運作，確保碰撞正確)
    m_Player->Update(m_Blocks);

    for (auto& enemy : m_Enemies)
    {
        enemy->Update(m_Blocks);
    }
    // 3. 攝影機與縮放設定
    float startCameraX = -213.0f;
    static float maxCameraX = startCameraX;

    float targetCameraX = std::max(startCameraX, m_Player->GetPosition().x);
    float zoom = 1.5f; // 🌟 你的放大倍率，大於 1 就是放大

    if (targetCameraX > maxCameraX) {
        maxCameraX = targetCameraX;
    }
    float cameraX = maxCameraX;

    // 左側隱形牆限制 (如果你覺得放大後能往回走的空間變大了，可以把這裡的 -600.0f 調小一點)
    float leftEdge = cameraX - 385.0f;
    if (m_Player->GetPosition().x < leftEdge)
    {
        m_Player->SetPosition({leftEdge, m_Player->GetPosition().y});
    }

    // ==========================================
    // 4. 繪圖階段：套用縮放與偏移的障眼法
    // ==========================================

    // -- 處理所有磚塊 --
    RenderWithCamera(m_Blocks, cameraX, zoom);
    RenderWithCamera(m_Enemies, cameraX, zoom);

    // -- 處理貓咪 --
    glm::vec2 realPlayerPos = m_Player->GetPosition(); // 記住貓咪的物理座標

    // 套用縮放與偏移
    m_Player->SetPosition({(realPlayerPos.x - cameraX) * zoom, realPlayerPos.y * zoom});
    m_Player->SetScale({zoom, zoom});

    m_Player->Draw(); // 畫出巨大化的貓咪

    // 恢復原狀
    m_Player->SetPosition(realPlayerPos);
    m_Player->SetScale({1.0f, 1.0f});

    m_Blocks.erase(
         std::remove_if(m_Blocks.begin(), m_Blocks.end(), [](const std::shared_ptr<Block>& block) {
             return block->IsDestoryed();
         }),
         m_Blocks.end()
     );
    // ==========================================
    // 死亡邏輯
    // ==========================================
    if (m_Player->GetPosition().y<-400.0f)
    {
        m_Player->Die();
    }
    if (m_Player->GetLives() <= 0) {
        // 切換到結束畫面或重新開始
        m_CurrentState = State::END;
        return; // 直接中斷這幀的 Update，避免畫面上出現奇怪的殘影
    }
    for (const auto& enemy:m_Enemies)
    {

        if (m_Player->IfCollides(enemy))
        {
            if (m_Player->GetVelocity().y<0.0f && m_Player->GetPosition().y>enemy->GetPosition().y)
            {
                enemy->Die();
                m_Player->Bounce();
            }
            else
            {
                m_Player->Die();
            }
        }
    }
    m_Enemies.erase(
        std::remove_if(m_Enemies.begin(), m_Enemies.end(), [](const std::shared_ptr<Enemy>& enemy) {
            return enemy->IsDead();
        }),
        m_Enemies.end()
    );
    // ==========================================
    // 5. 其他邏輯
    // ==========================================
    for (auto& block : m_Blocks)
    {
        // 檢查它是不是 EventBlock
        auto eventBlock = std::dynamic_pointer_cast<EventBlock>(block);

        if (eventBlock && eventBlock->IsActivated() && !eventBlock->HasSpawned())
        {
            int actionID = eventBlock->GetEventID(); // 🌟 拿出地圖數字！
            switch (actionID)
            {
            case 927:
                {
                    break;
                }
                default:
                break;
            }

        }
    }
    if (Util::Input::IsKeyPressed(Util::Keycode::R))
    {
        m_Player->SetPosition({-500.0f, 0.0f});
        maxCameraX = -150.0f;
    }

    if (Util::Input::IsKeyUp(Util::Keycode::ESCAPE) ||
        Util::Input::IfExit()) {
        m_CurrentState = State::END;
    }
}

void App::End() {
    LOG_TRACE("End");
}