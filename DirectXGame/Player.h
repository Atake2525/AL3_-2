#include "Model.h"
#include "WorldTransform.h"
#include "AABB.h"
#include "Audio.h"

class MapChipField;
class Enemy;
class DeathParticles;

#pragma once
class Player {

public:
	// マップとの当たり判定情報
	struct CollisionMapInfo {
		bool isHeadColFlag = false;
		//bool isGroundColFlag = false;
		bool isWallColFlag = false;
		// 着地フラグ
		bool landing = false;
		bool isWallHit = false;
		Vector3 MovePoint;
	};

	// デスフラグのgetter
	bool IsDead() const { return isDead_; }

	void SetIsDead(bool IsDead) { isDead_ = IsDead; }

	Vector3 GetWorldPosition();

	static inline const float kBlank = 1.0f;

	// 角
	enum Corner {
		kRightBottom,// 右下
		kLeftBottom, // 左下
		kRightTop,   // 右上
		kLeftTop,    // 左上

		kNumCorner   // 要素数
	};

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Model* model, uint32_t textureHandle, ViewProjection* viewProjection, const Vector3& position);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw(Model* model);

	//const ViewProjection* GetViewProjection() { return viewProjection_; }

	const WorldTransform& GetworldTransform()const { return worldTransform_; }

	// プレイヤーの速度を取得するgetter
	const Vector3& GetVelocity() const { return velocity_; }

	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }

	void Move();

	void ReturnMove(const CollisionMapInfo& info);

	void isOverheadCollision(const CollisionMapInfo& info);

	void MapCollisionUpside(CollisionMapInfo& info);
	void MapCollisionUnderside(CollisionMapInfo& info);
	void MapCollisionLeftside(CollisionMapInfo& info);
	void MapCollisionRightside(CollisionMapInfo& info);

	void GoalCollision(CollisionMapInfo& info);

	Vector3 CornerPosition(const Vector3& center, Corner corner);

	// 下に落ちた時の処理(死亡)
	void isUnderFloor();


	// 初期位置に戻す
	void DeadMove();

	void changeOnGround(const CollisionMapInfo& info);

	void goalCollision();

	// AABBを取得
	AABB GetAABB();

	// 衝突応答
	void OnCollision(const Enemy* enemy);

	bool GetGoalFlag() { return goal_; }

	float GetPlayerDeathNum() { return playerDeathNum_; }


private:
	// マップチップによるフィールド
	MapChipField* mapChipField_ = nullptr;
	// ワールド変換データ
	WorldTransform worldTransform_;
	// ビュープロジェクション
	ViewProjection* viewProjection_ = nullptr;
	
	// キャラクターの当たり判定サイズ
	static inline const float kWidth = 0.8f;
	static inline const float kHeight = 0.8f;


	// 着地時の速度減衰率
	static inline const float kAttenuationLoading = 0.1f;
	

	// 移動系
	Vector3 velocity_ = {};
	static inline const float kAcceleration = 0.01f;
	static inline const float kAttenuation = 0.2f;
	static inline const float kLimitRunSpeed = 0.4f;

	// 左右
	enum class LRDirection { 
		kRight,
		kLeft,
	};
	// マップチップ判定時の微小な数値
	float smallNum = 0.1f;
	LRDirection lrDirection_ = LRDirection::kRight;
	// 旋回開始時の角度
	float turnFirstRotationY_ = 0.0f;
	// 旋回タイマー
	float turnTimer_ = 0.0f;
	// 旋回時間<秒>
	static inline const float kTimeTurn = 3.5f;

	// 接地状態フラグ
	bool onGround_ = true;
	// 重力加速度(下方向)
	static inline const float kGravityAcceleration = 0.02f;
	// 最大落下速度(下方向)
	static inline const float kLimitFallSpeed = 5.0f;
	// ジャンプ初速(上方向)
	static inline const float kJumpAcceleration = 0.5f;


	//// ワールド座標を取得
	//Vector3 GetWorldPosition();

	// デスフラグ
	bool isDead_ = false;

	// モデル
	Model* model_ = nullptr;
	// テクスチャハンドル
	uint32_t textureHandle_ = 0u;

	DeathParticles* deathParticles_ = nullptr;


	// ゴール
	float goalTime_ = 0;

	bool goalFlag_ = false;

	bool goal_ = false;



	// プレイヤーの初期位置を取得
	Vector3 firstPos_ = {0};

	// 計算終了か(デスした時の浮遊)
	bool completeDeadMove = false;

	bool DeadMoving_ = false;
	// プレイヤーがデスした回数
	float playerDeathNum_ = 0;

	Vector3 nowPos_ = {0};

	// 初期位置に戻すタイマー
	float deadTimer_ = 0.0f;

	// 初期位置に戻るまでの時間
	float deadTime_ = 1.0f;

};
