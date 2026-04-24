using namespace geode::prelude;

#include "headers/TWSLayer.hpp"
#include "headers/TWSPackCell.hpp"
#include "../tps/headers/TWSPack.hpp"
#include "../header/boobs.hpp"
#include "./headers/TWSFilters.hpp"

#include <cctype>
#include <algorithm>
#include <matjson.hpp>
#include <Geode/ui/GeodeUI.hpp>
#include <Geode/loader/Event.hpp>

TWSLayer* TWSLayer::create() {
    auto ret = new TWSLayer();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
};

CCScene* TWSLayer::scene() {
    auto layer = TWSLayer::create();
    auto scene = CCScene::create();
    scene->addChild(layer);
    return scene;
}

bool TWSLayer::init() {
    if(!CCLayer::init())
        return false;

    setID("TWSLayer");

    auto director = CCDirector::sharedDirector();
    auto winSize = director->getWinSize();

    get = this;

    CCSprite* backSpr = CCSprite::createWithSpriteFrameName("GJ_arrow_03_001.png");
    CCMenuItemSpriteExtra* backBtn = CCMenuItemSpriteExtra::create(backSpr, this, menu_selector(TWSLayer::onClose));
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
    bg->setContentHeight(232);
    bg->setContentWidth(357);
    bg->setOpacity(135);
    bg->setPositionY(bg->getPositionY() - 15);

    outline = CCSprite::createWithSpriteFrameName("TWS_Outline.png"_spr);
    outline->setID("outline");
    this->addChild(outline);
    outline->setPosition(winSize / 2);
    outline->setScale(1.15);
    outline->setZOrder(1);

    pagesMenu = CCMenu::create();
    pagesMenu->setID("pages-menu");
    outline->addChild(pagesMenu, 1);
    pagesMenu->setPosition(0, 0);

    auto nextPageSpr = CCSprite::createWithSpriteFrameName("GJ_arrow_02_001.png");
    nextPageSpr->setFlipX(true);
    nextPage = CCMenuItemSpriteExtra::create(
        nextPageSpr,
        this,
        menu_selector(TWSLayer::onNextPage)
    );
    nextPage->setAnchorPoint({0, .5});
    pagesMenu->addChild(nextPage);
    nextPage->setPosition(ccp(outline->getContentSize().width + 1.5, (outline->getContentHeight() / 2) - 7));
    //nextPage->setVisible(false);

    auto prevPageSpr = CCSprite::createWithSpriteFrameName("GJ_arrow_02_001.png");
    prevPage = CCMenuItemSpriteExtra::create(
        prevPageSpr,
        this,
        menu_selector(TWSLayer::onPrevPage)
    );
    prevPage->setAnchorPoint({1, .5});
    pagesMenu->addChild(prevPage);
    prevPage->setPosition({-3.5, (outline->getContentHeight() / 2) - 7});
    prevPage->setVisible(false);
    
    pagesMenu->setContentSize(outline->getContentSize());

    buttonMenu = CCMenu::create();
    buttonMenu->setID("button-menu");
    addChild(buttonMenu, 1);
    
    buttonMenu->setPosition(0, 0);
    buttonMenu->setContentSize(winSize);

    auto discordSprite = CCSprite::createWithSpriteFrameName("gj_discordIcon_001.png");
    auto discordButton = CCMenuItemSpriteExtra::create(
        discordSprite,
        this,
        menu_selector(TWSLayer::onDiscord)
    );
    discordButton->setID("discord-button");
    buttonMenu->addChild(discordButton);
    discordButton->setPosition(ccp(director->getScreenRight() - 20, director->getScreenBottom() + 20));

    auto supportSprite = CCSprite::createWithSpriteFrameName("geode.loader/gift.png");
    auto supportButton = CCMenuItemSpriteExtra::create(
        supportSprite,
        this,
        menu_selector(TWSLayer::onSupport)
    );
    supportButton->setID("support-button");
    buttonMenu->addChild(supportButton);
    supportButton->setPosition(ccp(director->getScreenRight() - 20, director->getScreenBottom() + 55));

    auto creditsSprite = CCSprite::createWithSpriteFrameName("TWS_Credits.png"_spr);
    creditsSprite->setScale(0.7);
    auto creditsButton = CCMenuItemSpriteExtra::create(
        creditsSprite,
        this,
        menu_selector(TWSLayer::onCredits)
    );
    creditsButton->setID("credits-button");
    buttonMenu->addChild(creditsButton);
    creditsButton->setPosition(ccp(director->getScreenRight() - 20, director->getScreenBottom() + 90));

    auto refreshSpr = CCSprite::createWithSpriteFrameName("TWS_RefreshButton.png"_spr);
    refreshSpr->setScale(0.8);
    refreshButton = CCMenuItemSpriteExtra::create(
        refreshSpr,
        this,
        menu_selector(TWSLayer::onRefresh)
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
        menu_selector(TWSLayer::onPacksFolder)
    );
    filesBtn->setID("files-button");
    buttonMenu->addChild(filesBtn);
    filesBtn->setPosition(ccp(director->getScreenLeft() + 25, director->getScreenBottom() + 25));

    /*auto buttonSpr1 = CCSprite::createWithSpriteFrameName("TWS_Button.png"_spr);
    auto featuredSpr = CCSprite::createWithSpriteFrameName("GJ_sStarsIcon_001.png");
    buttonSpr1->addChild(featuredSpr);
    featuredSpr->setPosition(ccp(buttonSpr1->getContentSize().width / 2, buttonSpr1->getContentSize().height / 2));
    buttonSpr1->setScale(0.5);
    auto featuredBtn = CCMenuItemSpriteExtra::create(
        buttonSpr1,
        this,
        menu_selector(TWSLayer::onPacksFolder)
    );
    featuredBtn->setID("featured-button");
    buttonMenu->addChild(featuredBtn);
    featuredBtn->setPosition(ccp(director->getScreenLeft() + 25, director->getScreenBottom() + 25));*/
    // big yahu save me bro

    scroll = ScrollLayer::create(ccp(160, 193));
    scroll->setAnchorPoint(ccp(0, 0));
    scroll->ignoreAnchorPointForPosition(false);
    scroll->setZOrder(-1);
    scroll->setPosition(150, 14);

    outline->addChild(scroll);

    loading = LoadingCircleSprite::create(1);
    loading->setID("loading");
    loading->setScale(0.6f);
    loading->setPosition(ccp((outline->getContentWidth() / 2) + 68, outline->getContentHeight() / 2 + -16.875f));
    outline->addChild(loading);
    loading->setVisible(false);

    /*pageJson = matjson::parse(R"({
        "1": {
            "packID": 12,
            "packName": "featured test",
            "downloadLink": "https://github.com/Uproxide/storage/releases/download/a/Legacy.Uproxide.Difficulties.zip",
            "packLogo": "https://github.com/Uproxide/storage/releases/download/a/update.png",
            "packDescription": "testing tp.\n# hello!",
            "packCreator": "roxi",
            "packVersion": "1.2.0",
            "gdVersion": "2.2081",
            "packFeature": 1,
            "packDownloads": 105465,
            "creationDate": 0,
            "lastUpdated": 1755111695474
        },
        "2": {
            "packID": 12,
            "packName": "unfeatured test",
            "downloadLink": "https://github.com/Uproxide/storage/releases/download/a/Legacy.Uproxide.Difficulties.zip",
            "packLogo": "https://github.com/Uproxide/storage/releases/download/a/update.png",
            "packDescription": "testing tp.\n# hello!",
            "packCreator": "naomi",
            "packVersion": "1.2.0",
            "gdVersion": "2.2081",
            "packFeature": 0,
            "packDownloads": 105465,
            "creationDate": 0,
            "lastUpdated": 1755111695474
        }
    })").unwrap();*/
    boobs::search = "";
    boobs::page = 1;

    getTexturePacks(boobs::search); // web web web sahur
    getTexturePacksCount(boobs::search); // web web web sahur the 2

    inp = TextInput::create(110, "Search", "bigFont.fnt");
    inp->setContentHeight(20);
    inp->setAnchorPoint(ccp(0, 0));
    inp->ignoreAnchorPointForPosition(false); 
    this->outline->addChild(inp);

    inp->setPosition(ccp(18.5, 176.2));
    inp->hideBG();
    auto inputNode = inp->getInputNode();
    inputNode->setPositionY(inputNode->getPositionY() - 5);
    inputNode->setPositionX(5);
    inputNode->m_textLabel->setAnchorPoint(ccp(0, 0.5));
    inputNode->m_textLabel->setScale(0.5);
    inp->setDelegate(this);
    inp->setCommonFilter(CommonFilter::Any);

    auto filterMenu = CCMenu::create();
    filterMenu->setPosition(0, 0);
    outline->addChild(filterMenu, 1);
    filterMenu->setID("filter-menu");
    filterMenu->ignoreAnchorPointForPosition(false);
    filterMenu->setPosition({78, 110});
    filterMenu->setContentSize({131, 180});

    auto button1Spr = CCSprite::createWithSpriteFrameName("TWS_ButtonSmall.png"_spr);
    selected1Spr = CCSprite::createWithSpriteFrameName("TWS_ButtonSmallOutline.png"_spr);
    auto featuredSpr = CCSprite::createWithSpriteFrameName("GJ_starsIcon_001.png");
    button1Spr->addChild(featuredSpr);
    featuredSpr->setPosition(ccp(button1Spr->getContentSize().width / 2, button1Spr->getContentSize().height / 2));
    auto featuredBtn = CCMenuItemSpriteExtra::create(
        button1Spr,
        this,
        menu_selector(TWSLayer::onFilter)
    );
    featuredBtn->setTag(92210);
    filterMenu->addChild(featuredBtn);
    filterMenu->addChild(selected1Spr);
    featuredBtn->setPosition({24, 131});
    featuredBtn->setZOrder(1);
    selected1Spr->setPosition({24, 131});
    selected1Spr->setScale(1.025);
    selected1Spr->setVisible(false);

    auto button2Spr = CCSprite::createWithSpriteFrameName("TWS_ButtonSmall.png"_spr);
    selected2Spr = CCSprite::createWithSpriteFrameName("TWS_ButtonSmallOutline.png"_spr);
    auto recentSpr = CCSprite::createWithSpriteFrameName("GJ_timeIcon_001.png");
    button2Spr->addChild(recentSpr);
    recentSpr->setPosition(ccp(button2Spr->getContentSize().width / 2, button2Spr->getContentSize().height / 2));
    auto recentBtn = CCMenuItemSpriteExtra::create(
        button2Spr,
        this,
        menu_selector(TWSLayer::onFilter)
    );
    recentBtn->setTag(92211);
    filterMenu->addChild(recentBtn);
    filterMenu->addChild(selected2Spr);
    recentBtn->setPosition({65.5, 131});
    recentBtn->setZOrder(1);
    selected2Spr->setPosition({65.5, 131});
    selected2Spr->setScale(1.025);
    selected2Spr->setVisible(false);

    auto button3Spr = CCSprite::createWithSpriteFrameName("TWS_ButtonSmall.png"_spr);
    selected3Spr = CCSprite::createWithSpriteFrameName("TWS_ButtonSmallOutline.png"_spr);
    auto downloadsSpr = CCSprite::createWithSpriteFrameName("GJ_downloadsIcon_001.png");
    button3Spr->addChild(downloadsSpr);
    downloadsSpr->setPosition(ccp(button3Spr->getContentSize().width / 2, button3Spr->getContentSize().height / 2));
    auto downloadsBtn = CCMenuItemSpriteExtra::create(
        button3Spr,
        this,
        menu_selector(TWSLayer::onFilter)
    );
    downloadsBtn->setTag(92212);
    filterMenu->addChild(downloadsBtn);
    filterMenu->addChild(selected3Spr);
    downloadsBtn->setPosition({106, 131});
    downloadsBtn->setZOrder(1);
    selected3Spr->setPosition({106, 131});
    selected3Spr->setScale(1.025);
    selected3Spr->setVisible(false);

    auto button4Spr = CCSprite::createWithSpriteFrameName("TWS_ButtonLong.png"_spr);
    auto randomSpr = CCSprite::createWithSpriteFrameName("GJ_sRecentIcon_001.png");
    auto randomLabel = CCLabelBMFont::create("Feelin' Lucky!", "goldFont.fnt");
    button4Spr->addChild(randomSpr);
    button4Spr->addChild(randomLabel);
    randomSpr->setPosition(ccp(106.25, button4Spr->getContentSize().height / 2));
    randomSpr->setScale(0.9);
    randomLabel->setScale(0.5);
    randomLabel->setAnchorPoint(ccp(0, 0.5f));
    randomLabel->setPosition(ccp(6, button4Spr->getContentSize().height / 2));
    auto randomBtn = CCMenuItemSpriteExtra::create(
        button4Spr,
        this,
        menu_selector(TWSLayer::onRandom)
    );
    filterMenu->addChild(randomBtn);
    randomBtn->setPosition({65, 93});
    
    selected1Spr->setVisible(boobs::featuredSort);
    selected2Spr->setVisible(boobs::recentlyUpdatedSort);
    selected3Spr->setVisible(boobs::downloadsSort);

    /*auto inpMenu = CCMenu::create();
    inpMenu->setContentSize(inp->getContentSize());
    inp->addChild(inpMenu);
    
    
    auto filterSpr = CCSprite::createWithSpriteFrameName("TWS_SearchButton.png"_spr);
    filterSpr->setScale(0.9);
    auto filterBtn = CCMenuItemSpriteExtra::create(
        filterSpr,
        this,
        menu_selector(TWSLayer::onSort)
    );
    inpMenu->addChild(filterBtn);
    inpMenu->setLayout(
        RowLayout::create()
            ->setAxisAlignment(AxisAlignment::End)
    );
    inpMenu->setPosition(inp->getContentSize() / 2);
    filterBtn->setPositionX(filterBtn->getPositionX() + 5);
    inpMenu->setTouchPriority(-129);*/

    return true;
}

void TWSLayer::getTexturePacks(std::string searchQuery) {
    if (scroll && scroll->m_contentLayer->getChildrenCount() > 0) scroll->m_contentLayer->removeAllChildren();

    if (pageCount) {
        pageCount->setVisible(false);
    }

    if (loading) loading->setVisible(true);

    //if (pagesMenu) pagesMenu->setVisible(false);

    if (auto errorSlop = outline->getChildByID("error-text"_spr)) {
        outline->removeChild(errorSlop, true);
    }

    //if (nextPage && nextPage->isVisible()) nextPage->setVisible(false);
    //if (prevPage && prevPage->isVisible()) prevPage->setVisible(false);
    
    auto req = geode::utils::web::WebRequest();

    req.onProgress([](geode::utils::web::WebProgress const& progress) {
        //log::debug("Progress: ", progress.downloadProgress());
    });

    //log::info("{}", boobs::page);

    std::string url = fmt::format("https://textureworkshop.xyz/api/v2/tws/getTPs?page={}", boobs::page);

    if (Mod::get()->getSettingValue<bool>("version-filter")) {
        std::string currentUrlStr = url;
        url = fmt::format("{}&version={}", currentUrlStr, Loader::get()->getGameVersion());
    }

    if (!searchQuery.empty()) {
        std::string currentUrlStr = url;
        size_t pos = 0;
        while ((pos = searchQuery.find(" ", pos)) != std::string::npos) {
            searchQuery.replace(pos, 1, "%20");
            pos += 3;
        }
        url = fmt::format("{}&search={}", currentUrlStr, searchQuery);
    }

    if (boobs::downloadsSort) {
        std::string currentUrlStr = url;
        url = fmt::format("{}&sort=downloads", currentUrlStr);
    } else if (boobs::featuredSort) {
        std::string currentUrlStr = url;
        url = fmt::format("{}&sort=featured", currentUrlStr);
    } else if (boobs::recentlyUpdatedSort) {
        std::string currentUrlStr = url;
        url = fmt::format("{}&sort=recent", currentUrlStr);
    }

    //log::info("{}", Loader::get()->getGameVersion());

    m_getTPslistener.spawn( 
        req.get(url),
        [this](geode::utils::web::WebResponse res) {
            //log::debug("Response: {}", value.code());
            //log::debug("Body: {}", value.string().unwrap());
            if (res.ok()) {
                if (res.string().unwrap() == "{}") {
                    auto errorText = CCLabelBMFont::create("No texture packs found!", "bigFont.fnt");
                    outline->addChild(errorText);
                    errorText->setScale(0.2);
                    errorText->setID("error-text"_spr);
                    errorText->setPosition({ 225.5, 201.375 });
                    errorText->setAnchorPoint({ 0.5, 1 });
                    loading->setVisible(false);
                    nextPage->setVisible(false);
                    prevPage->setVisible(false);
                } else {
                    pageJson = res.json().unwrap();
                    //log::debug("Page JSON: {}", pageJson.dump());
                    setupTPCells();
                }
            } else {
                auto errorText = CCLabelBMFont::create(fmt::format("Something went wrong while getting TPs!\nPlease try again later.\n\nError Code: {}", res.code()).c_str(), "bigFont.fnt");
                outline->addChild(errorText);
                errorText->setScale(0.2);
                errorText->setID("error-text"_spr);
                errorText->setPosition({ 225.5, 201.375 });
                errorText->setAnchorPoint({ 0.5, 1 });
                loading->setVisible(false);
                nextPage->setVisible(false);
                prevPage->setVisible(false);
            }
        }
    );
}

void TWSLayer::getTexturePacksCount(std::string searchQuery) {
    log::info("Getting TP count for page {} with search query '{}'", boobs::page, searchQuery);
    auto req2 = geode::utils::web::WebRequest();
    std::string pageCountUrl = fmt::format("https://textureworkshop.xyz/api/v1/tws/getTPsCount?page={}", boobs::page);

    if (Mod::get()->getSettingValue<bool>("version-filter")) {
        std::string currentPageUrlStr = pageCountUrl;
        pageCountUrl = fmt::format("{}&version={}", currentPageUrlStr, Loader::get()->getGameVersion());
    }

    if (!searchQuery.empty()) {
        std::string currentPageUrlStr = pageCountUrl;
        size_t pos = 0;
        while ((pos = searchQuery.find(" ", pos)) != std::string::npos) {
            searchQuery.replace(pos, 1, "%20");
            pos += 3;
        }
        pageCountUrl = fmt::format("{}&search={}", currentPageUrlStr, searchQuery);
    }

    if (boobs::featuredSort) {
        std::string currentPageUrlStr = pageCountUrl;
        pageCountUrl = fmt::format("{}&featured=1", currentPageUrlStr);
    }

    m_getTPsCountlistener.spawn( 
        req2.get(pageCountUrl),
        [this](geode::utils::web::WebResponse res) {
            log::debug("Response: {}", res.code());
            log::debug("Body: {}", res.string().unwrap());
            if (!res.ok() || res.json().unwrap()["success"].asBool().unwrap() == false) {
                log::error("Failed to get TP count: {}", res.string().unwrap());
            } else {
                if (res.json().unwrap()["count"].asInt().unwrap() == 0) {
                    return;
                }

                if (res.json().unwrap()["pageCount"].asInt().unwrap() < boobs::page) {
                    boobs::page = res.json().unwrap()["pageCount"].asInt().unwrap();
                    getTexturePacks(boobs::search);
                    nextPage->setVisible(false);
                }

                auto director = CCDirector::sharedDirector();
                std::string formattedText = fmt::format("Page {}/{} ({} Total)", boobs::page, res.json().unwrap()["pageCount"].asInt().unwrap(), res.json().unwrap()["count"].asInt().unwrap()).c_str();
                if (pageCount) {
                    pageCount->setString(formattedText.c_str());
                    pageCount->setVisible(true);
                } else {
                    pageCount = CCLabelBMFont::create(formattedText.c_str(), "goldFont.fnt");
                    this->addChild(pageCount);
                    pageCount->setScale(0.3);
                    pageCount->setAnchorPoint({1, 1});
                    pageCount->setPosition(ccp(director->getScreenRight() - 2, director->getScreenTop() - 2));
                }
                
            }
        }
    );
}

void TWSLayer::onFilter(CCObject* btn) {
    switch (btn->getTag())
    {
    case 92210:
        boobs::featuredSort = !boobs::featuredSort;

        boobs::downloadsSort = false;
        boobs::recentlyUpdatedSort = false;

        break;

    case 92211:
        boobs::recentlyUpdatedSort = !boobs::recentlyUpdatedSort;

        boobs::downloadsSort = false;
        boobs::featuredSort = false;

        break;

    case 92212:
        boobs::downloadsSort = !boobs::downloadsSort;

        boobs::featuredSort = false;
        boobs::recentlyUpdatedSort = false;

        break;
    
    default:
        boobs::featuredSort = false;
        boobs::downloadsSort = false;
        boobs::recentlyUpdatedSort = false;
        break;
    }

    selected1Spr->setVisible(boobs::featuredSort);
    selected2Spr->setVisible(boobs::recentlyUpdatedSort);
    selected3Spr->setVisible(boobs::downloadsSort);

    getTexturePacks(boobs::search);
    getTexturePacksCount(boobs::search);
} 
// they call me yanderedev for this button logic #fuckyes 
// holy shit im the netanyahu of geode

void TWSLayer::onRandom(CCObject*) {
    if (!loadingAndSetupDoneRandomTP) {
        return;
    }

    auto loadingRandom = LoadingCircleSprite::create(1);
    loadingRandom->setID("loading-random");
    loadingRandom->setScale(0.6f);
    loadingRandom->setPosition(ccp(75.5, 56.5));
    outline->addChild(loadingRandom);
    loadingRandom->setVisible(true);

    if (randomTPNode) {
        randomTPNode->removeFromParentAndCleanup(true);
        randomTPNode = nullptr;
    }

    loadingAndSetupDoneRandomTP = false;

    auto req = geode::utils::web::WebRequest();
    std::string randomTPURL = "https://textureworkshop.xyz/api/v1/tws/getRandomTP";
    if (Mod::get()->getSettingValue<bool>("version-filter")) {
        std::string currentRandomTPURL = randomTPURL;
        currentRandomTPURL = fmt::format("{}?version={}", randomTPURL, Loader::get()->getGameVersion());
    }

    m_getRandomTPListener.spawn( 
        req.get(randomTPURL),
        [this, loadingRandom](geode::utils::web::WebResponse res) {
            log::debug("Response: {}", res.code());
            log::debug("Body: {}", res.string().unwrap());
            if (!res.ok()) {
                log::error("Failed to get random TP: {}", res.string().unwrap());
                loadingAndSetupDoneRandomTP = true;
            } else {
                auto tpObject = res.json().unwrap();
                bool featured = false;

                if (tpObject["packFeature"].asInt().unwrap() == 1) {
                    featured = true;
                } else {
                    featured = false;
                }

                TWSPack* tp = TWSPack::create(
                    tpObject["packID"].asInt().unwrap(),
                    tpObject["packName"].asString().unwrap(),
                    tpObject["downloadLink"].asString().unwrap(),
                    tpObject["packLogo"].asString().unwrap(),
                    tpObject["packDescription"].asString().unwrap(),
                    tpObject["packCreator"].asString().unwrap(),
                    tpObject["packVersion"].asString().unwrap(),
                    tpObject["gdVersion"].asString().unwrap(),
                    featured,
                    tpObject["packDownloads"].asInt().unwrap()
                );

                TWSPackInfo::create(tp)->show();
                loadingAndSetupDoneRandomTP = true;
                loadingRandom->removeFromParentAndCleanup(true);
            }
        }
    );
}

/* void TWSLayer::setupRandomTPNode(TWSPack* tp) {
    int reloadIconTries = 0;
    float scale = CCDirector::sharedDirector()->getContentScaleFactor()/4;

    randomTPNode = CCNode::create();
    outline->addChild(randomTPNode);

    randomTPNode->setContentSize({121, 72});
    randomTPNode->setAnchorPoint({0.5, 0.5});
    randomTPNode->setPosition({76.5, 56.375});

    auto tpName = CCLabelBMFont::create(tp->TPName.c_str(), "bigFont.fnt");
    randomTPNode->addChild(tpName);

    auto icon = geode::LazySprite::create({100, 100});
    icon->setLoadCallback(
        [this, &reloadIconTries, tp, icon](Result<> result) {
            if (!result.isOk()) 
            {
                if (reloadIconTries < 3) {
                    log::info("failed to load icon, please refresh this TWS page to try again."); 
                    // fallback
                    icon->initWithSpriteFrameName("TWS_Error.png"_spr);
                } else {
                    icon->loadFromUrl(tp->IconURL, geode::LazySprite::Format::kFmtPng);
                    reloadIconTries += 1;
                }
            }

            if (tp->featured) {
                auto featuredSpr = CCSprite::createWithSpriteFrameName("TWS_Featured.png"_spr);
                featuredSpr->setScale(0.325);
                featuredSpr->setPosition({ 18, 56 });
                randomTPNode->addChild(featuredSpr);
            }
        }
    );
    randomTPNode->addChild(icon);
    icon->loadFromUrl(tp->IconURL, geode::LazySprite::Format::kFmtPng);
    icon->setScale(0.325 * scale);
    icon->setPosition({ 18, 56 });
    icon->setZOrder(1);

    loadingAndSetupDoneRandomTP = true;
} */

void TWSLayer::setupTPCells() {
    if (pagesMenu) pagesMenu->setVisible(true);
    int i = 0;
    scroll->m_contentLayer->setAnchorPoint(ccp(0,1));

    scroll->m_contentLayer->removeAllChildren();
    loading->setVisible(false);

    tps.clear();

    if (boobs::page > 1) {
        prevPage->setVisible(true);
    } else {
        prevPage->setVisible(false);
    }
    nextPage->setVisible(true);

    for (auto& value : pageJson) {
        auto tpObject = value;

        bool featured = false;
        if (tpObject["packFeature"].asInt().unwrap() == 1) {
            featured = true;
        } else {
            featured = false;
        }

        TWSPack* tp = TWSPack::create(
            tpObject["packID"].asInt().unwrap(),
            tpObject["packName"].asString().unwrap(),
            tpObject["downloadLink"].asString().unwrap(),
            tpObject["packLogo"].asString().unwrap(),
            tpObject["packDescription"].asString().unwrap(),
            tpObject["packCreator"].asString().unwrap(),
            tpObject["packVersion"].asString().unwrap(),
            tpObject["gdVersion"].asString().unwrap(),
            featured,
            tpObject["packDownloads"].asInt().unwrap()
        );

        tps.push_back(tp);

        stupid = !stupid;

        TWSPack* existingTp = nullptr;
        for (auto* downloadingTp : boobs::downloading) {
            if (downloadingTp && downloadingTp->ID == tpObject["packID"].asInt().unwrap()) {
                existingTp = downloadingTp;
            }
        }

        TWSPackCell* tpCell;

        if (existingTp) {
            tpCell = TWSPackCell::create(existingTp, stupid);
        } else {
            tpCell = TWSPackCell::create(tp, stupid);
        }

        scroll->m_contentLayer->addChild(tpCell);
        tpCell->setPosition(0, 314 - (35 * i));
        tpCell->pagesMenu = pagesMenu;
        tpCell->inp = inp;
        i++;
    }
    if (i < 10) {
        nextPage->setVisible(false);
    }
    int tpCount = scroll->m_contentLayer->getChildrenCount();
    scroll->m_contentLayer->setContentSize(ccp(scroll->m_contentLayer->getContentSize().width, (35 * 10)));
    scroll->moveToTop();
}

void TWSLayer::onClose(CCObject*) {
    auto mainMenu = MenuLayer::scene(false);
    CCDirector::sharedDirector()->pushScene(CCTransitionFade::create(0.5f, mainMenu));
}

void TWSLayer::onDiscord(CCObject*) {
    utils::web::openLinkInBrowser("https://discord.gg/pUGPY9hQ22");
}

void TWSLayer::onSupport(CCObject*) {
    geode::createQuickPopup(
        "Support Me",
        "This mod tragically costs money to host and maintain cause storage is EXPENSIVE! GOD!\nHowever, if you enjoy TWS and want to support its development, consider supporting us on ko-fi for some cool perks!",
        "No Thanks", "Ok",
        [](auto, bool btn2) {
            if (btn2) {
                utils::web::openLinkInBrowser("https://ko-fi.com/teamsillywillies");
            }
        }
    );
}

void TWSLayer::onCredits(CCObject*) {
    FLAlertLayer::create(
        "Credits",
        "<cg>Uproxide</c> - Main Developer\n<co>ShineUA, Alphalaneous and TheSillyDoggo</c> - Pull Requests :3\n<cr>M336</c> - Serverside Code\n<cl>Brift</c> - Main Sprite Creator\n<cl>Dremsk1y</c> - Sprites\n<cp>Riley</c> - she spenda the money (hosting)",
        "Ok"
    )->show();
}

void TWSLayer::onRefresh(CCObject*) {
    // deprecated
}

void TWSLayer::onPacksFolder(CCObject*) {
    utils::file::openFolder(Loader::get()->getInstalledMod("geode.texture-loader")->getConfigDir());
}

// prev/next page of texture packs
void TWSLayer::onPrevPage(CCObject*) {
    if (boobs::page == 1) {
        return;
    }

    boobs::page -= 1;
    getTexturePacks(boobs::search);
    getTexturePacksCount(boobs::search);
}

void TWSLayer::onNextPage(CCObject*) {
    boobs::page += 1;
    getTexturePacks(boobs::search);
    getTexturePacksCount(boobs::search);
}

void TWSLayer::onSort(CCObject*) {
    auto sortPopup = TWSFilters::create();
    sortPopup->show();
}

void TWSLayer::onSearch(CCObject*) {
    boobs::search = inp->getString();
    boobs::page = 1;
    getTexturePacks(boobs::search);
    getTexturePacksCount(boobs::search);
}

void TWSLayer::keyBackClicked() {
    onClose(nullptr);
}

void TWSLayer::textChanged(CCTextInputNode*) {
    boobs::search = inp->getString();

    this->unschedule(schedule_selector(TWSLayer::doThingIdrk));
    this->scheduleOnce(schedule_selector(TWSLayer::doThingIdrk), 1.5f);
}

void TWSLayer::doThingIdrk(float) {
    getTexturePacks(boobs::search);
    getTexturePacksCount(boobs::search);
}

TWSLayer::~TWSLayer()
{
    //boobs::search = "";
    boobs::page = 1;
    get = nullptr;
}

/*
class TextureWorkshop : public cocos2d::CCLayer
{


texture workshop = running
hackers = false
deny ai thumbnail = true
good mod have 1 mil download free = true
geodoe 2.208 support = true
*/

