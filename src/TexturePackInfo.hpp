#pragma once

#include <Geode/Geode.hpp>
#include <Geode/Loader.hpp>
#include <Geode/utils/web.hpp>
#include <Geode/ui/TextInput.hpp>
#include <cctype>
#include <algorithm>

using namespace geode::prelude;

class TexturePackInfo : public Popup<std::string, std::string, std::string, std::string, std::string, std::string, geode::ByteVector, bool> {
public:
    std::string url;
    std::string name;
    std::string creator;
    std::string icon;
    std::string version;
    std::string description;
    bool feature;

    geode::ByteVector iconImg;

    bool setup(std::string tpName, std::string tpCreator, std::string tpDownloadLink, std::string tpIcon, std::string tpDownloadVersion, std::string tpDesc, geode::ByteVector iconData, bool featured) {
        log::info("hai");

        m_noElasticity = true;

        creator = tpCreator;
        name = tpName;
        url = tpDownloadLink;
        version = tpDownloadVersion;
        feature = featured;
        icon = tpIcon;
        iconImg = iconData;
        description = tpDesc;

        auto winSize = CCDirector::get()->getWinSize();

        if (featured) {
            auto featuredSpr = CCSprite::createWithSpriteFrameName("TWS_Featured.png"_spr);
            this->addChild(featuredSpr);
            featuredSpr->setScale(0.65);
            featuredSpr->setPosition(ccp(180, 240));

        }

        auto texturePackIconSpr = CCSprite::createWithSpriteFrameName("TWS_PlaceholderLogo.png"_spr);
        this->addChild(texturePackIconSpr);
        texturePackIconSpr->setScale(0.65);
        texturePackIconSpr->setPosition(ccp(180, 240));

        auto txtr = CCTextureCache::get()->textureForKey(fmt::format("logo-{}", name).c_str());
        this->retain();

        if (!txtr) {
            // n textur
        } else {
            texturePackIconSpr->initWithTexture(txtr);
        }


        auto tpNameMenu = CCMenu::create();

        auto texturePackName = CCLabelBMFont::create(
            tpName.c_str(),
            "bigFont.fnt"
        );
        tpNameMenu->addChild(texturePackName);
        tpNameMenu->setPosition(212, 251);
        tpNameMenu->setScale(0.6);
        tpNameMenu->setAnchorPoint(ccp(0, 0.5));
        tpNameMenu->setLayout(
            RowLayout::create()
                ->setAxisAlignment(AxisAlignment::Start)
        );
        this->addChild(tpNameMenu);
        tpNameMenu->setContentWidth(300);
        tpNameMenu->updateLayout();
        

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

        auto desc = MDTextArea::create(description, ccp(300, 150));
        this->addChild(desc);
        desc->setPosition(line->getPosition());
        desc->setPositionY(desc->getPositionY() - 81);


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


    static TexturePackInfo* create(std::string tpName, std::string tpCreator, std::string tpDownloadLink, std::string tpIcon, std::string tpDownloadVersion, std::string tpDesc, geode::ByteVector iconData, bool featured) {
        auto ret = new TexturePackInfo();
        if (ret && ret->initAnchored(342, 240, tpName, tpCreator, tpDownloadLink, tpIcon, tpDownloadVersion, tpDesc, iconData, featured, "TWS_Box.png"_spr)) {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }
};