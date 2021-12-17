/*
@brief	�C���N���[�h
*/
#include "pch.h"


/*
@fn		�R���X�g���N�^
@param	_pos �v���C���[�̍��W
@param	_size �v���C���[�̃T�C�Y
@param	_objectTag �v���C���[�̃^�O
@param	_sceneTag �V�[���̃^�O
*/
Player::Player(const Vector3& _pos, const Vector3& _size, const Tag& _objectTag, const SceneBase::Scene _sceneTag)
	: GameObject(_sceneTag, _objectTag)
	, MCameraPos(Vector3(0, -1200, 800))
	, MPointZ(66.0f)
	, mNowState(playerState::idle)
	, mPrevState(playerState::idle)
{
	//GameObject�����o�ϐ��̏�����
	mTag = _objectTag;
	SetScale(_size);
	SetPosition(_pos);

	//��������Player�̐������Ɠ�����Component���N���X�͎����ŊǗ��N���X�ɒǉ����ꎩ���ŉ�������
	mSkelComp = new SkeletalMeshComponent(this);
	//Renderer�N���X����Mesh�ǂݍ��݊֐��𗘗p����Mesh���Z�b�g(.gpmesh)
	mSkelComp->SetMesh(RENDERER->GetMesh("Assets/Player/doll.gpmesh"));

	// �X�P���g���̓ǂݍ���
	mSkelComp->SetSkeleton(RENDERER->GetSkeleton("Assets/Player/doll.gpskel"));

	// �A�j���[�V�����̓ǂݍ���
	mAnimations.resize((int)playerState::stateNum); // �z����m�ہimAnimations��vector�j(STSTE_NUM��enum�̒��̐�)
	mAnimations[(int)playerState::idle] = RENDERER->GetAnimation("Assets/Player/idle.gpanim");
	mAnimations[(int)playerState::run] = RENDERER->GetAnimation("Assets/Player/run.gpanim");

	//�A�j���[�V�����̍Đ�
	mSkelComp->PlayAnimation(mAnimations[(int)playerState::idle]);

	//�v���C���[���g�̓����蔻��(�{�b�N�X)
	mSelfBoxCollider = new BoxCollider(this, ColliderTag::playerTag, GetOnCollisionFunc());
	AABB box = { Vector3(750.0f,-750.0f,300.0f),Vector3(-750.0f,750.0f,3500.0f) };
	mSelfBoxCollider->SetObjectBox(box);


	//��]����                        ����]����l
	float radian = Math::ToRadians(90.0f);
	Quaternion rot = this->GetRotation();
	Quaternion inc(Vector3::UnitZ, radian);
	Quaternion target = Quaternion::Concatenate(rot, inc);
	SetRotation(target);

	// ���������蔻��̐���
	mLegs = new LegsCollider(this, _objectTag, _sceneTag);

	// �W�����v��ǉ�
	mJump = new Jump(this);

	// �Ńo�b�N�p //
	mDebug = false;
}

/*
@fn		�v���C���[�̃A�b�v�f�[�g
@param	_deltaTime �Ō�̃t���[������������̂ɗv��������
*/
void Player::UpdateGameObject(float _deltaTime)
{

	//�v���C���[�������낷�ʒu�ɃJ�������Z�b�g
	mMainCamera->SetViewMatrixLerpObject(MCameraPos, Vector3(mPosition.x,mPosition.y, MPointZ));
	//�v���C���[�������猩��ʒu�ɃJ�������Z�b�g
	//mMainCamera->SetViewMatrixLerpObject(Vector3(300, 0, 200), mPosition);
	// �f�o�b�N�p
	//mMainCamera->SetViewMatrixLerpObject(Vector3(0, -1000, 200), mPosition + testPos);


	// ���݂̏�Ԃ��炻�ꂼ��̂ӂ�܂����s���֐��ɃW�����v
	switch (mNowState)
	{
	case playerState::idle:
		mIdleBehavior();
		break;

	case playerState::run:
		mRunBehavior();
		break;

	default:
		break;
	}

	// �W�����v���Ă���W�����v�͂𑫂�
	if (mJump->GetJumpFlag())
	{
 		mVelocity.z += mJump->GetVelocity();
	}

	
	// �d��
 	if (!mLegs->GetIsGround() && !mDebug)
	{
		mVelocity.z -= MGravity/* * _deltaTime*/;
	}
	/*else
	{
		mVelocity.z = 0.0f;
	}*/

	// ���W���Z�b�g
	mPosition += mVelocity * _deltaTime;

	// ��Ԃ��؂�ւ������A�j���[�V�������J�n
	if (mNowState != mPrevState)
	{
		mSkelComp->PlayAnimation(mAnimations[(int)mNowState], 0.5f);
	}

	if (!mLegs->GetIsGround())
	{
		ComputeWorldTransform();
	}

	//���̃t���[���̃X�e�[�g��1�O�̃X�e�[�g�ɂȂ�
	mPrevState = mNowState;

	
	// �f�o�b�N���[�h
	if (mPosition.z < -300.0f)
	{
		mPosition.z = 300.0f;
		mDebug = true;
	}

	// �|�W�V�������Z�b�g
	SetPosition(mPosition);
	mLegs->SetIsGround(false);
}

/*
@fn		���͂������Ŏ󂯎��X�V�֐�
@brief	��{�I�ɂ����œ��͏���ϐ��ɕۑ���UpdateGameObject�ōX�V���s��
@param	_keyState �e���͋@��̓��͏��
@brief	�L�[�{�[�h�A�}�E�X�A�R���g���[���[
*/
void Player::GameObjectInput(const InputState& _keyState)
{
	// �����Ăق��������̃x�N�g��
	Vector3 inputVec = Vector3::Zero;

	// W�ŉ��Ɉړ�
	if (_keyState.m_keyboard.GetKeyValue(SDL_SCANCODE_W))
	{
		inputVec.y = 1.0f;
		mVelocity.y = mMoveSpeed;
	}
	// S�Ŏ�O�Ɉړ�
	else if (_keyState.m_keyboard.GetKeyValue(SDL_SCANCODE_S))
	{
		inputVec.y = -1.0f;
		mVelocity.y = -mMoveSpeed;
	}
	else
	{
		mVelocity.y = 0.0f;
	}
	// A�ō��Ɉړ�
	if (_keyState.m_keyboard.GetKeyValue(SDL_SCANCODE_A))
	{
		inputVec.x = 1.0f;
		mVelocity.x = mMoveSpeed;
	}
	// D�ŉE�Ɉړ�
	else if (_keyState.m_keyboard.GetKeyValue(SDL_SCANCODE_D))
	{
		inputVec.x = -1.0f;
		mVelocity.x = -mMoveSpeed;
	}
	else
	{
		mVelocity.x = 0.0f;
	}

	// �X�y�[�X�ŃW�����v
	if (_keyState.m_keyboard.GetKeyState(SDL_SCANCODE_SPACE) == ButtonState::Pressed &&
		mLegs->GetIsGround())
	{
		mJump->SetJumpStart(true);
	}

	// awsd�̂����ꂩ��������Ă�����
	if (_keyState.m_keyboard.GetKeyValue(SDL_SCANCODE_W) || _keyState.m_keyboard.GetKeyValue(SDL_SCANCODE_S) ||
		_keyState.m_keyboard.GetKeyValue(SDL_SCANCODE_A) || _keyState.m_keyboard.GetKeyValue(SDL_SCANCODE_D))
	{
		mNowState = playerState::run;
	}
	else
	{
		mNowState = playerState::idle;
		mVelocity = Vector3::Zero;
	}

	// ���̓x�N�g���̐��K��
	inputVec.Normalize();

	mAnimVec = inputVec;

	///// �Ńo�b�N�p //////
	if (_keyState.m_keyboard.GetKeyState(SDL_SCANCODE_B) == ButtonState::Released)
	{
		if (!mDebug)
		{
			mDebug = true;
		}
		else
		{
			mDebug = false;
		}
	}

	if (_keyState.m_keyboard.GetKeyValue(SDL_SCANCODE_UP) && mDebug)
	{
		mVelocity.z = mMoveSpeed;
	}
	else if (_keyState.m_keyboard.GetKeyValue(SDL_SCANCODE_DOWN) && mDebug)
	{
		mVelocity.z = -mMoveSpeed;
	}
	else
	{
		mVelocity.z = 0.0f;
	}
}

/*
@fn		��`�Ƌ�`�̉����߂�
@param	_myAABB	��ɂ���I�u�W�F�N�g�̋�`�����蔻��
@param	_pairAABB �q�b�g����I�u�W�F�N�g�̋�`�����蔻��
@param	_pairTag �q�b�g����I�u�W�F�N�g�̃^�O
*/
void Player::FixCollision(const AABB& _myAABB, const AABB& _pairAABB, const Tag& _pairTag)
{
	Vector3 ment = Vector3::Zero;
	CalcCollisionFixVec(_myAABB, _pairAABB, ment);

	SetPosition(mPosition + ment);
}

/*
@fn		�v���C���[���q�b�g�������̏���
@param	_hitObject �q�b�g�����Ώۂ̃Q�[���I�u�W�F�N�g�̃A�h���X
*/
void Player::OnCollision(const GameObject& _hitObject)
{
	//�q�b�g�����I�u�W�F�N�g�̃^�O���擾
	mTag = _hitObject.GetTag();

	//// �A�C�e���A�낤�����ȊO�Ɛݒu�����Ƃ�
	if (mTag != Tag::item &&
		mTag != Tag::candle)
	{
		// �����߂�
		FixCollision(mSelfBoxCollider->GetWorldBox(), _hitObject.GetAabb(), mTag);
	}
}


/*
�A�j���[�V����
*/

// �A�C�h����Ԃ̏���
void Player::mIdleBehavior()
{
	// ���߂Ă��̃X�e�[�g�ɓ���ꍇ
	if (mNowState != mPrevState)
	{
		// �ҋ@�A�j���[�V�����Đ��J�n
		mPrevState = mNowState;
		mSkelComp->PlayAnimation(mAnimations[(int)playerState::idle], 0.5f);
	}
}

// ������Ԃ̏���
void Player::mRunBehavior()
{
	// ���߂Ă��̃X�e�[�g�ɓ���ꍇ
	if (mNowState != mPrevState)
	{
		mPrevState = mNowState;
		mSkelComp->PlayAnimation(mAnimations[(int)playerState::run], 1.0f);
	}

	// ��]����
	RotateToNewForward(mAnimVec);
}