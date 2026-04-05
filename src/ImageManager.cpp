//
// Created by eatba on 2026/3/13.
//
#include "ImageManager.hpp"
#include "Util/Logger.hpp"

std::map<std::string, std::shared_ptr<Util::Image>> ImageManager::s_Images;

void ImageManager::LoadAll()
{
    s_Images["bg_blue"]= std::make_shared<Util::Image>(RESOURCE_DIR "/character/background_blue.png");
    s_Images["player_idle"]= std::make_shared<Util::Image>(RESOURCE_DIR "/character/player_idle.png");
    s_Images["player_jump"]= std::make_shared<Util::Image>(RESOURCE_DIR "/character/player_jump.png");
    s_Images["player_dead"]= std::make_shared<Util::Image>(RESOURCE_DIR "/character/player_dead.png");
    s_Images["block"]= std::make_shared<Util::Image>(RESOURCE_DIR "/character/brock1.png");
    s_Images["brick_5"]= std::make_shared<Util::Image>(RESOURCE_DIR "/character/brick_5.png");
    s_Images["brick_6"]= std::make_shared<Util::Image>(RESOURCE_DIR "/character/brick_6.png");
}

std::shared_ptr<Util::Image> ImageManager::Get(const std::string& name)
{
    auto it = s_Images.find(name); //it是迭代器 會回傳(key,value(指標))
    if (it!=s_Images.end())
    {
        return it->second; //指標
    }
    LOG_ERROR("suck can't find source: " + name);
    return nullptr;

}
