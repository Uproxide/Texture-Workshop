#pragma once

#include <Geode/Geode.hpp>
#include <Geode/Loader.hpp>
#include <Geode/utils/web.hpp>
#include <Geode/loader/Event.hpp>
#include <Geode/ui/TextInput.hpp>
#include <cctype>
#include <algorithm>
#include "boobs.hpp"
#include "TextureWorkshopLayer.hpp"

using namespace geode::prelude;


class TWSFilters : public Popup<> {
public:
    //CCMenuItemToggler* versionFilter;
    static inline TWSFilters* get = nullptr;

    bool setup() {
        log::info("hai");
        auto winSize = CCDirector::get()->getWinSize();

        get = this;
        
        this->setTitle("Filters");
        m_title->setScale(0.85);

        auto bg = cocos2d::extension::CCScale9Sprite::create("square02_small.png");
        m_mainLayer->addChild(bg);
        bg->setPosition(m_mainLayer->getContentSize() / 2);
        bg->setPositionY(bg->getPositionY() - 13);
        bg->setContentWidth(110);
        bg->setContentHeight(180);
        bg->setOpacity(100);

        auto menu = CCMenu::create();
        menu->setContentSize(bg->getContentSize());
        bg->addChild(menu);
        menu->setPosition(0, 0);

        //auto spr = ButtonSprite::create("Hi mom!");

        //auto btn = CCMenuItemSpriteExtra::create(
            //spr, this, nullptr
        //);

        //menu->addChild(btn);
        //btn->setPosition(menu->getContentSize() / 2);

        /*versionFilter = CCMenuItemToggler::createWithStandardSprites(
            this,
            menu_selector(TWSFilters::onToggle),
            1
        );
        versionFilter->setTag(1);
        versionFilter->toggle(boobs::versionFilter);
        menu->addChild(versionFilter);
        versionFilter->setPosition(menu->getContentSize() / 2);
        auto versionFilterLabel = CCLabelBMFont::create("Version\n Filter", "goldFont.fnt");
        versionFilter->addChild(versionFilterLabel);
        versionFilterLabel->setScale(0.6);
        versionFilterLabel->setPosition(versionFilter->getContentSize() / 2);
        versionFilterLabel->setPositionY(versionFilterLabel->getPositionY() + 33);*/

        auto lol = CCLabelBMFont::create("wait... thats all? WHAT THE fun :3", "chatFont.fnt");
        lol->setAnchorPoint(ccp(0, 0.5));
        lol->setScale(0.2);
        menu->addChild(lol);

        auto uc = CCLabelBMFont::create("Under Construction", "bigFont.fnt");
        uc->setAnchorPoint(ccp(0.5, 0.5));
        uc->setScale(0.275);
        menu->addChild(uc);
        uc->setPosition(menu->getContentSize().width / 2, menu->getContentSize().height / 2);

        return true;
    }

    void onToggle(CCObject* toggle) {
        switch (toggle->getTag()) {
            case 1:
                boobs::versionFilter = !boobs::versionFilter;
                Mod::get()->setSavedValue<bool>("version-filter", boobs::versionFilter);
                TextureWorkshopLayer::get->onGetTPsFinished();
                break;
            default:
                break;
        }
    }

    static TWSFilters* create() {
        auto ret = new TWSFilters();
        if (ret && ret->initAnchored(140, 230, "TWS_Box.png"_spr)) {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }
};