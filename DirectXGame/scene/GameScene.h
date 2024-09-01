#pragma once

#include "Audio.h"
#include "DirectXCommon.h"
#include "Input.h"
#include "Model.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include <vector>
#include "Matrix4x4.h"
#include "Vector3.h"
#include <cmath>
#include "DebugCamera.h"
#include "Skydome.h"
#include "MathUtilityForText.h"
#include "Player.h"
#include "MapChipField.h"
#include "CameraController.h"
#include "Enemy.h"
#include "AABB.h"
#include "DeathParticles.h"

/// <summary>
/// ゲームシーン
/// </summary>
class GameScene {

public: // メンバ関数
	/// <summary>
	/// コンストクラタ
	/// </summary>
	GameScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameScene();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	// すべての当たり判定を行う
	void CheckAllCollisions();

	// フェーズの切り替え
	void ChangePhase();
	

	std::vector<std::vector<WorldTransform*>> worldTransformBlocks_;
	std::vector<std::vector<WorldTransform*>> worldTransformGoal_;
	std::vector<std::vector<WorldTransform*>> worldTransformSoil_;
	std::vector<std::vector<WorldTransform*>> worldTransformWorldBorder_;

	//ワールドトランスフォーム
	//WorldTransform worldTransform_;
	//ビュープロジェクション
	ViewProjection viewProjection_;

	//デバッグカメラ有効
	bool isDebugCameraActive_ = false;

	//デバッグカメラ
	DebugCamera* debugCamera_ = nullptr;

	// 追従カメラ
	CameraController* cameraController_ = nullptr;

	// デスフラグのgetter
	bool IsFinished() const { return finished_; }
private: // メンバ変数
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;

	Player* player_ = nullptr;

	Model* modelPlayer_ = nullptr;

	Model* model_ = nullptr;

	Model* modelSkydome_ = nullptr;

	Skydome* skyDome_ = nullptr;

	
	uint32_t texturehandle_ = 0;

	std::list<Enemy*> enemies_;

	Model* modelEnemy_ = nullptr;
	// マップチップフィールド
	MapChipField* mapChipField_;

	Model* modelMapChip_ = nullptr;
	Model* goalModel_ = nullptr;
	Model* soilModel_ = nullptr;
	Model* worldBorderModel_ = nullptr;

	void GenerateBlocks();

	// パーティクル
	DeathParticles* deathParticles_ = nullptr;
	Model* deathParticleModel_ = nullptr;

	// ゲームのフェーズ(型)
	enum class Phase {
		kPlay,  // ゲームプレイ
		kDeath, // デス演出
		kClear, // クリア演出
	};

	// ゲームの現在フェーズ(変数)
	Phase phase_;

	bool finished_ = false;

	// sounds
	uint32_t SoundDataHandle_ = 0;

	/// <summary>
	/// ゲームシーン用
	/// </summary>
};
