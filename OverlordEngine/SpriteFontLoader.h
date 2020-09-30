#pragma once
#include "ContentLoader.h"
#include "SpriteFont.h"

// File header
struct BMFHeader
{
    char b;
    char m;
    char f;
    char format;
    
    enum ValidationStatus
    {
        OK,
        WFORMAT,
        OLD
    };
    
    inline ValidationStatus Validate()
    {
        bool header = 
            (b == 'B') && 
            (m == 'M') && 
            (f == 'F');
        
        bool version = (format == 3);
        
        if(!header) return ValidationStatus::WFORMAT;
        if(!version) return ValidationStatus::OLD;
        
        return ValidationStatus::OK;    
    }
};

#define BMF_B2_BACKTRACK -1
struct BMF_B2
{
    short lineHeight;
    short base;
    short scaleW;
    short scaleH;
    short pages;
    char bitField;
    char alphaChannel;
    char redChannel;
    char greenChannel;
    char blueChannel;
    char unused;
};

struct BMF_B3
{
    int id;
    short x;
    short y;
    short width;
    short height;
    short xOffset;
    short yOffset;
    short xAdvance;
    char page;
    char channel;
};

class SpriteFontLoader : public ContentLoader<SpriteFont>
{
public:
	SpriteFontLoader() = default;
	virtual ~SpriteFontLoader() = default;

	SpriteFontLoader(const SpriteFontLoader& other) = delete;
	SpriteFontLoader(SpriteFontLoader&& other) noexcept = delete;
	SpriteFontLoader& operator=(const SpriteFontLoader& other) = delete;
	SpriteFontLoader& operator=(SpriteFontLoader&& other) noexcept = delete;

protected:
	SpriteFont* LoadContent(const std::wstring& assetFile) override;
	void Destroy(SpriteFont* objToDestroy) override;
};

