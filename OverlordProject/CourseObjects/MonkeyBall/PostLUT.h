#ifndef POST_LUT_H
#define POST_LUT_H

#include "PostProcessingMaterial.h"

class TextureData;
class ID3D11EffectShaderResourceVariable;

class PostLUT : public PostProcessingMaterial
{
public:
	PostLUT();
	PostLUT(const PostLUT& other) = delete;
	PostLUT(PostLUT&& other) noexcept = delete;
	PostLUT& operator=(const PostLUT& other) = delete;
	PostLUT& operator=(PostLUT&& other) noexcept = delete;
	virtual ~PostLUT() = default;

protected:
	void LoadEffectVariables() override;
	void UpdateEffectVariables(RenderTarget* pRendertarget) override;

private:
	ID3DX11EffectShaderResourceVariable* m_pTextureMapVariabele;
	ID3DX11EffectShaderResourceVariable* m_pLut;

	TextureData* m_pLutTexture;
};

#endif // !POST_LUST_H