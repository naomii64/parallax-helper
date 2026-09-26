#pragma once

#include <Geode/Geode.hpp>
using namespace geode::prelude;

class NumberRequestPopup : public geode::Popup {
public:
    static NumberRequestPopup* create();

protected:
    bool init();

};