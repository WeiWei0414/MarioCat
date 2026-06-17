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
#include "StarEnemy.hpp"
#include "TurtleEnemy.hpp"
#include "EventBlock.hpp"
#include "FlyingEnemy.hpp"
#include "NormalEnemy.hpp"
#include "HiddenBlock.hpp"
#include "SpikyEnemy.hpp"
void App::Start() {
    LOG_TRACE("Start");

    ImageManager::LoadAll();
    m_Player = std::make_shared<Player>();
    // 這裡不用 SetPosition 了，LoadLevel 裡面會做

    m_Background = std::make_shared<Util::GameObject>();
    m_Background->SetDrawable(ImageManager::Get("bg_blue"));
    m_Background->SetZIndex(-10);
    m_CurrentLevel = 3;
    LoadLevel(m_CurrentLevel);
    m_CurrentState = State::UPDATE;

}

void App::Update() {
    // 1. 畫背景 (背景不受縮放與攝影機影響，直接填滿最底層)
    m_Background->Draw();
    // 3. 攝影機與縮放設定
    float startCameraX = -213.0f;
    float targetCameraX = std::max(startCameraX, m_Player->GetPosition().x);
    float zoom = 1.5f;

    // 🌟 全面換成 m_MaxCameraX
    if (targetCameraX > m_MaxCameraX) {
        m_MaxCameraX = targetCameraX;
    }
    float cameraX = m_MaxCameraX;

    // 左側隱形牆限制
    float leftEdge = cameraX - 385.0f;



    if (m_HorizPipePhase == 0 &&m_Player->GetPosition().x < leftEdge)
    {
        m_Player->SetPosition({leftEdge, m_Player->GetPosition().y});
    }
    if (m_HorizPipePhase == 0 && Util::Input::IsKeyDown(Util::Keycode::D)) {
        for (auto& block : m_Blocks) {
            auto eventBlock = std::dynamic_pointer_cast<EventBlock>(block);
            if (eventBlock && eventBlock->GetEventID() == 411) {

                glm::vec2 pPos = m_Player->GetPosition();
                glm::vec2 bPos = eventBlock->GetPosition();

                // 條件判斷：貓咪在左邊、Y軸對齊、且距離很近
                bool is_left = pPos.x < bPos.x;
                bool is_alignedY = std::abs(pPos.y - bPos.y) < 20.0f;
                // 計算貓咪右邊緣跟水管左邊緣的距離
                float distX = std::abs((bPos.x - eventBlock->GetScaledSize().x / 2.0f) - (pPos.x + m_Player->GetScaledSize().x / 2.0f));

                if (is_left && is_alignedY && distX < 15.0f) {
                    m_HorizPipePhase = 1;
                    m_HorizPipeTimer = 25; // 階段 1：慢慢走進去的時間 (可微調)

                    m_Player->SetZIndex(-1); // 躲到水管圖層後面
                    m_Player->SetPosition({pPos.x, bPos.y}); // 強制把 Y 軸對齊水管中心
                    break;
                }
            }
        }
    }
    if (m_PipeAnimPhase == 0 && Util::Input::IsKeyDown(Util::Keycode::S)) {
        for (auto& block : m_Blocks) {
            auto eventBlock = std::dynamic_pointer_cast<EventBlock>(block);
            if (eventBlock && eventBlock->GetEventID() == 42) {

                glm::vec2 pPos = m_Player->GetPosition();
                glm::vec2 bPos = eventBlock->GetPosition();

                bool is_above = pPos.y > bPos.y;
                bool is_alignedX = std::abs(pPos.x - bPos.x) < 20.0f;
                float distY = std::abs((pPos.y - m_Player->GetScaledSize().y / 2.0f) - (bPos.y + eventBlock->GetScaledSize().y / 2.0f));

                if (is_above && is_alignedX && distY < 10.0f) {


                    // 🌟 設定進入第一階段
                    m_PipeAnimPhase = 1;
                    m_PipeAnimationTimer = 60; // 第一階段播 60 幀

                    // 🌟 記住這根水管！我們等等要拿它來抖動！

                    m_OriginalPipeX = bPos.x;

                    m_Player->SetZIndex(-1);
                    m_Player->SetPosition({bPos.x, pPos.y});
                    m_Decorations.erase(
                        std::remove_if(m_Decorations.begin(), m_Decorations.end(),
                            [&bPos](const std::shared_ptr<Decoration>& deco) {
                                glm::vec2 dPos = deco->GetPosition();
                                // 檢查裝飾品的座標是不是跟水管非常接近 (容許 20 像素的微調誤差)
                                bool isCloseX = std::abs(dPos.x - bPos.x) < 50.0f;
                                bool isCloseY = std::abs(dPos.y - bPos.y) < 100.0f;

                                // 如果很近，就代表它是這個水管的告示牌，回傳 true 讓 erase 刪除它！
                                return isCloseX && isCloseY;
                            }),
                        m_Decorations.end()
                    );
                    break;
                }
            }
        }
    }
    // ==========================================
    // 🌟 44號：通關水管 (按 S 進入並切換關卡)
    // ==========================================
    if (m_ClearPipePhase == 0 && Util::Input::IsKeyDown(Util::Keycode::S)) {
        for (auto& block : m_Blocks) {
            auto eventBlock = std::dynamic_pointer_cast<EventBlock>(block);
            if (eventBlock && eventBlock->GetEventID() == 44) {

                glm::vec2 pPos = m_Player->GetPosition();
                glm::vec2 bPos = eventBlock->GetPosition();

                bool is_above = pPos.y > bPos.y;
                bool is_alignedX = std::abs(pPos.x - bPos.x) < 20.0f;
                // 計算貓咪腳底與水管頂部的距離
                float distY = std::abs((pPos.y - m_Player->GetScaledSize().y / 2.0f) - (bPos.y + eventBlock->GetScaledSize().y / 2.0f));

                // 只有站在水管正上方且距離很近時才觸發
                if (is_above && is_alignedX && distY < 10.0f) {


                    m_ClearPipePhase = 1;
                    m_ClearPipeTimer = 60; // 沉下去的時間 (60幀大約1秒)

                    m_Player->SetZIndex(-1); // 躲到水管圖層後面
                    m_Player->SetPosition({bPos.x, pPos.y}); // X軸強制對齊水管中心
                    break;
                }
            }
        }
    }
    if (m_ClearPipePhase > 0) {
        glm::vec2 pos = m_Player->GetPosition();

        if (m_ClearPipePhase == 1) {
            pos.y -= 1.5f; // 貓咪慢慢下沉
            m_ClearPipeTimer--;

            // 當計時器結束 (貓咪已經完全沉入水管)
            if (m_ClearPipeTimer <= 0) {
                LOG_INFO("水管通關！前往下一關！");
                m_CurrentLevel++;
                LoadLevel(m_CurrentLevel); // 載入第三關

                // 🌟 絕對防禦魔法：防止幽靈座標，載入完立刻退出這一幀！
                return;
            }
        }

        m_Player->SetPosition(pos);

        // --- 繪圖 (維持動畫期間的畫面，不執行普通物理與操控) ---
        RenderWithCamera(m_Blocks, cameraX, zoom);
        RenderWithCamera(m_Enemies, cameraX, zoom);
        RenderWithCamera(m_Decorations, cameraX, zoom);
        RenderWithCamera(m_Coins, cameraX, zoom); // 順便把金幣也畫出來以免消失

        glm::vec2 realPlayerPos = m_Player->GetPosition();
        m_Player->SetPosition({(realPlayerPos.x - cameraX) * zoom, realPlayerPos.y * zoom});
        m_Player->SetScale({zoom, zoom});
        m_Player->Draw();
        m_Player->SetPosition(realPlayerPos);
        m_Player->SetScale({1.0f, 1.0f});

        return; // 凍結其他所有操作
    }
    if (m_HorizPipePhase > 0) {
        glm::vec2 pos = m_Player->GetPosition();

        // --- 階段 1：慢慢走進水管 ---
        if (m_HorizPipePhase == 1) {
            pos.x += 1.5f; // 模擬走路速度
            m_HorizPipeTimer--;

            if (m_HorizPipeTimer <= 0) {
                m_HorizPipePhase = 2; // 準備噴射
            }
        }
        // --- 階段 2：超高速向左噴射 ---
        else if (m_HorizPipePhase == 2) {
            pos.x -= 15.0f; // 🌟 超高速向左衝刺！(數字越大飛越快)

            // 檢查是否飛出畫面左邊界 (假設攝影機左邊緣再往左 200 像素)
            if (pos.x < cameraX - 1000.0f) {

                m_Player->SetZIndex(5);  // 把圖層拉回來
                m_Player->Die();         // 死亡判定
                m_HorizPipePhase = 0;    // 結束動畫
            }
        }

        m_Player->SetPosition(pos);


        RenderWithCamera(m_Blocks, cameraX, zoom);
        RenderWithCamera(m_Enemies, cameraX, zoom);
        RenderWithCamera(m_Decorations, cameraX, zoom);

        glm::vec2 realPlayerPos = m_Player->GetPosition();
        m_Player->SetPosition({(realPlayerPos.x - cameraX) * zoom, realPlayerPos.y * zoom});
        m_Player->SetScale({zoom, zoom});
        m_Player->Draw();
        m_Player->SetPosition(realPlayerPos);
        m_Player->SetScale({1.0f, 1.0f});

        return; // 凍結其他所有操作
    }
    if (m_PipeAnimPhase > 0) {

        m_PipeAnimationTimer--;

        // 🎬 劇本 1：貓咪下沉
        if (m_PipeAnimPhase == 1) {
            glm::vec2 pos = m_Player->GetPosition();
            pos.y -= 1.4f;
            m_Player->SetPosition(pos);

            if (m_PipeAnimationTimer <= 0) {
                m_PipeAnimPhase = 2;
                m_PipeAnimationTimer = 40;
            }
        }
        // 🎬 劇本 2 & 3：水管發抖與升天
        else if (m_PipeAnimPhase == 2 || m_PipeAnimPhase == 3) {

            // 🌟 既然不存指標，我們就當場把它找出來！
            for (auto& block : m_Blocks) {
                auto eventBlock = std::dynamic_pointer_cast<EventBlock>(block);
                if (eventBlock && eventBlock->GetEventID() == 42) {

                    glm::vec2 pipePos = eventBlock->GetPosition();
                    glm::vec2 catPos = m_Player->GetPosition();

                    // --- 劇本 2：水管發抖 ---
                    if (m_PipeAnimPhase == 2) {
                        float offset = (m_PipeAnimationTimer % 4 < 2) ? 4.0f : -4.0f;
                        pipePos.x = m_OriginalPipeX + offset;

                        catPos.x = pipePos.x; // 貓咪跟著抖

                        if (m_PipeAnimationTimer <= 0) {
                            pipePos.x = m_OriginalPipeX; // 抖完擺正
                            m_PipeAnimPhase = 3;
                            m_PipeAnimationTimer = 180;
                        }
                    }
                    // --- 劇本 3：帶貓升天 ---
                    else if (m_PipeAnimPhase == 3) {
                        pipePos.y += 10.0f;
                        catPos.y += 10.0f;

                        if (m_PipeAnimationTimer <= 0 || pipePos.y > 1500.0f) {
                            m_PipeAnimPhase = 0;
                            m_Player->SetZIndex(5);
                            m_Player->Die(); // 演出結束，整死玩家
                        }
                    }

                    // 套用新座標
                    eventBlock->SetPosition(pipePos);
                    m_Player->SetPosition(catPos);
                    break; // 找到了就跳出迴圈，不用繼續找了
                }
            }
        }
        //水管飛天
        RenderWithCamera(m_Blocks, cameraX, zoom);
        RenderWithCamera(m_Enemies, cameraX, zoom);
        RenderWithCamera(m_Decorations, cameraX, zoom);

        // (套用縮放的畫貓咪邏輯)
        glm::vec2 realPlayerPos = m_Player->GetPosition();
        m_Player->SetPosition({(realPlayerPos.x - cameraX) * zoom, realPlayerPos.y * zoom});
        m_Player->SetScale({zoom, zoom});
        m_Player->Draw();
        m_Player->SetPosition(realPlayerPos);
        m_Player->SetScale({1.0f, 1.0f});
        return;
    }
    // ==========================================
    // 🎬 劇本：旗桿過關動畫演出
    // ==========================================
if (m_FlagAnimPhase > 0) {
        glm::vec2 pos = m_Player->GetPosition();

        if (m_FlagAnimPhase == 1) {
            // --- 階段 1：受重力影響掉落 ---
            static float fallSpeed = 0.0f; // 靜態變數，用來記憶當前的下墜速度
            fallSpeed += 0.8f;             // 🌟 重力加速度 (數字越大掉越快)
            pos.y -= fallSpeed;

            // 🌟 半路死判定：只要到達死亡高度，直接中斷所有動畫！
            if (m_IsTrollFlagDeath && pos.y <= m_FlagBottomY + 80.0f) {
                LOG_INFO("貪心的下場：滑桿半路死！動畫立刻中止！");
                m_Player->Die();
                m_FlagAnimPhase = 0;
                fallSpeed = 0.0f;    // 🌟 記得歸零，以免下次遊玩時速度爆表
                return;              // 🌟 立刻退出
            }

            // 正常滑到底部的判斷
            if (pos.y <= m_FlagBottomY) {
                pos.y = m_FlagBottomY; // 完美踩穩地板
                m_FlagAnimPhase = 2;   // 準備走向右邊
                m_FlagAnimTimer = 0;
                fallSpeed = 0.0f;      // 🌟 落地後速度歸零
            }
        }
        else if (m_FlagAnimPhase == 2) {
            // --- 階段 2：往右走向城堡 ---
            pos.x += 3.0f;
            m_FlagAnimTimer++;

            // 走大約 90 幀的距離後，停下來
            if (m_FlagAnimTimer > 90) {
                m_FlagAnimPhase = 3; // 🌟 進入階段 3 (停留)
                m_FlagAnimTimer = 0;
            }
        }
        else if (m_FlagAnimPhase == 3) {
            // --- 🌟 階段 3：終點前停留 1 秒鐘 ---
            m_FlagAnimTimer++;

            // 原地不動，純計時 (60 幀 = 1 秒)
            if (m_FlagAnimTimer > 60) {
                m_CurrentLevel++;
                LoadLevel(m_CurrentLevel);
                return;
            }
        }

        // --- 正常動畫期間的座標更新 ---
        m_Player->SetPosition(pos);

        // ==========================================
        // 🌟 讓世界繼續運轉，並保留「被怪或陷阱暗算」的判定！
        // ==========================================
        for (auto& enemy : m_Enemies)
        {
            enemy->Update(m_Blocks);
            if (std::dynamic_pointer_cast<FlyingEnemy>(enemy)!=nullptr)
            {
                glm::vec2 Pos = enemy->GetPosition();
                Pos.y+=12.0f; // 飛行怪物往下掉
                enemy->SetPosition(Pos);
                if (Pos.y>1000.0f)
                {
                    enemy->Die();
                }
            }

            // 動畫期間碰到真怪物，死！
            if (m_Player->IfCollides(enemy))
            {
                LOG_INFO("大意了！過關途中被怪物暗算！");
                m_Player->Die();
                m_FlagAnimPhase = 0;
                return;
            }
        }

        // 解除「偽裝成怪物的陷阱方塊」的時間暫停
        for (auto& block : m_Blocks)
        {
            auto eb = std::dynamic_pointer_cast<EventBlock>(block);
            if (eb && (eb->GetEventID() == 52 || eb->GetEventID() == 95))
            {
                glm::vec2 pPos = m_Player->GetPosition();
                glm::vec2 bPos = eb->GetPosition();

                bool is_under = pPos.y < bPos.y;
                bool is_alignedX = std::abs(bPos.x - pPos.x) < 80.0f;

                if (is_under && is_alignedX && !eb->HasSpawned())
                {
                    eb->SetSpawned(true);
                    if (eb->GetEventID() == 52) eb->SetVisible(true);
                }

                if (eb->HasSpawned())
                {
                    bPos.y -= 15.0f;
                    eb->SetPosition(bPos);

                    // 動畫期間被偽裝方塊砸到，死！
                    if (m_Player->IfCollidesWithBlock(eb))
                    {
                        LOG_INFO("慘！過關途中被偽裝方塊砸死！");
                        m_Player->Die();
                        m_FlagAnimPhase = 0;
                        return;
                    }
                }
            }
        }

        // --- 繪圖 (維持動畫期間的畫面，不執行普通物理與操控) ---
        RenderWithCamera(m_Blocks, cameraX, zoom);
        RenderWithCamera(m_Enemies, cameraX, zoom);
        RenderWithCamera(m_Decorations, cameraX, zoom);

        glm::vec2 realPlayerPos = m_Player->GetPosition();
        m_Player->SetPosition({(realPlayerPos.x - cameraX) * zoom, realPlayerPos.y * zoom});
        m_Player->SetScale({zoom, zoom});
        m_Player->Draw();
        m_Player->SetPosition(realPlayerPos);
        m_Player->SetScale({1.0f, 1.0f});

        return; // 凍結其他所有操作
    }
    //大部分陷阱
    for (auto& block : m_Blocks)
    {
        // ==========================================
        // 🌟 處理隱藏方塊 (HiddenBlock) 的頂撞與噴金幣
        // ==========================================
        auto hiddenBlock = std::dynamic_pointer_cast<HiddenBlock>(block);
        if (hiddenBlock)
        {
            // 如果它還是隱形的，我們才需要偵測貓咪有沒有頂它
            if (hiddenBlock->IsHidden())
            {
                glm::vec2 pPos = m_Player->GetPosition();
                glm::vec2 bPos = hiddenBlock->GetPosition();

                bool is_under = pPos.y < bPos.y;
                bool is_alignedX = std::abs(bPos.x - pPos.x) < 30.0f;

                // 動態預判觸碰距離
                float touchDistance = (hiddenBlock->GetScaledSize().y / 2.0f) + (m_Player->GetScaledSize().y / 2.0f) + m_Player->GetVelocity().y + 2.0f;
                bool is_touching = (bPos.y - pPos.y) <= touchDistance;

                // 只有當貓咪「往上跳 (Velocity.y > 0)」，且頭頂撞到方塊底部時才觸發！
                if (is_under && is_alignedX && is_touching && m_Player->GetVelocity().y > 0.0f)
                {
                    LOG_INFO("敲出隱藏方塊！");

                    // 1. 呼叫你寫好的 Reveal()，讓它顯形並換圖！
                    hiddenBlock->Reveal();

                    // 2. 噴出金幣！
                    glm::vec2 coinPos = hiddenBlock->GetPosition();
                    coinPos.y += hiddenBlock->GetScaledSize().y;
                    m_Coins.push_back(std::make_shared<Coin>(coinPos));

                    // 3. 物理回饋：強制把貓咪往下彈，營造「撞到磚塊」的手感
                    m_Player->SetVelocity({m_Player->GetVelocity().x, -2.0f});
                }
            }
        }
        auto eventBlock = std::dynamic_pointer_cast<EventBlock>(block);
        if (eventBlock && eventBlock->GetEventID() == 83)
        {
            glm::vec2 pPos = m_Player->GetPosition();
            glm::vec2 bPos = eventBlock->GetPosition();
            glm::vec2 pSize = m_Player->GetScaledSize();
            glm::vec2 bSize = eventBlock->GetScaledSize();

            // ⚠️ 就是這裡！我們手動把碰撞範圍「往外加 2.0f」，抵銷被推開的誤差
            bool touchX = std::abs(pPos.x - bPos.x) <= ((pSize.x + bSize.x) / 2.0f) + 2.0f;
            bool touchY = std::abs(pPos.y - bPos.y) <= ((pSize.y + bSize.y) / 2.0f) + 2.0f;

            if (touchX && touchY)
            {
                if (!eventBlock->HasSpawned()) // 避免使用 Activate() 導致變成空磚塊
                {
                    eventBlock->SetSpawned(true);
                    eventBlock->SetDrawable(ImageManager::Get("fake_cloud"));
                }

                m_Player->Die();
            }
        }
        // ==========================================
        // 🌟 46號：無限火球噴射器 (定時向上噴射，隨機弧線落下)
        // ==========================================
        if (eventBlock && eventBlock->GetEventID() == 46)
        {
            m_FireballTimers[eventBlock]++;

            // 🌟 關鍵修改：從固定的 120 幀改成讀取全域的 m_FireballInterval
            if (m_FireballTimers[eventBlock] >= m_FireballInterval)
            {
                m_FireballTimers[eventBlock] = 0;

                auto fireball = std::make_shared<SpikyEnemy>("fireball");
                fireball->SetAsFireball(true); // 讓牠獲得碰到地板就死亡的特權

                glm::vec2 bPos = eventBlock->GetPosition();
                float blockHalfHeight = eventBlock->GetScaledSize().y / 2.0f;
                float fireballHalfHeight = fireball->GetScaledSize().y / 2.0f;
                float spawnY = bPos.y + blockHalfHeight + fireballHalfHeight + 2.0f;
                fireball->SetPosition({bPos.x, spawnY});

                // 🌟 移動速度保持原本最完美的經典設定，不隨道具改變
                fireball->SetVelocity({0.0f, 14.0f});

                float randomDir = (rand() % 2 == 0) ? 1.0f : -1.0f;
                fireball->SetDirection(randomDir);

                m_Enemies.push_back(fireball);
            }
        }
        // ==========================================
        // 🌟 47號：火球狂暴道具 (玩家碰觸後吃掉，發射頻率翻倍)
        // ==========================================
        if (eventBlock && eventBlock->GetEventID() == 47)
        {
            glm::vec2 pPos = m_Player->GetPosition();
            glm::vec2 bPos = eventBlock->GetPosition();
            glm::vec2 pSize = m_Player->GetScaledSize();
            glm::vec2 bSize = eventBlock->GetScaledSize();

            // 🌟 核心修正：採用全方向 AABB 碰撞偵測（加上 2.0f 容錯，確保任何方向擦到都算數）
            bool touchX = std::abs(pPos.x - bPos.x) <= ((pSize.x + bSize.x) / 2.0f) + 2.0f;
            bool touchY = std::abs(pPos.y - bPos.y) <= ((pSize.y + bSize.y) / 2.0f) + 2.0f;

            // 不管是從左、右、上、下哪一個角度碰到，只要 X 與 Y 同時重疊就觸發！
            if (touchX && touchY)
            {


                m_FireballInterval /= 10;

                if (m_FireballInterval < 20) m_FireballInterval = 20;

                // 道具功成身退，將其標記為銷毀
                eventBlock->SetDestoryed(true);
            }
        }

        if (eventBlock && eventBlock->GetEventID() == 45)
        {
            if (m_SpawnCounts[eventBlock] >= 2)
            {
                continue;
            }

            glm::vec2 pPos = m_Player->GetPosition();
            glm::vec2 bPos = eventBlock->GetPosition();

            // 雷達範圍
            bool is_closeX = std::abs(pPos.x - bPos.x) < 200.0f;
            bool is_closeY = std::abs(pPos.y - bPos.y) < 300.0f;

            if (is_closeX && is_closeY)
            {
                // 2. 查閱畫面上是否已有怪物存活
                bool canSpawn = true;
                if (m_SpawnerTracker.count(eventBlock) > 0) {
                    if (!m_SpawnerTracker[eventBlock].expired()) {
                        canSpawn = false; // 畫面上那隻還活著，不准生！
                    }
                }

                // 3. 如果可以生成 (上一次生成的已經徹底死掉清空了)
                if (canSpawn)
                {
                    // 🌟 關鍵新增：累計這塊方塊的生成次數！
                    m_SpawnCounts[eventBlock]++;

                    auto enemy = std::make_shared<NormalEnemy>("cat_teki");

                    float blockHalfHeight = eventBlock->GetScaledSize().y / 2.0f;
                    float enemyHalfHeight = enemy->GetScaledSize().y / 2.0f;
                    float spawnY = bPos.y + blockHalfHeight + enemyHalfHeight + 2.0f;
                    enemy->SetPosition({bPos.x, spawnY});

                    enemy->SetVelocity({0.0f, 9.0f});
                    float dir = (pPos.x < bPos.x) ? -1.0f : 1.0f;
                    enemy->SetDirection(dir);

                    m_Enemies.push_back(enemy);

                    // 登錄到監視名單
                    m_SpawnerTracker[eventBlock] = enemy;
                }
            }
        }
        if (eventBlock && eventBlock->GetEventID() == 48)
        {
            // 1. 初始化：如果是第一次遇到這個平台，記住它的起點
            if (m_PlatformInitialPos.count(eventBlock) == 0) {
                m_PlatformInitialPos[eventBlock] = eventBlock->GetPosition();
                m_PlatformAngles[eventBlock] = 0.0f;
            }

            glm::vec2 oldBlockPos = eventBlock->GetPosition();

            // 2. 預先偵測：在平台移動前，檢查貓咪是不是「正踩在平台上方」
            glm::vec2 pPos = m_Player->GetPosition();
            glm::vec2 pSize = m_Player->GetScaledSize();
            glm::vec2 bSize = eventBlock->GetScaledSize();

            bool is_above = pPos.y > oldBlockPos.y;
            bool is_alignedX = std::abs(pPos.x - oldBlockPos.x) < ((pSize.x + bSize.x) / 2.0f) - 2.0f; // X軸有重疊
            float distY = std::abs((pPos.y - pSize.y / 2.0f) - (oldBlockPos.y + bSize.y / 2.0f));     // 腳底距離平台頂部

            // 只要距離小於 4 像素，就認定玩家正「騎」在升降梯上
            bool isPlayerRiding = is_above && is_alignedX && distY <= 4.0f;

            // 3. 移動平台：使用數學正弦波 (std::sin) 算出平滑的上下軌道
            m_PlatformAngles[eventBlock] += 0.02f; // 🌟 控制移動速度 (數字越大擺動越快)

            // 算出的新 Y 座標 = 初始高度 + (sin值 * 移動幅度)
            float newBlockY = m_PlatformInitialPos[eventBlock].y + std::sin(m_PlatformAngles[eventBlock]) * 240.0f; // 🌟 120.0f 是上下移動的範圍半徑

            eventBlock->SetPosition({oldBlockPos.x, newBlockY});

            // 4. 🌟 核心魔法：如果玩家在上面，強行幫玩家補上平台的位移落差（Delta Y）！
            if (isPlayerRiding)
            {
                float deltaY = newBlockY - oldBlockPos.y; // 算出平台這一幀移動了幾像素

                glm::vec2 playerPos = m_Player->GetPosition();
                playerPos.y += deltaY; // 讓貓咪同步上升或下降！
                m_Player->SetPosition(playerPos);
            }
        }
        if (eventBlock && eventBlock->GetEventID() == 53)
        {
            // 確保這個陷阱還沒發射過
            if (!eventBlock->HasSpawned())
            {
                glm::vec2 pPos = m_Player->GetPosition();
                glm::vec2 bPos = eventBlock->GetPosition();

                // 1. 設定感應雷達範圍：X 軸距離小於 100 像素 (玩家非常靠近了)
                bool is_closeX = std::abs(pPos.x - bPos.x) < 100.0f;

                // (選用) Y 軸高度限制：確保貓咪不要離方塊太遠 (例如在天上飛時不要誤觸)
                bool is_closeY = std::abs(pPos.y - bPos.y) < 300.0f;

                // 2. 只要玩家踏入雷達範圍
                if (is_closeX && is_closeY)
                {


                    auto enemy = std::make_shared<SpikyEnemy>("spiky_teki");

                    // ==========================================
                    // 🌟 關鍵修正：精算出生高度，確保完全不跟方塊重疊！
                    // ==========================================
                    float blockHalfHeight = eventBlock->GetScaledSize().y / 2.0f;
                    float enemyHalfHeight = enemy->GetScaledSize().y / 2.0f;

                    // 加上兩者的半高，再額外多加 2.0 像素的安全距離
                    float spawnY = bPos.y + blockHalfHeight + enemyHalfHeight + 2.0f;
                    enemy->SetPosition({bPos.x, spawnY});

                    // 重新賦予向上的彈射初速度！
                    enemy->SetVelocity({0.0f, 15.0f});
                    enemy->SetDirection(-1);
                    // 🌟 智能追蹤：讓怪物一彈出來，就朝著玩家的方向走！
                    // (如果玩家在左邊，dir 就是 -1.0f；在右邊就是 1.0f)
                    float dir = (pPos.x < bPos.x) ? -1.0f : 1.0f;
                    enemy->SetDirection(dir);

                    m_Enemies.push_back(enemy);
                    eventBlock->SetSpawned(true);
                }
            }
        }
        if (eventBlock && eventBlock->GetEventID() == 84)
        {
            glm::vec2 pPos = m_Player->GetPosition();
            glm::vec2 bPos = eventBlock->GetPosition();

            // 1. 設定感應雷達範圍：X 軸左右 150 像素內，且貓咪在感應器下方
            bool inZoneX = std::abs(pPos.x - bPos.x) < 150.0f;
            bool isUnder = pPos.y < bPos.y;

            // 2. 只要玩家進入區域，且這個陷阱還沒被觸發過 (!HasSpawned)
            if (inZoneX && isUnder && !eventBlock->HasSpawned())
            {


                // 一口氣產生 4 隻怪物
                for (int i = 0; i < 4; ++i)
                {
                    // 呼叫我們之前寫好的 NormalEnemy (預設 avoidCliff = false，所以牠們會笨笨地掉下懸崖)
                    auto enemy = std::make_shared<NormalEnemy>("teki_1");

                    // 🌟 精算生成位置：以感應器為中心，向左右平均散開
                    // (i - 1.5f) 會產出 -1.5, -0.5, 0.5, 1.5，乘上 60.0f 的間距
                    float offsetX = (i - 1.5f) * 60.0f;

                    // 高度設定在感應器的上方 100 像素，製造從天而降的感覺
                    glm::vec2 spawnPos = { bPos.x + offsetX, bPos.y + 100.0f };

                    enemy->SetPosition(spawnPos);

                    // 將怪物加入陣列，物理引擎與碰撞系統會自動接手後續的重力掉落與殺人判斷
                    m_Enemies.push_back(enemy);
                }

                // 3. 標記為已觸發，確保這個陷阱只會掉一次怪物！
                eventBlock->SetSpawned(true);
            }
        }
        if (eventBlock && eventBlock->GetEventID() == 85)
        {
            glm::vec2 pPos = m_Player->GetPosition();
            glm::vec2 bPos = eventBlock->GetPosition();

            // 1. 狀態判斷：如果還沒發射，開啟雷達偵測 (改用 HasSpawned)
            if (!eventBlock->HasSpawned())
            {
                // 雷達條件：
                // 貓咪在棒子的左邊且距離小於 300 像素 (X 軸警戒區)
                bool inRangeX = (bPos.x - pPos.x) > 0.0f && (bPos.x - pPos.x) < 300.0f;
                // 貓咪的高度跟棒子差不多 (Y 軸誤差在 60 像素內，代表在同一水平線上)
                bool inRangeY = std::abs(pPos.y - bPos.y) < 60.0f;

                if (inRangeX && inRangeY)
                {
                    // 🌟 關鍵修改：用 SetSpawned 取代 Activate，就不會被強制換圖片了！
                    eventBlock->SetSpawned(true);
                }
            }

            // 2. 狀態判斷：觸發後，無情地往左邊衝刺！(改用 HasSpawned)
            if (eventBlock->HasSpawned())
            {
                eventBlock->SetVisible(true);
                bPos.x -= 10.0f; // 突刺速度
                eventBlock->SetPosition(bPos);

                // 3. 殺人判定
                glm::vec2 pSize = m_Player->GetScaledSize();
                glm::vec2 bSize = eventBlock->GetScaledSize();

                bool touchX = std::abs(pPos.x - bPos.x) <= ((pSize.x + bSize.x) / 2.0f) + 1.0f;
                bool touchY = std::abs(pPos.y - bPos.y) <= ((pSize.y + bSize.y) / 2.0f) + 1.0f;

                if (touchX && touchY)
                {
                    m_Player->Die();
                }
            }
        }
        if (eventBlock && eventBlock->GetEventID() == 88)
        {
            glm::vec2 pPos = m_Player->GetPosition();
            glm::vec2 bPos = eventBlock->GetPosition();
            glm::vec2 pSize = m_Player->GetScaledSize();
            glm::vec2 bSize = eventBlock->GetScaledSize();

            // 1. 條件判定：貓咪必須在方塊上方，且 X 軸有重疊
            // (X 軸容錯減去 4.0f，避免玩家只是從邊緣擦過去也被刺死)
            bool is_above = pPos.y > bPos.y;
            bool is_alignedX = std::abs(bPos.x - pPos.x) < ((pSize.x + bSize.x) / 2.0f) - 4.0f;

            // 2. 計算距離：精算「貓咪腳底板」與「方塊頂部」的距離
            float distY = std::abs((pPos.y - pSize.y / 2.0f) - (bPos.y + bSize.y / 2.0f));

            // 3. 當距離極近 (貼在上面)，且貓咪「往下掉落或站立 (Velocity.y <= 0)」時觸發！
            if (is_above && is_alignedX && distY <= 2.0f && m_Player->GetVelocity().y <= 0.0f)
            {
                // 如果陷阱還沒被觸發過，就讓尖刺現形！
                if (is_above && is_alignedX && distY <= 2.0f && m_Player->GetVelocity().y <= 0.0f)
                {
                    if (!eventBlock->HasSpawned())
                    {

                        auto spike = std::make_shared<Decoration>("spike");

                        // 設定座標：讓尖刺比方塊稍微高一點點，營造「刺穿出來」的感覺
                        glm::vec2 spikePos = bPos;

                        spike->SetPosition(spikePos);

                        // 設定圖層：讓尖刺畫在方塊的上面
                        spike->SetZIndex(-5);

                        // 丟進大管家的裝飾品清單！
                        // 這樣它就會跟著畫面一起被畫出來，而且完全「沒有物理碰撞」，不會干擾系統！
                        m_Decorations.push_back(spike);

                        eventBlock->SetSpawned(true);
                    }


                    m_Player->Die(); // 無情擊殺
                }

            }
        }
        if (eventBlock && eventBlock->GetEventID() == 99 && m_FlagAnimPhase == 0)
        {
            glm::vec2 pPos = m_Player->GetPosition();
            glm::vec2 bPos = eventBlock->GetPosition();
            glm::vec2 pSize = m_Player->GetScaledSize();
            glm::vec2 bSize = eventBlock->GetScaledSize();

            // 🌟 核心記憶體：記住貓咪上一刻到底在左邊還是右邊！
            static bool wasOnLeft = true;

            // 如果貓咪完全退到旗桿左邊安全區，就記住它在左邊
            if (pPos.x < bPos.x - (bSize.x / 2.0f) - 5.0f) {
                wasOnLeft = true;
            }
            // 如果貓咪完全退到旗桿右邊安全區，就記住它在右邊
            else if (pPos.x > bPos.x + (bSize.x / 2.0f) + 5.0f) {
                wasOnLeft = false;
            }

            // 算觸碰範圍 (因為步驟1已經把物理推擠拔掉了，容錯值用正常的 2.0f 即可)
            bool touchX = std::abs(pPos.x - bPos.x) <= ((pSize.x + bSize.x) / 2.0f) + 2.0f;
            bool touchY = std::abs(pPos.y - bPos.y) <= ((pSize.y + bSize.y) / 2.0f) + 2.0f;

            // 🌟 只有當貓咪碰到旗桿，而且是「從左邊來的 (wasOnLeft)」，才觸發動畫！
            if (touchX && touchY && wasOnLeft)
            {
                LOG_INFO("觸發過關旗桿！");
                m_FlagAnimPhase = 1;

                // 惡意判定：貓咪的 Y 座標如果大於旗桿的 Y 座標
                m_IsTrollFlagDeath = (pPos.y > bPos.y);
                m_FlagBottomY = bPos.y - (bSize.y / 2.0f) + (pSize.y / 2.0f);

                // 吸附到左側邊緣
                float snapX = bPos.x - (bSize.x / 2.0f) - (pSize.x / 2.0f);

                m_Player->SetPosition({snapX, pPos.y});

            }
        }
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
        if (eventBlock && eventBlock->GetEventID()==41 && !eventBlock->HasSpawned())
        {
            glm::vec2 pPos = m_Player->GetPosition();
            glm::vec2 bPos = eventBlock->GetPosition();


            bool is_above = pPos.y > bPos.y;

            bool is_alignedX = std::abs(bPos.x - pPos.x) < 40.0f;

            bool is_closeY = (pPos.y - bPos.y) < 750.0f;
            if (is_above && is_alignedX && is_closeY)
            {
                auto flyingEnemy = std::make_shared<FlyingEnemy>("fly_teki");
                float spawnY = bPos.y + (eventBlock->GetScaledSize().y / 2.0f) + (flyingEnemy->GetScaledSize().y / 2.0f) + 1.0f;
                flyingEnemy->SetPosition({bPos.x, spawnY});

                // 3. 發射魔法：給予極大的向上初速度！(把它當子彈發射出去)
                flyingEnemy->SetVelocity({0.0f, 15.0f});

                // 4. 丟進大管家的陣列裡，下一幀開始它就會自動受重力影響並掉下來
                m_Enemies.push_back(flyingEnemy);

                // 5. 標記為已觸發，確保這個水管只會發射一次
                eventBlock->SetSpawned(true);
            }
        }
        if (eventBlock && eventBlock->GetEventID() == 95)
        {
            glm::vec2 pPos = m_Player->GetPosition();
            glm::vec2 bPos = eventBlock->GetPosition();

            // 條件：貓咪在方塊下方，且 X 軸距離靠近 (80像素內，可自己微調)
            bool is_under = pPos.y < bPos.y;
            bool is_alignedX = std::abs(bPos.x - pPos.x) < 80.0f;

            // 只要滿足條件，不需要管狀態，直接往下砸！
            if (is_under && is_alignedX)
            {
                eventBlock->Activate();
            }
            // if (eventBlock->IsActivated())
            // {
            //     bPos.y-=15.0f;
            //     eventBlock->SetPosition(bPos);
            //     if (m_Player->IfCollidesWithBlock(eventBlock))
            //     {
            //
            //         m_Player->Die();
            //     }
            // }
        }
        if (eventBlock && eventBlock->GetEventID() == 52)
        {
            glm::vec2 pPos = m_Player->GetPosition();
            glm::vec2 bPos = eventBlock->GetPosition();
            eventBlock->SetDrawable(ImageManager::Get("fly_teki_2"));

            bool is_under = pPos.y < bPos.y;
            bool is_alignedX = std::abs(bPos.x - pPos.x) < 80.0f;

            if (is_under && is_alignedX && !eventBlock->HasSpawned())
            {
                eventBlock->SetSpawned(true); // 🌟 改用 SetSpawned
                eventBlock->SetVisible(true);
            }

            if (eventBlock->HasSpawned()) // 🌟 改用 HasSpawned
            {
                bPos.y -= 15.0f;
                eventBlock->SetPosition(bPos);
                if (m_Player->IfCollidesWithBlock(eventBlock))
                {
                    m_Player->Die();
                }
            }
        }



    }
    bool is95Triggered = false;

    // 1. 偵測階段：掃描是否有任何一個 95 號方塊被貓咪驚動了？
    for (auto& block : m_Blocks) {
        auto eb = std::dynamic_pointer_cast<EventBlock>(block);
        if (eb && eb->GetEventID() == 95 && eb->IsActivated()) {
            is95Triggered = true;
            break; // 只要抓到一個啟動，全體連線成功！
        }
    }

    // 2. 執行階段：如果連線成功，地圖上所有的 95 號方塊同步化身隕石砸下！
    if (is95Triggered) {
        for (auto& block : m_Blocks) {
            auto eb = std::dynamic_pointer_cast<EventBlock>(block);
            if (eb && eb->GetEventID() == 95) {

                eb->Activate(); // 強制讓所有沒被驚動的 95 號也一起進入啟動狀態

                glm::vec2 bPos = eb->GetPosition();
                bPos.y -= 15.0f; // 保持原本 15.0f 的超神速下砸！
                eb->SetPosition(bPos);

                // 砸人判定（移到這裡後，整排方塊都有殺傷力！）
                if (m_Player->IfCollidesWithBlock(eb)) {

                    m_Player->Die();
                }
            }
        }
    }
    bool isFallingTriggered = false;


    for (auto& block : m_Blocks) {
        auto eb = std::dynamic_pointer_cast<EventBlock>(block);
        if (eb && (eb->GetEventID() == 97 || eb->GetEventID() == 98 || eb->GetEventID() == 77) && eb->HasSpawned()) {
            isFallingTriggered = true;
            break; // 只要抓到一個被踩，就觸發全體機關！
        }
    }

    // 2. 如果機關被觸發，就把所有的 97 跟 98 同步往下移動！
    if (isFallingTriggered) {
        for (auto& block : m_Blocks) {
            auto eb = std::dynamic_pointer_cast<EventBlock>(block);
            if (eb && (eb->GetEventID() == 97 || eb->GetEventID() == 98 || eb->GetEventID() == 77)) {

                eb->SetSpawned(true);

                glm::vec2 bPos = eb->GetPosition();
                bPos.y -= 3.0f; // 🌟 掉落速度 (建議設 3.0f，讓物理引擎的重力能順暢帶著貓咪一起往下)
                eb->SetPosition(bPos);
            }
        }
    }
    // 2. 物理運算 (維持在真實比例下運作，確保碰撞正確)
    m_Player->Update(m_Blocks);

    for (auto& enemy : m_Enemies)
    {
        enemy->Update(m_Blocks);
        if (enemy->GetPosition().y < -400.0f)
        {
            enemy->Die();
            continue;
        }
        if (std::dynamic_pointer_cast<FlyingEnemy>(enemy)!=nullptr)
        {
            glm::vec2 Pos = enemy->GetPosition();
            Pos.y+=12.0f;
            enemy->SetPosition(Pos);
            if (Pos.y>1000.0f)
            {
                enemy->Die();
            }
        }
    }
    for (auto& enemyA : m_Enemies) {

        // 嘗試看看 enemyA 是不是烏龜？
        auto turtle = std::dynamic_pointer_cast<TurtleEnemy>(enemyA);

        // 條件：只有「高速移動中的烏龜殼」，且還沒死掉，才有殺傷力
        if (turtle && turtle->GetState() == TurtleState::SHELL_MOVING && !turtle->IsDead()) {

            for (auto& enemyB : m_Enemies) {
                // 排除自己，也排除已經死亡的怪物
                if (enemyA != enemyB && !enemyB->IsDead()) {

                    glm::vec2 posA = enemyA->GetPosition();
                    glm::vec2 sizeA = enemyA->GetScaledSize();
                    glm::vec2 posB = enemyB->GetPosition();
                    glm::vec2 sizeB = enemyB->GetScaledSize();

                    // AABB 碰撞計算 (容錯稍微縮小一點，避免邊緣擦到就死)
                    bool colX = std::abs(posA.x - posB.x) < ((sizeA.x + sizeB.x) / 2.0f) - 2.0f;
                    bool colY = std::abs(posA.y - posB.y) < ((sizeA.y + sizeB.y) / 2.0f) - 2.0f;

                    // 如果撞到了！
                    if (colX && colY) {


                        enemyB->Die(); // 被撞到的怪物當場死亡

                        // 💡 瑪利歐經典設定：烏龜殼會直接貫穿過去連殺，不會停下來
                        // 如果你想讓烏龜殼撞到怪後停下來，可以在這裡加上 turtle->StopShell();
                    }
                }
            }
        }
    }


    // ==========================================
    // 4. 繪圖階段：套用縮放與偏移的障眼法
    // ==========================================

    // -- 處理所有磚塊 --
    RenderWithCamera(m_Mushrooms, cameraX, zoom);
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
    for (const auto& enemy : m_Enemies)
    {
        if (m_Player->IfCollides(enemy))
        {
            // 判斷貓咪是否從上方掉落踩中敵人
            bool isFallingOnEnemy = (m_Player->GetVelocity().y < 0.0f && m_Player->GetPosition().y > enemy->GetPosition().y);
            auto spiky = std::dynamic_pointer_cast<SpikyEnemy>(enemy);
            // 嘗試將敵人轉為烏龜，看看它是不是烏龜
            auto turtle = std::dynamic_pointer_cast<TurtleEnemy>(enemy);
            if (spiky)
            {
                m_Player->Die();
            }
            else if(turtle)
            {
                if (turtle->GetState() == TurtleState::WALKING) {
                    if (isFallingOnEnemy) {
                        turtle->TurnIntoShell(); // 🌟 踩扁變烏龜殼！
                        m_Player->Bounce();
                    } else {
                        m_Player->Die(); // 側邊碰到走路烏龜會死
                    }
                }
                else if (turtle->GetState() == TurtleState::SHELL_IDLE) {
                    // 🌟 踢烏龜殼！根據貓咪在左邊還是右邊，決定踢飛方向
                    float kickDir = (m_Player->GetPosition().x < turtle->GetPosition().x) ? 1.0f : -1.0f;
                    turtle->KickShell(kickDir);

                    if (isFallingOnEnemy) m_Player->Bounce();
                }
                else if (turtle->GetState() == TurtleState::SHELL_MOVING) {
                    if (isFallingOnEnemy) {
                        turtle->StopShell(); // 🌟 踩停高速移動的烏龜殼！
                        m_Player->Bounce();
                    } else if (turtle->CanKillPlayer()) {
                        m_Player->Die(); // 被高速烏龜殼撞死 (安全時間過後)
                    }
                }
            }
            else
            {
                // ==============================
                // 這裡是你原本的一般怪物邏輯
                // ==============================
                if (isFallingOnEnemy) {
                    enemy->Die();
                    m_Player->Bounce();
                } else {
                    m_Player->Die();
                }
            }
        }
    }
    for (auto& mushroom : m_Mushrooms)
    {
        mushroom->Update(m_Blocks);
        if (!mushroom->IsEaten() && m_Player->IfCollides(mushroom))
        {
            mushroom->Eat();
            int ID=mushroom->GetTypeID();
            switch (ID)
            {
            case 1:
                {
                    m_Player->PowerUp();
                    break;
                }
            case 3:
                m_Player->Die();
                break;
            }
        }
        for (auto& enemy : m_Enemies)
        {
            // 只有「還活著」的怪物，碰到蘑菇才會吃掉
            if (!enemy->IsDead() && enemy->IfCollides(mushroom))
            {
                mushroom->Eat(); // 蘑菇被怪物吃掉了！

                int ID = mushroom->GetTypeID();
                if (ID == 1) {
                    enemy->PowerUp(); // 怪物巨大化！
                }
                else if (ID == 3) {

                    enemy->Die(); // (選用小彩蛋) 怪物吃到毒蘑菇直接被毒死！
                }

                break; // 蘑菇已經被這隻怪物吃了，不用再檢查其他怪物了
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
    m_Mushrooms.erase(
        std::remove_if(m_Mushrooms.begin(), m_Mushrooms.end(), [](const std::shared_ptr<Mushroom>& m) {
            return m->IsEaten();
        }),
        m_Mushrooms.end()
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
                    auto enemy=std::make_shared<NormalEnemy>("teki_1");
                    glm::vec2 spawnPos = eventBlock->GetPosition();
                    float blockHalfHeight = eventBlock->GetScaledSize().y / 2.0f;
                    float enemyHalfHeight = enemy->GetScaledSize().y / 2.0f;
                    spawnPos.y += blockHalfHeight + enemyHalfHeight + 5.0f;
                    enemy->SetPosition(spawnPos);
                    enemy->SetDirection(1.0f);


                    m_Enemies.push_back(enemy);
                    break;
                }
            case 94:
                {
                    auto mushroom = std::make_shared<Mushroom>("mushroom_1",1);
                    glm::vec2 spawnPos=eventBlock->GetPosition();
                    spawnPos.y += (eventBlock->GetScaledSize().y / 2.0f) + (mushroom->GetScaledSize().y / 2.0f) + 1.0f;
                    mushroom->SetPosition(spawnPos);
                    mushroom->SetVelocity({0.0f,5.0f});
                    m_Mushrooms.push_back(mushroom);
                    eventBlock->SetSpawned(true);
                    break;
                }
            case 31:
                {
                    auto mushroom = std::make_shared<Mushroom>("mushroom_3",3);
                    glm::vec2 spawnPos=eventBlock->GetPosition();
                    spawnPos.y += (eventBlock->GetScaledSize().y / 2.0f) + (mushroom->GetScaledSize().y / 2.0f) + 1.0f;
                    mushroom->SetPosition(spawnPos);
                    mushroom->SetVelocity({0.0f,5.0f});
                    m_Mushrooms.push_back(mushroom);
                    eventBlock->SetVisible(true);
                    eventBlock->SetSpawned(true);
                    break;
                }
            case 96:
                {
                    auto star = std::make_shared<StarEnemy>("star1");

                    glm::vec2 spawnPos = eventBlock->GetPosition();
                    spawnPos.y += (eventBlock->GetScaledSize().y / 2.0f) + (star->GetScaledSize().y / 2.0f) + 1.0f;
                    star->SetPosition(spawnPos);

                    // 將它加入敵人陣列，共用現成的碰撞與死亡判定！
                    m_Enemies.push_back(star);
                    break;
                }

                // (未來如果還有噴蘑菇、現形等事件都寫在這裡)
            }

            // 標記為已生成，這樣下一幀就不會再噴一次了！
            eventBlock->SetSpawned(true);
        }
    }
    if (Util::Input::IsKeyPressed(Util::Keycode::R))
    {
        LoadLevel(m_CurrentLevel);
    }

    if (Util::Input::IsKeyUp(Util::Keycode::ESCAPE) ||
        Util::Input::IfExit()) {
        m_CurrentState = State::END;
    }
}
void App::LoadLevel(int level) {

    // 1. 大掃除：清空上一關殘留的所有物件
    m_Blocks.clear();
    m_Enemies.clear();
    m_Decorations.clear();
    m_Mushrooms.clear();
    m_Coins.clear();
    m_SpawnerTracker.clear();
    m_SpawnCounts.clear();
    m_FireballTimers.clear();
    m_FireballInterval = 120;
    m_PlatformInitialPos.clear();
    m_PlatformAngles.clear();

    // 2. 組合新的地圖路徑 (把 level 變成字串放進檔名)
    std::string mapPath = RESOURCE_DIR "/Map/level" + std::to_string(level) + ".txt";

    // 3. 讀取新地圖
    LevelDate mydata = MapManager::LoadMap(mapPath);

    // 🌟 防呆機制：如果回傳的地圖是空的，代表沒有下一關了 (全破！)
    if (mydata.blocks.empty()) {

        m_CurrentState = State::END;
        return;
    }

    // 4. 把新關卡的資料交給大管家
    m_Blocks = mydata.blocks;
    m_Enemies = mydata.enemies;
    m_Decorations = mydata.decorations;
    if (level == 3) {
        // 第三關專屬：X 更靠左 (-550.0f)，Y 從畫面最上方掉落 (600.0f 可依需求微調高度)
        // 注意：預設攝影機左邊界大約是 -598.0f，所以 -550.0f 已經是非常貼近左邊緣的極限位置了！
        m_Player->SetPosition({-570.0f, 600.0f});
    } else {
        // 第一關、第二關的預設起點
        m_Player->SetPosition({-500.0f, 0.0f});
    }
    // 5. 重置貓咪位置與相關狀態


    // 如果你有重置速度的方法也可以呼叫，例如 m_Player->SetVelocity({0.0f, 0.0f});
    m_MaxCameraX = -213.0f;
    // 6. 確保各種劇本動畫回到未觸發狀態
    m_FlagAnimPhase = 0;
    m_FlagAnimTimer = 0;
    m_PipeAnimPhase = 0;
    m_HorizPipePhase = 0;
    m_ClearPipePhase = 0;
    // (如果你還有其他的全域陷阱開關，記得要在這裡歸零)
}
void App::End() {
    LOG_TRACE("End");
}