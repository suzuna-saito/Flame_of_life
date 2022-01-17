/*
@brief	インクルード
*/
#include "pch.h"

/*
@fn		コンストラクタ
@param	_nowScene 現在のシーン
*/
Tutorial::Tutorial(const Scene& _nowScene)
	:SceneBase()
{
	SetScene(_nowScene);

	mMapCreate = new MapCreate();

	if (!mMapCreate->OpenFile())
	{
		// 床の生成
		mMapCreate->CreateGround();
		// スイッチの生成
		mMapCreate->CreateSwitch();
		// ろうそくの生成
		mMapCreate->CreateCandle();
		// プレイヤーの生成
		mMapCreate->CreatePlayer();
	}

	// カウントアップ
	new CountUp(_nowScene);
}

/*
@fn	デストラクタ
*/
Tutorial::~Tutorial()
{
	GAME_OBJECT_MANAGER->RemoveGameObjects(Scene::tutorial);

	delete mMapCreate;
}

void Tutorial::Input(const InputState& _state)
{
	// 当たり判定表示モードの切り替え
	if (_state.m_keyboard.GetKeyState(SDL_SCANCODE_0) == ButtonState::Pressed)
	{
		PHYSICS->ToggleDebugMode();
	}
}

/*
@fn	現在のシーン時に毎フレーム更新処理をする
*/
SceneBase* Tutorial::update()
{
	// プレイヤーがろうそくにたどり着いたら
	if (Candle::mCandleCount == 1)
	{
		// 遷移フラグをtrueにする
		mGameSceneFlag = true;
	}

	// 遷移フラグがtrueだったら
	if (mGameSceneFlag)
	{
		// カウントを減らす
		mNextSceneCount--;
		// カウントが0以下になったら
		if (mNextSceneCount <= 0)
		{
			return new HardStage(Scene::easy);
		}
	}

	CountUp::SetCountStartFlag(true);

	return this;
}