#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

class BlockTouchLayer : public CCLayer
{
    public:
        virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
        {
            return boundingBox().containsPoint(pTouch->getLocation());
        }

        void registerWithTouchDispatcher()
        {
            CCTouchDispatcher::get()->addTargetedDelegate(this, -129, true);
        }

        CREATE_FUNC(BlockTouchLayer)
};