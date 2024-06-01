#pragma once

using namespace geode::prelude;

class TexturePack
{
    public:
        std::string name, creator, download, icon, version, description;
        bool featured;

        static TexturePack* create(std::string p0, std::string p1, std::string p2, std::string p3, std::string p4, std::string p5, bool p6) {
            auto tp = new TexturePack();

            tp->name = p0;
            tp->creator = p1;
            tp->download = p2;
            tp->icon = p3;
            tp->version = p4;
            tp->description = p5;
            tp->featured = p6;

            return tp;
        }

        /*
            p0 = name
            p1 = creator
            p2 = download
            p3 = icon
            p4 = version
            p5 = description
        */
};