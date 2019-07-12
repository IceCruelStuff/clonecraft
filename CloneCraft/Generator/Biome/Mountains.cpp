#include "Mountains.h"

#include "World/WorldConstants.h"
#include "Util/Logger.h"

int Mountains::getHeight(ivec2 pos) const {
	double noise = perlin.getNoise(static_cast<dvec2>(pos));
	return Const::SEA_LEVEL + 40 + static_cast<int>(noise * 16);
}

Block Mountains::getBlock(int y, int height) const {
	if (y <= height - 1)
		return { BlockID::STONE };
	if (y < Const::SEA_LEVEL)
		return { BlockID::WATER };

	return { BlockID::AIR };
}

std::vector<StructureInfo> Mountains::getStructures() const {
	return { { StructureID::TREE, 0.005f } };
}

double Mountains::biomeValue(double temperature, double humidity) const {
	return medium(temperature) * low(humidity);
}