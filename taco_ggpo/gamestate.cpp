#include "gamestate.h"
#include "character_cmn_states.h"
#include "logging_system.h"
#include "gamestate_buffer.h"
#include "render_system.h"
#include "subsystems.h"

#define PLAYER_STARTING_POSITIONS 80.0f

void gamestate::Initialize()
{
	memset(&m_Player[0], 0, sizeof(playerstate));
	memset(&m_Player[1], 0, sizeof(playerstate));
	m_Player[0].PositionX = -PLAYER_STARTING_POSITIONS;
	m_Player[0].Facing = 1.0f;
	m_Player[0].PlaybackState.State = CMN_STATE_STAND;
	m_Player[0].PlaybackState.New = true;
	m_Player[0].PlaybackState.BufferedState = -1;
	m_Player[1].PositionX = PLAYER_STARTING_POSITIONS;
	m_Player[1].Facing = -1.0f;
	m_Player[1].PlaybackState.State = CMN_STATE_STAND;
	m_Player[1].PlaybackState.New = true;
	m_Player[1].PlaybackState.BufferedState = -1;
}

void gamestate::Update(uint32* Inputs, state_manager* StateManager)
{
	if (!GameStateBuffer->IsReplaying())
	{
		bool FlipDirections[] = {	m_Player[0].PositionX > m_Player[1].PositionX,
									m_Player[1].PositionX > m_Player[0].PositionX };
		m_Player[0].InputBuffer.Update(Inputs[0], m_FrameCount, FlipDirections[0]);
		m_Player[1].InputBuffer.Update(Inputs[1], m_FrameCount, FlipDirections[1]);
	}
	GameStateBuffer->Update();

	if (m_Player[0].Hitstop)
		m_Player[0].Hitstop--;
	if (m_Player[1].Hitstop)
		m_Player[1].Hitstop--;

	// Transition stage
	state_script* Script[2] = {	AdvancePlayerState(StateManager, &m_Player[0], &m_Player[1]),
								AdvancePlayerState(StateManager, &m_Player[1], &m_Player[0]) };
	// Movement and correction stage
	if (!m_Player[0].Hitstop)
		UpdateMovement(&m_Player[0]);
	if (!m_Player[1].Hitstop)
		UpdateMovement(&m_Player[1]);
	CorrectAndFinalizePositions(Script);

	// Hit detection
	HitDetection(StateManager, 0, 1, Script[0], Script[1]);
	HitDetection(StateManager, 1, 0, Script[1], Script[0]);
}

state_script* gamestate::AdvancePlayerState(state_manager* StateManager, playerstate* PlayerState, playerstate* OtherPlayer)
{
	state_script* Script = StateManager->GetScript(PlayerState->PlaybackState.State);
	bool ScriptFinished = false;
	float CurrentFacing = 1.0f;

	if (!PlayerState->Hitstop)
	{
		if (!PlayerState->PlaybackState.New)
			if (++PlayerState->PlaybackState.PlaybackCursor >= Script->TotalFrames)
				ScriptFinished = true;
		PlayerState->PlaybackState.New = false;
	}

	// TODO: Needs some testing to figure when to actually apply this
	if (PlayerState->PositionX > OtherPlayer->PositionX)
		CurrentFacing = -1.0f;
	if (PlayerState->PositionY < 0.0f)
	{
		// TODO: KD states
		PlayerState->PositionY = 0.0f;
		CMN_DEF_TRANSITION(CMN_STATE_LANDING);
	}
	// TODO: Perform cancel shit here? instead of cmn state?
	else if (PlayerState->PlaybackState.State < CMN_STATE_COUNT)
		UpdateCmnState[PlayerState->PlaybackState.State](	StateManager, 
															PlayerState, 
															Script,
															m_FrameCount,
															ScriptFinished, 
															CurrentFacing);
	else
	{
		for (uint32 i = 0; i < Script->Elements.CancelCount; i++)
		{
			if (Script->Elements.CancelElements[i].InRange(PlayerState->PlaybackState.PlaybackCursor))
			{
				if (Script->Elements.CancelElements[i].Flags & CANCEL_BUFFER_HIT)
				{
					cancel_list* CancelList = StateManager->GetCancelList(Script->Elements.CancelElements[i].Index);
					for (uint32 j = 0; j < CancelList->MoveCount; j++)
					{
						move_description* MoveDescription = StateManager->GetMoveDescription(CancelList->Moves[j]);
						if (PlayerState->InputBuffer.MatchInputs(MoveDescription, m_FrameCount, 3))
						{
							PlayerState->PlaybackState.BufferedState = MoveDescription->m_ScriptIndex;
							break;
						}
					}
				}
				if (!PlayerState->Hitstop)
				{
					if ((Script->Elements.CancelElements[i].Flags & CANCEL_EXECUTE) &&
						(PlayerState->Flags & PLAYER_ALLOW_CANCEL) &&
						(PlayerState->PlaybackState.BufferedState != -1))
					{
						PlayerState->PlaybackState.State = (uint32)PlayerState->PlaybackState.BufferedState;
						CmnStateDefInit(Script, PlayerState);
						break;
					}
				}
			}
		}
		
		if (ScriptFinished)
		{
			// TODO: Air recovery
			if (Script->Flags & SCRIPT_ENDS_CROUCHING)
			{
				CmnStateDefInit(Script, PlayerState);
				PlayerState->PlaybackState.State = CMN_STATE_CROUCH;
				CmnStateCrouch(	StateManager,
								PlayerState,
								Script,
								m_FrameCount,
								ScriptFinished,
								CurrentFacing);
			}
			else
			{
				CmnStateDefInit(Script, PlayerState);
				PlayerState->PlaybackState.State = CMN_STATE_STAND;
				CmnStateStand(	StateManager,
								PlayerState,
								Script,
								m_FrameCount,
								ScriptFinished,
								CurrentFacing);
			}
		}
	}
	return StateManager->GetScript(PlayerState->PlaybackState.State);
}

void gamestate::UpdateMovement(playerstate* PlayerState)
{
	// TODO: Run shit
	PlayerState->PositionX += PlayerState->VelocityX;
	PlayerState->PositionY += PlayerState->VelocityY;
	PlayerState->VelocityX += PlayerState->AccelerationX;
	PlayerState->VelocityY += PlayerState->AccelerationY;
}

void gamestate::CorrectAndFinalizePositions(state_script* Script[2])
{
	collision_box Pushbox[2], Result;
	for (uint32 i = 0; i < 2; i++)
	{
		for (uint32 j = 0; j < Script[i]->Elements.PushboxCount; j++)
		{
			if (Script[i]->Elements.PushboxElements[j].InRange(m_Player[i].PlaybackState.PlaybackCursor))
			{
				if (m_Player[i].Facing > 0.0f)
				{
					Pushbox[i].X = Script[i]->Elements.PushboxElements[j].Box.X + m_Player[i].PositionX;
					Pushbox[i].Y = Script[i]->Elements.PushboxElements[j].Box.Y + m_Player[i].PositionY;
					Pushbox[i].Width = Script[i]->Elements.PushboxElements[j].Box.Width;
					Pushbox[i].Height = Script[i]->Elements.PushboxElements[j].Box.Height;
				}
				else
				{
					Pushbox[i].X = m_Player[i].PositionX - (Script[i]->Elements.PushboxElements[j].Box.X + Script[i]->Elements.PushboxElements[j].Box.Width);
					Pushbox[i].Y = Script[i]->Elements.PushboxElements[j].Box.Y + m_Player[i].PositionY;
					Pushbox[i].Width = Script[i]->Elements.PushboxElements[j].Box.Width;
					Pushbox[i].Height = Script[i]->Elements.PushboxElements[j].Box.Height;
				}
				break;
			}
		}
	}

	// If neither player is in hitstun
	// TODO: Other cases, corner and pushback
	if (BoxIntersection(&Pushbox[0], &Pushbox[1], &Result))
	{
		float Repulsion = Result.Width / 2.0f;
		float P0RepulsionDir = 1.0f;
		float P1RepulsionDir = 1.0f;
		if (m_Player[0].PositionX > m_Player[1].PositionX)
		{
			P1RepulsionDir = -1.0f;
		}
		else if (m_Player[1].PositionX > m_Player[0].PositionX)
		{
			P0RepulsionDir = -1.0f;
		}
		else
		{
			// TODO: I don't know
		}
		m_Player[0].PositionX += P0RepulsionDir * Repulsion;
		m_Player[1].PositionX += P1RepulsionDir * Repulsion;
	}
}

void gamestate::HitDetection(state_manager* StateManager, uint32 PlayerIndex, uint32 OtherIndex, state_script* PlayerScript, state_script* OtherScript)
{
	if (!m_Player[PlayerIndex].DisableHitbox)
	{
		collision_box Hitbox, Hurtbox;
		for (uint32 i = 0; i < PlayerScript->Elements.HitboxCount; i++)
		{
			if (PlayerScript->Elements.HitboxElements->InRange(m_Player[PlayerIndex].PlaybackState.PlaybackCursor))
			{
				if (m_Player[PlayerIndex].Facing > 0.0f)
				{
					Hitbox.X = PlayerScript->Elements.HitboxElements[i].Box.X + m_Player[PlayerIndex].PositionX;
					Hitbox.Y = PlayerScript->Elements.HitboxElements[i].Box.Y + m_Player[PlayerIndex].PositionY;
					Hitbox.Width = PlayerScript->Elements.HitboxElements[i].Box.Width;
					Hitbox.Height = PlayerScript->Elements.HitboxElements[i].Box.Height;
				}
				else
				{
					Hitbox.X = m_Player[PlayerIndex].PositionX - (PlayerScript->Elements.HitboxElements[i].Box.X + PlayerScript->Elements.HitboxElements[i].Box.Width);
					Hitbox.Y = PlayerScript->Elements.HitboxElements[i].Box.Y + m_Player[PlayerIndex].PositionY;
					Hitbox.Width = PlayerScript->Elements.HitboxElements[i].Box.Width;
					Hitbox.Height = PlayerScript->Elements.HitboxElements[i].Box.Height;
				}
				for (uint32 j = 0; j < OtherScript->Elements.HurtboxCount; j++)
				{
					if (OtherScript->Elements.HurtboxElements->InRange(m_Player[OtherIndex].PlaybackState.PlaybackCursor))
					{
						if (m_Player[OtherIndex].Facing > 0.0f)
						{
							Hurtbox.X = OtherScript->Elements.HurtboxElements[j].Box.X + m_Player[OtherIndex].PositionX;
							Hurtbox.Y = OtherScript->Elements.HurtboxElements[j].Box.Y + m_Player[OtherIndex].PositionY;
							Hurtbox.Width = OtherScript->Elements.HurtboxElements[j].Box.Width;
							Hurtbox.Height = OtherScript->Elements.HurtboxElements[j].Box.Height;
						}
						else
						{
							Hurtbox.X = m_Player[OtherIndex].PositionX - (OtherScript->Elements.HurtboxElements[j].Box.X + OtherScript->Elements.HurtboxElements[j].Box.Width);
							Hurtbox.Y = OtherScript->Elements.HurtboxElements[j].Box.Y + m_Player[OtherIndex].PositionY;
							Hurtbox.Width = OtherScript->Elements.HurtboxElements[j].Box.Width;
							Hurtbox.Height = OtherScript->Elements.HurtboxElements[j].Box.Height;
						}
						if (BoxIntersection(&Hitbox, &Hurtbox))
						{
							ResolveHitAndApplyEffects(StateManager, PlayerIndex, OtherIndex, PlayerScript->Elements.HitboxElements[i].Effects, OtherScript);
							break;
						}
					}
				}
			}
		}
	}
	
}

void gamestate::ResolveHitAndApplyEffects(state_manager* StateManager, uint32 PlayerIndex, uint32 OtherIndex, uint8 EffectIndex, state_script* OtherScript)
{
	// TODO: find a good home for attack level defaults
	hitbox_effects* HitboxEffects = StateManager->GetHitboxEffects(EffectIndex);
	switch (HitboxEffects->AtkLvl)
	{
	case 0:
		m_Player[OtherIndex].PlaybackState.State = CMN_STATE_HIT_STAND_LVL0; 
		m_Player[OtherIndex].VelocityX = -(HitboxEffects->Knockback / 10.0f) * m_Player[OtherIndex].Facing;
		m_Player[OtherIndex].AccelerationX = (m_Player[OtherIndex].VelocityX / 10.0f);
		break;
	case 1:
		m_Player[OtherIndex].PlaybackState.State = CMN_STATE_HIT_STAND_LVL1;
		m_Player[OtherIndex].VelocityX = -(HitboxEffects->Knockback / 12.0f) * m_Player[OtherIndex].Facing;
		m_Player[OtherIndex].AccelerationX = (m_Player[OtherIndex].VelocityX / 12.0f);
	default:
		break;
	}
	m_Player[PlayerIndex].DisableHitbox = true;
	m_Player[PlayerIndex].Hitstop = HitboxEffects->Hitstop;
	m_Player[PlayerIndex].Flags |= PLAYER_ALLOW_CANCEL;
	m_Player[OtherIndex].Hitstop = HitboxEffects->Hitstop;
	m_Player[OtherIndex].PlaybackState.New = true;
	m_Player[OtherIndex].PlaybackState.PlaybackCursor = 0;
}
