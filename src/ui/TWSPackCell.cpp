#include "./headers/TWSPackCell.hpp"
#include "./headers/TWSPackInfo.hpp"
#include "../header/boobs.hpp"

bool TWSPackCell::init(TWSPack* tp, bool other) {
    if (!CCLayerColor::init())
        return false;

    texturePack = tp;
    texturePack->cell = this;

    lighterColor = other;

    int reloadIconTries = 0;

    if (lighterColor) {
        this->setOpacity(100);
    } else {
        this->setOpacity(50); 
    }

    this->setContentSize(ccp(315, 35));
    this->setAnchorPoint(ccp(0, 1));

    float scale = CCDirector::sharedDirector()->getContentScaleFactor()/4;
    
    // thank you geode lead developers for the awesome creation that is lazy sprite, i love you so much. mwah.
    icon = geode::LazySprite::create({100, 100});
    icon->setLoadCallback(
        [this, &reloadIconTries, tp](Result<> result) {
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

            if (texturePack->featured) {
                auto featuredSpr = CCSprite::createWithSpriteFrameName("TWS_Featured.png"_spr);
                featuredSpr->setScale(0.35);
                featuredSpr->setPosition({ 18, this->getContentSize().height / 2 });
                this->addChild(featuredSpr);
            }
        }
    );
    this->addChild(icon);
    icon->loadFromUrl(tp->IconURL, geode::LazySprite::Format::kFmtPng);
    icon->setScale(0.35 * scale);
    icon->setPosition({ 18, this->getContentSize().height / 2 });
    icon->setZOrder(1);

    // name setup yay
    nameLabel = CCLabelBMFont::create(tp->TPName.c_str(), "bigFont.fnt");
    nameLabel->setScale(0.4);
    nameLabel->setAnchorPoint(ccp(0, 0.5f));
    nameLabel->setPosition({35.5, 26.5}); // OH BOY I LOVE HARDCODING!
    nameLabel->setZOrder(1);
    this->addChild(nameLabel);

    versionLabel = CCLabelBMFont::create(fmt::format("v{}", tp->TPVersion).c_str(), "bigFont.fnt");
    versionLabel->setScale(0.2);
    versionLabel->setAnchorPoint(ccp(0, 1));
    versionLabel->setPosition({35.5, 12}); // OH BOY I LOVE HARDCODING!
    versionLabel->setColor({51, 153, 255});
    versionLabel->setZOrder(1);
    this->addChild(versionLabel);

    downloadCount = CCLabelBMFont::create(fmt::format("{}", tp->downloads).c_str(), "bigFont.fnt");
    downloadCount->setScale(0.2);
    downloadCount->setAnchorPoint(ccp(0, 1));
    downloadCount->setPosition(ccp(versionLabel->getPosition().x + versionLabel->getScaledContentWidth() + 8, 12)); // OH BOY I LOVE HARDCODING! kinda? wat?
    downloadCount->setColor({0, 255, 33});
    downloadCount->setZOrder(1);
    this->addChild(downloadCount);

    auto downloadIcons = CCSprite::createWithSpriteFrameName("GJ_downloadsIcon_001.png");
    downloadIcons->setScale(0.25);
    downloadIcons->setAnchorPoint(ccp(1, 1));
    downloadIcons->setPosition(ccp(downloadCount->getPosition().x - .3f, 11));
    downloadIcons->setZOrder(1);
    this->addChild(downloadIcons);

    auto texturePackCreator = CCLabelBMFont::create(
        fmt::format("By: {}", tp->TPCreator).c_str(),
        "goldFont.fnt"
    );
    texturePackCreator->setScale(0.3);
    texturePackCreator->setPosition({35.5, 16.5});
    texturePackCreator->setAnchorPoint(ccp(0, 0.5f));
    texturePackCreator->setZOrder(1);
    this->addChild(texturePackCreator);

    auto buttonMenu = CCMenu::create();
    buttonMenu->setContentSize(this->getContentSize());
    buttonMenu->setAnchorPoint(ccp(0, 1));
    buttonMenu->setPosition(0,0);

    auto tpInfoSpr = CCSprite::createWithSpriteFrameName("TWS_InfoButton.png"_spr);
    tpInfoSpr->setScale(.2);
    auto tpInfoPage = CCMenuItemSpriteExtra::create(
        tpInfoSpr,
        this,
        menu_selector(TWSPackCell::onInfo)
    );
    
    this->addChild(buttonMenu);
    buttonMenu->addChild(tpInfoPage);
    tpInfoPage->setPosition(ccp(nameLabel->getPosition().x + nameLabel->getScaledContentWidth() + 6, 25.5));

    auto tpDownloadSpr = CCSprite::createWithSpriteFrameName("TWS_DownloadButton.png"_spr);
    tpDownloadSpr->setScale(.65);
    tpDownload = CCMenuItemSpriteExtra::create(
        tpDownloadSpr,
        this,
        menu_selector(TWSPackCell::onDownload)
    );
    buttonMenu->addChild(tpDownload);
    tpDownload->setPosition({ 288, this->getContentHeight() / 2 });

    auto tpDeleteSpr = CCSprite::createWithSpriteFrameName("TWS_DeleteButton.png"_spr);
    tpDeleteSpr->setScale(.65);
    tpDelete = CCMenuItemSpriteExtra::create(
        tpDeleteSpr,
        this,
        menu_selector(TWSPackCell::onDelete)
    );
    buttonMenu->addChild(tpDelete);
    tpDelete->setPosition({ 288, this->getContentHeight() / 2 });
    tpDelete->setVisible(false);

    texturePack->downloadingIndicator = Slider::create(this, nullptr);
    this->addChild(texturePack->downloadingIndicator);
    texturePack->downloadingIndicator->m_groove->setPosition(tpDownload->getPosition());
    texturePack->downloadingIndicator->m_groove->setScale(0.575);
    texturePack->downloadingIndicator->m_groove->setAnchorPoint({1, 0.5});
    texturePack->downloadingIndicator->m_touchLogic->setVisible(false);

    if (std::filesystem::exists(fmt::format("{}/packs/{}.zip", Loader::get()->getInstalledMod("geode.texture-loader")->getConfigDir(), texturePack->TPName))) {
        if (texturePack->TPVersion != Mod::get()->getSavedValue<std::string>(texturePack->TPName)) {
            gradient = CCLayerGradient::create(ccc4(0, 0, 0, 100), ccc4(0, 60, 255, 99.96000000000001));
            gradient->setContentSize(this->getContentSize());
            gradient->setZOrder(-3);
            gradient->setVector(ccp(90, 0));
            this->addChild(gradient);
            this->setOpacity(0);

            tpDelete->setVisible(false);
            tpDownload->setVisible(true);
        } else {
            tpDelete->setVisible(true);
            tpDownload->setVisible(false);

            gradient = CCLayerGradient::create(ccc4(0, 0, 0, 100), ccc4(0, 255, 0, 100));
            gradient->setContentSize(this->getContentSize());
            gradient->setZOrder(-3);
            gradient->setVector(ccp(90, 0));
            this->addChild(gradient);
            this->setOpacity(0); 
        }
    }

    updateDownloadStata();

    return true;
};

void TWSPackCell::onInfo(CCObject*) {
    TWSPackInfo::create(texturePack)->show();
}

void TWSPackCell::onDownload(CCObject*) {
    texturePack->downloadTP();
    updateDownloadStata();
}

void TWSPackCell::onDelete(CCObject*) {
    geode::createQuickPopup(
        "Delete Pack",
        fmt::format("Are you sure you want to delete {}?", texturePack->TPName),
        "Nope", "Yeah",
        [this](auto, bool btn2) {
            if (btn2) {
                std::string fileName = fmt::format("{}/packs/{}.zip", Loader::get()->getInstalledMod("geode.texture-loader")->getConfigDir(), texturePack->TPName);
                std::filesystem::remove(fileName);
                Notification::create(fmt::format("Deleted {}!", texturePack->TPName), CCSprite::createWithSpriteFrameName("GJ_completesIcon_001.png"))->show();
                this->removeChild(gradient);
                gradient = nullptr;
                tpDelete->setVisible(false);
                tpDownload->setVisible(true);
                this->setOpacity((lighterColor) ? 100 : 50); 
                texturePack->downloadSuccessful = false;
            }
        }
    );
}

void TWSPackCell::updateDownloadStata() { // stata :joy:
    if (!texturePack) return;

    if (texturePack->isDownloading) {
        tpDownload->setVisible(false);
    }

    if (texturePack->downloadingIndicator) {
        texturePack->downloadingIndicator->setVisible(texturePack->isDownloading);
    }

    if (!std::filesystem::exists(fmt::format("{}/packs/{}.zip", Loader::get()->getInstalledMod("geode.texture-loader")->getConfigDir(), texturePack->TPName))) {
        tpDownload->setVisible(!texturePack->isDownloading);
    }

    if (texturePack->downloadSuccessful) {
        if (gradient) {
            this->removeChild(gradient);
            gradient = nullptr;
        }
        Notification::create("Download Complete!", CCSprite::createWithSpriteFrameName("GJ_completesIcon_001.png"));
        gradient = CCLayerGradient::create(ccc4(0, 0, 0, 100), ccc4(0, 255, 0, 100));
        gradient->setContentSize(this->getContentSize());
        gradient->setZOrder(-3);
        gradient->setVector(ccp(90, 0));
        this->addChild(gradient);
        this->setOpacity(0); 
        tpDelete->setVisible(true);
        tpDownload->setVisible(false);
    }
}