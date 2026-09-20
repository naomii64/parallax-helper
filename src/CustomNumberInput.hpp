#pragma once

#include <Geode/Geode.hpp>
using namespace geode::prelude;

class CustomNumberInput : public TextInput {    
public:
    static CustomNumberInput* create(float width, geode::ZStringView placeholder = "Num", geode::ZStringView font = "bigFont.fnt");
protected:
    bool init(float width, geode::ZStringView placeholder, geode::ZStringView font);
};