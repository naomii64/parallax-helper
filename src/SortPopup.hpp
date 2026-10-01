#pragma once

#include <Geode/Geode.hpp>
using namespace geode::prelude;

#include "Utils/RadioMenu.hpp"

class SortPopup : public geode::Popup {
public:
    static SortPopup* create(geode::Function<void(SortPopup*)> callback);

    ~SortPopup();

    int getSelectedSortingID() const;
protected:
    bool init(geode::Function<void(SortPopup*)> callback);

    geode::Function<void(SortPopup*)> m_callback = nullptr;

    RadioMenu* m_radioMenu = nullptr;
};