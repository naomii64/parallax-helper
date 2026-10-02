#pragma once

#include <Geode/Geode.hpp>
using namespace geode::prelude;

namespace ph::constants{
	namespace ui {
		constexpr ccColor3B DISABLED_COLOR = ccColor3B{166,166,166};
		constexpr int DISABLED_ALPHA = 175;

		constexpr float PADDING = 10.0f;

		// constexpr ccColor3B COLOR_TRIGGER_MOVE = ccColor3B{255,0,255};
		// constexpr ccColor3B COLOR_TRIGGER_ADV_FOLLOW = ccColor3B{204,255,199};
		// constexpr ccColor3B COLOR_TRIGGER_FOLLOW = ccColor3B{255,127,127};
		// constexpr ccColor3B COLOR_TRIGGER_SCALE = ccColor3B{63,191,255};
	};

	//strings for accessing settings and saved values
	namespace keystrings {
		constexpr std::string_view SAVED_SELECTED_SORTING_ID = "selected-sorting-id";
	}

	//to avoid explicit in copy-initialization build issues on android create a set of empty groups for finding a group ID like this
	//this is to be used when getting a new groupID
	const gd::unordered_set<int> EMPTY_SET{};
}
using namespace ph;