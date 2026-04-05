#include "App.hpp"

#include "Util/Image.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"
#include "ImageManager.hpp"
#include "Player.hpp"
#include "MapManager.hpp"
#include <algorithm>

void App::Start() {
    LOG_TRACE("Start");

    ImageManager::LoadAll();
    m_Player=std::make_shared<Player>();
    m_Player->SetPosition({-200.0f, 0.0f});
    m_Blocks=MapManager::LoadMap(RESOURCE_DIR "/Map/level1.txt");
    m_Background=std::make_shared<Util::GameObject>();
    m_Background->SetDrawable(ImageManager::Get("bg_blue"));
    m_Background->SetZIndex(-10);


    m_CurrentState = State::UPDATE;

}

void App::Update() {
    m_Background->Draw();
    m_Player->Update(m_Blocks);
    static float maxCameraX=0.0f;
    float targetCameraX = std::max(0.0f,m_Player->GetPosition().x);
    float zoom=2.0f;
    if (targetCameraX > maxCameraX) {
        maxCameraX = targetCameraX;
    }
    float cameraX = maxCameraX;
    float leftEdge = cameraX - 600.0f;
    if (m_Player->GetPosition().x<leftEdge)
    {
        m_Player->SetPosition({leftEdge,m_Player->GetPosition().y});
    }

    m_Player->SetPosition({m_Player->GetPosition().x-cameraX,m_Player->GetPosition().y});
    for (auto& block : m_Blocks)
    {
        block->SetPosition({block->GetPosition().x - cameraX, block->GetPosition().y});
    }
    for (auto& block : m_Blocks) {
        block->Draw();
    }
    m_Player->Draw();
    m_Player->SetPosition({m_Player->GetPosition().x + cameraX, m_Player->GetPosition().y});
    for (auto& block : m_Blocks) {
        block->SetPosition({block->GetPosition().x + cameraX, block->GetPosition().y});
    }



    if (Util::Input::IsKeyPressed(Util::Keycode::R))
    {
        m_Player->SetPosition({-200.0f,0.0f});
        maxCameraX = 0.0f;
    }

    //TODO: do your things here and delete this line <3

    
    /*
     * Do not touch the code below as they serve the purpose for
     * closing the window.
     */
    if (Util::Input::IsKeyUp(Util::Keycode::ESCAPE) ||
        Util::Input::IfExit()) {
        m_CurrentState = State::END;
    }
}

void App::End() { // NOLINT(this method will mutate members in the future)
    LOG_TRACE("End");
}
