#include "MapManager.hpp"
#include "Util/Logger.hpp" // 方便印出錯誤訊息
#include <fstream>
#include <sstream>
#include <ImageManager.hpp>
#include "EventBlock.hpp"
#include "Enemy.hpp"
#include "HiddenBlock.hpp"
#include "NormalEnemy.hpp"
#include "SpikyEnemy.hpp"
#include "TurtleEnemy.hpp"
LevelDate MapManager::LoadMap(const std::string& filePath)
{
    LevelDate level;
    std::ifstream file(filePath);
    if (!file.is_open())
    {
        LOG_ERROR("fuck cant open"+filePath);
        return level;
    }
    std::string line;
    int row=0;
    const float BLOCK_SIZE=29.0f;
    float startX=-625.0f;
    float startY=180.0f;

    while (std::getline(file,line))
    {
        std::stringstream ss(line);
        int tileID;
        int col=0;

        while (ss>>tileID)
        {
            glm::vec2 pos = {startX + (col * BLOCK_SIZE), startY - (row * BLOCK_SIZE)};
            if (tileID!=0)
            {
                switch (tileID)
                {
                case 1:{
                        auto block = std::make_shared<Block>("brick_1");
                        block->SetPosition({startX + (col * BLOCK_SIZE), startY - (row * BLOCK_SIZE)});
                        level.blocks.push_back(block);
                        block->SetBreakable(true);
                        break;

                }
                case 2:{
                        auto block = std::make_shared<EventBlock>("brick_2",tileID);
                        block->SetPosition({startX + (col * BLOCK_SIZE), startY - (row * BLOCK_SIZE)});
                        level.blocks.push_back(block);
                        break;

                }
                case 3:{
                        auto block = std::make_shared<Block>("brick_3");
                        block->SetPosition({startX + (col * BLOCK_SIZE), startY - (row * BLOCK_SIZE)});
                        level.blocks.push_back(block);
                        break;

                }
                case 4:{
                        auto block = std::make_shared<Block>("brick_4");
                        block->SetPosition({startX + (col * BLOCK_SIZE), startY - (row * BLOCK_SIZE)});
                        level.blocks.push_back(block);
                        break;

                }
                case 5:{
                        auto block = std::make_shared<Block>("brick_5");
                        block->SetPosition({startX + (col * BLOCK_SIZE), startY - (row * BLOCK_SIZE)});
                        level.blocks.push_back(block);
                        break;

                }
                case 6:{
                        auto block = std::make_shared<Block>("brick_6");
                        block->SetPosition({startX + (col * BLOCK_SIZE), startY - (row * BLOCK_SIZE)});
                        level.blocks.push_back(block);
                        break;
                }
                case 7:{
                        auto block = std::make_shared<Block>("brick_7");
                        block->SetPosition({startX + (col * BLOCK_SIZE), startY - (row * BLOCK_SIZE)});
                        level.blocks.push_back(block);
                        break;
                }
                case 8:{
                        auto block = std::make_shared<Block>("brick_8");
                        block->SetPosition({startX + (col * BLOCK_SIZE), startY - (row * BLOCK_SIZE)});
                        level.blocks.push_back(block);
                        break;
                }
                case 9:{
                        auto block = std::make_shared<Block>("brick_9");
                        block->SetPosition({startX + (col * BLOCK_SIZE), startY - (row * BLOCK_SIZE)});
                        level.blocks.push_back(block);
                        break;
                }
                case 10:{
                        auto block = std::make_shared<Block>("brick_10");
                        block->SetPosition({startX + (col * BLOCK_SIZE), startY - (row * BLOCK_SIZE)});
                        level.blocks.push_back(block);
                        break;
                }
                case 11:{
                        auto block = std::make_shared<Block>("brick_10");
                        block->SetPosition({startX + (col * BLOCK_SIZE), startY - (row * BLOCK_SIZE)});
                        level.blocks.push_back(block);
                        break;
                }
                case 41:
                    {
                        auto deco=std::make_shared<EventBlock>("pipe",tileID);
                        deco->SetPosition(pos);
                        level.blocks.push_back(deco);
                        break;
                    }
                case 411:
                    {
                        auto deco=std::make_shared<EventBlock>("pipe_left",tileID);
                        deco->SetPosition(pos);
                        deco->SetZIndex(-1);
                        level.blocks.push_back(deco);
                        break;
                    }
                case 42:
                    {
                        auto deco=std::make_shared<EventBlock>("pipe",tileID);
                        deco->SetPosition(pos);
                        level.blocks.push_back(deco);

                        auto sign = std::make_shared<Decoration>("sign", -8);
                        glm::vec2 signPos = pos;
                        signPos.y += 70.0f;
                        signPos.x-=10.0f;
                        sign->SetPosition(signPos);

                        level.decorations.push_back(sign);
                        break;
                    }

                case 43:
                    {
                        auto sign = std::make_shared<Decoration>("sign2", -8);
                        glm::vec2 signPos = pos;
                        signPos.y-=10.0f;
                        sign->SetPosition(signPos);

                        level.decorations.push_back(sign);
                        break;
                    }
                case 44:
                    {
                        auto deco=std::make_shared<EventBlock>("pipe",tileID);
                        deco->SetPosition(pos);
                        deco->SetZIndex(2);
                        level.blocks.push_back(deco);

                        break;
                    }
                case 45:
                    {
                        auto deco=std::make_shared<EventBlock>("pipe",tileID);
                        deco->SetPosition(pos);
                        deco->SetZIndex(2);
                        level.blocks.push_back(deco);

                        break;
                    }
                case 50:
                    {
                        auto enemy=std::make_shared<NormalEnemy>("teki_1",false);
                        enemy->SetPosition(pos);
                        level.enemies.push_back(enemy);
                        break;
                    }
                case 51:
                    {
                        auto enemy=std::make_shared<TurtleEnemy>("turtle_walk",true);
                        enemy->SetPosition(pos);
                        level.enemies.push_back(enemy);
                        break;
                    }
                case 52:
                    {
                        auto deco=std::make_shared<EventBlock>("fly_teki_2",tileID);
                        deco->SetPosition(pos);
                        deco->SetVisible(false);
                        level.blocks.push_back(deco);
                        break;
                    }
                case 53:
                    {
                        auto enemy=std::make_shared<EventBlock>("brick_2",tileID);
                        enemy->SetPosition(pos);
                        enemy->SetVisible(false);
                        enemy->SetZIndex(-9);
                        level.blocks.push_back(enemy);

                        break;
                    }

                case 80:
                    {
                        auto deco = std::make_shared<Decoration>("mountain");
                        deco->SetPosition(pos);
                        level.decorations.push_back(deco);
                        break;
                    }
                case 81:
                    {
                        auto deco = std::make_shared<Decoration>("grass");
                        deco->SetPosition(pos);
                        level.decorations.push_back(deco);
                        break;
                    }
                case 82:
                    {
                        auto deco = std::make_shared<Decoration>("cloud");
                        deco->SetPosition(pos);
                        level.decorations.push_back(deco);
                        break;
                    }
                case 83:
                    {
                        auto deco = std::make_shared<EventBlock>("cloud",tileID);
                        deco->SetPosition(pos);
                        level.blocks.push_back(deco);
                        break;
                    }
                case 84:
                    {
                        auto deco = std::make_shared<EventBlock>("cloud",tileID);
                        deco->SetPosition(pos);
                        deco->SetVisible(false);
                        deco->SetZIndex(-11);
                        level.blocks.push_back(deco);
                        break;
                    }
                case 85:
                    {
                        auto deco = std::make_shared<EventBlock>("bullet_1",tileID);
                        deco->SetPosition(pos);
                        deco->SetVisible(false);
                        level.blocks.push_back(deco);
                        break;
                    }
                case 86:
                    {
                        auto deco = std::make_shared<Decoration>("end");
                        deco->SetPosition(pos);
                        level.decorations.push_back(deco);
                        break;
                    }
                case 88:
                    {
                        auto deco=std::make_shared<EventBlock>("brick_10",tileID);
                        deco->SetPosition(pos);
                        level.blocks.push_back(deco);
                        break;
                    }
                case 91:{ //會往上移
                        auto block = std::make_shared<EventBlock>("brick_2",tileID);
                        block->SetPosition({startX + (col * BLOCK_SIZE), startY - (row * BLOCK_SIZE)});
                        level.blocks.push_back(block);
                        break;
                }
                case 92:{
                        auto block = std::make_shared<EventBlock>("brick_2",tileID);
                        block->SetPosition({startX + (col * BLOCK_SIZE), startY - (row * BLOCK_SIZE)});
                        level.blocks.push_back(block);
                        break;
                }
                case 921:{ //隱藏方塊
                        auto block = std::make_shared<HiddenBlock>();
                        block->SetPosition(pos);
                        level.blocks.push_back(block);
                        break;
                }
                case 93:{
                        auto block = std::make_shared<EventBlock>("brick_2",tileID);
                        block->SetPosition({startX + (col * BLOCK_SIZE), startY - (row * BLOCK_SIZE)});
                        level.blocks.push_back(block);
                        break;
                }
                case 94:{
                        auto block = std::make_shared<EventBlock>("brick_2",tileID);
                        block->SetPosition({startX + (col * BLOCK_SIZE), startY - (row * BLOCK_SIZE)});
                        level.blocks.push_back(block);
                        break;
                }
                case 31:{
                        auto block = std::make_shared<EventBlock>("brick_2",tileID);
                        block->SetPosition({startX + (col * BLOCK_SIZE), startY - (row * BLOCK_SIZE)});
                        block->SetVisible(false);
                        level.blocks.push_back(block);
                        break;
                }
                case 95:
                    {
                        auto block = std::make_shared<EventBlock>("brick_1",tileID);
                        block->SetPosition({startX + (col * BLOCK_SIZE), startY - (row * BLOCK_SIZE)});
                        level.blocks.push_back(block);
                        break;
                    }
                case 96:
                    {
                        auto block = std::make_shared<EventBlock>("brick_2",tileID);
                        block->SetPosition({startX + (col * BLOCK_SIZE), startY - (row * BLOCK_SIZE)});
                        level.blocks.push_back(block);
                        break;
                    }
                case 97:
                    {
                        auto block = std::make_shared<EventBlock>("brick_6",tileID);
                        block->SetPosition({startX + (col * BLOCK_SIZE), startY - (row * BLOCK_SIZE)});
                        level.blocks.push_back(block);
                        break;
                    }
                case 98:
                    {
                        auto block = std::make_shared<EventBlock>("brick_5",tileID);
                        block->SetPosition({startX + (col * BLOCK_SIZE), startY - (row * BLOCK_SIZE)});
                        level.blocks.push_back(block);
                        break;
                    }
                case 99:
                    {
                        auto block = std::make_shared<EventBlock>("straght",tileID);
                        block->SetPosition({startX + (col * BLOCK_SIZE), startY - (row * BLOCK_SIZE)});
                        level.blocks.push_back(block);
                        break;
                    }
                default:
                    {
                        auto block = std::make_shared<Block>("brick_6");
                        block->SetPosition({startX + (col * BLOCK_SIZE), startY - (row * BLOCK_SIZE)});
                        level.blocks.push_back(block);
                        break;
                    }
                }

            }
            col++;
        }
        row++;
    }

    file.close();
    return level;
}