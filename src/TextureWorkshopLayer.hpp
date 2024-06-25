#pragma once

using namespace geode::prelude;
#include <Geode/ui/TextInput.hpp>
#include <Geode/utils/web.hpp>
#include <Geode/loader/Event.hpp>
#include "TexturePack.hpp"

class TextureWorkshopLayer : public cocos2d::CCLayer, TextInputDelegate {
protected:
    cocos2d::CCSprite* m_background;
    void parseJson(std::string str);
    bool search = false;
    bool noresults = false;
    ScrollLayer* scroll = nullptr;
    
    CCScale9Sprite* bg;
    CCSprite* outline;
    CCLabelBMFont* tpAmount;
    CCMenuItemSpriteExtra* refreshButton;
    CCMenuItemSpriteExtra* searchBtn;
    CCMenu* buttonMenu;
    std::string inputText;
    EventListener<web::WebTask> m_listener;


    virtual void textChanged(CCTextInputNode* p0);
    
public:
    static TextureWorkshopLayer* create();
    static cocos2d::CCScene* scene();
    static inline TextureWorkshopLayer* get = nullptr;
    bool init();
    TextInput* inp;
    std::vector<TexturePack*> tps = {};
    void onClose(CCObject*);
    void searchTPs();
    void keyBackClicked();
    void getTexturePacks();
    void onGetTPsFinished();
    void onDiscord(CCObject*);
    void onSupport(CCObject*);
    void onPacksFolder(CCObject*);
    void onRefresh(CCObject*);
    void onCredits(CCObject*);
    void onRefreshSearch(CCObject*);
    void onFilter(CCObject*);
};