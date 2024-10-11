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
#include "boobs.hpp"
#include "TWSFilters.hpp"
#include "BlockTouchLayer.hpp"

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

    setID("TextureWorkshopLayer");

    auto director = CCDirector::sharedDirector();
    auto winSize = director->getWinSize();

    get = this;

    CCSprite* backSpr = CCSprite::createWithSpriteFrameName("GJ_arrow_03_001.png");
    CCMenuItemSpriteExtra* backBtn = CCMenuItemSpriteExtra::create(backSpr, this, menu_selector(TextureWorkshopLayer::onClose));
    backBtn->setID("back-button");

    CCMenu* backMenu = CCMenu::create();
    backMenu->setID("back-menu");
    backMenu->addChild(backBtn);
    addChild(backMenu, 1);

    backMenu->setPosition(ccp(director->getScreenLeft() + 25.f, director->getScreenTop() - 22.f));
    setKeyboardEnabled(true);
    setKeypadEnabled(true);

    m_background = CCLayerGradient::create({181, 69, 20, 255}, {48, 15, 104, 255}, {1, -1});
    m_background->setID("background");
    m_background->setAnchorPoint({ 0.f, 0.f });
    m_background->setContentSize(CCDirector::get()->getWinSize());
    addChild(m_background, -3);

    auto spriteTexture = CCSprite::create("TWS_Cubes.png"_spr);
    spriteTexture->setAnchorPoint({0, 0});

    ccTexParams params = {GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT};
    spriteTexture->getTexture()->setTexParameters(&params);

    auto rect = spriteTexture->getTextureRect();
    rect.size = rect.size * (CCPoint(winSize) / CCPoint(spriteTexture->getScaledContentSize()) * 2);
    rect.origin = CCPoint{0, 0};
    spriteTexture->setTextureRect(rect);
    spriteTexture->setRotation(18);
    spriteTexture->setOpacity(100);
    spriteTexture->setPosition({-winSize.width/2, 0});
    spriteTexture->setID("background-overlay");
    spriteTexture->setBlendFunc({ GL_SRC_ALPHA, GL_ONE_MINUS_CONSTANT_ALPHA });
    spriteTexture->runAction(CCRepeatForever::create(CCSequence::create(
        CCMoveBy::create(5, {16, 49}),
        CCMoveBy::create(0, {-16, -49}),
        nullptr
    )));

    addChild(spriteTexture, -2);

    auto bg = cocos2d::extension::CCScale9Sprite::create("square02_small.png");
    bg->setID("content-background");
    this->addChild(bg);

    bg->setPosition(winSize / 2);
    bg->setContentHeight(252);
    bg->setContentWidth(380);
    bg->setOpacity(135);
    bg->setPositionY(bg->getPositionY() - 15);

    outline = CCSprite::createWithSpriteFrameName("TWS_Outline.png"_spr);
    outline->setID("outline");
    this->addChild(outline);
    outline->setPosition(winSize / 2);
    outline->setScale(1.2);
    outline->setZOrder(1);

    auto block = BlockTouchLayer::create();
    block->ignoreAnchorPointForPosition(false);
    block->setAnchorPoint(ccp(0, 0.5f));
    block->setPositionY(outline->getContentHeight());
    block->setContentSize(ccp(block->getContentWidth(), 25));
    block->setTouchEnabled(true);

    outline->addChild(block);

    buttonMenu = CCMenu::create();
    buttonMenu->setID("button-menu");
    addChild(buttonMenu, 1);
    
    buttonMenu->setPosition(0, 0);
    buttonMenu->setContentSize(winSize);

    auto discordSprite = CCSprite::createWithSpriteFrameName("gj_discordIcon_001.png");
    auto discordButton = CCMenuItemSpriteExtra::create(
        discordSprite,
        this,
        menu_selector(TextureWorkshopLayer::onDiscord)
    );
    discordButton->setID("discord-button");
    buttonMenu->addChild(discordButton);
    discordButton->setPosition(ccp(director->getScreenRight() - 20, director->getScreenBottom() + 20));

    auto supportSprite = CCSprite::createWithSpriteFrameName("geode.loader/gift.png");
    auto supportButton = CCMenuItemSpriteExtra::create(
        supportSprite,
        this,
        menu_selector(TextureWorkshopLayer::onSupport)
    );
    supportButton->setID("support-button");
    buttonMenu->addChild(supportButton);
    supportButton->setPosition(ccp(director->getScreenRight() - 20, director->getScreenBottom() + 55));

    auto creditsSprite = CCSprite::createWithSpriteFrameName("TWS_Credits.png"_spr);
    creditsSprite->setScale(0.7);
    auto creditsButton = CCMenuItemSpriteExtra::create(
        creditsSprite,
        this,
        menu_selector(TextureWorkshopLayer::onCredits)
    );
    creditsButton->setID("credits-button");
    buttonMenu->addChild(creditsButton);
    creditsButton->setPosition(ccp(director->getScreenRight() - 20, director->getScreenBottom() + 90));

    auto refreshSpr = CCSprite::createWithSpriteFrameName("TWS_RefreshButton.png"_spr);
    refreshSpr->setScale(0.8);
    refreshButton = CCMenuItemSpriteExtra::create(
        refreshSpr,
        this,
        menu_selector(TextureWorkshopLayer::onRefresh)
    );
    refreshButton->setID("refresh-button");
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
    filesBtn->setID("files-button");
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

    auto inpMenu = CCMenu::create();
    inpMenu->setTouchPriority(-130);
    inpMenu->onEnter();
    inpMenu->setContentSize(inp->getContentSize());
    inp->addChild(inpMenu);
    
    
    auto filterSpr = CCSprite::createWithSpriteFrameName("TWS_SearchButton.png"_spr);
    filterSpr->setScale(0.9);
    auto filterBtn = CCMenuItemSpriteExtra::create(
        filterSpr,
        this,
        menu_selector(TextureWorkshopLayer::onFilter)
    );
    inpMenu->addChild(filterBtn);
    inpMenu->setLayout(
        RowLayout::create()
            ->setAxisAlignment(AxisAlignment::End)
    );
    inpMenu->setPosition(inp->getContentSize() / 2);
    filterBtn->setPositionX(filterBtn->getPositionX() + 5);

    loading = LoadingCircleSprite::create();
    loading->runAction(CCRepeatForever::create(CCRotateBy::create(1, 360)));
    loading->setID("loading");
    loading->setScale(0.6f);
    loading->setPosition(ccp(outline->getContentWidth() / 2, outline->getContentHeight() / 2 + -21.875f));
    outline->addChild(loading);

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

void TextureWorkshopLayer::onFilter(CCObject*) {
    TWSFilters::create()->show();
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
        "<cg>Uproxide</c> - Main Developer\n<co>ShineUA and Alphalaneous</c> - Pull Requests :3\n<cr>TheSillyDoggo</c> - Help with Searching\n<cr>M336</c> - Server\n<cl>Brift</c> - Sprites\n<cp>Riley</c> - Moral and Emotional Support",
        "Ok"
    )->show();
}

void TextureWorkshopLayer::onRefresh(CCObject*) {
    boobs::downloaded = false;
    loading->setVisible(!boobs::downloaded);
    scroll->removeFromParent();
    tpAmount->removeFromParent();
    refreshButton->setVisible(false);
    if (scroll) {
        scroll = nullptr;
        tpAmount = nullptr;
    }
    getTexturePacks();
}

void TextureWorkshopLayer::onRefreshSearch(CCObject*) {
    boobs::downloaded = true;
    scroll->removeFromParent();
    tpAmount->removeFromParent();
    refreshButton->setVisible(false);
    if (scroll) {
        scroll = nullptr;
        tpAmount = nullptr;
    }
    getTexturePacks();
}

void TextureWorkshopLayer::onPacksFolder(CCObject*) {
    utils::file::openFolder(Loader::get()->getInstalledMod("geode.texture-loader")->getConfigDir());
}

void TextureWorkshopLayer::getTexturePacks() {
    auto somethingWentWrong = CCLabelBMFont::create("", "bigFont.fnt");
    this->addChild(somethingWentWrong);
    somethingWentWrong->setScale(0.2);

    if (!boobs::downloaded) {
        m_listener.bind([this, somethingWentWrong] (web::WebTask::Event* e) {
            if (web::WebResponse* res = e->getValue()) {
                if (res->ok()) {
                    somethingWentWrong->setString("");
                    boobs::tpJson = res->json().value();
                    boobs::downloaded = true;
                    onGetTPsFinished();
                } else {
                    std::string error = fmt::format("Something Went Wrong:\n{}", res->string().value());
                    somethingWentWrong->setString(error.c_str());
                    somethingWentWrong->setPosition(CCDirector::get()->getWinSize() / 2);
                    refreshButton->setVisible(true);
                }
            } else if (e->isCancelled()) {
                log::info("The request was cancelled... So sad :(");
            }
        });

        auto req = web::WebRequest();
        req.userAgent(fmt::format("TextureWorkshopMod/{}", Mod::get()->getVersion()));
        req.certVerification(Mod::get()->getSettingValue<bool>("cert-verification"));
        
        m_listener.setFilter(req.get("https://textureworkshop.plusgdps.dev/api/v1/tws/getTPs"));

    } else {
        onGetTPsFinished();
    }
    
}

void TextureWorkshopLayer::parseJson(std::string str) {
    if (str == "[]") {
        
    } else {
        try {
            boobs::tpJson = matjson::parse(str);
            log::info("{}", boobs::tpJson);
        } catch (const std::exception& e) {
            log::error("Failed to parse JSON: {}", e.what());
        }
    }
    onGetTPsFinished();
}

void TextureWorkshopLayer::onGetTPsFinished() {
    if (scroll || tpAmount) {
        scroll->removeFromParent();
        tpAmount->removeFromParent();
    }

    // leaking memory my beloved (it breaks a bunch of things if its fixed to just keep it there :3)
    tps.clear();
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
    cells.clear();
    
    if (boobs::tpJson.is_object() && !search) {
        for (const auto& pair : boobs::tpJson.as_object()) {
            const auto& tpObject = pair.second;
            std::string tpName;
            std::string tpCreator;
            std::string tpDownloadURL;
            std::string tpDownloadVersion;
            std::string tpIcon;
            std::string tpDesc;
            std::string gdVersion;
            bool featured;
            int downloads;

            tpName = tpObject["packName"].as_string();
            tpCreator = tpObject["packCreator"].as_string();
            tpDownloadURL = tpObject["downloadLink"].as_string();
            tpDownloadVersion = tpObject["packVersion"].as_string();
            tpIcon = tpObject["packLogo"].as_string();
            tpDesc = tpObject["packDescription"].as_string();
            gdVersion = tpObject["gdVersion"].as_string();

            if (tpObject.contains("packDownloads")) // idk but i wanna be safe
                downloads = tpObject["packDownloads"].as_int();

            if (tpObject["packFeature"].as_int() == 1) {
                featured = true;
            } else {
                featured = false;
            }
            
            thing = !thing;

            tpCount += 1;

            if (boobs::versionFilter) {
                if (tpObject["gdVersion"].as_string() == Loader::get()->getGameVersion() || tpObject["gdVersion"].as_string() == "Any") {
                    TexturePack* tp = TexturePack::create(tpName, tpCreator, tpDownloadURL, tpIcon, tpDownloadVersion, tpDesc, gdVersion, featured, downloads);

                    auto cell = TexturePackCell::create(tp, thing);
                    cells.push_back(cell);
                    tps.push_back(tp);
                    cell->setPositionY(basePosY);
                    scroll->m_contentLayer->addChild(cell);
                    scroll->m_contentLayer->setAnchorPoint(ccp(0,1));
                } else if (tpObject["gdVersion"].as_string() == "2.204" && (Loader::get()->getGameVersion() == "2.204" || Loader::get()->getGameVersion() == "2.205")) {
                    TexturePack* tp = TexturePack::create(tpName, tpCreator, tpDownloadURL, tpIcon, tpDownloadVersion, tpDesc, gdVersion, featured, downloads);

                    auto cell = TexturePackCell::create(tp, thing);
                    cells.push_back(cell);
                    tps.push_back(tp);
                    cell->setPositionY(basePosY);
                    scroll->m_contentLayer->addChild(cell);
                    scroll->m_contentLayer->setAnchorPoint(ccp(0,1));
                }
            } else {
                TexturePack* tp = TexturePack::create(tpName, tpCreator, tpDownloadURL, tpIcon, tpDownloadVersion, tpDesc, gdVersion, featured, downloads);

                auto cell = TexturePackCell::create(tp, thing);
                cells.push_back(cell);
                tps.push_back(tp);
                cell->setPositionY(basePosY);
                scroll->m_contentLayer->addChild(cell);
                scroll->m_contentLayer->setAnchorPoint(ccp(0,1));
            }     
    
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

        std::string countThing = fmt::format("{} Texture Packs", tps.size());

        tpAmount = CCLabelBMFont::create(
            countThing.c_str(),
            "goldFont.fnt"
        );
        tpAmount->setID("texture-pack-count-label");
        this->addChild(tpAmount);
        tpAmount->setPosition(winSize / 2);
        tpAmount->setScale(0.475);
        tpAmount->setAnchorPoint(ccp(0.5, 1));
        tpAmount->setPositionY(director->getScreenBottom() + 13);
        tpAmount->setZOrder(5);
        refreshButton->setVisible(true);
        
    }

    loading->setVisible(!boobs::downloaded);
}

void TextureWorkshopLayer::textChanged(CCTextInputNode* p0){
    searchTPs();
}

void TextureWorkshopLayer::searchTPs() {
    auto content = scroll->m_contentLayer;

    content->removeAllChildrenWithCleanup(false);

    std::vector<TexturePackCell*> cellsToAdd;

    for (auto cell : cells)
    {
        auto name = inp->getString();

        if (inp->getString().starts_with("by:")) {
            if(utils::string::toLower(as<TexturePackCell*>(cell.data())->texturePack->creator).find(utils::string::toLower(name.substr(name.find(":") + 1, name.size() - name.find(":")))) != std::string::npos)
            {
                cellsToAdd.push_back(as<TexturePackCell*>(cell.data()));
            }
        } else {
            if (utils::string::toLower(as<TexturePackCell*>(cell.data())->texturePack->name).find(utils::string::toLower(inp->getString())) != std::string::npos)
            {
                cellsToAdd.push_back(as<TexturePackCell*>(cell.data()));
            }
        }
    }

    float height = std::max<float>(scroll->getContentSize().height, 35 * cellsToAdd.size());
    scroll->m_contentLayer->setContentSize(ccp(scroll->m_contentLayer->getContentSize().width, height));

    int i = 0;

    for (auto cell : cellsToAdd)
    {
        // literally have no idea what % does but i stole it from google
        cell->updateBG(i % 2 == 0);

        content->addChild(cell);

        i++;
    }
    
    CCArrayExt<TexturePackCell*> objects = scroll->m_contentLayer->getChildren();

    i = 0;
    
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

TextureWorkshopLayer::~TextureWorkshopLayer()
{
    get = nullptr;
}