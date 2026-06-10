#pragma once

using namespace geode::prelude;

class TWSPack;

class boobs {
    public:
        inline static bool downloaded = false;
        inline static bool versionFilter = false;
        inline static matjson::Value tpJson; // nothing to see here!

        inline static int page = 1;
        inline static int tpCount = 0;
        inline static std::string search = "";

        inline static bool downloadsSort = false;
        inline static bool recentlyUpdatedSort = false;
        inline static bool featuredSort = false;
        
        inline static std::vector<TWSPack*> downloading = { };
};