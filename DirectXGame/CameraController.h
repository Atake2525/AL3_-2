#pragma once
#include "ViewProjection.h"
#include "WorldTransform.h"

// 前方宣言
class Player;

class CameraController {

public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	void Reset();

	void SetTrarget(Player* target) { target_ = target; }

	const ViewProjection& GetViewProjection() { return viewProjection_; }

	// 矩形
	struct Rect {
		float left = 0.0f;   // 左端
		float right = 1.0f;  // 右端
		float bottom = 0.0f; // 下端
		float top = 1.0f;    // 上端
	};

	// 追従の各方向へのカメラ移動範囲
	static inline const Rect magine_ = {-9.0f, 9.0f, 0.0f, 9.0f};

	// カメラ移動範囲
	Rect movableArea_ = {5, 100, 0, 100};

	// カメラの目標座標
	Vector3 movePoint_;

	// 座標保管割合   実行後調整する
	static inline const float kInterpolationRate = 0.3f;

	// 速度掛け率
	static inline const float kVelocityBias = 5;

	void SetMovableArea(Rect area) { movableArea_ = area; }

private:
	// ビュープロジェクション
	ViewProjection viewProjection_;
	Player* target_ = nullptr;
	// 追従対象とカメラの座標の差(オフセット)
	Vector3 targetOffset_ = {0, 0, -20.0f};

	// オフセット拡大タイマー
	float sumTimer_ = 0.0f;

	// 拡大までの時間
	float sumTime_ = 1.2f;
};
