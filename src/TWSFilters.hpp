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
    bool setup() {
        log::info("hai");
        auto winSize = CCDirector::get()->getWinSize();
        
        this->setTitle("Filters");
        m_title->setScale(0.85);

        auto bg = cocos2d::extension::CCScale9Sprite::create("square02_small.png");
        m_mainLayer->addChild(bg);
        bg->setPosition(m_mainLayer->getContentSize() / 2);
        bg->setPositionY(bg->getPositionY() - 13);
        bg->setContentWidth(340);
        bg->setContentHeight(180);
        bg->setOpacity(100);

        auto menu = CCMenu::create();
        menu->setContentSize(bg->getContentSize());
        bg->addChild(menu);
        menu->setPosition(0, 0);

        auto versionFilter = CCMenuItemToggler::createWithStandardSprites(
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
        versionFilterLabel->setPositionY(versionFilterLabel->getPositionY() + 33);

        auto lol = CCLabelBMFont::create("wait... thats all? WHAT THE fun :3", "chatFont.fnt");
        lol->setAnchorPoint(ccp(0, 0.5));
        lol->setScale(0.2);
        menu->addChild(lol);

        return true;
    }

    void onToggle(CCObject* toggle) {
        switch (toggle->getTag()) {
            case 1:
                boobs::versionFilter = !boobs::versionFilter;
                Mod::get()->setSavedValue<bool>("version-filter", boobs::versionFilter);
                TextureWorkshopLayer::get->onRefresh(nullptr);
                break;
            default:
                break;
        }
    }

    static TWSFilters* create() {
        auto ret = new TWSFilters();
        if (ret && ret->initAnchored(380, 230, "TWS_Box.png"_spr)) {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }
};