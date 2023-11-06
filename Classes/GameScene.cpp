#include "GameScene.h"

USING_NS_CC;

Color3B Game::pickColor(int i)
{
    if (i == 1) return Color3B::RED;
    if (i == 2) return Color3B::GREEN;
    if (i == 3) return Color3B::BLUE;
    if (i == 4) return Color3B::YELLOW;
    if (i == 5) return Color3B::ORANGE;
    if (i == 6) return Color3B::MAGENTA;
    if (i == 7) return Color3B::WHITE;
    if (i == 8) return Color3B::GRAY;
    return Color3B::WHITE;
}

bool Game::init()
{
    if (!Scene::init()) return false;
    
    auto visibleSize = Director::getInstance()->getVisibleSize();
        
    auto background = Sprite::create("background.png", Rect(0,0,visibleSize.width,visibleSize.height));
    background->setPosition(0,0);
    background->setAnchorPoint(Vec2(0,0));
    addChild(background);
                
    populate(15,9,8);                


    return true;
}

void Game::populate(int height, int width, int colors)
{
    this->height = height;
    this->width = width;
    colorCount = colors;
    floorHeight = height*sizeOfBlock;
    floorWidth = width*sizeOfBlock;
    floor = Sprite::create("background.png", Rect(0,0,floorWidth,floorHeight));
    floor->setColor(Color3B::BLACK);

    arr = new Sprite**[width];
        for (int i = 0; i < width; i++)
        {
            arr[i] = new Sprite*[height];
            for (int j = 0; j < height; j++)
            {
                Sprite *sprite = Sprite::create("block.png");
                sprite->setColor(pickColor(RandomHelper::random_int(1,colorCount)));
                sprite->setPosition(Vec2(sizeOfBlock*i, sizeOfBlock*j));
                sprite->setAnchorPoint(Vec2(0,0));
                
                floor->addChild(sprite);
                arr[i][j] = sprite;
            }
        }
        auto visibleSize = Director::getInstance()->getVisibleSize();
        float scaleFactor = MIN((visibleSize.width - 2*offset)/floorWidth,
                                (visibleSize.height - 2*offset)/floorHeight);
        floor->setScale(scaleFactor);
        floor->setPosition(visibleSize.width/2,visibleSize.height/2);
        addChild(floor);
}

void Game::clear()
{
    floor->removeAllChildren();
    floor->removeFromParent();
    floor = nullptr;
    for (int i = 0; i < width; i++)
    {
        delete(arr[i]);
    }
    delete(arr);
    arr = nullptr;
    height = 0;
    width = 0;
}
