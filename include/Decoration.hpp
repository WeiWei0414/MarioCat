//
// Created by eatba on 2026/5/15.
//

#ifndef REPLACE_WITH_YOUR_PROJECT_NAME_DECORATION_HPP
#define REPLACE_WITH_YOUR_PROJECT_NAME_DECORATION_HPP
#include "Util/GameObject.hpp"
#include "ImageManager.hpp"
#include <memory>
#include <string>
class Decoration : public Util::GameObject {
public:
    explicit Decoration(const std::string& imageName) {
        SetDrawable(ImageManager::Get(imageName));


        SetZIndex(-5);
    }

    [[nodiscard]] glm::vec2 GetPosition() const { return m_Transform.translation; }
    void SetPosition(const glm::vec2& pos) { m_Transform.translation = pos; }
    void SetScale(const glm::vec2& scale) { m_Transform.scale = scale; }
};
#endif //REPLACE_WITH_YOUR_PROJECT_NAME_DECORATION_HPP