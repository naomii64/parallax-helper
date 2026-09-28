#pragma once

#include <Geode/Geode.hpp>
using namespace geode::prelude;

class CustomNumberInput : public TextInput {    
public:
    static CustomNumberInput* create(float width, geode::ZStringView placeholder = "Num", geode::ZStringView font = "bigFont.fnt");

    void setTypeInt(bool isSigned = true);
    void setTypeFloat();

    void enableArrows();

    template<typename T> 
    T getNumber() const {
        std::string inputString = getString();
        auto result = geode::utils::numFromString<T>(inputString);
        T num = T(0);

        if (result) num = result.unwrap();

        return num;
    }
    template<typename T>
    void setNumber(T number) {
        setString(fmt::to_string<T>(number));
    }

protected:
    bool init(float width, geode::ZStringView placeholder, geode::ZStringView font);

    bool m_isSigned = true;
};