#include "gamestate.h"
#include "character_cmn_states.h"
#include "logging_system.h"
#include "gamestate_buffer.h"
#include "render_system.h"
#include "subsystems.h"

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

	// Transition stage
	state_script* Script[2] = {	AdvancePlayerState(StateManager, &m_Player[0], &m_Player[1]),
								AdvancePlayerState(StateManager, &m_Player[1], &m_Player[0]) };
	// Movement and correction stage
	UpdateMovement(&m_Player[0]);
	UpdateMovement(&m_Player[1]);

	CorrectAndFinalizePositions(Script);

	// Hit detection
}


state_script* gamestate::AdvancePlayerState(state_manager* StateManager, playerstate* PlayerState, playerstate* OtherPlayer)
{
	state_script* Script = StateManager->GetScript(PlayerState->PlaybackState.State);
	bool ScriptFinished = false;
	float CurrentFacing = 1.0f;

	if (!PlayerState->PlaybackState.New)
		if (++PlayerState->PlaybackState.PlaybackCursor >= Script->TotalFrames)
			ScriptFinished = true;
	PlayerState->PlaybackState.New = false;

	// TODO: Needs some testing to figure when to actually apply this
	if (PlayerState->PositionX > OtherPlayer->PositionX)
		CurrentFacing = -1.0f;

	if (PlayerState->PositionY < 0.0f)
	{
		// TODO: Landing
	}
	else if (PlayerState->PlaybackState.State < CMN_STATE_COUNT)
		UpdateCmnState[PlayerState->PlaybackState.State](	StateManager, 
															PlayerState, 
															Script,
															m_FrameCount,
															ScriptFinished, 
															CurrentFacing);
	else
	{
		if (ScriptFinished)
		{
			PlayerState->PlaybackState.State = CMN_STATE_STAND;
			CmnStateStand(	StateManager,
							PlayerState,
							Script,
							m_FrameCount,
							ScriptFinished,
							CurrentFacing);
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
	if (BoxIntersection(&Pushbox[0], &Pushbox[1], &Result))
	{
		float Repulsion = Result.Width / 2.0f;
		m_Player[0].PositionX -= m_Player[0].Facing * Repulsion;
		m_Player[1].PositionX -= m_Player[1].Facing * Repulsion;
	}
}
