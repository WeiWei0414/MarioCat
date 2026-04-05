//
// Created by eatba on 2026/3/18.
//
#include "Character.hpp"
#include "Util/Image.hpp"
#include "ImageManager.hpp"

Character::Character(const std::string& ImagePath) {

    m_ImagePath = ImagePath;
    SetDrawable(ImageManager::Get(ImagePath));

}

void Character::SetImage(const std::string& ImagePath) {
    m_ImagePath = ImagePath;

    SetDrawable(ImageManager::Get(ImagePath));
}