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
	CMN_STATE_RUN,
	CMN_STATE_RUNBRAKE,
	CMN_STATE_PREJUMP,
	CMN_STATE_JUMPRISE,
	CMN_STATE_JUMPAPEX,
	CMN_STATE_JUMPFALL,
	CMN_STATE_LANDING,
	CMN_STATE_HIT_STAND_LVL0,
	CMN_STATE_HIT_STAND_LVL1,
	CMN_STATE_HIT_STAND_LVL2,
	CMN_STATE_HIT_CROUCH_LVL0,
	CMN_STATE_HIT_CROUCH_LVL1,
	CMN_STATE_HIT_CROUCH_LVL2,
	CMN_STATE_GUARD_STAND_LVL0,
	CMN_STATE_GUARD_STAND_LVL1,
	CMN_STATE_GUARD_STAND_LVL2,
	CMN_STATE_GUARD_CROUCH_LVL0,
	CMN_STATE_GUARD_CROUCH_LVL1,
	CMN_STATE_GUARD_CROUCH_LVL2,
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

#define CMN_STATE_SIG state_manager* StateManager, playerstate* PlayerState, state_script* Script, uint32 TimeStamp, bool ScriptFinished, float CurrentFacing
#define CMN_STATE_ARGS StateManager, PlayerState, Script, TimeStamp, ScriptFinished, CurrentFacing
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
CMN_STATE(CmnStateRun);
CMN_STATE(CmnStateRunBrake);
CMN_STATE(CmnStatePrejump);
CMN_STATE(CmnStateJumpRise);
CMN_STATE(CmnStateJumpApex);
CMN_STATE(CmnStateJumpFall);
CMN_STATE(CmnStateLanding);
CMN_STATE(CmnStateHitStand);
CMN_STATE(CmnStateHitCrouch);
CMN_STATE(CmnStateGuardStand);
CMN_STATE(CmnStateGuardCrouch);

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
	&CmnStateRun,
	&CmnStateRunBrake,
	&CmnStatePrejump,
	&CmnStateJumpRise,
	&CmnStateJumpApex,
	&CmnStateJumpFall,
	&CmnStateLanding,
	&CmnStateHitStand,
	&CmnStateHitStand,
	&CmnStateHitStand,
	&CmnStateHitCrouch,
	&CmnStateHitCrouch,
	&CmnStateHitCrouch,
	&CmnStateGuardStand,
	&CmnStateGuardStand,
	&CmnStateGuardStand,
	&CmnStateGuardCrouch,
	&CmnStateGuardCrouch,
	&CmnStateGuardCrouch
};

inline void CmnStateDefInit(state_script* Script, playerstate* PlayerState)
{
	PlayerState->PlaybackState.PlaybackCursor = 0;
	//PlayerState->BufferedJump = 0;
	PlayerState->DisableHitbox = false;
	PlayerState->Flags = 0;
	PlayerState->PlaybackState.BufferedState = -1;
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
	PlayerState->DisableHitbox = false;
	PlayerState->Flags = 0;
	PlayerState->PlaybackState.BufferedState = -1;
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
	PlayerState->DisableHitbox = false;
	PlayerState->Flags = 0;
	PlayerState->PlaybackState.BufferedState = -1;
	PlayerState->VelocityX = -StateManager->m_CharacterData.WalkBSpeed * PlayerState->Facing;
	PlayerState->VelocityY *= Script->ScalingYV;
	PlayerState->AccelerationX *= Script->ScalingXA;
	PlayerState->AccelerationY *= Script->ScalingYA;

	// TODO: Add run velocity and friction shit
}

inline bool CmnStateCancel(playerstate* PlayerState, state_manager* StateManager, uint32 TimeStamp, uint32 CancelListIndex)
{
	cancel_list* CancelList = StateManager->GetCancelList(CancelListIndex);
	for (uint32 i = 0; i < CancelList->MoveCount; i++)
	{
		move_description* MoveDescription = StateManager->GetMoveDescription(CancelList->Moves[i]);
		if (PlayerState->InputBuffer.MatchInputs(MoveDescription, TimeStamp, 3))
		{
			state_script* Script = StateManager->GetScript(MoveDescription->m_ScriptIndex);
			PlayerState->PlaybackState.State = MoveDescription->m_ScriptIndex;
			PlayerState->PlaybackState.PlaybackCursor = 0;
			PlayerState->DisableHitbox = false;
			PlayerState->Flags = 0;
			PlayerState->PlaybackState.BufferedState = -1;
			PlayerState->VelocityX *= Script->ScalingXV;
			PlayerState->VelocityY *= Script->ScalingYV;
			PlayerState->AccelerationX *= Script->ScalingXA;
			PlayerState->AccelerationY *= Script->ScalingYA;
			return true;
		}
	}
	return false;
}

CMN_STATE(CmnStateStand)
{
	float OldFacing = PlayerState->Facing;
	PlayerState->Facing = CurrentFacing;

	if (CmnStateCancel(PlayerState, StateManager, TimeStamp, 0))
		return;

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
		if (OldFacing != PlayerState->Facing)
			CMN_DEF_TRANSITION(CMN_STATE_STANDTURN);
		else if (ScriptFinished)
		{
			// TODO: Get loop point
			PlayerState->PlaybackState.PlaybackCursor = 0;
		}
	}

}

CMN_STATE(CmnStateStandTurn)
{
	if (CmnStateCancel(PlayerState, StateManager, TimeStamp, 0))
		return;

	if (ScriptFinished)
		CMN_DEF_TRANSITION(CMN_STATE_STAND);
}

CMN_STATE(CmnStateStand2Crouch)
{
	if (CmnStateCancel(PlayerState, StateManager, TimeStamp, 0))
		return;

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
		if (ScriptFinished)
			CMN_DEF_TRANSITION(CMN_STATE_CROUCH);
	}
	else
	{
		if (Script->Elements.StatusElements[0].InRange(PlayerState->PlaybackState.PlaybackCursor))
		{
			if (Script->Elements.StatusElements[0].StatusFlags & STATUS_CROUCHING)
				CMN_DEF_TRANSITION(CMN_STATE_CROUCH2STAND);
		}
		else
			CMN_DEF_TRANSITION(CMN_STATE_STAND);
	}
}

CMN_STATE(CmnStateCrouch)
{
	float OldFacing = PlayerState->Facing;
	PlayerState->Facing = CurrentFacing;
	
	if (CmnStateCancel(PlayerState, StateManager, TimeStamp, 0))
		return;

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
		if (OldFacing != PlayerState->Facing)
			CMN_DEF_TRANSITION(CMN_STATE_CROUCHTURN);
		else if (ScriptFinished)
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
	if (CmnStateCancel(PlayerState, StateManager, TimeStamp, 0))
		return;
	
	if (ScriptFinished)
		CMN_DEF_TRANSITION(CMN_STATE_CROUCH);
}

CMN_STATE(CmnStateCrouch2Stand)
{
	if (CmnStateCancel(PlayerState, StateManager, TimeStamp, 0))
		return;

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
		}
		else
			CMN_DEF_TRANSITION(CMN_STATE_STAND2CROUCH);
	}
	else
	{
		if (ScriptFinished)
			CMN_DEF_TRANSITION(CMN_STATE_STAND);
	}
}

CMN_STATE(CmnStateFWalk)
{
	if (CmnStateCancel(PlayerState, StateManager, TimeStamp, 0))
		return;

	PlayerState->Facing = CurrentFacing;
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
		if (ScriptFinished)
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
	if (CmnStateCancel(PlayerState, StateManager, TimeStamp, 0))
		return;

	PlayerState->Facing = CurrentFacing;
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
		if (ScriptFinished)
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

CMN_STATE(CmnStateRun)
{
	// Loop point
}

CMN_STATE(CmnStateRunBrake)
{

}

CMN_STATE(CmnStatePrejump)
{
	if (CmnStateCancel(PlayerState, StateManager, TimeStamp, 0))
		return;

	if (ScriptFinished)
	{
		if (PlayerState->BufferedJump == 8)
		{
			PlayerState->VelocityX = 0.0f;
		}
		else if (PlayerState->BufferedJump == 9)
		{
			PlayerState->VelocityX = StateManager->m_CharacterData.JumpVelocityX * PlayerState->Facing;
		}
		else
		{
			PlayerState->VelocityX = -StateManager->m_CharacterData.JumpVelocityX * PlayerState->Facing;
		}
		PlayerState->VelocityY = StateManager->m_CharacterData.JumpVelocityY;
		PlayerState->AccelerationX = 0.0f;
		PlayerState->AccelerationY = StateManager->m_CharacterData.JumpGravity;
		PlayerState->PlaybackState.State = CMN_STATE_JUMPRISE;
		PlayerState->PlaybackState.PlaybackCursor = 0;
		PlayerState->PlaybackState.BufferedState = -1;
		PlayerState->BufferedJump = 0;
		PlayerState->DisableHitbox = false;
		PlayerState->Flags = 0;
	}
}

CMN_STATE(CmnStateJumpRise)
{
	// Loop point
	if (ScriptFinished)
		PlayerState->PlaybackState.PlaybackCursor = 0;
	if (PlayerState->VelocityY < 0.5f)
	{
		PlayerState->PlaybackState.PlaybackCursor = 0;
		PlayerState->PlaybackState.State = CMN_STATE_JUMPAPEX;
	}
	if (PlayerState->PositionY > 80.0f)
	{
		if (!(PlayerState->Flags & PLAYER_USED_AIR_ACTION))
		{
			if (PlayerState->InputBuffer.MatchLastEntry(&CmnInputs[CMN_INPUT_UP]))
			{
				PlayerState->Flags |= PLAYER_USED_AIR_ACTION;
				if (PlayerState->InputBuffer.MatchLastEntry(&CmnInputs[CMN_INPUT_FORWARD_SIM]))
					PlayerState->VelocityX = StateManager->m_CharacterData.JumpVelocityX * PlayerState->Facing;
				else if (PlayerState->InputBuffer.MatchLastEntry(&CmnInputs[CMN_INPUT_BACK_SIM]))
					PlayerState->VelocityX = -StateManager->m_CharacterData.JumpVelocityX * PlayerState->Facing;
				else
					PlayerState->VelocityX = 0.0f;
				PlayerState->VelocityY = StateManager->m_CharacterData.JumpVelocityY / 2.0f;
				PlayerState->PlaybackState.State = CMN_STATE_JUMPRISE;
			}
		}
	}
}

CMN_STATE(CmnStateJumpApex)
{
	if (ScriptFinished)
	{
		PlayerState->PlaybackState.PlaybackCursor = 0;
		PlayerState->PlaybackState.State = CMN_STATE_JUMPFALL;
	}

	if (!(PlayerState->Flags & PLAYER_USED_AIR_ACTION))
	{
		if (PlayerState->InputBuffer.MatchLastEntry(&CmnInputs[CMN_INPUT_UP]))
		{
			PlayerState->Flags |= PLAYER_USED_AIR_ACTION;
			if (PlayerState->InputBuffer.MatchLastEntry(&CmnInputs[CMN_INPUT_FORWARD_SIM]))
				PlayerState->VelocityX = StateManager->m_CharacterData.JumpVelocityX * PlayerState->Facing;
			else if (PlayerState->InputBuffer.MatchLastEntry(&CmnInputs[CMN_INPUT_BACK_SIM]))
				PlayerState->VelocityX = -StateManager->m_CharacterData.JumpVelocityX * PlayerState->Facing;
			else
				PlayerState->VelocityX = 0.0f;
			PlayerState->VelocityY = StateManager->m_CharacterData.JumpVelocityY / 2.0f;
			PlayerState->PlaybackState.State = CMN_STATE_JUMPRISE;
		}
	}
}

CMN_STATE(CmnStateJumpFall)
{
	// TODO: Loop point
	if (ScriptFinished)
		PlayerState->PlaybackState.PlaybackCursor = 0;

	if (!(PlayerState->Flags & PLAYER_USED_AIR_ACTION))
	{
		if (PlayerState->InputBuffer.MatchLastEntry(&CmnInputs[CMN_INPUT_UP]))
		{
			PlayerState->Flags |= PLAYER_USED_AIR_ACTION;
			if (PlayerState->InputBuffer.MatchLastEntry(&CmnInputs[CMN_INPUT_FORWARD_SIM]))
				PlayerState->VelocityX = StateManager->m_CharacterData.JumpVelocityX * CurrentFacing;
			else if (PlayerState->InputBuffer.MatchLastEntry(&CmnInputs[CMN_INPUT_BACK_SIM]))
				PlayerState->VelocityX = -StateManager->m_CharacterData.JumpVelocityX * CurrentFacing;
			else
				PlayerState->VelocityX = 0.0f;
			PlayerState->VelocityY = StateManager->m_CharacterData.JumpVelocityY / 1.5f;
			PlayerState->PlaybackState.State = CMN_STATE_JUMPRISE;
		}
	}
}

CMN_STATE(CmnStateLanding)
{
	if (ScriptFinished)
	{
		CMN_DEF_TRANSITION(CMN_STATE_STAND);
		ScriptFinished = false;
		UpdateCmnState[CMN_STATE_STAND](CMN_STATE_ARGS);
	}
}

CMN_STATE(CmnStateHitStand)
{
	if (ScriptFinished)
		CMN_DEF_TRANSITION(CMN_STATE_STAND);
}

CMN_STATE(CmnStateHitCrouch)
{
	if (ScriptFinished)
		CMN_DEF_TRANSITION(CMN_STATE_CROUCH);
}

CMN_STATE(CmnStateGuardStand)
{
	if (ScriptFinished)
		CMN_DEF_TRANSITION(CMN_STATE_STAND);
}

CMN_STATE(CmnStateGuardCrouch)
{
	if (ScriptFinished)
		CMN_DEF_TRANSITION(CMN_STATE_CROUCH);
}