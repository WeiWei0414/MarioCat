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
                    LOG_INFO("階段1：貓咪開始下沉！");

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
                LOG_INFO("安全通關！");
                m_FlagAnimPhase = 0;
                m_CurrentState = State::END; // 真正結束遊戲
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
    for (auto& block : m_Blocks)
    {
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
            if (eventBlock->IsActivated())
            {
                bPos.y-=15.0f;
                eventBlock->SetPosition(bPos);
                if (m_Player->IfCollidesWithBlock(eventBlock))
                {

                    m_Player->Die();
                }
            }
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
    bool isFallingTriggered = false;


    for (auto& block : m_Blocks) {
        auto eb = std::dynamic_pointer_cast<EventBlock>(block);
        if (eb && (eb->GetEventID() == 97 || eb->GetEventID() == 98) && eb->IsActivated()) {
            isFallingTriggered = true;
            break; // 只要抓到一個被踩，就觸發全體機關！
        }
    }

    // 2. 如果機關被觸發，就把所有的 97 跟 98 同步往下移動！
    if (isFallingTriggered) {
        for (auto& block : m_Blocks) {
            auto eb = std::dynamic_pointer_cast<EventBlock>(block);
            if (eb && (eb->GetEventID() == 97 || eb->GetEventID() == 98)) {

                eb->Activate(); // 強制全體連線進入啟動狀態

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

            // 嘗試將敵人轉為烏龜，看看它是不是烏龜
            auto turtle = std::dynamic_pointer_cast<TurtleEnemy>(enemy);

            if (turtle)
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