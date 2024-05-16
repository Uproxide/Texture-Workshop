#pragma once

using namespace geode::prelude;

class JsonManager {
    public:
        inline static bool downloaded = false;
        inline static matjson::Value tpJson; // nothing to see here!
};