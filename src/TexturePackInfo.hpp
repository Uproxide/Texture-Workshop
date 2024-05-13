#pragma once

#include <Geode/Geode.hpp>
#include <Geode/Loader.hpp>
#include <Geode/utils/web.hpp>
#include <Geode/ui/TextInput.hpp>
#include <cctype>
#include <algorithm>

using namespace geode::prelude;

class TexturePackInfo : public Popup<std::string, std::string, std::string> {
public:
    std::string url;
    std::string name;
    std::string creator;

    bool setup(std::string tpName, std::string tpCreator, std::string tpDownloadLink) {
        log::info("hai");

        m_noElasticity = true;

        creator = tpCreator;
        name = tpName;
        url = tpDownloadLink;

        auto winSize = CCDirector::get()->getWinSize();

        auto featuredSpr = CCSprite::createWithSpriteFrameName("geode.loader/logo-glow.png");
        this->addChild(featuredSpr);
        featuredSpr->setScale(0.65);
        featuredSpr->setPosition(ccp(180, 240));

        auto texturePackIconSpr = CCSprite::createWithSpriteFrameName("TWS_PlaceholderLogo.png"_spr);
        this->addChild(texturePackIconSpr);
        texturePackIconSpr->setScale(0.65);
        texturePackIconSpr->setPosition(ccp(180, 240));

        auto texturePackName = CCLabelBMFont::create(
            tpName.c_str(),
            "bigFont.fnt"
        );
        this->addChild(texturePackName);
        texturePackName->setPosition(212, 251);
        texturePackName->setScale(0.6);
        texturePackName->setAnchorPoint(ccp(0, 0.5));

        std::string fullTPCreator = fmt::format("By {}", tpCreator);

        auto texturePackCreator = CCLabelBMFont::create(
            fullTPCreator.c_str(),
            "goldFont.fnt"
        );
        this->addChild(texturePackCreator);
        texturePackCreator->setPosition(213, 231);
        texturePackCreator->setScale(0.6);
        texturePackCreator->setAnchorPoint(ccp(0, 0.5));

        auto line = CCSprite::createWithSpriteFrameName("floorLine_001.png");
        this->addChild(line);
        line->setPosition(ccp(284.5, 206));
        line->setScale(0.675);

        auto dwnldBtnMenu = CCMenu::create();

        auto dwnldButtonSpr = CCSprite::createWithSpriteFrameName("TWS_DownloadButton.png"_spr);
        dwnldButtonSpr->setScale(0.825);
        auto dwnldButton = CCMenuItemSpriteExtra::create(
            dwnldButtonSpr,
            this,
            menu_selector(TexturePackInfo::onDownload)
        );
        this->addChild(dwnldBtnMenu);
        dwnldBtnMenu->addChild(dwnldButton);
        dwnldBtnMenu->setPosition(380, 220);
        dwnldBtnMenu->setAnchorPoint(ccp(1, 0.5));
        dwnldBtnMenu->setContentSize(dwnldButton->getContentSize());
        dwnldButton->setPosition(dwnldBtnMenu->getContentSize() / 2);

        return true;
    }

    void onDownload(CCObject*) {
        std::string fileName = fmt::format("{}/packs/{}.zip", Loader::get()->getLoadedMod("geode.texture-loader")->getConfigDir(), name);
        web::AsyncWebRequest()
            .fetch(url)
            .into(fileName)
            .then([this](auto file) {
                Notification::create("Download Successful", CCSprite::createWithSpriteFrameName("GJ_completesIcon_001.png"))->show();
                log::info("{}", file);
            })
            .expect([](std::string const& err) {
                Notification::create("Download Failed", CCSprite::createWithSpriteFrameName("GJ_deleteIcon_001.png"))->show();
            });
    }


    static TexturePackInfo* create(std::string tpName, std::string tpCreator, std::string tpDownloadLink) {
        auto ret = new TexturePackInfo();
        if (ret && ret->initAnchored(342, 240, tpName, tpCreator, tpDownloadLink, "GJ_square01.png")) {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }
};