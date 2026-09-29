#pragma once

#include <Geode/Geode.hpp>
using namespace geode::prelude;

//just used to help create popups
class PopupUtils {
public:
    static CCMenu* createTwoButtonMenu(std::function<void(bool)> callback,const char* btn1Title = "Cancel", const char* btn2Title = "Confirm");
};