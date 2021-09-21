#include "gamestate.h"

void gamestate::Update(permanent_state* PermanentState, uint32* Inputs)
{
	PermanentState->Player[0].InputBuffer.Update(Inputs[0], Player[0].Facing);
	PermanentState->Player[1].InputBuffer.Update(Inputs[1], Player[1].Facing);
}