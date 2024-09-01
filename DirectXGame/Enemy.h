#pragma once
#include "WorldTransform.h"
#include "ViewProjection.h"
#include "Model.h"
#include "AABB.h"

class MapChipField;
class Player;

class Enemy {
public:

	// 角
	enum Corner {
		kRightBottom, // 右下
		kLeftBottom,  // 左下
		kRightTop,    // 右上
		kLeftTop,     // 左上

		kNumCorner // 要素数
	};
	struct CollisionMapInfo {
		// 左壁衝突フラグ
		bool isCollisionLeft = false;
		// 右壁衝突フラグ
		bool isCollisionRight = false;
		// 左下空気確認フラグ
		bool isNoUnderLeft = false;
		// 右下空気確認フラグ
		bool isNoUnderRigh = false;
		// 移動量
		Vector3 movePoint;
	};
	void Initialize(Model* model, ViewProjection* viewProjection, const Vector3& position);

	void Update();

	void Draw();

	// 衝突応答
	void OnCollision(const Player* player);

	// 壁の衝突
	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }

	Vector3 GetWorldPosition();

	AABB GetAABB();

	void MapCollision(CollisionMapInfo& info);

	void isCollisionLeft(CollisionMapInfo& info);
	void isCollisionRight(CollisionMapInfo& info);
	void isCollisionUnderLeft(CollisionMapInfo& info);
	void isCollisionUnderRight(CollisionMapInfo& info);

	Vector3 CornerPosition(const Vector3& center, Corner corner);

	bool IsAlive() { return isAlive; }


	private:

	WorldTransform worldTransform_;

	ViewProjection* viewProjection_ = nullptr;

	Model* model_ = nullptr;

	static inline const float kWidth = 0.8f;
	static inline const float kHeight = 0.8f;

	// 歩行の速さ
	static inline const float kWalkSpeed = 0.02f;

	// 速度
	Vector3 velocity_ = {};

	// 経過時間
	float walkTimer_ = 0.0f;

	// 最初の角度[度]
	static inline const float kWalkMotionAngleStart = 20.0f;
	// 最後の角度[度]
	static inline const float kWalkMotionAngleEnd = 45.0f;
	// アニメーションの周期となる時間[秒]
	static inline const float kWalkMotionTime = 2.0f;

	// 左右
	enum class LRDirection {
		kRight,
		kLeft,
	};
	// 0 : Left  1 : Right
	float MoveDirection = 0;
	// マップチップ判定時の微小な数値
	float smallNum = 0.1f;
	LRDirection lrDirection_ = LRDirection::kLeft;
	// 旋回開始時の角度
	float turnFirstRotationY_ = 0.0f;
	// 旋回タイマー
	float turnTimer_ = 0.0f;
	// 旋回時間<秒>
	static inline const float kTimeTurn = 1.0f;

	bool isAlive = true;

	MapChipField* mapChipField_ = nullptr;
	Player* player_ = nullptr;

};
