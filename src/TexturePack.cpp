#include <Geode/Geode.hpp>
#include <Geode/utils/web.hpp>
#include "TexturePack.hpp"
#include "boobs.hpp"
#include "TextureWorkshopLayer.hpp"

using namespace geode::prelude;

void TexturePack::downloadPack()
{
    if (std::find(boobs::downloads.begin(), boobs::downloads.end(), this) != boobs::downloads.end())
        return log::error("already downloading the pack :3");

    std::string fileName = fmt::format("{}/packs/{}.zip", Loader::get()->getLoadedMod("geode.texture-loader")->getConfigDir(), name);
    m_downloadTP.bind([this] (web::WebTask::Event* e) {
            if (web::WebResponse* res = e->getValue()) {
                if (res->into(fmt::format("{}/packs/{}.zip", Loader::get()->getInstalledMod("geode.texture-loader")->getConfigDir(), name))) {
                    std::string versionSaveThing = fmt::format("{} Version", name);
                    Mod::get()->setSavedValue<std::string>(versionSaveThing, version);
                    Notification::create("Download Successful", CCSprite::createWithSpriteFrameName("GJ_completesIcon_001.png"))->show();
                    
                    if (popup)
                        popup->keyBackClicked();
                    
                    if (TextureWorkshopLayer::get)
                        TextureWorkshopLayer::get->onRefresh(nullptr);
                } else {
                    Notification::create("Download Failed", CCSprite::createWithSpriteFrameName("GJ_deleteIcon_001.png"))->show();
                    std::filesystem::remove(fmt::format("{}/packs/{}.zip", Loader::get()->getInstalledMod("geode.texture-loader")->getConfigDir(), name));
                }

                boobs::downloads.erase(std::find(boobs::downloads.begin(), boobs::downloads.end(), this));
                
            } else if (e->isCancelled()) {
                log::info("The request was cancelled... So sad :(");
                boobs::downloads.erase(std::find(boobs::downloads.begin(), boobs::downloads.end(), this));
            }
            else if (auto progress = e->getProgress())
            {
                if (slider)
                    slider->setValue(progress->downloadProgress().value_or(0.69420f) / 100.0f);

                if (slider2)
                    slider2->setValue(progress->downloadProgress().value_or(0.69420f) / 100.0f);
            }
        });

    auto req = web::WebRequest();
        
    m_downloadTP.setFilter(req.get(download));
    // req.userAgent(fmt::format("TextureWorkshopMod/{}", Mod::get()->getVersion()));
    req.certVerification(Mod::get()->getSettingValue<bool>("cert-verification"));

    boobs::downloads.push_back(this);
}

bool TexturePack::isDownloading()
{
    return std::find(boobs::downloads.begin(), boobs::downloads.end(), this) != boobs::downloads.end();
}
