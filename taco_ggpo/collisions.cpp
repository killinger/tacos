#include "collisions.h"
#include <algorithm>

bool BoxIntersection(collision_box* Box0, collision_box* Box1, collision_box* Result)
{
	//float X0Min = std::min(Box0->X, Box0->X + Box0->Width);
	//float X0Max = std::max(Box0->X, Box0->X + Box0->Width);
	//float Y0Min = std::min(Box0->Y, Box0->Y + Box0->Height);
	//float Y0Max = std::max(Box0->Y, Box0->Y + Box0->Height);

	//float X1Min = std::min(Box1->X, Box1->X + Box1->Width);
	//float X1Max = std::max(Box1->X, Box1->X + Box1->Width);
	//float Y1Min = std::min(Box1->Y, Box1->Y + Box1->Height);
	//float Y1Max = std::max(Box1->Y, Box1->Y + Box1->Height);

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