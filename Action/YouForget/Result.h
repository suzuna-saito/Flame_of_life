#pragma once

class Result :public ResultBase
{
public:

	/*
	@fn		�R���X�g���N�^
	@param	_nowScene ���݂̃V�[��
	*/
	Result(const Scene& _nowScene);

	/*
	@fn	�f�X�g���N�^
	*/
	~Result();

	/*
	@fn	�^�C�g���V�[�����ɖ��t���[���X�V����������
	*/
	SceneBase* update() override;

	/*
	@fn	�^�C�g���V�[�����ɖ��t���[�����͏���������
	*/
	void Input(const InputState& _inputState)override;
};