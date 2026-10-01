#pragma once

#include <Geode/Geode.hpp>
using namespace geode::prelude;

#include <nwo5.silly-api/include/include.hpp>
using namespace nwo5::editor::prelude;

namespace ph::settings{
    inline nwo5::settings::Setting<ccColor3B> depthLabelColorDefault{"depth-label-color-default"};
    inline nwo5::settings::Setting<ccColor3B> depthLabelColorPositive{"depth-label-color-positive"};
    inline nwo5::settings::Setting<ccColor3B> depthLabelColorNegative{"depth-label-color-negative"};

    inline nwo5::settings::Setting<float> rootObjectScale{"float-root-object-scale"};
	inline nwo5::settings::Setting<float> followObjectScale{"float-follow-object-scale"};
		
	inline nwo5::settings::Setting<int> rootObjectID{"int-root-object-ID"};
	inline nwo5::settings::Setting<int> followObjectID{"int-follow-object-ID"};

    inline nwo5::settings::Setting<std::vector<geode::Keybind>> keyLayerListUp{"keybind-layerlist-up"};
    inline nwo5::settings::Setting<std::vector<geode::Keybind>> keyLayerListDown{"keybind-layerlist-down"};
};