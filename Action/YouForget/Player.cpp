/*
@brief	インクルード
*/
#include "pch.h"

// 静的メンバ
// プレイヤーが操作可能かどうか　true _可能 false _不可能
bool Player::mOperable = true;

bool Player::mMoveFlag = false;

/*
@fn		コンストラクタ
@param	_pos プレイヤーの座標
@param	_size プレイヤーのサイズ
@param	_objectTag プレイヤーのタグ
@param	_sceneTag シーンのタグ
*/
Player::Player(const Vector3& _pos, const Vector3& _size, const ObjTag& _objectTag, const SceneBase::SceneType _sceneTag)
	: GameObject(ObjTag::ePlayer)
	, mCameraPos(Vector3(0, -1400, _pos.z + 1800.0f))
	, mReturnPos(_pos)
	, mDifference(Vector3::Zero)
	, MStartCameraPos(2000.0f)
	, MAddCameraPos(100.0f)
	, mCameraPointX(_pos.x)
	, MCameraPointZ(90.0f)
	, MinitCameraPointX(_pos.x)
	, MChagePosZ(70.0f)
	, MRedoingPosZ(-700.0f)
	, MReturnAddZ(100.0f)
	, MRedoingSpeedZ(900.0f)
	, MMaxJumpVel(1700.0f)
	, mStartFlag(true)
	, mVibrationFlag(false)
	, mNowState(playerState::idle)
	, mPrevState(playerState::idle)
{
	//GameObjectメンバ変数の初期化
	mTag = _objectTag;
	SetScale(_size);
	SetPosition(_pos);

	//生成したPlayerの生成時と同じくComponent基底クラスは自動で管理クラスに追加され自動で解放される
	mSkelComp = new SkeletalMeshComponent(this);
	//Rendererクラス内のMesh読み込み関数を利用してMeshをセット(.gpmesh)
	mSkelComp->SetMesh(RENDERER->GetMesh("Assets/Player/doll.gpmesh"));

	// スケルトンの読み込み
	mSkelComp->SetSkeleton(RENDERER->GetSkeleton("Assets/Player/doll.gpskel"));

	// アニメーションの読み込み
	mAnimations.resize((int)playerState::stateNum); // 配列を確保（mAnimationsはvector）(STSTE_NUMはenumの中の数)
	mAnimations[(int)playerState::idle] = RENDERER->GetAnimation("Assets/Player/idle.gpanim");
	mAnimations[(int)playerState::run] = RENDERER->GetAnimation("Assets/Player/run.gpanim");

	//アニメーションの再生
	mSkelComp->PlayAnimation(mAnimations[(int)playerState::idle]);

	// エフェクト
	// プレイヤーの丸影エフェクト生成
	new CircledShadow(this,Color::LightPink);
	// プレイヤーが落ちた時のエフェクト
	mFallEffect = new FallEffect(this);


	//プレイヤー自身の当たり判定(ボックス)
	mSelfBoxCollider = new BoxCollider(this, GameObject::ObjTag::ePlayer, GetOnCollisionFunc());
	AABB box = { Vector3(750.0f,-750.0f,0.0f),Vector3(-750.0f,750.0f,3500.0f) };
	mSelfBoxCollider->SetObjectBox(box);


	//回転処理                        ↓回転する値
	float radian = Math::ToRadians(90.0f);
	Quaternion rot = this->GetRotation();
	Quaternion inc(Vector3::UnitZ, radian);
	Quaternion target = Quaternion::Concatenate(rot, inc);
	SetRotation(target);

	// 足元当たり判定の生成
	mLegs = new LegsCollider(this, ObjTag::ePlayerLegs, _sceneTag);

	// ジャンプを追加
	mJump = new Jump(this);


	// でバック用 //
	mDebug = false;
	mCount = 0;
}

/*
@fn		プレイヤーのアップデート
@param	_deltaTime 最後のフレームを完了するのに要した時間
*/
void Player::UpdateGameObject(float _deltaTime)
{
	//// カメラを一定の高さまで持ってくる
	//if (mCameraPos.z <= MStartCameraPos && !mStartFlag)
	//{
	//	mCameraPos.z += MAddCameraPos* _deltaTime;
	//}
	//else if (mCameraPos.z >= MStartCameraPos)
	//{
	//	// 一定の高さまで来たら行動できるようにする
	//	mOperable = true;
	//	mStartFlag = true;
	//}
	
	if (mPosition.z >= MChagePosZ)
	{
		Vector3 distance = Vector3::Zero;

		// 距離ベクトル
		distance.x = mCameraPointX - MinitCameraPointX;

		// 1に近かったら
		if (!Math::NearZero(distance.Length(), 5.0f))
		{
			Vector3 difference = Vector3::Zero;
			Vector3 camera = Vector3(mCameraPointX, 0.0f, 0.0f);
			Vector3 pos = Vector3(MinitCameraPointX, 0.0f, 0.0f);
			// 現在のポジションとリスポーン地点の座標を用いて線形補間を行い、次の座標を計算する
			difference = Vector3::Lerp(camera, pos, 0.05f);

			mCameraPointX = difference.x;
		}

		//プレイヤーを見下ろす位置にカメラをセット
		mMainCamera->SetViewMatrixLerpObject(mCameraPos, Vector3(mCameraPointX, mPosition.y + 50.0f, MCameraPointZ));
	}
	else
	{
		Vector3 distance = Vector3::Zero;

		// 距離ベクトル
		distance.x = mCameraPointX - mPosition.x;

		// 1に近かったら
		if (!Math::NearZero(distance.Length(), 5.0f))
		{
			Vector3 difference = Vector3::Zero;
			Vector3 camera = Vector3(mCameraPointX, 0.0f, 0.0f);
			Vector3 pos = Vector3(mPosition.x, 0.0f, 0.0f);
			// 現在のポジションとリスポーン地点の座標を用いて線形補間を行い、次の座標を計算する
			difference = Vector3::Lerp(camera, pos, 0.05f);

			mCameraPointX = difference.x;
		}
		//プレイヤーを見下ろす位置にカメラをセット
		mMainCamera->SetViewMatrixLerpObject(mCameraPos, Vector3(mCameraPointX, mPosition.y, mPosition.z));
	}

	//プレイヤーを横から見る位置にカメラをセット
	//mMainCamera->SetViewMatrixLerpObject(Vector3(1000, 0, 200), mPosition);
	// デバック用
	//mMainCamera->SetViewMatrixLerpObject(Vector3(0, -1000, 200), mPosition + testPos);


	// 現在の状態からそれぞれのふるまいを行う関数にジャンプ
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

	// ジャンプしてたらジャンプ力を足す
	if (mJump->GetJumpFlag())
	{
		mMoveSpeed = 830.0f;
		mVelocity.z += mJump->GetVelocity();
		mLegs->SetIsGround(false);
	}

	// velocityが一定数まで行ったら、ジャンプ力をなくす
	if (mVelocity.z >= MMaxJumpVel)
	{
		mGravity = 50.0f;

		mJump->SetEndJump(true);
	}

	if (mGravity <= 90.0f)
	{
		mGravity += 1.0f;
	}

	// 重力
	if (!mLegs->GetIsGround() && !mDebug && mOperable)
	{
		mVelocity.z -= mGravity;
	}
	else if (mOperable)
	{
		mVelocity.z = 0.0f;
	}


	// 座標をセット
	mPosition += (mVelocity + mInputSpeed) * _deltaTime;

	// 状態が切り替わったらアニメーションを開始
	if (mNowState != mPrevState)
	{
		mSkelComp->PlayAnimation(mAnimations[(int)mNowState], 0.5f);
	}

	if (!mLegs->GetIsGround())
	{
		ComputeWorldTransform();
	}

	//このフレームのステートは1つ前のステートになる
	mPrevState = mNowState;


	// プレイヤーが落ちたら
	if (mPosition.z <= MRedoingPosZ && mOperable)
	{
		// 振動フラグをtrueにする
		mVibrationFlag = true;
		// 落ちた時のエフェクトの描画
		mFallEffect->SetThisVisible(true);
		// 動作が出来なくする
		mOperable = false;

		mVelocity = Vector3::Zero;
	}

	if (!mOperable)
	{
		// 復帰位置まで移動させる
		mRedoing(mPosition, mReturnPos);
	}


	// ポジションをセット
	SetPosition(mPosition);
	mLegs->SetIsGround(false);
}

/*
@fn		入力を引数で受け取る更新関数
@brief	基本的にここで入力情報を変数に保存しUpdateGameObjectで更新を行う
@param	_keyState 各入力機器の入力状態
@brief	キーボード、マウス、コントローラー
*/
void Player::GameObjectInput(const InputState& _keyState)
{
	// 向いてほしい向きのベクトル
	Vector3 inputVec = Vector3::Zero;
	mInputSpeed = Vector3::Zero;

	if (mOperable)
	{
		// Wで奥に移動
		if (_keyState.m_controller.GetLAxisVec().y < -0.5f ||
			_keyState.m_keyboard.GetKeyValue(SDL_SCANCODE_W))
		{
			inputVec.y = 1.0f;
		}
		// Sで手前に移動
		else if (_keyState.m_controller.GetLAxisVec().y > 0.5f ||
			_keyState.m_keyboard.GetKeyValue(SDL_SCANCODE_S))
		{
			inputVec.y = -1.0f;
		}

		// Aで左に移動
		if (_keyState.m_controller.GetLAxisVec().x < -0.5f ||
			_keyState.m_keyboard.GetKeyValue(SDL_SCANCODE_A))
		{
			inputVec.x = 1.0f;
		}
		// Dで右に移動
		else if (_keyState.m_controller.GetLAxisVec().x > 0.5f ||
			_keyState.m_keyboard.GetKeyValue(SDL_SCANCODE_D))
		{
			inputVec.x = -1.0f;
		}

		// Aボタンかスペースでジャンプ
		if ((_keyState.m_controller.GetButtonState(SDL_CONTROLLER_BUTTON_A) == ButtonState::Pressed ||
			_keyState.m_keyboard.GetKeyState(SDL_SCANCODE_SPACE) == ButtonState::Pressed) &&
			mLegs->GetIsGround())
		{
			mJump->SetJumpStart(true);
		}

		// いずれかが押されていたら
		if (inputVec != Vector3::Zero)
		{
			mNowState = playerState::run;

			// 入力ベクトルの正規化
			inputVec.Normalize();
			// 移動
			mInputSpeed = inputVec * mMoveSpeed;
			mMoveFlag = true;
			mCount = 0;
		}
		else
		{
			mNowState = playerState::idle;
			if (mCount < 80)
			{
				mCount++;
			}
			else
			{
				mMoveFlag = false;
			}
		}

		// アニメーションする方向
		mAnimVec = inputVec;
	}
	else
	{
		if (mCount < 80)
		{
			mCount++;
		}
		else
		{
			mMoveFlag = false;
		}
	}
	/*else if (mVibrationFlag)
	{
		SDL_GameControllerRumble(_keyState.m_controller)
	}*/
}

/*
@fn		矩形と矩形の押し戻し
@param	_myAABB	基準にするオブジェクトの矩形当たり判定
@param	_pairAABB ヒットするオブジェクトの矩形当たり判定
@param	_pairTag ヒットするオブジェクトのタグ
*/
void Player::FixCollision(const AABB& _myAABB, const AABB& _pairAABB, const ObjTag& _pairTag)
{
	Vector3 ment = Vector3::Zero;
	CalcCollisionFixVec(_myAABB, _pairAABB, ment);

	SetPosition(mPosition + ment);
}

/*
@fn		プレイヤーがヒットした時の処理
@param	_hitObject ヒットした対象のゲームオブジェクトのアドレス
*/
void Player::OnCollision(const GameObject& _hitObject)
{
	//ヒットしたオブジェクトのタグを取得
	ObjTag hitObjectTag = _hitObject.GetTag();

	if (mOperable &&
		hitObjectTag == ObjTag::eGround)
	{
		// 押し戻し
		FixCollision(mSelfBoxCollider->GetWorldBox(), _hitObject.GetAabb(), mTag);

		mMoveSpeed = 800.0f;
	}
}


// 復帰位置まで移動させる
void Player::mRedoing(Vector3 _nowPos, const Vector3 _returnPos)
{
	Vector3 returnPos = Vector3::Zero;

	returnPos = _returnPos;
	returnPos.z += MReturnAddZ;
	// 浮上するとき
	if (mPosition.z < returnPos.z)
	{
		mVelocity.z = MRedoingSpeedZ;
	}
	// 浮上した後
	else
	{
		mVelocity.z = 0.0f;

		// 現在のポジションとリスポーン地点の座標を用いて線形補間を行い、次の座標を計算する
		mDifference = Vector3::Lerp(_nowPos, returnPos, 0.05f);

		mPosition = mDifference;
	}

	Vector3 distance = Vector3::Zero;

	// 距離ベクトル
	distance = returnPos - mPosition;

	// 1に近かったら
	if (Math::NearZero(distance.Length(), 5.0f))
	{
		// 行動を可能にする
		mOperable = true;
	}

}


/*
アニメーション
*/

// アイドル状態の処理
void Player::mIdleBehavior()
{
	// 初めてこのステートに入る場合
	if (mNowState != mPrevState)
	{
		// 待機アニメーション再生開始
		mPrevState = mNowState;
		mSkelComp->PlayAnimation(mAnimations[(int)playerState::idle], 0.5f);
	}
}

// ラン状態の処理
void Player::mRunBehavior()
{
	// 初めてこのステートに入る場合
	if (mNowState != mPrevState)
	{
		mPrevState = mNowState;
		mSkelComp->PlayAnimation(mAnimations[(int)playerState::run], 1.0f);
	}

	// 回転処理
	RotateToNewForward(mAnimVec);
}