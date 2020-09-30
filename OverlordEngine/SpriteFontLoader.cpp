#include "stdafx.h"
#include "SpriteFontLoader.h"
#include "BinaryReader.h"
#include "ContentManager.h"
#include "TextureData.h"

SpriteFont* SpriteFontLoader::LoadContent(const std::wstring& assetFile)
{
	auto pBinReader = new BinaryReader();
	pBinReader->Open(assetFile);

	if (!pBinReader->Exists())
	{
		Logger::LogFormat(LogLevel::Error, L"SpriteFontLoader::LoadContent > Failed to read the assetFile!\nPath: \'%s\'", assetFile.c_str());
		return nullptr;
	}
    
	//See BMFont Documentation for Binary Layout

	//Parse the Identification bytes (B,M,F)
	//If Identification bytes doesn't match B|M|F,
	//Log Error (SpriteFontLoader::LoadContent > Not a valid .fnt font) &
	//return nullptr
	//...
    
    // If the header doesn't match, 
    //  we've been given the wrong file format
    
    
    // Get the validation status from the header validor
    BMFHeader::ValidationStatus status = pBinReader->Read<BMFHeader>().Validate();
    
    if(status == BMFHeader::ValidationStatus::WFORMAT)
    {
        Logger::LogError(L"SpriteFontLoader::LoadContent > Not a valid .fnt font");
        return nullptr;
    }

	//Parse the version (version 3 required)
	//If version is < 3,
	//Log Error (SpriteFontLoader::LoadContent > Only .fnt version 3 is supported)
	//return nullptr
	//...
    
    if(status == BMFHeader::ValidationStatus::WFORMAT)
    {
        Logger::LogError(L"SpriteFontLoader::LoadContent > Only .fnt version 3 is supported");
        return nullptr;
    }

	//Valid .fnt file
	auto pSpriteFont = new SpriteFont();
	//SpriteFontLoader is a friend class of SpriteFont
	//That means you have access to its privates (pSpriteFont->m_FontName = ... is valid)
    
    
	//**********
	// BLOCK 0 *
	//**********
	//Retrieve the blockId and blockSize
	//Retrieve the FontSize (will be -25, BMF bug) [SpriteFont::m_FontSize]
	//Move the binreader to the start of the FontName [BinaryReader::MoveBufferPosition(...) or you can set its position using BinaryReader::SetBufferPosition(...))
	//Retrieve the FontName [SpriteFont::m_FontName]
	//...
    char b0Id = pBinReader->Read<char>();
    int b0Size = pBinReader->Read<int>();
    (void)b0Id;
    (void)b0Size;
    
    short fontSize = pBinReader->Read<short>();
    pBinReader->MoveBufferPosition(12);
    pSpriteFont->m_FontSize = fontSize;
    pSpriteFont->m_FontName = pBinReader->ReadNullString();
    
	//**********
	// BLOCK 1 *
	//**********
	//Retrieve the blockId and blockSize
	//Retrieve Texture Width & Height [SpriteFont::m_TextureWidth/m_TextureHeight]
	//Retrieve PageCount
	//> if pagecount > 1
	//> Log Error (SpriteFontLoader::LoadContent > SpriteFont (.fnt): Only one texture per font allowed)
	//Advance to Block2 (Move Reader)
	//...
    char b1Id = pBinReader->Read<char>();
    int b1Size = pBinReader->Read<int>();
    (void)b1Id;
    (void)b1Size;
    
    // Read block 2 and backtrack 1 since 15bytes get padded up to 16
    auto block = pBinReader->Read<BMF_B2>();
    pBinReader->MoveBufferPosition(BMF_B2_BACKTRACK);
    
    pSpriteFont->m_TextureWidth = block.scaleW;
    pSpriteFont->m_TextureHeight = block.scaleH;
    
    if(block.pages > 1)
    {
        Logger::LogError(L"SpriteFontLoader::LoadContent > SpriteFont (.fnt): Only one texture per font allowed, " + std::to_wstring(block.pages));
        return nullptr;        
    }

	//**********
	// BLOCK 2 *
	//**********
	//Retrieve the blockId and blockSize
	//Retrieve the PageName (store Local)
	//	> If PageName is empty
	//	> Log Error (SpriteFontLoader::LoadContent > SpriteFont (.fnt): Invalid Font Sprite [Empty])
	//>Retrieve texture filepath from the assetFile path
	//> (ex. c:/Example/somefont.fnt => c:/Example/) [Have a look at: wstring::rfind()]
	//>Use path and PageName to load the texture using the ContentManager [SpriteFont::m_pTexture]
	//> (ex. c:/Example/ + 'PageName' => c:/Example/somefont_0.png)
	//...
    
    char b2Id = pBinReader->Read<char>();
    int b2Size = pBinReader->Read<int>();
    (void)b2Id;
    (void)b2Size;
    
    std::wstring pageName = pBinReader->ReadNullString();
    if(pageName.empty())
    {
        Logger::LogError(L"SpriteFontLoader::LoadContent > SpriteFont (.fnt): Invalid Font Sprite [Empty]");
        return nullptr;
    }
    
    // Construct the path
    std::wstring pagePath = assetFile;
    
    size_t slashPos = pagePath.find_last_of('/');
    if(slashPos >= 0)
    {
        pagePath.erase(slashPos + 1, pagePath.size() - slashPos);
        pagePath += pageName;
    }        
    
    pSpriteFont->m_pTexture = ContentManager::Load<TextureData>(pagePath);
    
	//**********
	// BLOCK 3 *
	//**********
	//Retrieve the blockId and blockSize
	//Retrieve Character Count (see documentation)
	//Retrieve Every Character, For every Character:
	//> Retrieve CharacterId (store Local) and cast to a 'wchar_t'
	//> Check if CharacterId is valid (SpriteFont::IsCharValid), Log Warning and advance to next character if not valid
	//> Retrieve the corresponding FontMetric (SpriteFont::GetMetric) [REFERENCE!!!]
	//> Set IsValid to true [FontMetric::IsValid]
	//> Set Character (CharacterId) [FontMetric::Character]
	//> Retrieve Xposition (store Local)
	//> Retrieve Yposition (store Local)
	//> Retrieve & Set Width [FontMetric::Width]
	//> Retrieve & Set Height [FontMetric::Height]
	//> Retrieve & Set OffsetX [FontMetric::OffsetX]
	//> Retrieve & Set OffsetY [FontMetric::OffsetY]
	//> Retrieve & Set AdvanceX [FontMetric::AdvanceX]
	//> Retrieve & Set Page [FontMetric::Page]
	//> Retrieve Channel (BITFIELD!!!) 
	//	> See documentation for BitField meaning [FontMetrix::Channel]
	//> Calculate Texture Coordinates using Xposition, Yposition, TextureWidth & TextureHeight [FontMetric::TexCoord]
	//...
    
    char b3Id = pBinReader->Read<char>();
    int b3Size = pBinReader->Read<int>();
    (void)b3Id;
    (void)b3Size;
    
    int charCount = b3Size / sizeof(BMF_B3);
    Logger::LogInfo(std::to_wstring(sizeof(BMF_B3)));
    Logger::LogInfo(std::to_wstring(charCount));
    
    auto textureDimentions = pSpriteFont->m_pTexture->GetDimension();
    
    for(int i = 0; i < charCount; ++i)
    {
        BMF_B3 character = pBinReader->Read<BMF_B3>();
        wchar_t wcharId = (wchar_t)character.id;
        
        FontMetric& metric = pSpriteFont->GetMetric(wcharId);
        metric.IsValid = true;
        metric.Character = wcharId;
        metric.Width = character.width;
        metric.Height = character.height;
        metric.OffsetX = character.xOffset;
        metric.OffsetY = character.yOffset;
        metric.AdvanceX = character.xAdvance;
        metric.Page = character.page;
        metric.Channel = (character.channel == 1) ? 2 : (character.channel == 2) ? 1 : (character.channel == 4) ? 0 : 3;
        metric.TexCoord.x = character.x / textureDimentions.x;
        metric.TexCoord.y = character.y / textureDimentions.y;
    }

	delete pBinReader;
	return pSpriteFont;
}

void SpriteFontLoader::Destroy(SpriteFont* objToDestroy)
{
	SafeDelete(objToDestroy);
}
