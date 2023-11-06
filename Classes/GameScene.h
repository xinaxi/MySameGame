#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "cocos2d.h"
#include <set>

USING_NS_CC;

static int sizeOfBlock = 40;
static int offset = 60;
static int defaultHeight = 10;
static int defaultWidth = 16;
static int defaultColorCount = 3;
static float speed = 3.0;

class Game : public cocos2d::Scene
{
    Sprite ***arr = nullptr;
    int height = 0;
    int width = 0;
    
    Sprite *floor = nullptr;
    float floorHeight = 0;
    float floorWidth = 0;
    
    int colorCount = 0;
    
    Color3B pickColor(int);
    
    void populate(int = defaultHeight, int = defaultWidth, int = defaultColorCount);
    void clear();
    void setListeners(EventListenerTouchOneByOne *);
    Vec2 findBlock(Sprite *);
    void manageTouch(Sprite *);
    void findAll(std::set<Vec2> &, int, int, Color3B);
    void removeSet(std::set<Vec2> &);
    void moveBlock(int, int, int);

public:

    virtual bool init();
        
    // implement the "static create()" method manually
    CREATE_FUNC(Game);
};

#endif // __GAME_SCENE_H__