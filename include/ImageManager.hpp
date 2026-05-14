//
// Created by eatba on 2026/3/13.
//

#ifndef REPLACE_WITH_YOUR_PROJECT_NAME_IMAGEMANAGER_HPP
#define REPLACE_WITH_YOUR_PROJECT_NAME_IMAGEMANAGER_HPP
#include <map>
#include <string>
#include <memory>
#include <vector>
#include "Util/Image.hpp"


class ImageManager
{
    public:
    static void LoadAll();
    static std::shared_ptr<Util::Image> Get(const std::string& name);

    private:
    static std::map<std::string, std::shared_ptr<Util::Image>> s_Images;
};
#endif //REPLACE_WITH_YOUR_PROJECT_NAME_IMAGEMANAGER_HPP