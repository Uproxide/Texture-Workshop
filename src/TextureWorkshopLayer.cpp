#include <Geode/Geode.hpp>

using namespace geode::prelude;

#include "TextureWorkshopLayer.hpp"
#include "TexturePackCell.hpp"
#include <Geode/Geode.hpp>
#include <Geode/utils/web.hpp>
#include <Geode/ui/TextInput.hpp>
#include <cctype>
#include <algorithm>
#include <matjson.hpp>

TextureWorkshopLayer* TextureWorkshopLayer::create() {
    auto ret = new TextureWorkshopLayer();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
};

CCScene* TextureWorkshopLayer::scene() {
    auto layer = TextureWorkshopLayer::create();
    auto scene = CCScene::create();
    scene->addChild(layer);
    return scene;
}

bool TextureWorkshopLayer::init() {
    if(!CCLayer::init())
        return false;

    auto director = CCDirector::sharedDirector();
    auto winSize = director->getWinSize();

    CCSprite* backSpr = CCSprite::createWithSpriteFrameName("GJ_arrow_03_001.png");
    CCMenuItemSpriteExtra* backBtn = CCMenuItemSpriteExtra::create(backSpr, this, menu_selector(TextureWorkshopLayer::onClose));

    CCMenu* backMenu = CCMenu::create();
    backMenu->addChild(backBtn);
    addChild(backMenu, 1);

    backMenu->setPosition(ccp(director->getScreenLeft() + 25.f, director->getScreenTop() - 22.f));
    setKeyboardEnabled(true);
    setKeypadEnabled(true);

    m_background = CCSprite::createWithSpriteFrameName("TWS_Background.png"_spr);
    m_background->setAnchorPoint({ 0.f, 0.f });
    addChild(m_background, -2);

    auto bg = cocos2d::extension::CCScale9Sprite::create("square02_small.png");

    this->addChild(bg);

    bg->setPosition(winSize / 2);
    bg->setContentHeight(255);
    bg->setContentWidth(380);
    bg->setOpacity(135);
    

    auto logo = CCSprite::createWithSpriteFrameName("TWS_Logo.png"_spr);
    this->addChild(logo);
    logo->setPosition(winSize / 2);
    logo->setPositionY(director->getScreenTop() - 20);
    logo->setScale(0.8);

    outline = CCSprite::createWithSpriteFrameName("TWS_Outline.png"_spr);
    this->addChild(outline);
    outline->setPosition(winSize / 2);
    outline->setScale(1.2);
    outline->setZOrder(1);

    auto logo2 = CCSprite::createWithSpriteFrameName("TWS_Workshop.png"_spr);
    this->addChild(logo2);
    logo2->setPosition(winSize / 2);
    logo2->setPositionY(director->getScreenTop() - 35);
    logo2->setScale(0.5);
    logo2->setZOrder(2);

    CCMenu* buttonMenu = CCMenu::create();
    addChild(buttonMenu, 1);

    auto discordSprite = CCSprite::createWithSpriteFrameName("gj_discordIcon_001.png");
    auto discordButton = CCMenuItemSpriteExtra::create(
        discordSprite,
        this,
        menu_selector(TextureWorkshopLayer::onDiscord)
    );
    buttonMenu->addChild(discordButton);

    buttonMenu->setPosition(ccp(director->getScreenRight() - 20, director->getScreenBottom() + 20));

    auto supportSprite = CCSprite::createWithSpriteFrameName("geode.loader/gift.png");
    auto supportButton = CCMenuItemSpriteExtra::create(
        supportSprite,
        this,
        menu_selector(TextureWorkshopLayer::onSupport)
    );
    buttonMenu->addChild(supportButton);
    supportButton->setPositionY(discordButton->getPositionY() + 35);

    getTexturePacks();

    return true;
}

void TextureWorkshopLayer::onDiscord(CCObject*) {
    geode::createQuickPopup(
        "Discord",
        "Join the Discord for Support, Texture Pack Submissions, and More!",
        "No Thanks", "Ok",
        [](auto, bool btn2) {
            if (btn2) {
                geode::utils::web::openLinkInBrowser("https://discord.gg/pUGPY9hQ22");
            }
        }
    );
}

void TextureWorkshopLayer::onSupport(CCObject*) {
    geode::createQuickPopup(
        "Support Me",
        "Making mods like this is fun, and thats thanks to you! if you want to support my shinanigans, Please support me on Patreon!",
        "No Thanks", "Ok",
        [](auto, bool btn2) {
            if (btn2) {
                geode::utils::web::openLinkInBrowser("https://www.patreon.com/uproxide");
            }
        }
    );
}

void TextureWorkshopLayer::getTexturePacks() {
    web::AsyncWebRequest()
        .fetch("https://uproxide.xyz/api/v1/tws/getTPs.php")
        .text()
        .then([this](std::string const& json) {
            parseJson(json);
        })
        .expect([this](std::string const& json) {
            log::error("something went wrong :3");
        });
}

void TextureWorkshopLayer::parseJson(std::string str) {
    if (str == "[]") {
        empty = true;
    } else {
        try {
            tpJson = matjson::parse(str);
            log::info("{}", tpJson);
        } catch (const std::exception& e) {
            log::error("Failed to parse JSON: {}", e.what());
        }
    }

    onGetTPsFinished();
}

void TextureWorkshopLayer::onGetTPsFinished() {
    auto director = CCDirector::sharedDirector();
    auto winSize = director->getWinSize();

    bool thing = false;

    scroll = ScrollLayer::create(ccp(317, 211));
	scroll->setAnchorPoint(ccp(0, 0));
    scroll->ignoreAnchorPointForPosition(false);

    int basePosY = 207;
    int tpCount = 0;
    this->outline->addChild(scroll);

    scroll->setZOrder(-1);
    scroll->setPositionY(0.5);

	
	scroll->m_contentLayer->removeAllChildren();
	
	if (tpJson.is_object()) {
	    for (const auto& pair : tpJson.as_object()) {
	        const auto& tpObject = pair.second;
            std::string tpName;
            std::string tpCreator;
            std::string tpDownloadURL;
            std::string tpDownloadVersion;
            bool featured;

            tpName = tpObject["packName"].as_string();
            tpCreator = tpObject["packCreator"].as_string();
            tpDownloadURL = tpObject["downloadLink"].as_string();
            tpDownloadVersion = tpObject["packVersion"].as_string();

            if (tpObject["packFeature"].as_int() == 1) {
                featured = true;
            } else {
                featured = false;
            }
            
            thing = !thing;

            tpCount += 1;

	        auto cell = TexturePackCell::create(tpName, tpCreator, tpDownloadURL, tpDownloadVersion, featured, thing);
	        cell->setPositionY(basePosY);
	        scroll->m_contentLayer->addChild(cell);
	        scroll->m_contentLayer->setAnchorPoint(ccp(0,1));
	
	        float height = std::max<float>(scroll->getContentSize().height, 45 * scroll->m_contentLayer->getChildrenCount());
	
	        scroll->m_contentLayer->setContentSize(ccp(scroll->m_contentLayer->getContentSize().width, height));
	
	        CCArrayExt<TexturePackCell*> objects = scroll->m_contentLayer->getChildren();

            int i = 0;
	
	
			for (auto* obj : objects) {
	            i++;
				obj->setPositionY(height - (45 * i));
	
			}

	
	        scroll->moveToTop();
	    }

        std::string countThing = fmt::format("{} Texture Packs", tpCount);

        auto tpAmount = CCLabelBMFont::create(
            countThing.c_str(),
            "goldFont.fnt"
        );
        this->addChild(tpAmount);
        tpAmount->setPosition(winSize / 2);
        tpAmount->setScale(0.475);
        tpAmount->setAnchorPoint(ccp(0.5, 1));
        tpAmount->setPositionY(director->getScreenBottom() + 30);
            
    }
}


void TextureWorkshopLayer::keyBackClicked() {
    TextureWorkshopLayer::onClose(nullptr);
}

void TextureWorkshopLayer::onClose(CCObject*) {
    auto mainMenu = MenuLayer::scene(false);
    CCDirector::sharedDirector()->pushScene(CCTransitionFade::create(0.5f, mainMenu));
}
