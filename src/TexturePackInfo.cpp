#include "TexturePackInfo.hpp"
#include "TexturePackCell.hpp"

void TexturePackInfo::onDownload(CCObject*) {
    texturePack->downloadPack();

    updateDownloadIndicator();
    as<TexturePackCell*>(texturePack->cell)->updateDownloadIndicator();
}