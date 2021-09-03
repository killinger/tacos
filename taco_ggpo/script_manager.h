#pragma once
#include <map>
#include <vector>
#include "gamestate.h"
#include "command.h"



struct HitboxInfo
{
	float width;
	float height;
	float x;
	float y;
};

struct HurtboxInfo
{
	float width;
	float height;
	float x;
	float y;
};

class frame_info
{
public:
	int m_cancelList;
	int m_hitboxes;
	int m_hurtboxes;
	int m_AnimationIndex;
	float m_xShift;
};

class character_script
{
public:
	std::string					m_ScriptName;
	int							m_FrameCount;
	frame_info*					m_Frames;
	int							m_defaultTransition;
	int							m_scriptType;
	int							m_transitions;
};

class script_manager
{
public:
	int											m_scriptCount;
	int											m_cancelListCount;
	int											m_hitboxCount;
	int											m_hurtboxCount;
	character_script*							m_Scripts;
	std::vector<std::string>*					m_cancelLists;
	HitboxInfo*									m_hitboxes;
	HurtboxInfo*								m_hurtboxes;
	std::vector<std::map<std::string, int>>		m_transitionMaps;
	std::vector<Command>						m_Commands;

	script_manager();
	~script_manager();
	void						initializeTestData();
	//void						readTestDataFromFile();
	void						readTestDataFromFile2();
	frame_info*					getCurrentFrame(playbackstate* playbackState);
	std::vector<std::string>	getCancelList(int cancelList);
	std::string					getScriptName(int script);
	void						Update(playbackstate* playbackState, std::string trigger);
};

