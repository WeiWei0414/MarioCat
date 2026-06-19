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
    s_Images["brick_1"]= std::make_shared<Util::Image>(RESOURCE_DIR "/character/brick_1.png");
    s_Images["brick_2"]= std::make_shared<Util::Image>(RESOURCE_DIR "/character/brick_2.png");
    s_Images["brick_3"]= std::make_shared<Util::Image>(RESOURCE_DIR "/character/brick_3.png");
    s_Images["brick_4"]= std::make_shared<Util::Image>(RESOURCE_DIR "/character/brick_4.png");
    s_Images["brick_5"]= std::make_shared<Util::Image>(RESOURCE_DIR "/character/brick_5.png");
    s_Images["brick_6"]= std::make_shared<Util::Image>(RESOURCE_DIR "/character/brick_6.png");
    s_Images["brick_7"]= std::make_shared<Util::Image>(RESOURCE_DIR "/character/brick_7.png");
    s_Images["brick_8"]= std::make_shared<Util::Image>(RESOURCE_DIR "/character/brick_8.png");
    s_Images["brick_9"]= std::make_shared<Util::Image>(RESOURCE_DIR "/character/brick_9.png");
    s_Images["brick_10"]= std::make_shared<Util::Image>(RESOURCE_DIR "/character/brick_10.png");
    s_Images["brick_11"]= std::make_shared<Util::Image>(RESOURCE_DIR "/character/brick_11.png");
    s_Images["mountain"]= std::make_shared<Util::Image>(RESOURCE_DIR "/character/mountain.png");
    s_Images["grass"]= std::make_shared<Util::Image>(RESOURCE_DIR "/character/grass.png");
    s_Images["cloud"]= std::make_shared<Util::Image>(RESOURCE_DIR "/character/cloud.png");
    s_Images["fake_cloud"]= std::make_shared<Util::Image>(RESOURCE_DIR "/character/fake_cloud.png");
    s_Images["pipe"]= std::make_shared<Util::Image>(RESOURCE_DIR "/character/pipe.png");
    s_Images["pipe_left"]= std::make_shared<Util::Image>(RESOURCE_DIR "/character/pipe_left.png");
    s_Images["long_pipe"]= std::make_shared<Util::Image>(RESOURCE_DIR "/character/long_pipe.png");
    s_Images["coin"]= std::make_shared<Util::Image>(RESOURCE_DIR "/character/coin.png");
    s_Images["sign"]= std::make_shared<Util::Image>(RESOURCE_DIR "/character/sign.png");
    s_Images["sign2"]= std::make_shared<Util::Image>(RESOURCE_DIR "/character/flag2.png");
    s_Images["mushroom_1"]= std::make_shared<Util::Image>(RESOURCE_DIR "/character/mushroom_1.png");
    s_Images["mushroom_2"]= std::make_shared<Util::Image>(RESOURCE_DIR "/character/mushroom_2.png");
    s_Images["mushroom_3"]= std::make_shared<Util::Image>(RESOURCE_DIR "/character/mushroom_3.png");
    s_Images["player_powerup"]= std::make_shared<Util::Image>(RESOURCE_DIR "/character/player_powerup.png");
    s_Images["straght"]= std::make_shared<Util::Image>(RESOURCE_DIR "/character/straght.png");
    s_Images["straght2"]= std::make_shared<Util::Image>(RESOURCE_DIR "/character/straght2.png");
    s_Images["end"]= std::make_shared<Util::Image>(RESOURCE_DIR "/character/end.png");
    s_Images["spiky_teki"]= std::make_shared<Util::Image>(RESOURCE_DIR "/character/spiky_teki.png");
    s_Images["spike"]= std::make_shared<Util::Image>(RESOURCE_DIR "/character/spike.png");
    s_Images["item1"]= std::make_shared<Util::Image>(RESOURCE_DIR "/character/item1.png");
    //敵人
    s_Images["teki_1"]= std::make_shared<Util::Image>(RESOURCE_DIR "/character/teki_1.png");
    s_Images["teki_powerup"]= std::make_shared<Util::Image>(RESOURCE_DIR "/character/teki_powerup.png");
    s_Images["fly_teki"]= std::make_shared<Util::Image>(RESOURCE_DIR "/character/fly_teki.png");
    s_Images["cat_teki"]= std::make_shared<Util::Image>(RESOURCE_DIR "/character/cat_teki.png");
    s_Images["fireball"]= std::make_shared<Util::Image>(RESOURCE_DIR "/character/fireball.png");

    s_Images["fly_teki_2"]= std::make_shared<Util::Image>(RESOURCE_DIR "/character/fly_teki_2.png");
    s_Images["star1"]= std::make_shared<Util::Image>(RESOURCE_DIR "/character/star1.png");
    s_Images["turtle_walk"]= std::make_shared<Util::Image>(RESOURCE_DIR "/character/turtle_walk.png");
    s_Images["turtle_shell"]= std::make_shared<Util::Image>(RESOURCE_DIR "/character/turtle_shell.png");
    s_Images["bullet"]= std::make_shared<Util::Image>(RESOURCE_DIR "/character/bullet.png");
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
