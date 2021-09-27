#include "gamestate.h"
#include "character_cmn_states.h"
#include "logging_system.h"
#include "subsystems.h"

void gamestate::Update(uint32* Inputs, state_manager* StateManager)
{
	bool FlipDirections[] =
	{
		m_Player[0].PositionX > m_Player[1].PositionX,
		m_Player[1].PositionX > m_Player[0].PositionX
	};

	m_Player[0].InputBuffer.Update(Inputs[0], m_FrameCount, FlipDirections[0]);
	m_Player[1].InputBuffer.Update(Inputs[1], m_FrameCount, FlipDirections[1]);

	// Transition stage
	AdvancePlayerState(StateManager, &m_Player[0], &m_Player[1]);
	AdvancePlayerState(StateManager, &m_Player[1], &m_Player[0]);

	UpdateMovement(&m_Player[0]);
	UpdateMovement(&m_Player[1]);

	// Bounds check and correction stage
	PerformPositionCorrection(StateManager);
}


void gamestate::AdvancePlayerState(state_manager* StateManager, playerstate* PlayerState, playerstate* OtherPlayer)
{
	if (PlayerState->PlaybackState.State < CMN_STATE_COUNT)
		UpdateCmnState[PlayerState->PlaybackState.State](StateManager, PlayerState, OtherPlayer);
}

void gamestate::UpdateMovement(playerstate* PlayerState)
{
	// TODO: Run shit
	PlayerState->PositionX += PlayerState->VelocityX;
	PlayerState->PositionY += PlayerState->VelocityY;
	PlayerState->VelocityX += PlayerState->AccelerationX;
	PlayerState->VelocityY += PlayerState->AccelerationY;
}

void gamestate::PerformPositionCorrection(state_manager* StateManager)
{
	state_script* Script[2] =
	{
		StateManager->GetScript(m_Player[0].PlaybackState.State),
		StateManager->GetScript(m_Player[1].PlaybackState.State)
	};

	collision_box Pushbox[2];
	collision_box Result;

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
		m_Player[0].PositionX -= m_Player[0].Facing * (Result.Width / 2.0f);
		m_Player[1].PositionX -= m_Player[1].Facing * (Result.Width / 2.0f);
	}
}
