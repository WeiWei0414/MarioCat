//
// Created by eatba on 2026/3/20.
//
#include "Block.hpp"
#include <vector>
#include "ImageManager.hpp"
Block::Block(const std::string& imageName)
{
    SetDrawable(ImageManager::Get(imageName));
    m_Transform.translation={0.0f,0.0f};
}
