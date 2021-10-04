/*
@brief	プリプロセッサ
*/
#pragma once


class TitleScene : public SceneBase
{
public:

	/*
	@fn		コンストラクタ
	@param	_nowScene 現在のシーン
	*/
	TitleScene(const Scene& _nowScene);

	/*
	@fn	デストラクタ
	*/
	~TitleScene();

	/*
	@fn	タイトルシーン時に毎フレーム更新処理をする
	*/
	SceneBase* update() override;

	/*
	@fn	タイトルシーン時に毎フレーム入力処理をする
	*/
	void Input(const InputState& _inputState)override;

private:
	SpriteComponent* spriteComponent;

	bool mGameSceneFlag;


};

