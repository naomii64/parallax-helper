#pragma once

#include <Geode/Geode.hpp>
using namespace geode::prelude;

#include "CustomNumberInput.hpp"

class DuplicateAndLayerPopup : public geode::Popup {
public:
    static DuplicateAndLayerPopup* create(geode::Function<void(float,float,int)> onConfirmCallback);//function input is range min, range max, count

protected:
    bool init(geode::Function<void(float,float,int)> onConfirmCallback);
    geode::Function<void(float,float,int)> m_onConfirmCallback;

    CustomNumberInput* m_layerCountInput = nullptr;
    CustomNumberInput* m_rangeInput0 = nullptr;
    CustomNumberInput* m_rangeInput1 = nullptr;
};
