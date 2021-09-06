#include "script_manager.h"
#include "defs.h"
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include <cstdio>

script_manager::script_manager()
{
}


script_manager::~script_manager()
{
}

void script_manager::initializeTestData()
{
	readTestDataFromFile2();

	//// Script Initialize
	//m_scriptCount = 3;
	//m_Scripts = new character_script[m_scriptCount];

	//m_Scripts[0].m_ScriptName = "idle";
	//m_Scripts[0].m_defaultTransition = 0;
	//m_Scripts[0].m_transitionMap["5A"] = 1;
	//m_Scripts[0].m_transitionMap["5B"] = 2;
	//m_Scripts[0].m_FrameCount = 20;
	//m_Scripts[0].m_Frames = new frame_info[m_Scripts[0].m_FrameCount];
	//for (int i = 0; i < m_Scripts[0].m_FrameCount; i++)
	//{
	//	m_Scripts[0].m_Frames[i].m_cancelList = 0;
	//	m_Scripts[0].m_Frames[i].m_hitboxes = -1;
	//	m_Scripts[0].m_Frames[i].m_hurtboxes = -1;
	//}

	//m_Scripts[1].m_ScriptName = "5A";
	//m_Scripts[1].m_defaultTransition = 0;
	//m_Scripts[1].m_transitionMap["5B"] = 2;
	//m_Scripts[1].m_FrameCount = 30;
	//m_Scripts[1].m_Frames = new frame_info[m_Scripts[1].m_FrameCount];
	//for (int i = 0; i < m_Scripts[1].m_FrameCount; i++)
	//{
	//	m_Scripts[1].m_Frames[i].m_hurtboxes = 0;
	//	if (i >= 10 && i < 20)
	//	{
	//		m_Scripts[1].m_Frames[i].m_hitboxes = 0;
	//		m_Scripts[1].m_Frames[i].m_cancelList = 1;
	//	}
	//	else
	//	{
	//		m_Scripts[1].m_Frames[i].m_hitboxes = -1;
	//		m_Scripts[1].m_Frames[i].m_cancelList = -1;
	//	}
	//}

	//m_Scripts[2].m_ScriptName = "5B";
	//m_Scripts[2].m_defaultTransition = 0;
	//m_Scripts[2].m_FrameCount = 30;
	//m_Scripts[2].m_Frames = new frame_info[m_Scripts[2].m_FrameCount];
	//for (int i = 0; i < m_Scripts[2].m_FrameCount; i++)
	//{
	//	m_Scripts[2].m_Frames[i].m_cancelList = -1;
	//	m_Scripts[2].m_Frames[i].m_hurtboxes = -1;
	//	m_Scripts[2].m_Frames[i].m_hitboxes = -1;
	//}

	//// cancel list Initialize
	//m_cancelListCount = 2;
	//m_cancelLists = new std::vector<std::string>[m_cancelListCount];
	//m_cancelLists[0].push_back("5B");
	//m_cancelLists[0].push_back("5A");
	//m_cancelLists[1].push_back("5B");

	//// Command Initialize
	//Command Command = { 0 };
	//Command.type = 0;
	//Command.button = 0;
	//Command.directions[0] = 5;

	//m_commandTriggers["5A"].push_back(Command);

	//Command.type = 0;
	//Command.button = 1;
	//Command.directions[0] = 5;

	//m_commandTriggers["5B"].push_back(Command);

	//// hitbox Initialize
	//m_hitboxCount = 1;
	//m_hitboxes = new HitboxInfo[m_hitboxCount];
	//m_hitboxes[0].width = 90.0f;
	//m_hitboxes[0].height = 40.0f;

	//// hurtbox Initialize
	//m_hurtboxCount = 1;
	//m_hurtboxes = new HurtboxInfo[m_hurtboxCount];
	//m_hurtboxes[0].width = 40.0f;
	//m_hurtboxes[0].height = 120.0f;
}

//void script_manager::readTestDataFromFile()
//{
//	FILE* filePointer;
//	char fileBuffer[65536];
//
//	rapidjson::Document document;
//	{
//		fopen_s(&filePointer, "data/scripts/commands.json", "rb");
//		rapidjson::FileReadStream fileReadStream(filePointer, fileBuffer, sizeof(fileBuffer));
//		document.ParseStream<rapidjson::kParseCommentsFlag>(fileReadStream);
//		fclose(filePointer);
//
//		assert(document.IsObject());
//		assert(document.HasMember("Command"));
//
//		Command Command = { 0 };
//
//		for (unsigned int i = 0; i < document["Command"].Size(); i++)
//		{
//			Command.type = document["Command"][i]["type"].GetInt();
//			Command.button = document["Command"][i]["button"].GetInt();
//			Command.bufferTime = document["Command"][i]["bufferTime"].GetInt();
//			Command.directions[0] = document["Command"][i]["directions"][0].GetInt();
//			m_commandTriggers[document["Command"][i]["Trigger"].GetString()].push_back(Command);
//		}
//	}
//
//	{
//		fopen_s(&filePointer, "data/scripts/cancellists.json", "rb");
//		rapidjson::FileReadStream fileReadStream(filePointer, fileBuffer, sizeof(fileBuffer));
//		document.ParseStream<rapidjson::kParseCommentsFlag>(fileReadStream);
//		fclose(filePointer);
//
//		m_cancelListCount = document["cancelLists"].Size();
//		m_cancelLists = new std::vector<std::string>[m_cancelListCount];
//
//		for (int i = 0; i < m_cancelListCount; i++)
//		{
//			for (unsigned int j = 0; j < document["cancelLists"][i].Size(); j++)
//			{
//				m_cancelLists[i].push_back(document["cancelLists"][i][j].GetString());
//			}
//		}
//	}
//
//	{
//		fopen_s(&filePointer, "data/scripts/hitboxes.json", "rb");
//		rapidjson::FileReadStream fileReadStream(filePointer, fileBuffer, sizeof(fileBuffer));
//		document.ParseStream<rapidjson::kParseCommentsFlag>(fileReadStream);
//		fclose(filePointer);
//
//		m_hitboxCount = document["hitboxes"].Size();
//		m_hitboxes = new HitboxInfo[m_hitboxCount];
//
//		for (int i = 0; i < m_hitboxCount; i++)
//		{
//			m_hitboxes[i].height = document["hitboxes"][i]["height"].GetFloat();
//			m_hitboxes[i].width = document["hitboxes"][i]["width"].GetFloat();
//			m_hitboxes[i].x = document["hitboxes"][i]["x"].GetFloat();
//			m_hitboxes[i].y = document["hitboxes"][i]["y"].GetFloat();
//		}
//	}
//
//	{
//		fopen_s(&filePointer, "data/scripts/hurtboxes.json", "rb");
//		rapidjson::FileReadStream fileReadStream(filePointer, fileBuffer, sizeof(fileBuffer));
//		document.ParseStream<rapidjson::kParseCommentsFlag>(fileReadStream);
//		fclose(filePointer);
//
//		m_hurtboxCount = document["hurtboxes"].Size();
//		m_hurtboxes = new HurtboxInfo[m_hurtboxCount];
//
//		for (int i = 0; i < m_hurtboxCount; i++)
//		{
//			m_hurtboxes[i].height = document["hurtboxes"][i]["height"].GetFloat();
//			m_hurtboxes[i].width = document["hurtboxes"][i]["width"].GetFloat();
//			m_hurtboxes[i].x = document["hurtboxes"][i]["x"].GetFloat();
//			m_hurtboxes[i].y = document["hurtboxes"][i]["y"].GetFloat();
//		}
//	}
//
//	std::vector<std::string> scriptNames;
//	{
//		fopen_s(&filePointer, "data/scripts/scripts.json", "rb");
//		rapidjson::FileReadStream fileReadStream(filePointer, fileBuffer, sizeof(fileBuffer));
//		document.ParseStream<rapidjson::kParseCommentsFlag>(fileReadStream);
//		fclose(filePointer);
//
//		for (unsigned int i = 0; i < document.Size(); i++)
//		{
//			scriptNames.push_back(document[i].GetString());
//		}
//	}
//
//	m_scriptCount = scriptNames.size();
//	m_Scripts = new character_script[m_scriptCount];
//
//	for (int i = 0; i < m_scriptCount; i++)
//	{
//		std::string fileName = "data/scripts/" + scriptNames[i] + ".json";
//
//		fopen_s(&filePointer, fileName.c_str(), "rb");
//		rapidjson::FileReadStream fileReadStream(filePointer, fileBuffer, sizeof(fileBuffer));
//		document.ParseStream<rapidjson::kParseCommentsFlag>(fileReadStream);
//		fclose(filePointer);
//
//		m_Scripts[i].m_ScriptName = scriptNames[i];
//		m_Scripts[i].m_defaultTransition = document["defaultTransition"].GetInt();
//
//		for (unsigned int j = 0; j < document["transitions"].Size(); j++)
//		{
//			m_Scripts[i].m_transitionMap[document["transitions"][j]["Trigger"].GetString()] = document["transitions"][j]["Script"].GetInt();
//		}
//
//		m_Scripts[i].m_FrameCount = document["frames"].Size();
//		m_Scripts[i].m_Frames = new frame_info[m_Scripts[i].m_FrameCount];
//		for (unsigned int j = 0; j < m_Scripts[i].m_FrameCount; j++)
//		{
//			m_Scripts[i].m_Frames[j].m_cancelList = document["frames"][j]["cancelList"].GetInt();
//			m_Scripts[i].m_Frames[j].m_hitboxes = document["frames"][j]["hitboxes"].GetInt();
//			m_Scripts[i].m_Frames[j].m_hurtboxes = document["frames"][j]["hurtboxes"].GetInt();
//			if (document["frames"][j].HasMember("xShift"))
//			{
//				m_Scripts[i].m_Frames[j].m_xShift = document["frames"][j]["xShift"].GetFloat();
//			}
//			else
//			{
//				m_Scripts[i].m_Frames[j].m_xShift = 0.0f;
//			}
//		}
//	}
//}

void script_manager::readTestDataFromFile2()
{
	FILE* filePointer;
	char fileBuffer[65536];

	rapidjson::Document document;
	{
		fopen_s(&filePointer, "data/testscripts/commands.json", "rb");
		rapidjson::FileReadStream fileReadStream(filePointer, fileBuffer, sizeof(fileBuffer));
		document.ParseStream<rapidjson::kParseCommentsFlag>(fileReadStream);
		fclose(filePointer);

		assert(document.IsObject());
		assert(document.HasMember("command"));

		command Command = { 0 };

		for (unsigned int i = 0; i < document["command"].Size(); i++)
		{
			Command.type = document["command"][i]["type"].GetInt();
			Command.button = document["command"][i]["button"].GetInt();
			Command.bufferTime = document["command"][i]["bufferTime"].GetInt();
			Command.directions[0] = document["command"][i]["directions"][0].GetInt();
			Command.Trigger = document["command"][i]["trigger"].GetString();
			m_Commands.push_back(Command);
		}
	}

	std::vector<std::string> scriptNames;
	{
		fopen_s(&filePointer, "data/testscripts/scripts.json", "rb");
		rapidjson::FileReadStream fileReadStream(filePointer, fileBuffer, sizeof(fileBuffer));
		document.ParseStream<rapidjson::kParseCommentsFlag>(fileReadStream);
		fclose(filePointer);

		for (unsigned int i = 0; i < document["include"].Size(); i++)
		{
			scriptNames.push_back(document["include"][i].GetString());
		}

		m_transitionMaps.resize(document["transitions"].Size());
		for (unsigned int i = 0; i < document["transitions"].Size(); i++)
		{
			for (unsigned int j = 0; j < document["transitions"][i].Size(); j++)
			{
				m_transitionMaps[i][document["transitions"][i][j]["trigger"].GetString()] = document["transitions"][i][j]["script"].GetInt();
			}
		}
	}

	m_scriptCount = scriptNames.size();
	m_Scripts = new character_script[m_scriptCount];

	for (unsigned int i = 0; i < m_scriptCount; i++)
	{
		std::string fileName = "data/testscripts/" + scriptNames[i] + ".json";

		fopen_s(&filePointer, fileName.c_str(), "rb");
		rapidjson::FileReadStream fileReadStream(filePointer, fileBuffer, sizeof(fileBuffer));
		document.ParseStream<rapidjson::kParseCommentsFlag>(fileReadStream);
		fclose(filePointer);

		m_Scripts[i].m_ScriptName = scriptNames[i];
		m_Scripts[i].m_transitions = document["transitions"].GetInt();
		m_Scripts[i].m_defaultTransition = document["defaultTransition"].GetInt();
		m_Scripts[i].m_scriptType = document["type"].GetInt();
		m_Scripts[i].m_FrameCount = document["frames"].Size();
		m_Scripts[i].m_Frames = new frame_info[m_Scripts[i].m_FrameCount];

		for (int j = 0; j < m_Scripts[i].m_FrameCount; j++)
		{
			m_Scripts[i].m_Frames[j].m_hurtboxes = document["frames"][j]["hurtboxes"].GetInt();

			if (document["frames"][j].HasMember("animationIndex"))
			{
				m_Scripts[i].m_Frames[j].m_AnimationIndex = document["frames"][j]["animationIndex"].GetInt();
			}
			else
			{
				m_Scripts[i].m_Frames[j].m_AnimationIndex = -1;
			}
		}
	}

	{
		fopen_s(&filePointer, "data/testscripts/hitboxes.json", "rb");
		rapidjson::FileReadStream fileReadStream(filePointer, fileBuffer, sizeof(fileBuffer));
		document.ParseStream<rapidjson::kParseCommentsFlag>(fileReadStream);
		fclose(filePointer);

		m_hitboxCount = document["hitboxes"].Size();
		m_hitboxes = new hitbox_info[m_hitboxCount];

		for (int i = 0; i < m_hitboxCount; i++)
		{
			m_hitboxes[i].height = document["hitboxes"][i]["height"].GetFloat();
			m_hitboxes[i].width = document["hitboxes"][i]["width"].GetFloat();
			m_hitboxes[i].x = document["hitboxes"][i]["x"].GetFloat();
			m_hitboxes[i].y = document["hitboxes"][i]["y"].GetFloat();
		}
	}

	{
		fopen_s(&filePointer, "data/testscripts/hurtboxes.json", "rb");
		rapidjson::FileReadStream fileReadStream(filePointer, fileBuffer, sizeof(fileBuffer));
		document.ParseStream<rapidjson::kParseCommentsFlag>(fileReadStream);
		fclose(filePointer);

		m_hurtboxCount = document["hurtboxes"].Size();
		m_hurtboxes = new hurtbox_info[m_hurtboxCount];

		for (int i = 0; i < m_hurtboxCount; i++)
		{
			m_hurtboxes[i].height = document["hurtboxes"][i]["height"].GetFloat();
			m_hurtboxes[i].width = document["hurtboxes"][i]["width"].GetFloat();
			m_hurtboxes[i].x = document["hurtboxes"][i]["x"].GetFloat();
			m_hurtboxes[i].y = document["hurtboxes"][i]["y"].GetFloat();
		}
	}
}

frame_info* script_manager::getCurrentFrame(playbackstate* playbackState)
{
	return &m_Scripts[playbackState->Script].m_Frames[playbackState->PlaybackCursor];
}

std::vector<std::string> script_manager::getCancelList(int cancelList)
{
	return m_cancelLists[cancelList];

}

std::string script_manager::getScriptName(int script)
{
	return m_Scripts[script].m_ScriptName;
}

void script_manager::Update(playbackstate* playbackState, std::string trigger)
{
	character_script* script = m_Scripts + playbackState->Script;

	if (trigger != "")
	{
		if (script->m_scriptType == SCRIPT_TYPE_CONTINUOUS)
		{
			if (playbackState->Script == m_transitionMaps[script->m_transitions][trigger])
			{
				if (playbackState->PlaybackCursor < script->m_FrameCount - 1)
				{
					playbackState->PlaybackCursor++;
				}
				else
				{
					playbackState->PlaybackCursor = 0;
				}
			}
			else
			{
				playbackState->PendingScript = m_transitionMaps[script->m_transitions][trigger];
				playbackState->PlaybackCursor = 0;
			}
		}
		else if (script->m_scriptType == SCRIPT_TYPE_ATTACK)
		{
			if (playbackState->PlaybackCursor < script->m_FrameCount - 1)
			{
				playbackState->PlaybackCursor++;
			}
			else
			{
				playbackState->PendingScript = m_transitionMaps[script->m_transitions][trigger];
				playbackState->PlaybackCursor = 0;
			}
		}
	}
	else
	{
		if (script->m_scriptType == SCRIPT_TYPE_CONTINUOUS)
		{
			if (playbackState->Script == script->m_defaultTransition)
			{
				if (playbackState->PlaybackCursor < script->m_FrameCount - 1)
				{
					playbackState->PlaybackCursor++;
				}
				else
				{
					playbackState->PendingScript = script->m_defaultTransition;
					playbackState->PlaybackCursor = 0;
				}
			}
			else
			{
				playbackState->PendingScript = script->m_defaultTransition;
				playbackState->PlaybackCursor = 0;
			}
		}
		else
		{
			if (playbackState->PlaybackCursor < script->m_FrameCount - 1)
			{
				playbackState->PlaybackCursor++;
			}
			else
			{
				playbackState->PendingScript = script->m_defaultTransition;
				playbackState->PlaybackCursor = 0;
			}
		}
	}
}
