#pragma once
#include <map>
#include <vector>
#include "gamestate.h"
#include "command.h"

// TODO: fix the fucking naming convention jesus christ
// TODO: try impact of using smaller data types
// TODO: why are some of these classes and some struct it looks ridiculous and you are ridiculous 

// TODO: Do these two really need different structs? All collision boxes should have the same fields
struct hitbox_info
{
	float width;
	float height;
	float x;
	float y;
};

struct hurtbox_info
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
	int m_Hitboxes;
	int m_Hurtboxes;
	int m_AnimationIndex;
	int m_ApplyRun;
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
	hitbox_info*								m_hitboxes;
	hurtbox_info*								m_hurtboxes;
	std::vector<std::map<std::string, int>>		m_transitionMaps;
	std::vector<command>						m_Commands;

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