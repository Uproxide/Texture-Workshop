#pragma once

#include <Geode/Geode.hpp>
#include <Geode/utils/web.hpp>
#include <Geode/loader/Event.hpp>
#include <Geode/ui/TextInput.hpp>
#include <cctype>
#include <algorithm>
#include "TexturePackInfo.hpp"
#include "TextureWorkshopLayer.hpp"
#include <Geode/utils/file.hpp>
#include "TexturePack.hpp"

using namespace geode::prelude;

class TexturePackCell : public CCLayerColor {
    public:

        bool thingy;
        int opacity;

        geode::ByteVector imgData;
        CCLayerGradient* gradient;

        TexturePack* texturePack;

        Slider* downloadProgressSlider;
        CCMenuItemSpriteExtra* downloadButton;

        EventListener<web::WebTask> m_getIcon;


        bool init(TexturePack* tp, bool thing) {
            if (!CCLayerColor::init())
                return false;

            thingy = thing;
            texturePack = tp;

            texturePack->cell = this;

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

            std::filesystem::path filePath = fmt::format("{}/packs/{}.zip", Loader::get()->getInstalledMod("geode.texture-loader")->getConfigDir(), tp->name);
            float scale = CCDirector::sharedDirector()->getContentScaleFactor()/4;

            gradient = CCLayerGradient::create(ccc4(0, 0, 0, 100), ccc4(0, 255, 0, 100));
            gradient->setContentSize(this->getContentSize());
            gradient->setZOrder(-3);
            gradient->setVector(ccp(90, 0));

            std::string versionSaveThing = fmt::format("{} Version", tp->name);

            if (std::filesystem::exists(filePath)) {
                if (Mod::get()->getSavedValue<std::string>(versionSaveThing) == tp->version) {
                    gradient = CCLayerGradient::create(ccc4(0, 0, 0, 100), ccc4(0, 255, 0, 100));
                    gradient->setContentSize(this->getContentSize());
                    gradient->setZOrder(-3);
                    gradient->setVector(ccp(90, 0));
                    this->addChild(gradient);
                    this->setOpacity(0); 
                } else {
                    gradient = CCLayerGradient::create(ccc4(0, 0, 0, 100), ccc4(94, 137, 255, 100));
                    gradient->setContentSize(this->getContentSize());
                    gradient->setZOrder(-3);
                    gradient->setVector(ccp(90, 0));
                    this->addChild(gradient);
                    this->setOpacity(0); 
                }
                
            }

            if (tp->featured) {
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

            auto txtr = CCTextureCache::get()->textureForKey(fmt::format("logo-{}", tp->name).c_str());
            this->retain();

            if (!txtr) {
                m_getIcon.bind([this, texturePackIconSpr, scale, tp] (web::WebTask::Event* e) {
                if (web::WebResponse* res = e->getValue()) {
                    if (res->ok()) {
                        imgData = res->data();
                        auto image = Ref(new CCImage());
                        image->initWithImageData(const_cast<uint8_t*>(res->data().data()),res->data().size());
                        std::string theKey = fmt::format("logo-{}", tp->name);
                        auto texture = CCTextureCache::get()->addUIImage(image,theKey.c_str());
                        image->release();
                        this->autorelease();
                        texturePackIconSpr->initWithTexture(texture);
                        texturePackIconSpr->setScale(0.35 * scale);
                    } else {
                        // icon has NO icon hwta
                    }
                    
                    
                } else if (e->isCancelled()) {
                    log::info("The request was cancelled... So sad :(");
                }
                });

                auto req = web::WebRequest();
                // req.userAgent(fmt::format("TextureWorkshopMod/{}", Mod::get()->getVersion()));
                req.certVerification(Mod::get()->getSettingValue<bool>("cert-verification"));
                
                m_getIcon.setFilter(req.get(tp->icon));
            } else {
                auto image = Ref(new CCImage());
                image->initWithImageData(const_cast<uint8_t*>(imgData.data()),imgData.size());
                std::string theKey = fmt::format("logo-{}", tp->name);
                auto texture = CCTextureCache::get()->addUIImage(image,theKey.c_str());
                image->release();
                this->autorelease();
                texturePackIconSpr->initWithTexture(texture);
                texturePackIconSpr->setScale(0.35 * scale);
            }

            auto texturePackName = CCLabelBMFont::create(
                tp->name.c_str(),
                "bigFont.fnt"
            );
            this->addChild(texturePackName);
            texturePackName->setPosition(50, 27);
            texturePackName->setScale(0.375);
            texturePackName->setAnchorPoint(ccp(0, 0.5));

            std::string fullTPCreator = fmt::format("By {}", tp->creator);

            auto texturePackCreator = CCLabelBMFont::create(
                fullTPCreator.c_str(),
                "goldFont.fnt"
            );
            texturePackCreator->setScale(0.375);

            auto creatorMenu = CCMenu::create();
            creatorMenu->setPosition(50, 8);
            creatorMenu->setAnchorPoint(ccp(0, 0.5));

            auto creatorButton = CCMenuItemSpriteExtra::create(
                texturePackCreator,
                this,
                menu_selector(TexturePackCell::onCreator)
            );
            creatorMenu->addChild(creatorButton);
            creatorMenu->setLayout(
                RowLayout::create()
                    ->setAxisAlignment(AxisAlignment::Start)
            );

            this->addChild(creatorMenu);


            std::string fullTexturePackVersion = fmt::format("v{}", tp->version);

            auto texturePackVersion = CCLabelBMFont::create(
                fullTexturePackVersion.c_str(),
                "bigFont.fnt"
            );
            this->addChild(texturePackVersion);
            texturePackVersion->setPosition(50, 17.5);
            texturePackVersion->setScale(0.25);
            texturePackVersion->setAnchorPoint(ccp(0, 0.5));
            texturePackVersion->setColor(ccc3(0, 200, 255));

            auto texturePackDownloadsIcon = CCSprite::createWithSpriteFrameName("GJ_downloadsIcon_001.png");
            texturePackDownloadsIcon->setScale(0.325f);
            texturePackDownloadsIcon->setPosition(texturePackVersion->getPosition() + ccp(texturePackVersion->getScaledContentWidth() + 8, -0.5f));
            this->addChild(texturePackDownloadsIcon);

            auto texturePackDownloadsCount = CCLabelBMFont::create(fmt::format("{}", texturePack->downloads).c_str(), "bigFont.fnt");
            texturePackDownloadsCount->setScale(0.25f);
            texturePackDownloadsCount->setAnchorPoint(ccp(0, 0.5f));
            texturePackDownloadsCount->setColor(ccc3(100, 200, 35));
            texturePackDownloadsCount->setPosition(texturePackDownloadsIcon->getPosition() + ccp(4, 0.5f));
            this->addChild(texturePackDownloadsCount);


            auto viewBtnMenu = CCMenu::create();

            auto downloadButtonSpr = CCSprite::createWithSpriteFrameName("TWS_DownloadButton.png"_spr);
            downloadButtonSpr->setScale(0.55);
            downloadButton = CCMenuItemSpriteExtra::create(
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

            downloadProgressSlider = Slider::create(this, nullptr);
            downloadProgressSlider->getThumb()->setVisible(false);
            downloadProgressSlider->m_sliderBar->setVisible(true);
            downloadProgressSlider->setPosition(getContentSize() * ccp(1, 0.5f) + ccp(-80, 0));
            downloadProgressSlider->setAnchorPoint(CCPointZero);
            downloadProgressSlider->setScaleX(0.35f);
            downloadProgressSlider->setScaleY(0.5f);

            this->addChild(downloadProgressSlider);
            texturePack->slider = downloadProgressSlider;

            if (!std::filesystem::exists(filePath)) {
                viewBtnMenu->addChild(downloadButton);
            } else if (std::filesystem::exists(filePath) && Mod::get()->getSavedValue<std::string>(versionSaveThing) != tp->version) {
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

            updateDownloadIndicator();

            return true;
        }

        void onView(CCObject*) {
            auto info = TexturePackInfo::create(texturePack);
            info->show();
        }

        void onDownload(CCObject*) {
            texturePack->downloadPack();
            updateDownloadIndicator();
        }

        void onCreator(CCObject*) {
            TextureWorkshopLayer::get->inp->setString(fmt::format("by:{}", texturePack->creator));
            TextureWorkshopLayer::get->searchTPs();
        }

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
                        TextureWorkshopLayer::get->onRefresh(nullptr);
                    }
                }
            );
        }

        void updateDownloadIndicator()
        {
            downloadButton->setVisible(!texturePack->isDownloading());
            downloadProgressSlider->setVisible(texturePack->isDownloading());
        }

        static TexturePackCell* create(TexturePack* tp, bool thing) {
            TexturePackCell* pRet = new TexturePackCell();
            if (pRet && pRet->init(tp, thing)) {
                pRet->autorelease();
                return pRet;
            } else {
                delete pRet;
                return nullptr;
            }
        }

        ~TexturePackCell()
        {
            texturePack->slider = nullptr;
            texturePack->cell = nullptr;
        }

};
