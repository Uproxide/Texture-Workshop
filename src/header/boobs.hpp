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

/*
    Well, I’m a Mug-blooded American, ya hear?
    These freedom-loving veins got nothing but root beer
    In ‘09 I got diagnosed with some form of epilepsy
    Good doctor told me that my cure was made by Pepsi
    Every day I get dressed up in my bootcut jeans
    I remind the CDC that we already got our vaccines
    Libtards are trying to ban my foamy carbonation
    I call up my patriots to fight back for the sake of our nation!
*/