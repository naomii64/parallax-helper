#pragma once

#include "constants.hpp"

#include <Geode/Geode.hpp>
using namespace geode::prelude;

namespace Utils{
    enum class LayerSortingType : int {
    	BY_DEPTH=0,
    	BY_GROUPID,
    	_count
    };
    
    inline LayerSortingType getSortingType(){
    	int gottenfromthefile = Mod::get()->getSavedValue<int>(constants::keystrings::SELECTED_SORTING_ID,0);

    	return LayerSortingType(std::clamp(gottenfromthefile,0,int(LayerSortingType::_count) - 1));
    }

    inline void enableNode(Button* item){
        item->setEnabled(true);
        item->setColor(ccWHITE);
        item->setOpacity(255);
    }
    inline void disableNode(Button* item){
        item->setEnabled(false);
        item->setColor(constants::ui::DISABLED_COLOR);
        item->setOpacity(constants::ui::DISABLED_ALPHA);
    }
};