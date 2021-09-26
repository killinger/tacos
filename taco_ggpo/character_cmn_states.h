#pragma once
enum cmn_states
{
	CMN_STATE_STAND = 0,
	CMN_STATE_CROUCH,
	CMN_STATE_FWALK,
	CMN_STATE_BWALK,
	CMN_STATE_STAND2CROUCH,
	CMN_STATE_CROUCH2STAND,
	CMN_STATE_PREJUMP,
	CMN_STATE_COUNT
};

enum cmn_inputs
{
	CMN_INPUT_FORWARD_EX = 0,
	CMN_INPUT_FORWARD_SIM,
	CMN_INPUT_BACK_EX,
	CMN_INPUT_BACK_SIM,
	CMN_INPUT_DOWN,
	CMN_INPUT_UP
};

input_description CmnInputs[]
{
	{ INPUT_FORWARD, DIRECTION_RESTRICTION_EXACT },
	{ INPUT_FORWARD, DIRECTION_RESTRICTION_SIMILAR },
	{ INPUT_BACK, DIRECTION_RESTRICTION_EXACT },
	{ INPUT_BACK, DIRECTION_RESTRICTION_SIMILAR },
	{ INPUT_DOWN, DIRECTION_RESTRICTION_SIMILAR },
	{ INPUT_UP, DIRECTION_RESTRICTION_SIMILAR }
};

#define CMN_STATE_SIG state_manager* StateManager, playerstate* PlayerState, playerstate* OtherPlayer
#define CMN_STATE_RETURN_TYPE void
#define CMN_STATE(state) CMN_STATE_RETURN_TYPE state(CMN_STATE_SIG)
#define CMN_DEF_TRANSITION(state) do { CmnStateDefInit(Script, PlayerState); PlayerState->PlaybackState.State = state; } while(0)

CMN_STATE(CmnStateStand);
CMN_STATE(CmnStateFWalk);
CMN_STATE(CmnStateBWalk);
CMN_STATE(CmnStateCrouch);
CMN_STATE(CmnStatePrejump);

CMN_STATE_RETURN_TYPE(*UpdateCmnState[CMN_STATE_COUNT])(CMN_STATE_SIG) =
{
	&CmnStateStand,
	&CmnStateFWalk,
	&CmnStateBWalk,
};

inline void CmnStateDefInit(state_script* Script, playerstate* PlayerState)
{
	PlayerState->PlaybackState.PlaybackCursor = 0;
	PlayerState->BufferedJump = 0;
	PlayerState->VelocityX *= Script->ScalingXV;
	PlayerState->VelocityY *= Script->ScalingYV;
	PlayerState->AccelerationX *= Script->ScalingXA;
	PlayerState->AccelerationY *= Script->ScalingYA;

	// TODO: Add run shit
}

CMN_STATE(CmnStateStand)
{
	state_script* Script = StateManager->GetScript(PlayerState->PlaybackState.State);

	// Check cancel list first
	// TODO: Update facing

	if (PlayerState->InputBuffer.MatchLastEntry(&CmnInputs[CMN_INPUT_UP]))
	{
		CMN_DEF_TRANSITION(CMN_STATE_PREJUMP);

		if (PlayerState->InputBuffer.MatchLastEntry(&CmnInputs[CMN_INPUT_FORWARD_SIM]))
			PlayerState->BufferedJump = 9;
		else if (PlayerState->InputBuffer.MatchLastEntry(&CmnInputs[CMN_INPUT_BACK_SIM]))
			PlayerState->BufferedJump = 7;
		else
			PlayerState->BufferedJump = 8;
	}
	else if (PlayerState->InputBuffer.MatchLastEntry(&CmnInputs[CMN_INPUT_FORWARD_EX]))
		//TODO: Apply walkspeed to velocity
		CMN_DEF_TRANSITION(CMN_STATE_FWALK);
	else if (PlayerState->InputBuffer.MatchLastEntry(&CmnInputs[CMN_INPUT_BACK_EX]))
		//TODO: Apply walkspeed to velocity
		CMN_DEF_TRANSITION(CMN_STATE_BWALK);
	else if (PlayerState->InputBuffer.MatchLastEntry(&CmnInputs[CMN_INPUT_DOWN]))
		CMN_DEF_TRANSITION(CMN_STATE_STAND2CROUCH);
	else
	{
		if (++PlayerState->PlaybackState.PlaybackCursor >= Script->TotalFrames)
		{
			// TODO: Get loop point
			PlayerState->PlaybackState.PlaybackCursor = 0;
		}
	}
}

CMN_STATE(CmnStateFWalk)
{
	state_script* Script = StateManager->GetScript(PlayerState->PlaybackState.State);

	// Check cancel list first
	// TODO: Update facing

	if (PlayerState->InputBuffer.MatchLastEntry(&CmnInputs[CMN_INPUT_UP]))
	{
		CMN_DEF_TRANSITION(CMN_STATE_PREJUMP);

		if (PlayerState->InputBuffer.MatchLastEntry(&CmnInputs[CMN_INPUT_FORWARD_SIM]))
			PlayerState->BufferedJump = 9;
		else if (PlayerState->InputBuffer.MatchLastEntry(&CmnInputs[CMN_INPUT_BACK_SIM]))
			PlayerState->BufferedJump = 7;
		else
			PlayerState->BufferedJump = 8;
	}
	else if (PlayerState->InputBuffer.MatchLastEntry(&CmnInputs[CMN_INPUT_FORWARD_EX]))
	{
		//TODO: Apply walkspeed to velocity
		if (++PlayerState->PlaybackState.PlaybackCursor >= Script->TotalFrames)
		{
			// TODO: Get loop point
			PlayerState->PlaybackState.PlaybackCursor = 0;
		}
	}
	else if (PlayerState->InputBuffer.MatchLastEntry(&CmnInputs[CMN_INPUT_BACK_EX]))
		//TODO: Apply walkspeed to velocity
		CMN_DEF_TRANSITION(CMN_STATE_BWALK);
	else if (PlayerState->InputBuffer.MatchLastEntry(&CmnInputs[CMN_INPUT_DOWN]))
		CMN_DEF_TRANSITION(CMN_STATE_STAND2CROUCH);
	else
		CMN_DEF_TRANSITION(CMN_STATE_STAND);
}

CMN_STATE(CmnStateBWalk)
{
	state_script* Script = StateManager->GetScript(PlayerState->PlaybackState.State);

	// Check cancel list first
	// TODO: Update facing

	if (PlayerState->InputBuffer.MatchLastEntry(&CmnInputs[CMN_INPUT_UP]))
	{
		CMN_DEF_TRANSITION(CMN_STATE_PREJUMP);

		if (PlayerState->InputBuffer.MatchLastEntry(&CmnInputs[CMN_INPUT_FORWARD_SIM]))
			PlayerState->BufferedJump = 9;
		else if (PlayerState->InputBuffer.MatchLastEntry(&CmnInputs[CMN_INPUT_BACK_SIM]))
			PlayerState->BufferedJump = 7;
		else
			PlayerState->BufferedJump = 8;
	}
	else if (PlayerState->InputBuffer.MatchLastEntry(&CmnInputs[CMN_INPUT_FORWARD_EX]))
		//TODO: Apply walkspeed to velocity
		CMN_DEF_TRANSITION(CMN_STATE_FWALK);
	else if (PlayerState->InputBuffer.MatchLastEntry(&CmnInputs[CMN_INPUT_BACK_EX]))
	{
		//TODO: Apply walkspeed to velocity
		if (++PlayerState->PlaybackState.PlaybackCursor >= Script->TotalFrames)
		{
			// TODO: Get loop point
			PlayerState->PlaybackState.PlaybackCursor = 0;
		}
	}
	else if (PlayerState->InputBuffer.MatchLastEntry(&CmnInputs[CMN_INPUT_DOWN]))
		CMN_DEF_TRANSITION(CMN_STATE_STAND2CROUCH);
	else
		CMN_DEF_TRANSITION(CMN_STATE_STAND);
}