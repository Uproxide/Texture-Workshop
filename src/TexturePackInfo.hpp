#pragma once

#include <Geode/Geode.hpp>
#include <Geode/Loader.hpp>
#include <Geode/utils/web.hpp>
#include <Geode/loader/Event.hpp>
#include <Geode/ui/TextInput.hpp>
#include <cctype>
#include <algorithm>
#include "TexturePack.hpp"
#include "boobs.hpp"
#include "TextureWorkshopLayer.hpp"

using namespace geode::prelude;


class TexturePackInfo : public Popup<TexturePack*> {
public:

    TexturePack* texturePack;
    CCMenuItemSpriteExtra* downloadButton;
    CCMenuItemSpriteExtra* deleteButton;
    Slider* downloadProgressSlider;

    bool setup(TexturePack* tp) {
        log::info("hai");

        m_noElasticity = true;

        texturePack = tp;
        texturePack->popup = this;

        std::filesystem::path filePath = fmt::format("{}/packs/{}.zip", Loader::get()->getInstalledMod("geode.texture-loader")->getConfigDir(), tp->name);

        auto winSize = CCDirector::get()->getWinSize();
        float scale = CCDirector::sharedDirector()->getContentScaleFactor()/4;

        auto texturePackIconSpr = CCSprite::createWithSpriteFrameName("TWS_PlaceholderLogo.png"_spr);
        this->addChild(texturePackIconSpr);
        texturePackIconSpr->setScale(0.65);
        texturePackIconSpr->setPosition(this->getContentSize() / 2);
        texturePackIconSpr->setPositionY(texturePackIconSpr->getPositionY() + 80);
        texturePackIconSpr->setPositionX(texturePackIconSpr->getPositionX() - 116);
        texturePackIconSpr->setZOrder(1);

        if (tp->featured) {
            auto featuredSpr = CCSprite::createWithSpriteFrameName("TWS_Featured.png"_spr);
            this->addChild(featuredSpr);
            featuredSpr->setScale(0.65);
            featuredSpr->setPosition(texturePackIconSpr->getPosition());
        }

        auto txtr = CCTextureCache::get()->textureForKey(fmt::format("logo-{}", tp->name).c_str());
        this->retain();

        if (!txtr) {
            // n textur
        } else {
            texturePackIconSpr->initWithTexture(txtr);
            texturePackIconSpr->setScale(0.65 * scale);
        }


        auto tpNameMenu = CCMenu::create();

        auto texturePackName = CCLabelBMFont::create(
            tp->name.c_str(),
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
        tpNameMenu->setPositionY(tpNameMenu->getPositionY() + 17);

        auto texturePackVer = CCLabelBMFont::create(
            tp->version.c_str(),
            "bigFont.fnt"
        );
        texturePackVer->setScale(0.4);
        texturePackVer->setColor(ccc3(0, 200, 255));
        texturePackVer->setAnchorPoint(ccp(0, 0.5));
        this->addChild(texturePackVer);
        texturePackVer->setPosition(tpNameMenu->getPosition());
        texturePackVer->setPositionY(texturePackVer->getPositionY() - 13);

        std::string gdVerStr = fmt::format("Works on GD {}.", tp->gd);
        if (tp->gd == "Any") {
            gdVerStr = "Works on Any GD Version!";
        }

        auto gdVer = CCLabelBMFont::create(
            gdVerStr.c_str(),
            "bigFont.fnt"
        );
        gdVer->setScale(0.4);
        gdVer->setColor(ccc3(0, 200, 255));
        gdVer->setAnchorPoint(ccp(0, 0.5));
        this->addChild(gdVer);
        gdVer->setPosition(texturePackVer->getPosition());
        gdVer->setPositionY(gdVer->getPositionY() - 11);
        

        std::string fullTPCreator = fmt::format("By {}", tp->creator);

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
        texturePackCreator->setPositionY(texturePackCreator->getPositionY() - 18);

        auto line = CCSprite::createWithSpriteFrameName("floorLine_001.png");
        this->addChild(line);
        line->setPosition(this->getContentSize() / 2);
        line->setPositionY(line->getPositionY() + 45);
        line->setScale(0.675);

        std::string fullDesc = fmt::format("# {}\n{}", tp->name, tp->description);

        auto desc = MDTextArea::create(fullDesc, ccp(300, 150));
        this->addChild(desc);
        desc->setPosition(line->getPosition());
        desc->setPositionY(desc->getPositionY() - 81);

        auto viewBtnMenu = CCMenu::create();

        auto downloadButtonSpr = CCSprite::createWithSpriteFrameName("TWS_DownloadButton.png"_spr);
        downloadButtonSpr->setScale(.5);
        downloadButton = CCMenuItemSpriteExtra::create(
            downloadButtonSpr,
            this,
            menu_selector(TexturePackInfo::onDownload)
        );

        auto deleteButtonSpr = CCSprite::createWithSpriteFrameName("TWS_DeleteButton.png"_spr);
        deleteButtonSpr->setScale(.5);
        deleteButton = CCMenuItemSpriteExtra::create(
            deleteButtonSpr,
            this,
            menu_selector(TexturePackInfo::onDelete)
        );


        this->addChild(viewBtnMenu);
        viewBtnMenu->setLayout(RowLayout::create());
        viewBtnMenu->setContentSize(downloadButtonSpr->getContentSize());

        if (!std::filesystem::exists(filePath)) {
            viewBtnMenu->addChild(downloadButton);
            downloadButton->setPosition(downloadButtonSpr->getContentSize() / 2);

            deleteButton = nullptr; // its not added so i dont wanna crash
        } else {
            viewBtnMenu->addChild(deleteButton);
            deleteButton->setPosition(downloadButtonSpr->getContentSize() / 2);
        }
        
        viewBtnMenu->setPosition(line->getPosition());
        viewBtnMenu->setPositionX(viewBtnMenu->getPositionX() + 150);
        viewBtnMenu->setPositionY(viewBtnMenu->getPositionY() + 53);

        downloadProgressSlider = Slider::create(this, nullptr);
        downloadProgressSlider->getThumb()->setVisible(false);
        downloadProgressSlider->m_sliderBar->setVisible(true);
        downloadProgressSlider->setAnchorPoint(CCPointZero);
        downloadProgressSlider->setScale(0.5f);

        m_mainLayer->addChildAtPosition(downloadProgressSlider, Anchor::TopRight, ccp(-65, -20));
        texturePack->slider2 = downloadProgressSlider;

        auto req = web::WebRequest();
        req.userAgent(fmt::format("TextureWorkshopMod/{}", Mod::get()->getVersion()));
        req.certVerification(Mod::get()->getSettingValue<bool>("cert-verification"));
            
        m_downloadTP.setFilter(req.get(texturePack->download));
        updateDownloadIndicator();

        return true;
    }

    void onDownload(CCObject*);

    void onDelete(CCObject*) {
            geode::createQuickPopup(
                "Delete Pack",
                fmt::format("Are you sure you want to delete {}?", texturePack->name),
                "Nope", "Yeah",
                [this](auto, bool btn2) {
                    if (btn2) {
                        std::string fileName = fmt::format("{}/packs/{}.zip", Loader::get()->getInstalledMod("geode.texture-loader")->getConfigDir(), texturePack->name);
                        std::filesystem::remove(fileName);
                        Notification::create(fmt::format("Deleted {}!", texturePack->name), CCSprite::createWithSpriteFrameName("GJ_completesIcon_001.png"))->show();
                        this->onClose(nullptr);
		                TextureWorkshopLayer::get->onRefresh(nullptr);
                    }
                }
            );
        }

    void updateDownloadIndicator()
    {
        downloadButton->setVisible(!texturePack->isDownloading());
        downloadProgressSlider->setVisible(texturePack->isDownloading());
        as<AnchorLayoutOptions*>(downloadProgressSlider->getLayoutOptions())->setOffset(ccp(-65, -20) + ccp((deleteButton && deleteButton->isVisible()) ? -15 : 0, 0));
    }

    static TexturePackInfo* create(TexturePack* tp) {
        auto ret = new TexturePackInfo();
        if (ret && ret->initAnchored(342, 240, tp, "TWS_Box.png"_spr)) {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }

    ~TexturePackInfo()
    {
        texturePack->popup = nullptr;
        texturePack->slider2 = nullptr;
    }
};