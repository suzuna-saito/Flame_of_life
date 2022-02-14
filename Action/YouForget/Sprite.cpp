/*
@brief	�C���N���[�h
*/
#include "pch.h"

/*
@fn		�R���X�g���N�^
@param	_fileName �摜�t�@�C���̖��O
*/
Sprite::Sprite(const std::string _fileName)
	:GameObject(SceneBase::Scene::other, Tag::UI )
{
	mSpriteComponent = new SpriteComponent(this);

	mSpriteTexture = RENDERER->GetTexture(_fileName);
	mSpriteComponent->SetTexture(mSpriteTexture);
}

/*
@fn		�w��̃t�@�C���̕`�������
*/
void Sprite::NotVisible()
{
	mSpriteComponent->SetVisible(false);
}