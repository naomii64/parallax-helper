#pragma once

#include <Geode/Geode.hpp>
using namespace geode::prelude;

namespace ph::constants{
	namespace ui {
		constexpr ccColor3B DISABLED_COLOR = ccColor3B{166,166,166};
		constexpr int DISABLED_ALPHA = 175;

		constexpr float PADDING = 10.0f;
	};

	//strings for accessing settings and saved values
	namespace keystrings {
		constexpr std::string_view SAVED_SELECTED_SORTING_ID = "selected-sorting-id";

		// constexpr std::string_view SETTING_ROOT_OBJ_SCALE = "float-root-object-scale";
		// constexpr std::string_view SETTING_FOLLOW_OBJ_SCALE = "float-follow-object-scale";
		
		// constexpr std::string_view SETTING_ROOT_OBJ_ID = "int-root-object-ID";
		// constexpr std::string_view SETTING_FOLLOW_OBJ_ID = "int-follow-object-ID";
		
		// constexpr std::string_view SETTING_DEPTH_LABEL_COLOR_DEFAULT = "depth-label-color-default";
		// constexpr std::string_view SETTING_DEPTH_LABEL_COLOR_POSITIVE = "depth-label-color-positive";
		// constexpr std::string_view SETTING_DEPTH_LABEL_COLOR_NEGATIVE = "depth-label-color-negative";

		// constexpr std::string_view SETTING_KEYBIND_LAYERLIST_UP = "keybind-layerlist-up";
		// constexpr std::string_view SETTING_KEYBIND_LAYERLIST_DOWN = "keybind-layerlist-down";
	}

	//to avoid explicit in copy-initialization build issues on android create a set of empty groups for finding a group ID like this
	//this is to be used when getting a new groupID
	const gd::unordered_set<int> EMPTY_SET{};
}
using namespace ph;