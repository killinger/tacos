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

	UpdatePosition(&m_Player[0]);
	UpdatePosition(&m_Player[1]);

	m_FrameCount++;
}


void gamestate::AdvancePlayerState(state_manager* StateManager, playerstate* PlayerState, playerstate* OtherPlayer)
{
	if (PlayerState->PlaybackState.State < CMN_STATE_COUNT)
		UpdateCmnState[PlayerState->PlaybackState.State](StateManager, PlayerState, OtherPlayer);

	// Ground bounds check if airborne
}

void gamestate::UpdatePosition(playerstate* PlayerState)
{
	PlayerState->PositionX += PlayerState->VelocityX;
	PlayerState->PositionY += PlayerState->VelocityY;
	PlayerState->VelocityX += PlayerState->AccelerationX;
	PlayerState->VelocityY += PlayerState->AccelerationY;
}
