#include "player_graphics.h"
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include "render_system.h"
#include "subsystems.h"

player_graphics::player_graphics()
{
}


player_graphics::~player_graphics()
{
}

// TODO: error checking
uint32 player_graphics::Initialize(const char* Filename)
{
	FILE* FilePointer;
	char FileBuffer[65536];
	rapidjson::Document Document;
	
	std::string JSONFileName = Filename + (std::string)".json";
	fopen_s(&FilePointer, JSONFileName.c_str(), "rb");
	rapidjson::FileReadStream FileReadStream(FilePointer, FileBuffer, sizeof(FileBuffer));
	
	Document.ParseStream<rapidjson::kParseCommentsFlag>(FileReadStream);
	fclose(FilePointer);

	m_FrameCount = Document["frames"].Size();
	m_AnimationFrames = new sf::IntRect[m_FrameCount];
	for (uint32 i = 0; i < m_FrameCount; i++)
	{
		m_AnimationFrames[i] = sf::IntRect(	Document["frames"][i]["frame"]["x"].GetInt(),
											Document["frames"][i]["frame"]["y"].GetInt(),
											Document["frames"][i]["frame"]["w"].GetInt(),
											Document["frames"][i]["frame"]["h"].GetInt());
	}

	std::string PNGFileName = Filename + std::string(".png");
	m_CharacterTexture.loadFromFile(PNGFileName);
	m_CharacterSprite.setTexture(m_CharacterTexture);

	return 1;
}

void player_graphics::SetAnimation(uint32 Index)
{
	// TODO: Figure this shit out
	m_CharacterSprite.setTextureRect(m_AnimationFrames[Index]);
}

void player_graphics::SetAnimation(uint32 Index, float Facing)
{
	sf::IntRect TextureRect = m_AnimationFrames[Index];
	if (Facing == -1.0f)
	{
		TextureRect.left += TextureRect.width;
		TextureRect.width = -TextureRect.width;
	}
	m_CharacterSprite.setTextureRect(TextureRect);
}

void player_graphics::SetPosition(float PositionX, float PositionY, int Facing)
{
	sf::Vector2f ViewCenter = RenderSystem->GetViewCenter();
	float Adjustment = (float)Facing * (m_CharacterSprite.getTextureRect().width / 2.0f);
	m_CharacterSprite.setPosition(sf::Vector2f(PositionX - Adjustment, -PositionY - ViewCenter.y - m_CharacterSprite.getTextureRect().height));
}

void player_graphics::NextFrame()
{
	static int32 FrameIndex = 0;

	m_CharacterSprite.setTextureRect(m_AnimationFrames[FrameIndex]);

	FrameIndex++;
	FrameIndex %= m_FrameCount;
}