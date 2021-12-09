#pragma once


// �Ԃ̃I�u�W�F�N�g
class Rose :public GameObject
{
public:
	// �R���X�g���N�^
	Rose(const Vector3& _pos, const Vector3& _size, const Tag& _objectTag, const SceneBase::Scene _sceneTag);

	/*
	@fn	�f�X�g���N�^
	*/
	~Rose() {};

	// �X�V
	void UpdateGameObject(float _deltaTime)override;

	// �A�C�e�����������ꂽ��
	static int mItemCount;
private:

	//�Q�[���I�u�W�F�N�g�̃��b�V���|�C���^�ϐ�
	MeshComponent* mMeshComponent;

	// �l�p�̒n�ʂ̓����蔻��𐶐�
	BoxCollider* mSelfBoxCollider;

	/*
	@fn �����蔻�肪�s���Hit�����ۂɌĂ΂��֐�
	@param	��������GameObject
	*/
	void OnCollision(const GameObject& _hitObject)override;

	// �v���C���[�Ɠ����������ǂ����̔���
	bool mCollisionFlag;
};
