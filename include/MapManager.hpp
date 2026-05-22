//
// Created by eatba on 2026/3/20.
//

#ifndef REPLACE_WITH_YOUR_PROJECT_NAME_MAPMANAGER_HPP
#define REPLACE_WITH_YOUR_PROJECT_NAME_MAPMANAGER_HPP
#include "Block.hpp"
#include <vector>
#include <string>
#include <memory>
#include "Enemy.hpp"
#include "Decoration.hpp"
struct LevelDate
{
    std::vector<std::shared_ptr<Block>> blocks;
    std::vector<std::shared_ptr<Enemy>> enemies;
    std::vector<std::shared_ptr<Decoration>> decorations;
};
class MapManager
{
public:
    static LevelDate LoadMap(const std::string& filepath);
};
#endif //REPLACE_WITH_YOUR_PROJECT_NAME_MAPMANAGER_HPP