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

        ghc::filesystem::path filePath = fmt::format("{}/packs/{}.zip", Loader::get()->getInstalledMod("geode.texture-loader")->getConfigDir(), name);

        auto winSize = CCDirector::get()->getWinSize();
        float scale = CCDirector::sharedDirector()->getContentScaleFactor()/4;

        auto texturePackIconSpr = CCSprite::createWithSpriteFrameName("TWS_PlaceholderLogo.png"_spr);
        this->addChild(texturePackIconSpr);
        texturePackIconSpr->setScale(0.65);
        texturePackIconSpr->setPosition(this->getContentSize() / 2);
        texturePackIconSpr->setPositionY(texturePackIconSpr->getPositionY() + 80);
        texturePackIconSpr->setPositionX(texturePackIconSpr->getPositionX() - 100);
        texturePackIconSpr->setZOrder(1);

        if (featured) {
            auto featuredSpr = CCSprite::createWithSpriteFrameName("TWS_Featured.png"_spr);
            this->addChild(featuredSpr);
            featuredSpr->setScale(0.65);
            featuredSpr->setPosition(texturePackIconSpr->getPosition());
        }

        auto txtr = CCTextureCache::get()->textureForKey(fmt::format("logo-{}", name).c_str());
        this->retain();

        if (!txtr) {
            // n textur
        } else {
            texturePackIconSpr->initWithTexture(txtr);
            texturePackIconSpr->setScale(0.65 * scale);
        }


        auto tpNameMenu = CCMenu::create();

        auto texturePackName = CCLabelBMFont::create(
            tpName.c_str(),
            "bigFont.fnt"
        );
        tpNameMenu->addChild(texturePackName);
        // tpNameMenu->setPositionY(tpNameMenu->getPositionY() + 120);
        // tpNameMenu->setPositionX(tpNameMenu->getPositionX() + 32);
        tpNameMenu->setScale(0.6);
        tpNameMenu->setAnchorPoint(ccp(0, 0.5));
        tpNameMenu->setLayout(
            RowLayout::create()
                ->setAxisAlignment(AxisAlignment::Start)
        );
        this->addChild(tpNameMenu);
        tpNameMenu->setContentWidth(300);
        tpNameMenu->updateLayout();
        tpNameMenu->setPosition(texturePackIconSpr->getPosition());
        tpNameMenu->setPositionX(tpNameMenu->getPositionX() + 35);
        tpNameMenu->setPositionY(tpNameMenu->getPositionY() + 7);
        

        std::string fullTPCreator = fmt::format("By {}", tpCreator);

        auto texturePackCreator = CCLabelBMFont::create(
            fullTPCreator.c_str(),
            "goldFont.fnt"
        );
        this->addChild(texturePackCreator);
        // texturePackCreator->setPositionY(texturePackCreator->getPositionY() + 103);
        // texturePackCreator->setPositionX(texturePackCreator->getPositionX() + 32);
        texturePackCreator->setScale(0.6);
        texturePackCreator->setAnchorPoint(ccp(0, 0.5));
        texturePackCreator->setPosition(texturePackIconSpr->getPosition());
        texturePackCreator->setPositionX(texturePackCreator->getPositionX() + 35);
        texturePackCreator->setPositionY(texturePackCreator->getPositionY() - 7);

        auto line = CCSprite::createWithSpriteFrameName("floorLine_001.png");
        this->addChild(line);
        line->setPosition(this->getContentSize() / 2);
        line->setPositionY(line->getPositionY() + 45);
        line->setScale(0.675);

        auto desc = MDTextArea::create(description, ccp(300, 150));
        this->addChild(desc);
        desc->setPosition(line->getPosition());
        desc->setPositionY(desc->getPositionY() - 81);

        auto viewBtnMenu = CCMenu::create();

        auto downloadButtonSpr = CCSprite::createWithSpriteFrameName("TWS_DownloadButton.png"_spr);
        downloadButtonSpr->setScale(.5);
        auto downloadButton = CCMenuItemSpriteExtra::create(
            downloadButtonSpr,
            this,
            menu_selector(TexturePackInfo::onDownload)
        );

        auto deleteButtonSpr = CCSprite::createWithSpriteFrameName("TWS_DeleteButton.png"_spr);
        deleteButtonSpr->setScale(.5);
        auto deleteButton = CCMenuItemSpriteExtra::create(
            deleteButtonSpr,
            this,
            menu_selector(TexturePackInfo::onDelete)
        );


        this->addChild(viewBtnMenu);
        viewBtnMenu->setLayout(RowLayout::create());
        viewBtnMenu->setContentSize(downloadButtonSpr->getContentSize());

        if (!ghc::filesystem::exists(filePath)) {
            viewBtnMenu->addChild(downloadButton);
            downloadButton->setPosition(downloadButtonSpr->getContentSize() / 2);
        } else {
            viewBtnMenu->addChild(deleteButton);
            deleteButton->setPosition(downloadButtonSpr->getContentSize() / 2);
        }
        
        viewBtnMenu->setPosition(line->getPosition());

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
                this->onClose(nullptr);
                auto workshopLayer = TextureWorkshopLayer::scene();
		        CCDirector::sharedDirector()->pushScene(workshopLayer);
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
                        this->onClose(nullptr);
		                CCDirector::sharedDirector()->pushScene(workshopLayer);
                    }
                }
            );
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