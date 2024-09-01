#include "CameraController.h"
#include "MathUtilityForText.h"
#include "Player.h"
#include <algorithm>
#define NOMINMAX

void CameraController::Initialize() {
	viewProjection_.Initialize();
	//movableArea_ = {0, 100, 0, 100};
};

void CameraController::Update() {
	// 追従対象のワールドトランスフォームを参照
	const WorldTransform& targetWorldTransform = target_->GetworldTransform();
	const Vector3& targetVelocity = target_->GetVelocity();
	// 追従対象とオフセットと追従対象の速度カメラの目標座標を計算
	movePoint_ = targetWorldTransform.translation_ + targetOffset_ + targetVelocity * kVelocityBias;

	// 座標保管によりゆったり追従
	viewProjection_.translation_ = Lerp(viewProjection_.translation_, movePoint_, kInterpolationRate);

	// 移動範囲制限
	viewProjection_.translation_.x = (std::max)(viewProjection_.translation_.x, movableArea_.left);
	viewProjection_.translation_.x = (std::min)(viewProjection_.translation_.x, movableArea_.right);
	viewProjection_.translation_.y = (std::max)(viewProjection_.translation_.y, movableArea_.bottom);
	viewProjection_.translation_.y = (std::min)(viewProjection_.translation_.y, movableArea_.top);

	// 追従対象が画面外に出ないように補正
	viewProjection_.translation_.x = (std::max)(viewProjection_.translation_.x, targetWorldTransform.translation_.x + magine_.left);
	viewProjection_.translation_.x = (std::min)(viewProjection_.translation_.x, targetWorldTransform.translation_.x + magine_.right);
	viewProjection_.translation_.y = (std::max)(viewProjection_.translation_.y, targetWorldTransform.translation_.y + magine_.bottom);
	viewProjection_.translation_.y = (std::min)(viewProjection_.translation_.y, targetWorldTransform.translation_.y + magine_.top);

	// 高さが8以上の時オフセットを拡大する
	if (viewProjection_.translation_.y > 8.9f && sumTimer_ <= sumTime_) {
		sumTimer_ += 1.0f / 60;
	} else if (sumTimer_ >= 0.0f) {
		sumTimer_ -= 1.0f / 60;
	}
	if (sumTimer_ > sumTime_) {
		if (targetOffset_.z >= -30.0f) {
			targetOffset_.z -= 10.0f / 60;
		}
	} else if (sumTimer_ < 0) {
		if (targetOffset_.z <= -20.0f) {
			targetOffset_.z += 10.0f / 60;
		}
	}

	// 行列を更新する
	viewProjection_.UpdateMatrix();

	Vector3 floatTransform = {viewProjection_.translation_.x + magine_.left, viewProjection_.translation_.x + magine_.right, 0};

};

void CameraController::Reset() {
	// 追従対象のワールドトランスフォームを参照
	const WorldTransform& targetWorldTransform = target_->GetworldTransform();
	// 追従対象とオフセットからカメラの座標を計算
	viewProjection_.translation_ = targetWorldTransform.translation_ + targetOffset_;
}