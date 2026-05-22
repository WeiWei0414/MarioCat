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
    m_Decorations = mydata.decorations;
    m_Background=std::make_shared<Util::GameObject>();
    m_Background->SetDrawable(ImageManager::Get("bg_blue"));
    m_Background->SetZIndex(-10);

    m_CurrentState = State::UPDATE;
}

void App::Update() {
    // 1. 畫背景 (背景不受縮放與攝影機影響，直接填滿最底層)
    m_Background->Draw();
    for (auto& block : m_Blocks)
    {
        auto eventBlock = std::dynamic_pointer_cast<EventBlock>(block);
        if (eventBlock && eventBlock->GetEventID() == 91)
        {
            glm::vec2 pPos = m_Player->GetPosition();
            glm::vec2 bPos = eventBlock->GetPosition();

            // 1. 計算兩者不重疊的「安全距離」(方塊半高 + 貓咪半高 + 1.0f 微小空隙)
            float safeDistance = (eventBlock->GetScaledSize().y / 2.0f) + (m_Player->GetScaledSize().y / 2.0f) + 1.0f;

            // 2. 動態預判：把貓咪「這幀即將移動的速度」加進觸發距離
            float touchDistance = safeDistance + m_Player->GetVelocity().y;

            bool is_under = pPos.y < bPos.y;
            bool is_alignedX = std::abs(bPos.x - pPos.x) < 40.0f;
            bool is_touching = (bPos.y - pPos.y) <= touchDistance; // 只要進入這幀的射程範圍就觸發！

            // 如果貓咪往上跳，且預判這幀會撞上
            if (is_under && is_alignedX && is_touching && m_Player->GetVelocity().y > 0.0f)
            {
                // 🌟 絕對防禦魔法：精準計算貓咪這幀結束時的高度，把方塊硬推到貓咪頭頂上 1 像素的地方！
                float predictedPlayerY = pPos.y + m_Player->GetVelocity().y;
                bPos.y = predictedPlayerY + safeDistance;

                eventBlock->SetPosition(bPos);
            }
        }
    }
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
    RenderWithCamera(m_Decorations, cameraX, zoom);
    RenderWithCamera(m_Coins, cameraX, zoom);
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
    m_Coins.erase(
            std::remove_if(m_Coins.begin(), m_Coins.end(), [](const std::shared_ptr<Coin>& coin) {
                return coin->Update(); // Update 回傳 true 就代表該刪除了
            }),
            m_Coins.end()
        );

    // 2. 畫出金幣 (記得加到 RenderWithCamera 裡面，讓它會跟著畫面縮放！)
    // 把這行加在 RenderWithCamera(m_Blocks, ...) 的附近

    for (auto& block : m_Blocks) {
        auto eventBlock = std::dynamic_pointer_cast<EventBlock>(block);

        // 如果它是事件方塊，被撞到了 (Activated)，而且還沒生成過獎勵 (!HasSpawned)
        if (eventBlock && eventBlock->IsActivated() && !eventBlock->HasSpawned()) {

            int actionID = eventBlock->GetEventID();
            switch (actionID) {

            case 2: { // 🌟 噴金幣方塊

                    // 在方塊「正上方」生成一枚金幣！
                    glm::vec2 coinPos = eventBlock->GetPosition();
                    coinPos.y += eventBlock->GetScaledSize().y;

                    m_Coins.push_back(std::make_shared<Coin>(coinPos));
                    break;
            }
            case 93:
                {
                    auto enemy=std::make_shared<Enemy>("teki_1");
                    glm::vec2 spawnPos = eventBlock->GetPosition();
                    float blockHalfHeight = eventBlock->GetScaledSize().y / 2.0f;
                    float enemyHalfHeight = enemy->GetScaledSize().y / 2.0f;
                    spawnPos.y += blockHalfHeight + enemyHalfHeight + 5.0f;
                    enemy->SetPosition(spawnPos);
                    m_Enemies.push_back(enemy);
                }

                // (未來如果還有噴蘑菇、現形等事件都寫在這裡)
            }

            // 標記為已生成，這樣下一幀就不會再噴一次了！
            eventBlock->SetSpawned(true);
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