#pragma once

using namespace geode::prelude;

class TextureWorkshopLayer : public cocos2d::CCLayer {
protected:
    cocos2d::CCSprite* m_background;
    void parseJson(std::string str);
    bool empty = false;
    ScrollLayer* scroll = nullptr;
    CCScale9Sprite* bg;
    CCSprite* outline;
    CCLabelBMFont* tpAmount;
    CCMenuItemSpriteExtra* refreshButton;
    CCMenu* buttonMenu;
public:
    static TextureWorkshopLayer* create();
    static cocos2d::CCScene* scene();
    bool init();
    void onClose(CCObject*);
    void keyBackClicked();
    void getTexturePacks();
    void onGetTPsFinished();
    void onDiscord(CCObject*);
    void onSupport(CCObject*);
    void onPacksFolder(CCObject*);
    void onRefresh(CCObject*);
};