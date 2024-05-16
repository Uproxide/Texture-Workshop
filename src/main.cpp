#include <Geode/Geode.hpp>

using namespace geode::prelude;

#include <Geode/modify/MenuLayer.hpp>
#include "TextureWorkshopLayer.hpp"
#include <Geode/utils/web.hpp>
#include <matjson.hpp>

class $modify(MyMenuLayer, MenuLayer) {
	bool init() {
		if (!MenuLayer::init()) {
			return false;
		}


		auto wsButton = CCMenuItemSpriteExtra::create(
			CCSprite::createWithSpriteFrameName("TWS_MainButton.png"_spr),
			this,
			menu_selector(MyMenuLayer::onMyButton)
		);

		auto menu = this->getChildByID("right-side-menu");
		menu->addChild(wsButton);

		wsButton->setID("workshop-button"_spr);

		menu->updateLayout();


		return true;
	}

	void onMyButton(CCObject*) {
		auto workshopLayer = TextureWorkshopLayer::scene();
		CCDirector::sharedDirector()->pushScene(CCTransitionFade::create(0.5f, workshopLayer));
	}
};
