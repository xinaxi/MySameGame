#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include <set>


USING_NS_CC;

static int sizeOfBlock = 40;
static int defaultHeight = 10;
static int defaultWidth = 16;
static int defaultColorCount = 3;
static float speed = 3.0;

static std::string fontType = "Arial";
static std::string strStart = "Старт";
static std::string strHeight = "Высота";
static std::string strWidth = "Ширина";
static std::string strColors = "Цвета";

static int minHeight = 1;
static int maxHeight = 30;
static int minWidth = 1;
static int maxWidth = 30;
static int minColorCount = 2;
static int maxColorCount = 8;


class Game : public cocos2d::Scene
{
    Sprite ***arr = nullptr;
    int height = 0;
    int width = 0;
    int score = 0;
    ui::TextField *textFieldScore = nullptr;

    EventListenerTouchOneByOne *listener = nullptr;
    
    Sprite *floor = nullptr;
    float floorHeight = 0;
    float floorWidth = 0;
    int offset = 0;

    int colorCount = 0;
    
    Color3B pickColor(int);
    
    void populate(int = defaultHeight, int = defaultWidth, int = defaultColorCount);
    void clear();
    void setListeners();
    Vec2 findBlock(Sprite *);
    void manageTouch(Sprite *);
    void findAll(std::set<Vec2> &, int, int, Color3B);
    void removeSet(std::set<Vec2> &);
    void moveBlock(int, int, int);
    void resize(int = defaultHeight, int = defaultWidth, int = defaultColorCount);
    bool check();
    void setScore();
    
public:

    virtual bool init();
        
    // implement the "static create()" method manually
    CREATE_FUNC(Game);
};

#endif // __GAME_SCENE_H__