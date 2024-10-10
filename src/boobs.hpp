#pragma once

using namespace geode::prelude;

class boobs {
    public:
        inline static bool downloaded = false;
        inline static bool versionFilter = true;
        inline static matjson::Value tpJson; // nothing to see here!
        inline static std::vector<TexturePack*> downloads = { }; // what the freak, a pointer !!!
};