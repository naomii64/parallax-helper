#pragma once

#include <Geode/Geode.hpp>
using namespace geode::prelude;

//a button that displays triggers
class TriggerButton : public Button {
public:
    static TriggerButton* create(geode::Button::ButtonCallback callback = nullptr);

    void setLabel(const geode::ZStringView& string);
    void addTrigger(int objectID);
protected:
    bool init(geode::Button::ButtonCallback callback);

    Label* m_label = nullptr;
    CCNode* m_triggerSpriteNode = nullptr;

    static constexpr int SPR_SIZE = 32;
};