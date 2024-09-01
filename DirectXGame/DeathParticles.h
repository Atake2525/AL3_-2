#pragma once
#include "WorldTransform.h"
#include "Model.h"
#include "ViewProjection.h"
#include "array"
#include "MathUtilityForText.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <cmath>

class DeathParticles {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Model* model, ViewProjection* viewProjection, const Vector3& position);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw(Model* model);

	bool IsFinished() const { return isFinished_; }

	void SetIsFinished(bool isFinished) { isFinished_ = isFinished; }

	private:
	Model* model_ = nullptr;

	ViewProjection* viewProjection_;

	// パーティクルの個数
	static inline const uint32_t kNumParticles = 8;

	// 存続時間(消滅までの時間) <秒>
	static inline const float kDuration = 0.8f;

	// 移動の速さ
	static inline const float kSpeed = 0.04f;

	// 分割数
	static inline const float kPartitionParticles = float(kNumParticles);

	// 終了フラグ
	bool isFinished_ = false;
	// 経過時間カウント
	float counter_ = 0.0f;

	// 色変更オブジェクト
	ObjectColor objectColor_;
	// 色の数値
	Vector4 color_;
	
	// 分割した1個分の角度
	static inline const float kAngleUnit = (2 * float(M_PI)) / kPartitionParticles;

	std::array<WorldTransform, kNumParticles> worldTransforms_;

};
