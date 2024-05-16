#pragma once

#include <Geode/Geode.hpp>
#include <Geode/utils/web.hpp>
#include <Geode/ui/TextInput.hpp>
#include <cctype>
#include <algorithm>
#include "TexturePackInfo.hpp"
#include <Geode/utils/file.hpp>

using namespace geode::prelude;

class TexturePackCell : public CCLayerColor {
    public:

        std::string url;
        std::string name;
        std::string creator;
        std::string version;
        std::string icon;
        std::string description;
        bool featured;
        bool thingy;
        int opacity;

        geode::ByteVector imgData;
        CCLayerGradient* gradient;

        bool init(std::string tpName, std::string tpCreator, std::string tpDownloadURL, std::string tpDownloadVersion, std::string tpIcon, std::string tpDesc, bool feature, bool thing) {
            if (!CCLayerColor::init())
                return false;

            thingy = thing;

            if (thing) {
                this->setOpacity(100);
                opacity = 100;
            } else {
                this->setOpacity(50); 
                opacity = 50;
            }

            this->setContentSize(ccp(315, 35));
            this->setAnchorPoint(ccp(0, 1));
            this->setPositionY(207);

            creator = tpCreator;
            name = tpName;
            url = tpDownloadURL;
            version = tpDownloadVersion;
            featured = feature;
            icon = tpIcon;
            description = tpDesc;

            ghc::filesystem::path filePath = fmt::format("{}/packs/{}.zip", Loader::get()->getInstalledMod("geode.texture-loader")->getConfigDir(), name);

            gradient = CCLayerGradient::create(ccc4(0, 0, 0, 100), ccc4(0, 255, 0, 100));
            gradient->setContentSize(this->getContentSize());
            gradient->setZOrder(-3);
            gradient->setVector(ccp(90, 0));

            if (ghc::filesystem::exists(filePath)) {
                this->addChild(gradient);
                this->setOpacity(0);
            }

            if (featured) {
                auto featuredSpr = CCSprite::createWithSpriteFrameName("TWS_Featured.png"_spr);
                this->addChild(featuredSpr);
                featuredSpr->setScale(0.35);
                featuredSpr->setPosition(this->getContentSize() / 2);
                featuredSpr->setPositionX(30);
            }
            
            auto texturePackIconSpr = CCSprite::createWithSpriteFrameName("TWS_PlaceholderLogo.png"_spr);
            this->addChild(texturePackIconSpr);
            texturePackIconSpr->setScale(0.35);
            texturePackIconSpr->setPosition(this->getContentSize() / 2);
            texturePackIconSpr->setPositionX(30);
            

            auto txtr = CCTextureCache::get()->textureForKey(fmt::format("logo-{}", name).c_str());
            this->retain();

            if (!txtr) {
                web::AsyncWebRequest()
                    .fetch(icon)
                    .bytes()
                    .then([this, texturePackIconSpr](geode::ByteVector const& data) {
                        imgData = data;
                        auto image = Ref(new CCImage());
                        image->initWithImageData(const_cast<uint8_t*>(data.data()),data.size());
                        std::string theKey = fmt::format("logo-{}", name);
                        auto texture = CCTextureCache::get()->addUIImage(image,theKey.c_str());
                        image->release();
                        this->autorelease();
                        texturePackIconSpr->initWithTexture(texture);
                    })
                    .expect([this](std::string const& error) {
                        
                    });
            } else {
                auto image = Ref(new CCImage());
                image->initWithImageData(const_cast<uint8_t*>(imgData.data()),imgData.size());
                std::string theKey = fmt::format("logo-{}", name);
                auto texture = CCTextureCache::get()->addUIImage(image,theKey.c_str());
                image->release();
                this->autorelease();
                texturePackIconSpr->initWithTexture(texture);
            }
            

            

            auto texturePackName = CCLabelBMFont::create(
                tpName.c_str(),
                "bigFont.fnt"
            );
            this->addChild(texturePackName);
            texturePackName->setPosition(50, 27);
            texturePackName->setScale(0.375);
            texturePackName->setAnchorPoint(ccp(0, 0.5));

            std::string fullTPCreator = fmt::format("By {}", tpCreator);

            auto texturePackCreator = CCLabelBMFont::create(
                fullTPCreator.c_str(),
                "goldFont.fnt"
            );
            this->addChild(texturePackCreator);
            texturePackCreator->setPosition(50, 8);
            texturePackCreator->setScale(0.375);
            texturePackCreator->setAnchorPoint(ccp(0, 0.5));

            std::string fullTexturePackVersion = fmt::format("v{}", tpDownloadVersion);

            auto texturePackVersion = CCLabelBMFont::create(
                fullTexturePackVersion.c_str(),
                "bigFont.fnt"
            );
            this->addChild(texturePackVersion);
            texturePackVersion->setPosition(50, 17.5);
            texturePackVersion->setScale(0.25);
            texturePackVersion->setAnchorPoint(ccp(0, 0.5));
            texturePackVersion->setColor(ccc3(0, 200, 255));


            auto viewBtnMenu = CCMenu::create();

            auto downloadButtonSpr = CCSprite::createWithSpriteFrameName("TWS_DownloadButton.png"_spr);
            downloadButtonSpr->setScale(0.55);
            auto downloadButton = CCMenuItemSpriteExtra::create(
                downloadButtonSpr,
                this,
                menu_selector(TexturePackCell::onDownload)
            );

            auto viewButtonSpr = CCSprite::createWithSpriteFrameName("TWS_InfoButton.png"_spr);
            viewButtonSpr->setScale(0.55);
            auto viewButton = CCMenuItemSpriteExtra::create(
                viewButtonSpr,
                this,
                menu_selector(TexturePackCell::onView)
            );

            auto deleteButtonSpr = CCSprite::createWithSpriteFrameName("TWS_DeleteButton.png"_spr);
            deleteButtonSpr->setScale(0.55);
            auto deleteButton = CCMenuItemSpriteExtra::create(
                deleteButtonSpr,
                this,
                menu_selector(TexturePackCell::onDelete)
            );

            if (!ghc::filesystem::exists(filePath)) {
                viewBtnMenu->addChild(downloadButton);
            } else {
                viewBtnMenu->addChild(deleteButton);
            }

            this->addChild(viewBtnMenu);
            viewBtnMenu->addChild(viewButton);
            viewBtnMenu->setPosition(305, 17.5);
            viewBtnMenu->setAnchorPoint(ccp(1, 0.5));
            viewBtnMenu->setContentSize(viewButton->getContentSize());
            viewBtnMenu->setContentWidth(100);
            viewButton->setPosition(viewBtnMenu->getContentSize() / 2);
            viewBtnMenu->setLayout(
                RowLayout::create()
                    ->setAxisAlignment(AxisAlignment::End)
            );

            return true;
        }

        void onView(CCObject*) {
            auto info = TexturePackInfo::create(name, creator, url, icon, version, description, imgData, featured);
            info->show();
        }

        void onDownload(CCObject*) {
            std::string fileName = fmt::format("{}/packs/{}.zip", Loader::get()->getInstalledMod("geode.texture-loader")->getConfigDir(), name);
            web::AsyncWebRequest()
                .fetch(url)
                .into(fileName)
                .then([this](auto file) {
                    Notification::create("Download Successful", CCSprite::createWithSpriteFrameName("GJ_completesIcon_001.png"))->show();
                    auto workshopLayer = TextureWorkshopLayer::scene();
		            CCDirector::sharedDirector()->pushScene(workshopLayer);
                    log::info("{}", file);
                })
                .expect([this](std::string const& err) {
                    Notification::create("Download Failed", CCSprite::createWithSpriteFrameName("GJ_deleteIcon_001.png"))->show();
                    std::string fileName = fmt::format("{}/packs/{}.zip", Loader::get()->getInstalledMod("geode.texture-loader")->getConfigDir(), name);
                    ghc::filesystem::remove(fileName);
                });
        }

        void onDelete(CCObject*) {
            geode::createQuickPopup(
                "Delete Pack",
                fmt::format("Are you sure you want to delete {}?", name),
                "Nope", "Yeah",
                [this](auto, bool btn2) {
                    if (btn2) {
                        std::string fileName = fmt::format("{}/packs/{}.zip", Loader::get()->getInstalledMod("geode.texture-loader")->getConfigDir(), name);
                        ghc::filesystem::remove(fileName);
                        Notification::create(fmt::format("Deleted {}!", name), CCSprite::createWithSpriteFrameName("GJ_completesIcon_001.png"))->show();
                        auto workshopLayer = TextureWorkshopLayer::scene();
		                CCDirector::sharedDirector()->pushScene(workshopLayer);
                    }
                }
            );
        }

        static TexturePackCell* create(std::string tpName, std::string tpCreator, std::string tpDownloadURL, std::string tpDownloadVersion, std::string tpIcon, std::string tpDesc, bool featured, bool thing) {
            TexturePackCell* pRet = new TexturePackCell();
            if (pRet && pRet->init(tpName, tpCreator, tpDownloadURL, tpDownloadVersion, tpIcon, tpDesc, featured, thing)) {
                pRet->autorelease();
                return pRet;
            } else {
                delete pRet;
                return nullptr;
            }
        }

};