#pragma once

#include <Geode/Geode.hpp>
using namespace geode::prelude;

#include "CustomNumberInput.hpp"

class NumberRequestPopup : public geode::Popup {
public:
    static NumberRequestPopup* create(std::function<void(NumberRequestPopup*,bool)> callback);

    CustomNumberInput* m_numberInput = nullptr;
protected:
    bool init(std::function<void(NumberRequestPopup*,bool)> callback);

    std::function<void(NumberRequestPopup*,bool)> m_callback;

};