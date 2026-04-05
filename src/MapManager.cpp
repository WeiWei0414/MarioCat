#include "MapManager.hpp"
#include "Util/Logger.hpp" // 方便印出錯誤訊息
#include <fstream>
#include <sstream>
#include <ImageManager.hpp>
std::vector<std::shared_ptr<Block>> MapManager::LoadMap(const std::string& filePath)
{
    std::vector<std::shared_ptr<Block>> blocks;
    std::ifstream file(filePath);
    if (!file.is_open())
    {
        LOG_ERROR("fuck cant open"+filePath);
        return blocks;
    }
    std::string line;
    int row=0;
    const float BLOCK_SIZE=30.0f;
    float startX=-625.0f;
    float startY=100.0f;

    while (std::getline(file,line))
    {
        std::stringstream ss(line);
        int tileID;
        int col=0;
        while (ss>>tileID)
        {
            std::string imageKey;
            if (tileID!=0)
            {
                switch (tileID)
                {
                case 5:
                    imageKey="brick_5";
                    break;
                case 6:
                    imageKey="brick_6";
                    break;

                    default:
                    imageKey="block";
                }
                auto block = std::make_shared<Block>(imageKey);
                block->SetPosition({startX + (col * BLOCK_SIZE), startY - (row * BLOCK_SIZE)});
                blocks.push_back(block);
            }
            col++;
        }
        row++;
    }

    file.close();
    return blocks;
}