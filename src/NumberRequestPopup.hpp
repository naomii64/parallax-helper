#pragma once

#include <Geode/Geode.hpp>
#include "CustomNumberInput.hpp"

class NumberRequestPopup : public geode::Popup {
protected:
    geode::Function<void(NumberRequestPopup*,bool)> m_callback;

    bool init(geode::Function<void(NumberRequestPopup*,bool)> callback);
public:
    static NumberRequestPopup* create(geode::Function<void(NumberRequestPopup*,bool)> callback);

    CustomNumberInput* m_numberInput = nullptr;
};
