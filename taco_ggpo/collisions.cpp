#include "collisions.h"
#include <algorithm>

bool BoxIntersection(collision_box* Box0, collision_box* Box1, collision_box* Result)
{
	float XMin = std::max(Box0->X, Box1->X);
	float XMax = std::min(Box0->X + Box0->Width, Box1->X + Box1->Width);
	float YMin = std::max(Box0->Y, Box1->Y);
	float YMax = std::min(Box0->Y + Box0->Height, Box1->Y + Box1->Height);

	if (XMin < XMax && YMin < YMax)
	{
		Result->X = XMin;
		Result->Y = YMin;
		Result->Width = XMax - XMin;
		Result->Height = YMax - YMin;
		return true;
	}
		
	return false;
}