#pragma once

using namespace geode::prelude;

class TexturePack
{
    public:
        std::string name, creator, download, icon, version, gd, description;
        bool featured;

        static TexturePack* create(std::string p0, std::string p1, std::string p2, std::string p3, std::string p4, std::string p5, std::string p6, bool p7) {
            auto tp = new TexturePack();

            tp->name = p0;
            tp->creator = p1;
            tp->download = p2;
            tp->icon = p3;
            tp->version = p4;
            tp->description = p5;
            tp->gd = p6;
            tp->featured = p7;

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