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
                
    populate(19,17,5);                

    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = [this](Touch *touch, Event *event)
    {
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        Vec2 locationInNode = target->convertToNodeSpace(touch->getLocation());
        if (Rect(0, 0, sizeOfBlock, sizeOfBlock).containsPoint(locationInNode))
        {
            manageTouch(target);
            return true;
        }
        return false;
    };

    setListeners(listener);

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

void Game::setListeners(EventListenerTouchOneByOne *listener)
{
    for (int i = 0; i < width; i++)
    {
        for (int j = 0; j < height; j++)
        {
            _eventDispatcher->addEventListenerWithSceneGraphPriority(listener->clone(), arr[i][j]);
        }
    }
}

Vec2 Game::findBlock(Sprite *block)
{
    for (int i = 0; i < width; i++)
    {
        for (int j = 0; j < height; j++)
        {
            if (arr[i][j] == block)
            {
                return Vec2(i,j);
            }
        }
    }
    log("you can't be here!");
}

void Game::manageTouch(Sprite *block)
{
    Vec2 first = findBlock(block);
    int i = first.x;
    int j = first.y;

    std::set<Vec2> set;
    findAll(set, first.x, first.y, block->getColor());
    if (set.size() >= 3)
    {
        removeSet(set);
        for (int i = 0; i < width; i++)
        {
            int down = 0;                       //how many empty blocks in a column
            for (int j = 0; j < height; j++)
            {
                if (arr[i][j]) 
                {
                    moveBlock(i,j,down);
                }
                else down++;
            }
        }
    }
}

void Game::findAll(std::set<Vec2> &set, int i, int j, Color3B color)
{
    if (!arr[i][j] || arr[i][j]->getColor() != color)
    {
        return;
    }

    set.insert(Vec2(i,j));

    //checking upper
    if ((j + 1 < height) && !set.count(Vec2(i,j+1)))
    {
        findAll(set, i, j + 1, color);
    }

    //lower
    if ((j > 0) && !set.count(Vec2(i,j-1)))
    {
        findAll(set, i, j - 1, color);
    }

    //left
    if ((i > 0) && !set.count(Vec2(i-1,j)))
    {
        findAll(set, i - 1, j, color);
    }

    //right
    if ((i + 1 < width) && !set.count(Vec2(i+1,j)))
    {
        findAll(set, i + 1, j, color);
    }
}

void Game::removeSet(std::set<Vec2> &set)
{
    for (auto elem : set)
    {
        int i = elem.x;
        int j = elem.y;
        arr[i][j]->removeFromParent();
        arr[i][j] = nullptr;
    }
}

void Game::moveBlock(int i, int j, int down)
{
    if (!down) return;
    int newJ = j - down;
    arr[i][newJ] = arr[i][j];
    arr[i][j] = nullptr;
    auto move = MoveBy::create(down/speed, Vec2(0, -down*sizeOfBlock));
    arr[i][newJ]->runAction(move);
}