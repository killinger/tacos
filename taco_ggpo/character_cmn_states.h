#pragma once
enum cmn_states
{
	CMN_STATE_STAND = 0,
	CMN_STATE_STANDTURN, 
	CMN_STATE_STAND2CROUCH, 
	CMN_STATE_CROUCH, 
	CMN_STATE_CROUCHTURN, 
	CMN_STATE_CROUCH2STAND, 
	CMN_STATE_FWALK, 
	CMN_STATE_BWALK,
	CMN_STATE_RUNSTART, 
	CMN_STATE_RUNLOOP,
	CMN_STATE_RUNBRAKE,
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
#define CMN_FWALK_TRANSITION(state) do { CmnStateFWalkInit(StateManager, Script, PlayerState); PlayerState->PlaybackState.State = state; } while(0)
#define CMN_BWALK_TRANSITION(state) do { CmnStateBWalkInit(StateManager, Script, PlayerState); PlayerState->PlaybackState.State = state; } while(0)

CMN_STATE(CmnStateStand);
CMN_STATE(CmnStateStandTurn);
CMN_STATE(CmnStateStand2Crouch);
CMN_STATE(CmnStateCrouch);
CMN_STATE(CmnStateCrouchTurn);
CMN_STATE(CmnStateCrouch2Stand);
CMN_STATE(CmnStateFWalk);
CMN_STATE(CmnStateBWalk);
CMN_STATE(CmnStateRunStart);
CMN_STATE(CmnStateRunLoop);
CMN_STATE(CmnStateRunBrake);
CMN_STATE(CmnStatePrejump);

CMN_STATE_RETURN_TYPE(*UpdateCmnState[CMN_STATE_COUNT])(CMN_STATE_SIG) =
{
	&CmnStateStand,
	&CmnStateStandTurn,
	&CmnStateStand2Crouch,
	&CmnStateCrouch,
	&CmnStateCrouchTurn,
	&CmnStateCrouch2Stand,
	&CmnStateFWalk,
	&CmnStateBWalk,
	&CmnStateRunStart,
	&CmnStateRunLoop,
	&CmnStateRunBrake,
	&CmnStatePrejump
};

inline void CmnStateDefInit(state_script* Script, playerstate* PlayerState)
{
	PlayerState->PlaybackState.PlaybackCursor = 0;
	PlayerState->BufferedJump = 0;
	PlayerState->VelocityX *= Script->ScalingXV;
	PlayerState->VelocityY *= Script->ScalingYV;
	PlayerState->AccelerationX *= Script->ScalingXA;
	PlayerState->AccelerationY *= Script->ScalingYA;

	// TODO: Add run velocity and friction shit
}

inline void CmnStateFWalkInit(state_manager* StateManager, state_script* Script, playerstate* PlayerState)
{
	PlayerState->PlaybackState.PlaybackCursor = 0;
	PlayerState->BufferedJump = 0;
	PlayerState->VelocityX = StateManager->m_CharacterData.WalkFSpeed * PlayerState->Facing;
	PlayerState->VelocityY *= Script->ScalingYV;
	PlayerState->AccelerationX *= Script->ScalingXA;
	PlayerState->AccelerationY *= Script->ScalingYA;

	// TODO: Add run velocity and friction shit
}

inline void CmnStateBWalkInit(state_manager* StateManager, state_script* Script, playerstate* PlayerState)
{
	PlayerState->PlaybackState.PlaybackCursor = 0;
	PlayerState->BufferedJump = 0;
	PlayerState->VelocityX = -StateManager->m_CharacterData.WalkBSpeed * PlayerState->Facing;
	PlayerState->VelocityY *= Script->ScalingYV;
	PlayerState->AccelerationX *= Script->ScalingXA;
	PlayerState->AccelerationY *= Script->ScalingYA;

	// TODO: Add run velocity and friction shit
}

CMN_STATE(CmnStateStand)
{
	state_script* Script = StateManager->GetScript(PlayerState->PlaybackState.State);

	// TODO: Needs some testing to figure when to actually apply this
	float NewFacing = 1.0f;
	if (PlayerState->PositionX > OtherPlayer->PositionX)
		NewFacing = -1.0f;

	// TODO: Check cancel list first

	if (PlayerState->InputBuffer.MatchLastEntry(&CmnInputs[CMN_INPUT_UP]))
	{
		if (PlayerState->InputBuffer.MatchLastEntry(&CmnInputs[CMN_INPUT_FORWARD_SIM]))
			PlayerState->BufferedJump = 9;
		else if (PlayerState->InputBuffer.MatchLastEntry(&CmnInputs[CMN_INPUT_BACK_SIM]))
			PlayerState->BufferedJump = 7;
		else
			PlayerState->BufferedJump = 8;
		
		CMN_DEF_TRANSITION(CMN_STATE_PREJUMP);
	}
	else if (PlayerState->InputBuffer.MatchLastEntry(&CmnInputs[CMN_INPUT_FORWARD_EX]))
	{
		CMN_FWALK_TRANSITION(CMN_STATE_FWALK);
	}
	else if (PlayerState->InputBuffer.MatchLastEntry(&CmnInputs[CMN_INPUT_BACK_EX]))
	{
		CMN_BWALK_TRANSITION(CMN_STATE_BWALK);
	}
	else if (PlayerState->InputBuffer.MatchLastEntry(&CmnInputs[CMN_INPUT_DOWN]))
	{
		CMN_DEF_TRANSITION(CMN_STATE_STAND2CROUCH);
	}
	else
	{
		if (NewFacing != PlayerState->Facing)
			CMN_DEF_TRANSITION(CMN_STATE_STANDTURN);
		else if (++PlayerState->PlaybackState.PlaybackCursor >= Script->TotalFrames)
		{
			// TODO: Get loop point
			PlayerState->PlaybackState.PlaybackCursor = 0;
		}
	}
}

CMN_STATE(CmnStateStandTurn)
{
	state_script* Script = StateManager->GetScript(PlayerState->PlaybackState.State);
}

CMN_STATE(CmnStateStand2Crouch)
{
	state_script* Script = StateManager->GetScript(PlayerState->PlaybackState.State);

	if (PlayerState->InputBuffer.MatchLastEntry(&CmnInputs[CMN_INPUT_UP]))
	{
		if (PlayerState->InputBuffer.MatchLastEntry(&CmnInputs[CMN_INPUT_FORWARD_SIM]))
			PlayerState->BufferedJump = 9;
		else if (PlayerState->InputBuffer.MatchLastEntry(&CmnInputs[CMN_INPUT_BACK_SIM]))
			PlayerState->BufferedJump = 7;
		else
			PlayerState->BufferedJump = 8;
		CMN_DEF_TRANSITION(CMN_STATE_PREJUMP);
	}
	else if (PlayerState->InputBuffer.MatchLastEntry(&CmnInputs[CMN_INPUT_FORWARD_EX]))
		CMN_FWALK_TRANSITION(CMN_STATE_FWALK);
	else if (PlayerState->InputBuffer.MatchLastEntry(&CmnInputs[CMN_INPUT_BACK_EX]))
		CMN_BWALK_TRANSITION(CMN_STATE_BWALK);
	else if (PlayerState->InputBuffer.MatchLastEntry(&CmnInputs[CMN_INPUT_DOWN]))
	{
		if (++PlayerState->PlaybackState.PlaybackCursor >= Script->TotalFrames)
			CMN_DEF_TRANSITION(CMN_STATE_CROUCH);
	}
	else
	{
		if (Script->Elements.StatusElements[0].InRange(PlayerState->PlaybackState.PlaybackCursor))
		{
			if (Script->Elements.StatusElements[0].StatusFlags & STATUS_CROUCHING)
				CMN_DEF_TRANSITION(CMN_STATE_CROUCH2STAND);
			else
				CMN_DEF_TRANSITION(CMN_STATE_STAND);
		}
	}
}

CMN_STATE(CmnStateCrouch)
{
	state_script* Script = StateManager->GetScript(PlayerState->PlaybackState.State);
	
	// TODO: Needs some testing to figure when to actually apply this
	float NewFacing = 1.0f;
	if (PlayerState->PositionX > OtherPlayer->PositionX)
		NewFacing = -1.0f;

	if (PlayerState->InputBuffer.MatchLastEntry(&CmnInputs[CMN_INPUT_UP]))
	{
		if (PlayerState->InputBuffer.MatchLastEntry(&CmnInputs[CMN_INPUT_FORWARD_SIM]))
			PlayerState->BufferedJump = 9;
		else if (PlayerState->InputBuffer.MatchLastEntry(&CmnInputs[CMN_INPUT_BACK_SIM]))
			PlayerState->BufferedJump = 7;
		else
			PlayerState->BufferedJump = 8;
		CMN_DEF_TRANSITION(CMN_STATE_PREJUMP);
	}
	else if (PlayerState->InputBuffer.MatchLastEntry(&CmnInputs[CMN_INPUT_FORWARD_EX]))
		CMN_FWALK_TRANSITION(CMN_STATE_FWALK);
	else if (PlayerState->InputBuffer.MatchLastEntry(&CmnInputs[CMN_INPUT_BACK_EX]))
		CMN_BWALK_TRANSITION(CMN_STATE_BWALK);
	else if (PlayerState->InputBuffer.MatchLastEntry(&CmnInputs[CMN_INPUT_DOWN]))
	{
		if (NewFacing != PlayerState->Facing)
			CMN_DEF_TRANSITION(CMN_STATE_CROUCHTURN);
		else if (++PlayerState->PlaybackState.PlaybackCursor >= Script->TotalFrames)
		{
			// TODO: Get loop point
			PlayerState->PlaybackState.PlaybackCursor = 0;
		}
	}
	else
		CMN_DEF_TRANSITION(CMN_STATE_CROUCH2STAND);
}

CMN_STATE(CmnStateCrouchTurn)
{
	state_script* Script = StateManager->GetScript(PlayerState->PlaybackState.State);
}

CMN_STATE(CmnStateCrouch2Stand)
{
	state_script* Script = StateManager->GetScript(PlayerState->PlaybackState.State);

	if (PlayerState->InputBuffer.MatchLastEntry(&CmnInputs[CMN_INPUT_UP]))
	{
		if (PlayerState->InputBuffer.MatchLastEntry(&CmnInputs[CMN_INPUT_FORWARD_SIM]))
			PlayerState->BufferedJump = 9;
		else if (PlayerState->InputBuffer.MatchLastEntry(&CmnInputs[CMN_INPUT_BACK_SIM]))
			PlayerState->BufferedJump = 7;
		else
			PlayerState->BufferedJump = 8;
		CMN_DEF_TRANSITION(CMN_STATE_PREJUMP);
	}
	else if (PlayerState->InputBuffer.MatchLastEntry(&CmnInputs[CMN_INPUT_FORWARD_EX]))
		CMN_FWALK_TRANSITION(CMN_STATE_FWALK);
	else if (PlayerState->InputBuffer.MatchLastEntry(&CmnInputs[CMN_INPUT_BACK_EX]))
		CMN_BWALK_TRANSITION(CMN_STATE_BWALK);
	else if (PlayerState->InputBuffer.MatchLastEntry(&CmnInputs[CMN_INPUT_DOWN]))
	{
		if (Script->Elements.StatusElements[0].InRange(PlayerState->PlaybackState.PlaybackCursor))
		{
			if (Script->Elements.StatusElements[0].StatusFlags & STATUS_CROUCHING)
				CMN_DEF_TRANSITION(CMN_STATE_CROUCH);
			else
				CMN_DEF_TRANSITION(CMN_STATE_STAND2CROUCH);
		}
	}
	else
	{
		if (++PlayerState->PlaybackState.PlaybackCursor >= Script->TotalFrames)
			CMN_DEF_TRANSITION(CMN_STATE_STAND);
	}
}

CMN_STATE(CmnStateFWalk)
{
	state_script* Script = StateManager->GetScript(PlayerState->PlaybackState.State);

	// TODO: Needs some testing to figure out if the turn state should happen
	if (PlayerState->PositionX > OtherPlayer->PositionX)
		PlayerState->Facing = -1.0f;
	else
		PlayerState->Facing = 1.0f;

	// TODO: Check cancel list first

	if (PlayerState->InputBuffer.MatchLastEntry(&CmnInputs[CMN_INPUT_UP]))
	{
		if (PlayerState->InputBuffer.MatchLastEntry(&CmnInputs[CMN_INPUT_FORWARD_SIM]))
			PlayerState->BufferedJump = 9;
		else if (PlayerState->InputBuffer.MatchLastEntry(&CmnInputs[CMN_INPUT_BACK_SIM]))
			PlayerState->BufferedJump = 7;
		else
			PlayerState->BufferedJump = 8;
		CMN_DEF_TRANSITION(CMN_STATE_PREJUMP);
	}
	else if (PlayerState->InputBuffer.MatchLastEntry(&CmnInputs[CMN_INPUT_FORWARD_EX]))
	{
		if (++PlayerState->PlaybackState.PlaybackCursor >= Script->TotalFrames)
		{
			// TODO: Get loop point
			PlayerState->PlaybackState.PlaybackCursor = 0;
		}
	}
	else if (PlayerState->InputBuffer.MatchLastEntry(&CmnInputs[CMN_INPUT_BACK_EX]))
		CMN_BWALK_TRANSITION(CMN_STATE_BWALK);
	else if (PlayerState->InputBuffer.MatchLastEntry(&CmnInputs[CMN_INPUT_DOWN]))
		CMN_DEF_TRANSITION(CMN_STATE_STAND2CROUCH);
	else
		CMN_DEF_TRANSITION(CMN_STATE_STAND);
}

CMN_STATE(CmnStateBWalk)
{
	state_script* Script = StateManager->GetScript(PlayerState->PlaybackState.State);

	if (PlayerState->PositionX > OtherPlayer->PositionX)
		PlayerState->Facing = -1.0f;
	else
		PlayerState->Facing = 1.0f;

	// TODO: Check cancel list first

	if (PlayerState->InputBuffer.MatchLastEntry(&CmnInputs[CMN_INPUT_UP]))
	{
		if (PlayerState->InputBuffer.MatchLastEntry(&CmnInputs[CMN_INPUT_FORWARD_SIM]))
			PlayerState->BufferedJump = 9;
		else if (PlayerState->InputBuffer.MatchLastEntry(&CmnInputs[CMN_INPUT_BACK_SIM]))
			PlayerState->BufferedJump = 7;
		else
			PlayerState->BufferedJump = 8;
		CMN_DEF_TRANSITION(CMN_STATE_PREJUMP);
	}
	else if (PlayerState->InputBuffer.MatchLastEntry(&CmnInputs[CMN_INPUT_FORWARD_EX]))
		CMN_FWALK_TRANSITION(CMN_STATE_FWALK);
	else if (PlayerState->InputBuffer.MatchLastEntry(&CmnInputs[CMN_INPUT_BACK_EX]))
	{
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

CMN_STATE(CmnStateRunStart)
{
	state_script* Script = StateManager->GetScript(PlayerState->PlaybackState.State);
}

CMN_STATE(CmnStateRunLoop)
{
	state_script* Script = StateManager->GetScript(PlayerState->PlaybackState.State);
}

CMN_STATE(CmnStateRunBrake)
{
	state_script* Script = StateManager->GetScript(PlayerState->PlaybackState.State);
}

CMN_STATE(CmnStatePrejump)
{
	state_script* Script = StateManager->GetScript(PlayerState->PlaybackState.State);
}