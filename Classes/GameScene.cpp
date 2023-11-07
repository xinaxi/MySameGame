#include "GameScene.h"

USING_NS_CC;

int clamp(int value, int min, int max)
{
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

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

    //blueish background on full screen
    auto background = Sprite::create("background.png", Rect(0,0,visibleSize.width,visibleSize.height));
    background->setPosition(0,0);
    background->setAnchorPoint(Vec2(0,0));
    addChild(background);
    offset = visibleSize.width/30;

    //listener for all the blocks in the game
    listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = [this](Touch *touch, Event *event)
    {
        //checking the intersection with block
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        Vec2 locationInNode = target->convertToNodeSpace(touch->getLocation());
        if (Rect(0, 0, sizeOfBlock, sizeOfBlock).containsPoint(locationInNode))
        {
            manageTouch(target);
            return true;
        }
        return false;
    };
    listener->retain();     //otherwise autoreleasePool would erase it
    

    //setup ui till the end of that method
    auto interface = Node::create();
    interface->setAnchorPoint({0,1});
    interface->setPosition(0,visibleSize.height - offset/5);
    float xPos = offset;                        //position to put next widget
    float fontSize = visibleSize.height/30;
    background->addChild(interface);


    auto labelHeight = Label::createWithSystemFont(strHeight, fontType, fontSize);
    labelHeight->setAnchorPoint({0,1});
    labelHeight->setPosition({xPos,0});
    xPos += labelHeight->getContentSize().width + offset/2;
    interface->addChild(labelHeight);
      
    auto textFieldHeight = ui::TextField::create(std::to_string(minHeight)+" .. "+std::to_string(maxHeight),fontType,fontSize);
    textFieldHeight->setAnchorPoint({0,1});
    textFieldHeight->setPosition({xPos,0});
    xPos += 3*offset;
    interface->addChild(textFieldHeight);


    auto labelWidth = Label::createWithSystemFont(strWidth, fontType, fontSize);
    labelWidth->setAnchorPoint({0,1});
    labelWidth->setPosition({xPos,0});
    xPos += labelWidth->getContentSize().width + offset/2;
    interface->addChild(labelWidth);
      
    auto textFieldWidth = ui::TextField::create(std::to_string(minWidth)+" .. "+std::to_string(maxWidth),fontType,fontSize);
    textFieldWidth->setAnchorPoint({0,1});
    textFieldWidth->setPosition({xPos,0});
    xPos += 3*offset;
    interface->addChild(textFieldWidth);


    auto labelColors = Label::createWithSystemFont(strColors, fontType, fontSize);
    labelColors->setAnchorPoint({0,1});
    labelColors->setPosition({xPos,0});
    xPos += labelColors->getContentSize().width + offset/2;
    interface->addChild(labelColors);
      
    auto textFieldColors = ui::TextField::create(std::to_string(minColorCount)+" .. "+std::to_string(maxColorCount),fontType,fontSize);
    textFieldColors->setAnchorPoint({0,1});
    textFieldColors->setPosition({xPos,0});
    xPos += 5*offset;
    interface->addChild(textFieldColors);

    textFieldScore = ui::TextField::create("",fontType,fontSize);
    textFieldScore->setAnchorPoint({0,1});
    textFieldScore->setPosition({xPos,0});
    interface->addChild(textFieldScore);

    auto button = ui::Button::create();
    button->setTitleText(strStart);
    button->setTitleFontSize(fontSize);
    button->setAnchorPoint({1,1});
    button->setPosition({visibleSize.width - offset, 0});
    interface->addChild(button);

    button->addTouchEventListener([=](Ref* sender, ui::Widget::TouchEventType type){
        if (type == ui::Widget::TouchEventType::ENDED)
        {
            auto tryStoi = [](ui::TextField* textField, int defaultValue)
            {
                try
                {
                    return std::stoi(textField->getString());
                }
                catch(...)
                {
                    return defaultValue;
                }
            };
            
            int h = tryStoi(textFieldHeight, defaultHeight);
            int w = tryStoi(textFieldWidth, defaultWidth);
            int c = tryStoi(textFieldColors, defaultColorCount);

            h = clamp(h, minHeight, maxHeight);
            w = clamp(w, minWidth, maxWidth);
            c = clamp(c, minColorCount, maxColorCount);

            textFieldHeight->setString(std::to_string(h));
            textFieldWidth->setString(std::to_string(w));
            textFieldColors->setString(std::to_string(c));

            this->resize(h, w, c);
        }
    });

    populate();

    return true;
}

void Game::populate(int height, int width, int colors)
{
    this->height = height;
    this->width = width;
    colorCount = colors;
    score = 0;
    setScore();
    floorHeight = height*sizeOfBlock;
    floorWidth = width*sizeOfBlock;
    floor = Sprite::create("background.png", Rect(0,0,floorWidth,floorHeight));     //black grid just right the size of all blocks
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
    //we want some offset from the screen edges
    auto visibleSize = Director::getInstance()->getVisibleSize();
    float scaleFactor = MIN((visibleSize.width - 2*offset)/floorWidth,
                            (visibleSize.height - 2*offset)/floorHeight);    //depends on which side is closer to the edge
    floor->setScale(scaleFactor);
    floor->setPosition(visibleSize.width/2,visibleSize.height/2);       //centering
    addChild(floor);
    setListeners();
}

void Game::clear()
{
    floor->removeAllChildren();
    floor->removeFromParent();
    floor = nullptr;
    for (int i = 0; i < width; i++)
    {
        delete[] arr[i];
    }
    delete[] arr;
    arr = nullptr;
    height = 0;
    width = 0;
}

void Game::setListeners()
{
    for (int i = 0; i < width; i++)
    {
        for (int j = 0; j < height; j++)
        {
            _eventDispatcher->addEventListenerWithSceneGraphPriority(listener->clone(), arr[i][j]);
        }
    }
}

//finding coordinates in our own array
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

    //launch recursive function to find a group of same color
    std::set<Vec2> set;     //remembering coordinates of blocks
    findAll(set, first.x, first.y, block->getColor());
    if (set.size() >= 3)
    {
        removeSet(set);

        //move all that were upper the group
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

    //if no more moves avalible restart the game
    if (!check())
    {
        resize(height, width, colorCount);
    }
}

//finding the group of blocks with the same color
void Game::findAll(std::set<Vec2> &set, int i, int j, Color3B color)
{
    if (!arr[i][j] || arr[i][j]->getColor() != color)
    {
        return;
    }

    set.insert(Vec2(i,j));    //only unique values are getting in

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
    score += set.size()*(set.size()-1);
    setScore();
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

void Game::resize(int newHeight, int newWidth, int newColors)
{
    clear();
    populate(newHeight, newWidth, newColors);
}

//check if there are still moves
bool Game::check()
{
    for (int i = 0; i < width; i++)
    {
        for (int j = 0; j < height; j++)
        {
            std::set<Vec2> set;
            if (!arr[i][j]) continue;
            findAll(set, i, j, arr[i][j]->getColor());
            if (set.size() >= 3)
            {
                return true;
            }
        }   
    }
    return false;
}

void Game::setScore()
{
    textFieldScore->setString(std::to_string(score));
}
