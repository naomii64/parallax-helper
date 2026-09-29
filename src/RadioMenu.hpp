#pragma once

#include <Geode/Geode.hpp>
using namespace geode::prelude;


class RadioMenu : public CCMenu {
public:
    static RadioMenu* create(const cocos2d::CCSize &size);
    int addEntry(const char* labelText);

    void untoggleAll();

    void selectEntry(int entryID);
    
    int m_selectedIndex = 0;
protected:
    bool init(const cocos2d::CCSize &size);

    CCMenu* m_menu = nullptr;
};

class RadioMenuEntry : public CCMenu {
public:
    static RadioMenuEntry* create(const cocos2d::CCSize &size,const char* labelText);

    RadioMenu* m_parent = nullptr;
    CCMenuItemToggler* m_checkbox = nullptr;

    int m_index = -1;
protected:
    bool init(const cocos2d::CCSize &size,const char* labelText);
};
