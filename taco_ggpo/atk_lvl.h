#pragma once

struct atk_lvl_defaults
{
	float NormalKnockback;
	uint8 NormalHitstopAttacker;
	uint8 NormalHitstopDefender;
	uint8 Hitstun;
};

namespace atk_lvl
{
	atk_lvl_defaults Defaults[2] =
	{
		{	8.0f, 8, 8,	10 },
		{	15.0f, 10, 10, 12 }
	};
}