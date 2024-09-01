#include "DeathParticles.h"
#include "cassert"
#include <algorithm>

void DeathParticles::Initialize(Model* model, ViewProjection* viewProjection, const Vector3& position) {
	assert(model);

	model_ = model;
	viewProjection_ = viewProjection;
	objectColor_.Initialize();
	color_ = {1, 1, 1, 1};

	// ワールド変換の初期化
	for (auto& worldTransform : worldTransforms_) {
		worldTransform.Initialize();
		worldTransform.translation_ = position;
	}

}

void DeathParticles::Update() {
	for (auto& worldTransform : worldTransforms_) {
		worldTransform.UpdateMatrix();
	}
	// 終了なら何もしない
	if (isFinished_) { return; }
	for (uint32_t i = 0; i < kNumParticles; i++) {
		// 基本となる速度ベクトル
		Vector3 velocity = {kSpeed, 0 , 0};
		// 回転角を計算する
		float angle = kAngleUnit * i;
		// Z軸回り回転行列
		Matrix4x4 matrixRotation = MakeRotateZMatrix(angle);
		// 基本ベクトルを回転させて速度ベクトルを得る
		velocity = MathTransform(velocity, matrixRotation);
		// 移動量処理
		worldTransforms_[i].translation_ += velocity;
		worldTransforms_[i].rotation_.y += velocity.x  * 3;
	}
	// カウンターを1フレーム分の秒数進める
	counter_ += 1.0f / 60.0f;

	color_.w = std::clamp(1.0f - counter_, 0.0f, 1.0f);
	// 色変更オブジェクトに色の数値を設定する
	objectColor_.SetColor(color_);
	// 色変更オブジェクトをVRAMに転送
	objectColor_.TransferMatrix();

	// 存続時間の上限に達したら
	if (counter_ >= kDuration) {
		counter_ = kDuration;
		// 終了扱いにする
		isFinished_ = true;
		counter_ = 0.0f;
		color_.w = 1.0f;
	}
}

void DeathParticles::Draw(Model* model) {
	if (isFinished_) {
		return;
	}
	for (auto& worldTransform : worldTransforms_) {
		model->Draw(worldTransform, *viewProjection_, &objectColor_);
	}
}