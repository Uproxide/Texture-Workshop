#pragma once

#include <Geode/Geode.hpp>
#include <Geode/utils/web.hpp>
#include <Geode/ui/TextInput.hpp>
#include <cctype>
#include <algorithm>
#include "TexturePackInfo.hpp"


using namespace geode::prelude;

class TexturePackCell : public CCLayerColor {
    public:

        std::string url;
        std::string name;
        std::string creator;
        std::string version;
        bool featured;

        bool init(std::string tpName, std::string tpCreator, std::string tpDownloadURL, std::string tpDownloadVersion, bool feature, bool thing) {
            if (!CCLayerColor::init())
                return false;

            if (thing) {
               this->setOpacity(100); 
            } else {
                this->setOpacity(50); 
            }
                
            

            this->setContentSize(ccp(315, 45));
            this->setAnchorPoint(ccp(0, 1));
            this->setPositionY(207);

            creator = tpCreator;
            name = tpName;
            url = tpDownloadURL;
            version = tpDownloadVersion;
            featured = feature;

            if (featured) {
                auto featuredSpr = CCSprite::createWithSpriteFrameName("geode.loader/logo-glow.png");
                this->addChild(featuredSpr);
                featuredSpr->setScale(0.475);
                featuredSpr->setPosition(this->getContentSize() / 2);
                featuredSpr->setPositionX(30);
            }
            
            auto texturePackIconSpr = CCSprite::createWithSpriteFrameName("TWS_PlaceholderLogo.png"_spr);
            this->addChild(texturePackIconSpr);
            texturePackIconSpr->setScale(0.475);
            texturePackIconSpr->setPosition(this->getContentSize() / 2);
            texturePackIconSpr->setPositionX(30);

            auto texturePackName = CCLabelBMFont::create(
                tpName.c_str(),
                "bigFont.fnt"
            );
            this->addChild(texturePackName);
            texturePackName->setPosition(55, 35);
            texturePackName->setScale(0.475);
            texturePackName->setAnchorPoint(ccp(0, 0.5));

            std::string fullTPCreator = fmt::format("By {}", tpCreator);

            auto texturePackCreator = CCLabelBMFont::create(
                fullTPCreator.c_str(),
                "goldFont.fnt"
            );
            this->addChild(texturePackCreator);
            texturePackCreator->setPosition(55, 13);
            texturePackCreator->setScale(0.475);
            texturePackCreator->setAnchorPoint(ccp(0, 0.5));

            std::string fullTexturePackVersion = fmt::format("v{}", tpDownloadVersion);

            auto texturePackVersion = CCLabelBMFont::create(
                fullTexturePackVersion.c_str(),
                "bigFont.fnt"
            );
            this->addChild(texturePackVersion);
            texturePackVersion->setPosition(55, 23.5);
            texturePackVersion->setScale(0.3);
            texturePackVersion->setAnchorPoint(ccp(0, 0.5));
            texturePackVersion->setColor(ccc3(0, 200, 255));


            auto viewBtnMenu = CCMenu::create();

            auto viewButtonSpr = CCSprite::createWithSpriteFrameName("TWS_DownloadButton.png"_spr);
            viewButtonSpr->setScale(0.75);
            auto viewButton = CCMenuItemSpriteExtra::create(
                viewButtonSpr,
                this,
                menu_selector(TexturePackCell::onView)
            );
            this->addChild(viewBtnMenu);
            viewBtnMenu->addChild(viewButton);
            viewBtnMenu->setPosition(260, 4.5);
            viewBtnMenu->setAnchorPoint(ccp(1, 0.5));
            viewBtnMenu->setContentSize(viewButton->getContentSize());
            viewButton->setPosition(viewBtnMenu->getContentSize() / 2);

            return true;
        }

        void onView(CCObject*) {
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

        static TexturePackCell* create(std::string tpName, std::string tpCreator, std::string tpDownloadURL, std::string tpDownloadVersion, bool featured, bool thing) {
            TexturePackCell* pRet = new TexturePackCell();
            if (pRet && pRet->init(tpName, tpCreator, tpDownloadURL, tpDownloadVersion, featured, thing)) {
                pRet->autorelease();
                return pRet;
            } else {
                delete pRet;
                return nullptr;
            }
        }

};