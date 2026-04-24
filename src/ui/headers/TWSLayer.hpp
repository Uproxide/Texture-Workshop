#pragma once

using namespace geode::prelude;
#include <Geode/ui/TextInput.hpp>
#include <Geode/utils/web.hpp>
#include <Geode/loader/Event.hpp>
#include <Geode/utils/async.hpp>

#include "../../tps/headers/TWSPack.hpp"
#include "../../ui/headers/TWSPackInfo.hpp"

class TWSLayer : public cocos2d::CCLayer, TextInputDelegate {
protected:
    CCLayerGradient* m_background;
    void parseJson(std::string str);
    bool search = false;
    bool noresults = false;
    ScrollLayer* scroll = nullptr;

    bool stupid = true;
    bool loadingAndSetupDoneRandomTP = true;
    
    CCScale9Sprite* bg;

    CCSprite* outline;
    CCSprite* selected1Spr;
    CCSprite* selected2Spr;
    CCSprite* selected3Spr;

    CCLabelBMFont* pageCount;
    
    CCMenuItemSpriteExtra* refreshButton;
    CCMenuItemSpriteExtra* searchBtn;
    CCMenuItemSpriteExtra* prevPage;
    CCMenuItemSpriteExtra* nextPage;
    CCMenuItemSpriteExtra* sortButton;

    CCMenu* buttonMenu;
    std::string inputText;
    async::TaskHolder<geode::utils::web::WebResponse> m_getTPslistener;
    async::TaskHolder<geode::utils::web::WebResponse> m_getTPsCountlistener;
    async::TaskHolder<geode::utils::web::WebResponse> m_getRandomTPListener;
    CCMenu* pagesMenu = nullptr;
    TextInput* inp;

    LoadingCircleSprite* loading;

    matjson::Value pageJson;

    CCNode* randomTPNode = nullptr;

    //int page = 1;

    virtual void textChanged(CCTextInputNode* p0);

    ~TWSLayer();
    
public:
    static TWSLayer* create();
    static cocos2d::CCScene* scene();
    static inline TWSLayer* get = nullptr;
    bool init();
    

    std::vector<TWSPack*> tps = {};

    void onClose(CCObject*);
    void searchTPs();
    void keyBackClicked();

    void getTexturePacks(std::string searchQuery);
    void getTexturePacksCount(std::string searchQuery);
    void setupTPCells();
    void setupRandomTPNode(TWSPack* tp);

    void onDiscord(CCObject*);
    void onSupport(CCObject*);
    void onPacksFolder(CCObject*);
    void onRefresh(CCObject*);
    void onCredits(CCObject*);
    void onRefreshSearch(CCObject*);
    void onSearch(CCObject*);
    void onFilter(CCObject*);
    void onRandom(CCObject*);

    void onSort(CCObject*);

    void onNextPage(CCObject*);
    void onPrevPage(CCObject*);

    void doThingIdrk(float);
};