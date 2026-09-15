#pragma once

#include <Geode/cocos/cocoa/CCObject.h>

#include "export.hpp"

namespace alpha::ui {

enum class Cursor {
    NONE
};

class ALPHAS_UI_PACK_DLL CursorManager : public cocos2d::CCObject {
public:
    static CursorManager* get();
    void setCursor(Cursor cursor);
    void resetCursor();
protected:
    void init();
    void update(float dt);
    bool isMouseInWindow();
    
    Cursor m_currentCursor = Cursor::NONE;

#if defined(GEODE_IS_WINDOWS)
public:
protected:
    std::unordered_map<Cursor, HCURSOR> m_cursors;
#elif defined(GEODE_IS_MACOS)
    bool m_hidden = false;
    std::unordered_map<Cursor, void*> m_cursors;
#endif
};

}