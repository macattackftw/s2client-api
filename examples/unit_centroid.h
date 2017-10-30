#ifndef UNIT_CENTROID_H
#define UNIT_CENTROID_H

#include "sc2api/sc2_api.h"
#include "sc2api/sc2_common.h"

sc2::Point2D GetUnitCentroid(const sc2::Units &units)
{
	sc2::Point2D ret_val;
	for (auto *u : units)
	{
		ret_val.x += u->pos.x;
		ret_val.y += u->pos.y;
	}

	ret_val.x /= units.size();
	ret_val.y /= units.size();

	return ret_val;
}

#endif