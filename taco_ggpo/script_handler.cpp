#include "script_handler.h"
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include <unordered_map>
#include <vector>

void script_handler::Initialize()
{
	ReadFromDirectory("data/sparsetest");
}

void script_handler::Update(playbackstate* PlaybackState)
{
	state_script* Script = &m_Scripts[PlaybackState->Script];

	if (PlaybackState->PlaybackCursor < Script->TotalFrames - 1)
		PlaybackState->PlaybackCursor++;
	else
		PlaybackState->PlaybackCursor = 0;
}

state_script* script_handler::GetScript(playbackstate* PlaybackState)
{
	return &m_Scripts[PlaybackState->Script];
}

cancel_list* script_handler::GetCancelList(uint16 Index)
{
	return &m_CancelLists[Index];
}

move_list* script_handler::GetMove(uint32 Index)
{
	return &m_Moves[Index];
}

void script_handler::ReadFromDirectory(const char* Path)
{
	FILE* filePointer;
	char fileBuffer[15000];
	rapidjson::Document Document;
	std::string Filepath;
	std::vector<std::string> Scripts;
	std::unordered_map<std::string, uint32> ScriptHandleToIndexMap;
	std::unordered_map<std::string, uint32> CancelHandleToIndexMap;
	std::unordered_map<std::string, uint32> MoveHandleToIndexMap;
	std::unordered_map<std::string, uint32> HitEffectToHandleToIndexMap;

	Filepath = Path + (std::string)"/scripts.json";
	{
		fopen_s(&filePointer, Filepath.c_str(), "rb");
		rapidjson::FileReadStream fileReadStream(filePointer, fileBuffer, sizeof(fileBuffer));
		Document.ParseStream<rapidjson::kParseCommentsFlag>(fileReadStream);
		fclose(filePointer);

		for (unsigned int i = 0; i < Document.Size(); i++)
		{
			std::string ScriptName = Document[i].GetString();
			ScriptHandleToIndexMap[ScriptName] = i;
			Scripts.push_back(Document[i].GetString());
		}
	}

	Filepath = Path + (std::string)"/moves.json";
	{
		fopen_s(&filePointer, Filepath.c_str(), "rb");
		rapidjson::FileReadStream fileReadStream(filePointer, fileBuffer, sizeof(fileBuffer));
		Document.ParseStream<rapidjson::kParseCommentsFlag>(fileReadStream);
		fclose(filePointer);

		m_MoveCount = Document.Size();
		m_Moves = new move_list[m_MoveCount];

		for (unsigned int i = 0; i < m_MoveCount; i++)
		{
			m_Moves[i].ScriptIndex = ScriptHandleToIndexMap[Document[i]["Name"].GetString()];
			m_Moves[i].InputMask = Document[i]["InputMask"].GetUint();
			MoveHandleToIndexMap[Document[i]["Name"].GetString()] = i;
		}
	}

	Filepath = Path + (std::string)"/cancellists.json";
	{
		fopen_s(&filePointer, Filepath.c_str(), "rb");
		rapidjson::FileReadStream fileReadStream(filePointer, fileBuffer, sizeof(fileBuffer));
		Document.ParseStream<rapidjson::kParseCommentsFlag>(fileReadStream);
		fclose(filePointer);

		m_CancelCount = Document.Size();
		m_CancelLists = new cancel_list[m_CancelCount];

		for (unsigned int i = 0; i < Document.Size(); i++)
		{
			CancelHandleToIndexMap[Document[i]["Handle"].GetString()] = i;
			m_CancelLists[i].MoveCount = Document[i]["List"].Size();
			m_CancelLists[i].Moves = new uint8[m_CancelLists[i].MoveCount];
			for (unsigned int j = 0; j < m_CancelLists[i].MoveCount; j++)
			{
				m_CancelLists[i].Moves[j] = MoveHandleToIndexMap[Document[i]["List"][j].GetString()];
			}
		}
	}

	m_ScriptCount = Scripts.size();
	m_Scripts = new state_script[m_ScriptCount];

	for (unsigned int i = 0; i < m_ScriptCount; i++)
	{
		std::string ScriptName = "/" + Scripts[i] + ".json";
		Filepath = Path + ScriptName;

		fopen_s(&filePointer, Filepath.c_str(), "rb");
		rapidjson::FileReadStream fileReadStream(filePointer, fileBuffer, sizeof(fileBuffer));
		Document.ParseStream<rapidjson::kParseCommentsFlag>(fileReadStream);
		fclose(filePointer);

		m_Scripts[i].Name = Document["Name"].GetString();
		m_Scripts[i].TotalFrames = Document["TotalFrames"].GetInt();
		// First and last hitbox frames are inferred 
		m_Scripts[i].ScalingXV = Document["ScalingXV"].GetFloat();
		m_Scripts[i].ScalingYV = Document["ScalingYV"].GetFloat();
		m_Scripts[i].ScalingXA = Document["ScalingXA"].GetFloat();
		m_Scripts[i].ScalingYA = Document["ScalingYA"].GetFloat();
		
		m_Scripts[i].Flags = 0;
		m_Scripts[i].Flags |= Document["Flags"].GetUint();
		
		if (Document.HasMember("HitboxElements"))
		{
			m_Scripts[i].Elements.HitboxCount = Document["HitboxElements"].Size();
			m_Scripts[i].Elements.HitboxElements = new hitbox_element[m_Scripts[i].Elements.HitboxCount];
			for (unsigned int j = 0; j < m_Scripts[i].Elements.HitboxCount; j++)
			{
				//m_Scripts[i].Elements.HitboxElements[j].EfectIndex = HitEffectToHandleToIndexMap[Document["HitboxElements"][j]["Effect"].GetString()];
				m_Scripts[i].Elements.HitboxElements[j].FrameStart = (int8)Document["HitboxElements"][j]["FrameStart"].GetUint();
				m_Scripts[i].Elements.HitboxElements[j].FrameEnd = (int8)Document["HitboxElements"][j]["FrameEnd"].GetUint();
				m_Scripts[i].Elements.HitboxElements[j].Box.X = Document["HitboxElements"][j]["X"].GetFloat();
				m_Scripts[i].Elements.HitboxElements[j].Box.Y = Document["HitboxElements"][j]["Y"].GetFloat();
				m_Scripts[i].Elements.HitboxElements[j].Box.Width = Document["HitboxElements"][j]["Width"].GetFloat();
				m_Scripts[i].Elements.HitboxElements[j].Box.Height = Document["HitboxElements"][j]["Height"].GetFloat();

				if (j == 0)
					m_Scripts[i].FirstHitboxFrame = m_Scripts[i].Elements.HitboxElements[j].FrameStart;
				else if (j == (m_Scripts[i].Elements.HitboxCount - 1))
					m_Scripts[i].LastHitboxFrame = m_Scripts[i].Elements.HitboxElements[j].FrameEnd;
			}
		}
		else
		{
			m_Scripts[i].Elements.HitboxCount = 0;
			m_Scripts[i].Elements.HitboxElements = NULL;
			m_Scripts[i].FirstHitboxFrame = -1;
			m_Scripts[i].LastHitboxFrame = -1;
		}

		if (Document.HasMember("HurtboxElements"))
		{
			m_Scripts[i].Elements.HurtboxCount = Document["HurtboxElements"].Size();
			m_Scripts[i].Elements.HurtboxElements = new hurtbox_element[m_Scripts[i].Elements.HurtboxCount];
			for (unsigned int j = 0; j < m_Scripts[i].Elements.HurtboxCount; j++)
			{
				m_Scripts[i].Elements.HurtboxElements[j].FrameStart = (uint8)Document["HurtboxElements"][j]["FrameStart"].GetUint();
				m_Scripts[i].Elements.HurtboxElements[j].FrameEnd = (uint8)Document["HurtboxElements"][j]["FrameEnd"].GetUint();
				m_Scripts[i].Elements.HurtboxElements[j].Box.X = Document["HurtboxElements"][j]["X"].GetFloat();
				m_Scripts[i].Elements.HurtboxElements[j].Box.Y = Document["HurtboxElements"][j]["Y"].GetFloat();
				m_Scripts[i].Elements.HurtboxElements[j].Box.Width = Document["HurtboxElements"][j]["Width"].GetFloat();
				m_Scripts[i].Elements.HurtboxElements[j].Box.Height = Document["HurtboxElements"][j]["Height"].GetFloat();
			}
		}
		else
		{
			m_Scripts[i].Elements.HurtboxCount = 0;
			m_Scripts[i].Elements.HurtboxElements = NULL;
		}

		if (Document.HasMember("PushboxElements"))
		{
			m_Scripts[i].Elements.PushboxCount = Document["PushboxElements"].Size();
			m_Scripts[i].Elements.PushboxElements = new pushbox_element[m_Scripts[i].Elements.PushboxCount];
			for (unsigned int j = 0; j < m_Scripts[i].Elements.PushboxCount; j++)
			{
				m_Scripts[i].Elements.PushboxElements[j].FrameStart = (uint8)Document["PushboxElements"][j]["FrameStart"].GetUint();
				m_Scripts[i].Elements.PushboxElements[j].FrameEnd = (uint8)Document["PushboxElements"][j]["FrameEnd"].GetUint();
				m_Scripts[i].Elements.PushboxElements[j].Box.X = Document["PushboxElements"][j]["X"].GetFloat();
				m_Scripts[i].Elements.PushboxElements[j].Box.Y = Document["PushboxElements"][j]["Y"].GetFloat();
				m_Scripts[i].Elements.PushboxElements[j].Box.Width = Document["PushboxElements"][j]["Width"].GetFloat();
				m_Scripts[i].Elements.PushboxElements[j].Box.Height = Document["PushboxElements"][j]["Height"].GetFloat();
			}
		}
		else
		{
			m_Scripts[i].Elements.PushboxCount = 0;
			m_Scripts[i].Elements.PushboxElements = NULL;
		}

		if (Document.HasMember("CancelElements"))
		{
			m_Scripts[i].Elements.CancelCount = Document["CancelElements"].Size();
			m_Scripts[i].Elements.CancelElements = new frame_element[m_Scripts[i].Elements.CancelCount];
			for (unsigned int j = 0; j < m_Scripts[i].Elements.CancelCount; j++)
			{
				m_Scripts[i].Elements.CancelElements[j].FrameStart = (uint8)Document["CancelElements"][j]["FrameStart"].GetUint();
				m_Scripts[i].Elements.CancelElements[j].FrameEnd = (uint8)Document["CancelElements"][j]["FrameEnd"].GetUint();
				m_Scripts[i].Elements.CancelElements[j].Index = CancelHandleToIndexMap[Document["CancelElements"][j]["CancelList"].GetString()];
			}
		}
		else
		{
			m_Scripts[i].Elements.CancelCount = 0;
			m_Scripts[i].Elements.CancelElements = NULL;
		}

		if (Document.HasMember("ForceElements"))
		{
			m_Scripts[i].Elements.ForceCount = Document["ForceElements"].Size();
			m_Scripts[i].Elements.ForceElements = new force_element[m_Scripts[i].Elements.ForceCount];
			for (unsigned int j = 0; j < m_Scripts[i].Elements.ForceCount; j++)
			{
				m_Scripts[i].Elements.ForceElements[j].FrameStart = (uint8)Document["ForceElements"][j]["FrameStart"].GetUint();
				m_Scripts[i].Elements.ForceElements[j].FrameEnd = (uint8)Document["ForceElements"][j]["FrameEnd"].GetUint();
				m_Scripts[i].Elements.ForceElements[j].Target = Document["ForceElements"][j]["Target"].GetUint();
				m_Scripts[i].Elements.ForceElements[j].Amount = Document["ForceElements"][j]["Amount"].GetFloat();
			}
		}
		else
		{
			m_Scripts[i].Elements.ForceCount = 0;
			m_Scripts[i].Elements.ForceElements = NULL;
		}

	}
}
