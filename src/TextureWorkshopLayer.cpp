#include <Geode/Geode.hpp>

using namespace geode::prelude;

#include "TextureWorkshopLayer.hpp"
#include "TexturePackCell.hpp"
#include <Geode/Geode.hpp>
#include <Geode/utils/web.hpp>
#include <cctype>
#include <algorithm>
#include <matjson.hpp>
#include <Geode/ui/GeodeUI.hpp>
#include <Geode/loader/Event.hpp>
#include "JsonManager.hpp"

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

    get = this;

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
    m_background->setScale(1.5);
    addChild(m_background, -2);

    m_background->setScaleX((winSize.width + 10.f) / m_background->getTextureRect().size.width);
    m_background->setScaleY((winSize.height + 10.f) / m_background->getTextureRect().size.height);
    m_background->setPosition(ccp(-5, -5));

    auto bg = cocos2d::extension::CCScale9Sprite::create("square02_small.png");

    this->addChild(bg);

    bg->setPosition(winSize / 2);
    bg->setContentHeight(252);
    bg->setContentWidth(380);
    bg->setOpacity(135);
    bg->setPositionY(bg->getPositionY() - 15);

    outline = CCSprite::createWithSpriteFrameName("TWS_Outline.png"_spr);
    this->addChild(outline);
    outline->setPosition(winSize / 2);
    outline->setScale(1.2);
    outline->setZOrder(1);

    buttonMenu = CCMenu::create();
    addChild(buttonMenu, 1);
    
    buttonMenu->setPosition(0, 0);
    buttonMenu->setContentSize(winSize);

    auto discordSprite = CCSprite::createWithSpriteFrameName("gj_discordIcon_001.png");
    auto discordButton = CCMenuItemSpriteExtra::create(
        discordSprite,
        this,
        menu_selector(TextureWorkshopLayer::onDiscord)
    );
    buttonMenu->addChild(discordButton);
    discordButton->setPosition(ccp(director->getScreenRight() - 20, director->getScreenBottom() + 20));

    auto supportSprite = CCSprite::createWithSpriteFrameName("geode.loader/gift.png");
    auto supportButton = CCMenuItemSpriteExtra::create(
        supportSprite,
        this,
        menu_selector(TextureWorkshopLayer::onSupport)
    );
    buttonMenu->addChild(supportButton);
    supportButton->setPosition(ccp(director->getScreenRight() - 20, director->getScreenBottom() + 55));

    auto creditsSprite = CCSprite::createWithSpriteFrameName("TWS_Credits.png"_spr);
    creditsSprite->setScale(0.7);
    auto creditsButton = CCMenuItemSpriteExtra::create(
        creditsSprite,
        this,
        menu_selector(TextureWorkshopLayer::onCredits)
    );
    buttonMenu->addChild(creditsButton);
    creditsButton->setPosition(ccp(director->getScreenRight() - 20, director->getScreenBottom() + 90));

    auto refreshSpr = CCSprite::createWithSpriteFrameName("TWS_RefreshButton.png"_spr);
    refreshSpr->setScale(0.8);
    refreshButton = CCMenuItemSpriteExtra::create(
        refreshSpr,
        this,
        menu_selector(TextureWorkshopLayer::onRefresh)
    );
    buttonMenu->addChild(refreshButton);
    refreshButton->setPosition(ccp(director->getScreenLeft() + 25, director->getScreenBottom() + 65));
    refreshButton->setVisible(false);

    auto filesSpr = CCSprite::createWithSpriteFrameName("TWS_FileButton.png"_spr);
    filesSpr->setScale(0.8);
    auto filesBtn = CCMenuItemSpriteExtra::create(
        filesSpr,
        this,
        menu_selector(TextureWorkshopLayer::onPacksFolder)
    );
    buttonMenu->addChild(filesBtn);
    filesBtn->setPosition(ccp(director->getScreenLeft() + 25, director->getScreenBottom() + 25));

    inp = TextInput::create(300, "Search", "bigFont.fnt");
    inp->setContentHeight(20);
    inp->setAnchorPoint(ccp(0, 0));
    inp->ignoreAnchorPointForPosition(false); 
    this->outline->addChild(inp);

    inp->setPosition(ccp(6, 194.2));
    inp->hideBG();
    auto inputNode = inp->getInputNode();
    inputNode->setPositionY(inputNode->getPositionY() - 5);
    inputNode->setPositionX(5);
    inputNode->m_placeholderLabel->setAnchorPoint(ccp(0, 0.5));
    inputNode->m_placeholderLabel->setScale(0.5);
    inp->setDelegate(this);
    inp->setCommonFilter(CommonFilter::Any);
    

    getTexturePacks();

    if (auto available = Mod::get()->hasAvailableUpdate()) {
         auto popup = createQuickPopup(
            "Update Available!",
            fmt::format(
                "<cl>Texture Workshop</c> has a new update!\n<ca>{}</c> -> <cg>{}</c>\n\n"
                "Would you like to <cj>Update?</c>",
                Mod::get()->getVersion(),
                available.value()
            ),
            "OK", "Update",
            [](auto, bool btn2) {
                if (btn2) {
                    openInfoPopup(Mod::get());
                }
            },
            false
        );
        popup->m_scene = this;
        popup->show();
    }

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
        "Making mods like this is fun, and thats thanks to you! if you want to support my shinanigans, Please support me on Ko-Fi!",
        "No Thanks", "Ok",
        [](auto, bool btn2) {
            if (btn2) {
                geode::utils::web::openLinkInBrowser("https://ko-fi.com/uproxide");
            }
        }
    );
}

void TextureWorkshopLayer::onCredits(CCObject*) {
    FLAlertLayer::create(
        "Credits",
        "<cg>Uproxide</c> - Main Developer\n<cr>TheSillyDoggo</c> - Help with Searching\n<cl>Brift</c> - Sprites\n<cp>Riley</c> - Moral and Emotional Support",
        "Ok"
    )->show();
}

void TextureWorkshopLayer::onRefresh(CCObject*) {
    JsonManager::downloaded = false;
    scroll->removeFromParent();
    tpAmount->removeFromParent();
    refreshButton->setVisible(false);
    scroll = nullptr;
    getTexturePacks();
}

void TextureWorkshopLayer::onRefreshSearch(CCObject*) {
    JsonManager::downloaded = true;
    scroll->removeFromParent();
    tpAmount->removeFromParent();
    refreshButton->setVisible(false);
    scroll = nullptr;
    getTexturePacks();
}

void TextureWorkshopLayer::onPacksFolder(CCObject*) {
    utils::file::openFolder(Loader::get()->getInstalledMod("geode.texture-loader")->getConfigDir());
}

void TextureWorkshopLayer::getTexturePacks() {

    if (!JsonManager::downloaded) {
        m_listener.bind([this] (web::WebTask::Event* e) {
            if (web::WebResponse* res = e->getValue()) {
                JsonManager::tpJson = res->json().value();
                JsonManager::downloaded = true;
                onGetTPsFinished();
            } else if (e->isCancelled()) {
                log::info("The request was cancelled... So sad :(");
            }
        });

        auto req = web::WebRequest();
        
        m_listener.setFilter(req.get("https://uproxide.xyz/api/v1/tws/getTPs.php"));

    } else {
        onGetTPsFinished();
    }
    
}

void TextureWorkshopLayer::parseJson(std::string str) {
    if (str == "[]") {
        
    } else {
        try {
            JsonManager::tpJson = matjson::parse(str);
            log::info("{}", JsonManager::tpJson);
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

    scroll = ScrollLayer::create(ccp(313, 180));
	scroll->setAnchorPoint(ccp(0, 0));
    scroll->ignoreAnchorPointForPosition(false);


    int basePosY = 207;
    int tpCount = 0;
    this->outline->addChild(scroll);

    scroll->setZOrder(-1);
    scroll->setPositionX(0);
    scroll->setPositionY(8);
    

	
	scroll->m_contentLayer->removeAllChildren();
	
	if (JsonManager::tpJson.is_object() && !search) {
	    for (const auto& pair : JsonManager::tpJson.as_object()) {
	        const auto& tpObject = pair.second;
            std::string tpName;
            std::string tpCreator;
            std::string tpDownloadURL;
            std::string tpDownloadVersion;
            std::string tpIcon;
            std::string tpDesc;
            bool featured;

            tpName = tpObject["packName"].as_string();
            tpCreator = tpObject["packCreator"].as_string();
            tpDownloadURL = tpObject["downloadLink"].as_string();
            tpDownloadVersion = tpObject["packVersion"].as_string();
            tpIcon = tpObject["packLogo"].as_string();
            tpDesc = tpObject["packDescription"].as_string();

            if (tpObject["packFeature"].as_int() == 1) {
                featured = true;
            } else {
                featured = false;
            }
            
            thing = !thing;

            tpCount += 1;
            
            TexturePack* tp = TexturePack::create(tpName, tpCreator, tpDownloadURL, tpIcon, tpDownloadVersion, tpDesc, featured);

	        auto cell = TexturePackCell::create(tp, thing);
            tps.push_back(tp);
	        cell->setPositionY(basePosY);
	        scroll->m_contentLayer->addChild(cell);
	        scroll->m_contentLayer->setAnchorPoint(ccp(0,1));
	
	        float height = std::max<float>(scroll->getContentSize().height, 35 * scroll->m_contentLayer->getChildrenCount());
	
	        scroll->m_contentLayer->setContentSize(ccp(scroll->m_contentLayer->getContentSize().width, height));
	
	        CCArrayExt<TexturePackCell*> objects = scroll->m_contentLayer->getChildren();

            int i = 0;
	
	
			for (auto* obj : objects) {
	            i++;
				obj->setPositionY(height - (35 * i));
	
			}

	
	        scroll->moveToTop();
	    }

        std::string countThing = fmt::format("{} Texture Packs", tpCount);

        tpAmount = CCLabelBMFont::create(
            countThing.c_str(),
            "goldFont.fnt"
        );
        this->addChild(tpAmount);
        tpAmount->setPosition(winSize / 2);
        tpAmount->setScale(0.475);
        tpAmount->setAnchorPoint(ccp(0.5, 1));
        tpAmount->setPositionY(director->getScreenBottom() + 13);
        tpAmount->setZOrder(5);
        refreshButton->setVisible(true);
        
    }

}

void TextureWorkshopLayer::textChanged(CCTextInputNode* p0){
    searchTPs();
}

void TextureWorkshopLayer::searchTPs() {
    auto content = scroll->m_contentLayer;

    bool thing = false;

    content->removeAllChildren();

    for (size_t i = 0; i < tps.size(); i++)
    {
        thing = !thing;

        auto name = inp->getString();

        if (inp->getString().starts_with("by:")) {
            if(utils::string::toLower(tps[i]->creator).find(utils::string::toLower(name.substr(name.find(":") + 1, name.size() - name.find(":")))) != std::string::npos)
            {
                auto cell = TexturePackCell::create(tps[i], thing);

                content->addChild(cell);
            }
        } else {
            if (utils::string::toLower(tps[i]->name).find(utils::string::toLower(inp->getString())) != std::string::npos)
            {
                auto cell = TexturePackCell::create(tps[i], thing);

                content->addChild(cell);
            }
        }
    }

    float height = std::max<float>(scroll->getContentSize().height, 35 * content->getChildrenCount());

    scroll->m_contentLayer->setContentSize(ccp(scroll->m_contentLayer->getContentSize().width, height));
	
	CCArrayExt<TexturePackCell*> objects = scroll->m_contentLayer->getChildren();

    int i = 0;
	
	
	for (auto* obj : objects) {
	    i++;
		obj->setPositionY(height - (35 * i));
	}

	
	scroll->moveToTop();
}


void TextureWorkshopLayer::keyBackClicked() {
    TextureWorkshopLayer::onClose(nullptr);
}

void TextureWorkshopLayer::onClose(CCObject*) {
    auto mainMenu = MenuLayer::scene(false);
    CCDirector::sharedDirector()->pushScene(CCTransitionFade::create(0.5f, mainMenu));
}
