/*
@brief	�v���v���Z�b�T
*/
#include "pch.h"

//���݂̃V�[���̏�����
SceneBase::Scene SceneBase::mIsScene = Scene::other;

/*
@fn	�R���X�g���N�^
*/
SceneBase::SceneBase()
	: mSprite(nullptr)
	, mMapCreate(nullptr)
	, mClearFlag(false)
	, mGameSceneFlag(false)
	, mNextSceneCount(0)
{
	//����
	const Vector3 AmbientLight = Vector3(0.4f, 0.4f, 0.4f);

	// ���C�g��ݒ�(�ݒ肵�Ȃ��Ɖ����f��Ȃ�)
	RENDERER->SetAmbientLight(Vector3(0.8f, 0.8f, 0.8f));
	DirectionalLight& dir = RENDERER->GetDirectionalLight();
	dir.m_direction = Vector3(0.0f, 0.0f, 0.0f);
	dir.m_diffuseColor = Vector3(1.0f, 1.0f, 1.0f);
	//dir.m_diffuseColor = Vector3(0.5f, 0.5f, 0.8f);
	dir.m_specColor = Vector3(0.8f, 0.8f, 0.8f);
}