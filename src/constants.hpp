#pragma once

#include <Geode/Geode.hpp>

namespace constants{
	namespace ui {
		constexpr ccColor3B DISABLED_COLOR = ccColor3B{166,166,166};
		constexpr int DISABLED_ALPHA = 175;

		constexpr float PADDING = 10.0f;
	};

	namespace keystrings {
		constexpr std::string_view SELECTED_SORTING_ID = "selected-sorting-id";
	}

	//to avoid explicit in copy-initialization build issues on android create a set of empty groups for finding a group ID like this
	//this is to be used when getting a new groupID
	const gd::unordered_set<int> EMPTY_SET{};

	enum LayerSortingType : int {
		BY_DEPTH=0,
		BY_GROUPID,
		_count
	};
	//idk where else to put this
	inline LayerSortingType getSortingType(){
		int gottenfromthefile = Mod::get()->getSavedValue<int>(constants::keystrings::SELECTED_SORTING_ID,0);
		//clamp it in case of user tampering
		if(gottenfromthefile < 0) return LayerSortingType(0);
		if(gottenfromthefile >= LayerSortingType::_count) return LayerSortingType(LayerSortingType::_count - 1);

		return LayerSortingType(gottenfromthefile);
	}
}