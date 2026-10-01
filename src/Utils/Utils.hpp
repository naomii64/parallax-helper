#pragma once

#include "constants.hpp"

#include <Geode/Geode.hpp>
using namespace geode::prelude;

namespace ph::Utils{
    enum class LayerSortingType : int {
    	BY_DEPTH=0,
    	BY_GROUPID,
    	_count
    };
    
    inline LayerSortingType getSortingType(){
    	int gottenfromthefile = Mod::get()->getSavedValue<int>(constants::keystrings::SAVED_SELECTED_SORTING_ID,0);

    	return LayerSortingType(std::clamp(gottenfromthefile,0,int(LayerSortingType::_count) - 1));
    }

    void enableButton(Button* item);
    void disableButton(Button* item);    
    CCMenu* createTwoButtonMenu(std::function<void(bool)> callback,const char* btn1Title = "Cancel", const char* btn2Title = "Confirm");
};
using namespace ph;