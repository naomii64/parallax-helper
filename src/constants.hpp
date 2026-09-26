#pragma once

#include <Geode/Geode.hpp>

namespace constants{
	namespace ui {
		constexpr ccColor3B disabledColor = ccColor3B{166,166,166};
		constexpr int disabledAlpha = 175;
	};

	
	//to avoid explicit in copy-initialization build issues on android create a set of empty groups for finding a group ID like this
	//this is to be used when getting a new groupID
	const gd::unordered_set<int> EMPTY_SET{};
}