#pragma once

/*
* �t�F�[�h�C���A�t�F�[�h�A�E�g�p�N���X
*/
class Fade : public ParticleEffectBase
{
public:
	// �t�F�[�h�C���@or �t�F�[�h�A�E�g
	typedef enum class FadeType :unsigned char
	{
		eIn,	// �C��
		eOut	// �A�E�g
	};

	/// <summary>
	/// �R���X�g���N�^
	/// </summary>
	Fade();
	// �f�X�g���N�^
	~Fade() {};

	/// <summary>
	/// �X�V����
	/// </summary>
	/// <param name="_deltaTime">�Ō�̃t���[������������̂ɗv��������</param>
	void UpdateGameObject(float _deltaTime)override;

	static bool mFadeFlag;	// �t�F�[�h�C���A�t�F�[�h�A�E�g�̍X�V����������t���O true: �X�V���Ă�

private:
	float mFadeSpeed;		// �t�F�[�h������X�s�[�h

public:	// �Q�b�^�[�A�Z�b�^�[
	/// <summary>
	/// �t�F�[�h�C���܂��̓t�F�[�h�A�E�g���n�߂�ݒ������
	/// </summary>
	/// <param name="_color">�F</param>
	/// <param name="_fadeType">�t�F�[�h�C�����t�F�[�h�A�E�g��</param>
	void SetFade(Vector3 _color, FadeType _fadeType);
};