#define NOMINMAX
#include "Player.h"
#include "DebugText.h"
#include "Input.h"
#include "MapChipField.h"
#include "MathUtilityForText.h"
#include "TextureManager.h"
#include "DeathParticles.h"
#include "Audio.h"
#include <algorithm>
#include <cassert>
#include <numbers>

void Player::Initialize(Model* model, uint32_t textureHandle, ViewProjection* viewProjection, const Vector3& position) {
	assert(model);

	model_ = model;
	textureHandle_ = textureHandle;

	worldTransform_.Initialize();
	viewProjection_ = viewProjection;
	worldTransform_.translation_ = position;
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;
	firstPos_ = position;
	/* worldTransform_.translation_.x = 2;
	worldTransform_.translation_.y = 2;*/
};

void Player::MapCollisionUpside(CollisionMapInfo& info) {

	// 上昇あり？
	if (info.MovePoint.y <= 0) {
		return;
	}

	// 移動後の4つの角の座標
	std::array<Vector3, kNumCorner> positionNew;

	for (uint32_t i = 0; i < positionNew.size(); ++i) {
		positionNew[i] = CornerPosition(worldTransform_.translation_ + info.MovePoint, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	MapChipType mapChipTypeNext;
	// 真上の当たり判定を行う
	bool hit = false;
	// 左上点の判定
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapChiptypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChiptypeByIndex(indexSet.xIndex, indexSet.yIndex + 1);
	if (mapChipType == MapChipType::KBlock && mapChipTypeNext != MapChipType::KBlock) {
		hit = true;
	}
	// 右上点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChiptypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChiptypeByIndex(indexSet.xIndex, indexSet.yIndex + 1);
	if (mapChipType == MapChipType::KBlock && mapChipTypeNext != MapChipType::KBlock) {
		hit = true;
	}
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapChiptypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChiptypeByIndex(indexSet.xIndex, indexSet.yIndex + 1);
	if (mapChipType == MapChipType::kSoil && mapChipTypeNext != MapChipType::kSoil) {
		hit = true;
	}
	// 右上点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChiptypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChiptypeByIndex(indexSet.xIndex, indexSet.yIndex + 1);
	if (mapChipType == MapChipType::kSoil && mapChipTypeNext != MapChipType::kSoil) {
		hit = true;
	}
	// ブロックにヒット
	if (hit) {
		// 現在座標が壁の外か判定
		MapChipField::IndexSet indexSetNow;
		indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + Vector3(0, +kHeight / 2.0f, 0));
		if (indexSetNow.yIndex != indexSet.yIndex) {
			// めり込みを排除する方向に移動量を設定する
			indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + info.MovePoint + Vector3(0, +kHeight / 2.0f, 0));
			// めり込み先のブロックの範囲矩形
			MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			info.MovePoint.y = std::max(0.0f, rect.bottom - worldTransform_.translation_.y - (kHeight / 2.0f + kBlank));
			// 天井に当たったことを記録する
			info.isHeadColFlag = true;
		}
	}
}

// マップ衝突判定下方向
void Player::MapCollisionUnderside(CollisionMapInfo& info) {
	// 下降あり？
	if (info.MovePoint.y >= 0) {
		return;
	}

	MapChipType mapChipType;
	// 真下の当たり判定を行う
	bool hit = false;

	// 移動後の4つの角の座標
	std::array<Vector3, kNumCorner> positionsNew;

	for (uint32_t i = 0; i < positionsNew.size(); i++) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.MovePoint, static_cast<Corner>(i));
	}
	// 左下点の判定
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
	mapChipType = mapChipField_->GetMapChiptypeByIndex(indexSet.xIndex, indexSet.yIndex - uint32_t(kHeight));
	if (mapChipType == MapChipType::KBlock) {
		hit = true;
	}
	// 右下点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
	mapChipType = mapChipField_->GetMapChiptypeByIndex(indexSet.xIndex, indexSet.yIndex - uint32_t(kHeight));
	if (mapChipType == MapChipType::KBlock) {
		hit = true;
	}
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
	mapChipType = mapChipField_->GetMapChiptypeByIndex(indexSet.xIndex, indexSet.yIndex - uint32_t(kHeight));
	if (mapChipType == MapChipType::kSoil) {
		hit = true;
	}
	// 右下点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
	mapChipType = mapChipField_->GetMapChiptypeByIndex(indexSet.xIndex, indexSet.yIndex - uint32_t(kHeight));
	if (mapChipType == MapChipType::kSoil) {
		hit = true;
	}

	// ブロックにヒット？
	if (hit) {
		onGround_ = false;
		// めり込みを排除する方向に移動量を設定する
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + Vector3(0, +kHeight * 2.0f, 0));
		// めり込み先ブロックの範囲矩形
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		info.MovePoint.y = std::min(0.0f, rect.bottom - worldTransform_.translation_.y + (kHeight / 2.0f + kBlank));
		// 地面に当たったことを記録する
		info.landing = true;
	}
}

void Player::MapCollisionLeftside(CollisionMapInfo& info) {
	// 左移動あり？
	if (info.MovePoint.x >= 0) {
		return;
	}

	std::array<Vector3, kNumCorner> positionNew;

	for (uint32_t i = 0; i < positionNew.size(); ++i) {
		positionNew[i] = CornerPosition(worldTransform_.translation_ + info.MovePoint, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	MapChipType mapChipTypeNext;
	bool hit = false;
	// 左上点の判定
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapChiptypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChiptypeByIndex(indexSet.xIndex + 1, indexSet.yIndex);
	if (mapChipType == MapChipType::KBlock && mapChipTypeNext != MapChipType::KBlock) {
		hit = true;
	}
	// 左下点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kLeftBottom]);
	mapChipType = mapChipField_->GetMapChiptypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChiptypeByIndex(indexSet.xIndex + 1, indexSet.yIndex);
	if (mapChipType == MapChipType::KBlock && mapChipTypeNext != MapChipType::KBlock) {
		hit = true;
	}
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapChiptypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChiptypeByIndex(indexSet.xIndex + 1, indexSet.yIndex);
	if (mapChipType == MapChipType::kSoil && mapChipTypeNext != MapChipType::kSoil) {
		hit = true;
	}
	// 左下点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kLeftBottom]);
	mapChipType = mapChipField_->GetMapChiptypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChiptypeByIndex(indexSet.xIndex + 1, indexSet.yIndex);
	if (mapChipType == MapChipType::kSoil && mapChipTypeNext != MapChipType::kSoil) {
		hit = true;
	}
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapChiptypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChiptypeByIndex(indexSet.xIndex + 1, indexSet.yIndex);
	if (mapChipType == MapChipType::kWorldBorder && mapChipTypeNext != MapChipType::kWorldBorder) {
		hit = true;
	}
	// 左下点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kLeftBottom]);
	mapChipType = mapChipField_->GetMapChiptypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChiptypeByIndex(indexSet.xIndex + 1, indexSet.yIndex);
	if (mapChipType == MapChipType::kWorldBorder && mapChipTypeNext != MapChipType::kWorldBorder) {
		hit = true;
	}

	// ブロックにヒット？
	if (hit) {
		// 現在座標が壁の外か判定
		MapChipField::IndexSet indexSetNow;
		indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + Vector3(-kWidth / 2.0f, 0, 0));
		if (indexSetNow.xIndex != indexSet.xIndex) {
			// めり込みを排除する方向に移動量を設定する
			indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + info.MovePoint + Vector3(-kWidth / 2.0f, 0, 0));
			MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			info.MovePoint.x = std::min(0.0f, rect.right - worldTransform_.translation_.x + (kWidth / 2.0f + kBlank));
			// 急ブレーキ
			velocity_.x *= (1.0f - kAttenuation);
			info.isWallColFlag = true;
		}
	}
}

void Player::MapCollisionRightside(CollisionMapInfo& info) {
	// 右移動あり？
	if (info.MovePoint.x <= 0) {
		return;
	}

	std::array<Vector3, kNumCorner> positionNew;

	for (uint32_t i = 0; i < positionNew.size(); ++i) {
		positionNew[i] = CornerPosition(worldTransform_.translation_ + info.MovePoint, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	MapChipType mapChipTypeNext;
	// 真上の当たり判定を行う
	bool hit = false;
	// 右上点の判定
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChiptypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChiptypeByIndex(indexSet.xIndex - 1, indexSet.yIndex);
	if (mapChipType == MapChipType::KBlock && mapChipTypeNext != MapChipType::KBlock) {
		hit = true;
	}
	// 右下点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kRightBottom]);
	mapChipType = mapChipField_->GetMapChiptypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChiptypeByIndex(indexSet.xIndex - 1, indexSet.yIndex);
	if (mapChipType == MapChipType::KBlock && mapChipTypeNext != MapChipType::KBlock) {
		hit = true;
	}
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChiptypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChiptypeByIndex(indexSet.xIndex - 1, indexSet.yIndex);
	if (mapChipType == MapChipType::kSoil && mapChipTypeNext != MapChipType::kSoil) {
		hit = true;
	}
	// 右下点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kRightBottom]);
	mapChipType = mapChipField_->GetMapChiptypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChiptypeByIndex(indexSet.xIndex - 1, indexSet.yIndex);
	if (mapChipType == MapChipType::kSoil && mapChipTypeNext != MapChipType::kSoil) {
		hit = true;
	}
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChiptypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChiptypeByIndex(indexSet.xIndex - 1, indexSet.yIndex);
	if (mapChipType == MapChipType::kWorldBorder && mapChipTypeNext != MapChipType::kWorldBorder) {
		hit = true;
	}
	// 右下点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kRightBottom]);
	mapChipType = mapChipField_->GetMapChiptypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChiptypeByIndex(indexSet.xIndex - 1, indexSet.yIndex);
	if (mapChipType == MapChipType::kWorldBorder && mapChipTypeNext != MapChipType::kWorldBorder) {
		hit = true;
	}

	// ブロックにヒット？
	if (hit) {
		// 現在座標が壁の外か判定
		MapChipField::IndexSet indexSetNow;
		indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + Vector3(+kWidth / 2.0f, 0, 0));
		if (indexSetNow.xIndex != indexSet.xIndex) {
			// めり込みを排除する方向に移動量を設定する
			indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + info.MovePoint + Vector3(+kWidth / 2.0f, 0, 0));
			MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			info.MovePoint.x = std::max(0.0f, rect.left - worldTransform_.translation_.x - (kWidth / 2.0f + kBlank));
			// 急ブレーキ
			velocity_.x *= (1.0f - kAttenuation);
			info.isWallHit = true;
		}
	}
}

void Player::GoalCollision(CollisionMapInfo& info) {
	std::array<Vector3, kNumCorner> positionNew;

	for (uint32_t i = 0; i < positionNew.size(); ++i) {
		positionNew[i] = CornerPosition(worldTransform_.translation_ + info.MovePoint, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	MapChipType mapChipTypeNext;
	MapChipField::IndexSet indexSet;
	// 真上の当たり判定を行う
	bool goalHit = 0;
	
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapChiptypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChiptypeByIndex(indexSet.xIndex + 1, indexSet.yIndex);
	if (mapChipType == MapChipType::kGoal && mapChipTypeNext != MapChipType::kGoal) {
		goalHit = true;
	}

	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kLeftBottom]);
	mapChipType = mapChipField_->GetMapChiptypeByIndex(indexSet.xIndex, indexSet.yIndex + 1);
	mapChipTypeNext = mapChipField_->GetMapChiptypeByIndex(indexSet.xIndex + 1, indexSet.yIndex);
	if (mapChipType == MapChipType::kGoal && mapChipTypeNext != MapChipType::kGoal) {
		goalHit = true;
	}

	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChiptypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChiptypeByIndex(indexSet.xIndex - 1, indexSet.yIndex);
	if (mapChipType == MapChipType::kGoal && mapChipTypeNext != MapChipType::kGoal) {
		goalHit = true;
	}

	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kRightBottom]);
	mapChipType = mapChipField_->GetMapChiptypeByIndex(indexSet.xIndex, indexSet.yIndex + 1);
	mapChipTypeNext = mapChipField_->GetMapChiptypeByIndex(indexSet.xIndex - 1, indexSet.yIndex);
	if (mapChipType == MapChipType::kGoal && mapChipTypeNext != MapChipType::kGoal) {
		goalHit = true;
	}


	if (goalHit) {
		goalFlag_ = true;
		//DebugText::GetInstance()->ConsolePrintf("%d\n", goalHit);
	} else {
		goalFlag_ = false;
		goalTime_ = 0.0f;
	}
	
}

// 判定結果を反映して移動させる
void Player::ReturnMove(const CollisionMapInfo& info) {
	// 移動
	worldTransform_.translation_ += info.MovePoint;
};

void Player::isOverheadCollision(const CollisionMapInfo& info) {
	// 天井に当たった？
	if (info.isHeadColFlag) {
		DebugText::GetInstance()->ConsolePrintf("hit ceiling\n");
		velocity_.y = 0;
	}
}

Vector3 Player::CornerPosition(const Vector3& center, Corner corner) {
	Vector3 offsetTable[kNumCorner] = {
	    {+kWidth / 2.0f, -kHeight / 2.0f, 0}, // kRightBottom
	    {-kWidth / 2.0f, -kHeight / 2.0f, 0}, // kLeftBottom
	    {+kWidth / 2.0f, +kHeight / 2.0f, 0}, // kRightTop
	    {-kWidth / 2.0f, +kHeight / 2.0f, 0}  // kLeftTop
	};
	return center + offsetTable[static_cast<uint32_t>(corner)];
}

void Player::Move() {
	///  移動入力
	///
	// 接地状態
	CollisionMapInfo info;
	if (onGround_) {
		// 左右移動操作
		if (Input::GetInstance()->PushKey(DIK_D) || Input::GetInstance()->PushKey(DIK_A)) {
			Vector3 acceleration = {};

			if (Input::GetInstance()->PushKey(DIK_D)) {
				acceleration.x += kAcceleration;
				if (lrDirection_ != LRDirection::kRight) {
					lrDirection_ = LRDirection::kRight;
					turnFirstRotationY_ = worldTransform_.rotation_.y;
					turnTimer_ = 0.01f;
				}
				if (velocity_.x < 0.0f) {
					// 速度と逆方向に入力中は急ブレーキ
					velocity_.x *= (1.0f - kAttenuation);
				}
			} else if (Input::GetInstance()->PushKey(DIK_A)) {
				acceleration.x -= kAcceleration;
				if (lrDirection_ != LRDirection::kLeft) {
					lrDirection_ = LRDirection::kLeft;
					turnFirstRotationY_ = worldTransform_.rotation_.y;
					turnTimer_ = 0.01f;
				}
				if (velocity_.x > 0.0f) {
					// 速度と逆方向に入力中は急ブレーキ
					velocity_.x *= (1.0f - kAttenuation);
				}
			}
			// 加速 // 減速
			velocity_.x += acceleration.x;
			// 最大速度制限
			velocity_.x = std::clamp(velocity_.x, -kLimitRunSpeed, kLimitRunSpeed);
		} else {
			// 非入力時は移動減衰をかける
			velocity_.x *= (1.0f - kAttenuation);
		}
		if (turnTimer_ > 0.0f) {
			turnTimer_ += kTimeTurn / 60;

			// 左右の自キャラ角度テーブル
			float destinationRotationYTable[] = {std::numbers::pi_v<float> / 2.0f, std::numbers::pi_v<float> * 3.0f / 2.0f};
			// 状態に応じた角度を取得する
			float destinationRotationY = destinationRotationYTable[static_cast<uint32_t>(lrDirection_)];
			// 自キャラの角度を設定する
			worldTransform_.rotation_.y = easeInOut(turnTimer_, turnFirstRotationY_, destinationRotationY);
		}
		if (turnTimer_ > 1.0f) {
			turnTimer_ = 0.0f;
		}

		if (Input::GetInstance()->PushKey(DIK_SPACE)) {
			Vector3 jumpAcceleration = {0, kJumpAcceleration, 0};
			// ジャンプ初速
			velocity_ += jumpAcceleration;
		}
		// ジャンプ開始
		if (velocity_.y > 0.0f) {
			// 空中状態に移行
			onGround_ = false;
		}
	} else {
		// 空中
		// 落下速度
		velocity_ += Vector3(0, -kGravityAcceleration, 0);
		// 落下速度制限
		velocity_.y = std::max(velocity_.y, -kLimitFallSpeed);
	}
};

void Player::changeOnGround(const CollisionMapInfo& info) {
	// info.landing = false;

	//// 地面との当たり判定
	//// 落下中か
	// if (velocity_.y < 0) {
	//	// Y座標が地面以下になったら着地
	//	if (worldTransform_.translation_.y <= 2.0f) {
	//		info.landing = true;
	//	}
	// }
	if (onGround_) {
		// 接地状態の処理
		// ジャンプ開始
		if (velocity_.y > 0.0f) {
			onGround_ = false;
		} else {
			// 落下判定

			// 落下なら空中状態に切り替え
			// 移動後の4つの角の座標
			std::array<Vector3, kNumCorner> positionsNew;

			for (uint32_t i = 0; i < positionsNew.size(); i++) {
				positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.MovePoint, static_cast<Corner>(i));
			}
			MapChipType mapChipType;

			// 真下の当たり判定を行う
			bool hit = false;
			// 左下点の判定
			MapChipField::IndexSet indexSet;
			indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom] /* + Vector3(0, -smallNum, 0)*/);
			mapChipType = mapChipField_->GetMapChiptypeByIndex(indexSet.xIndex, indexSet.yIndex);
			if (mapChipType == MapChipType::KBlock) {
				hit = true;
			}
			// 右下点の判定
			indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom] /* + Vector3(0, -smallNum, 0) */);
			mapChipType = mapChipField_->GetMapChiptypeByIndex(indexSet.xIndex, indexSet.yIndex);
			if (mapChipType == MapChipType::KBlock) {
				hit = true;
			}

			// 落下開始
			if (!hit) {
				// 空中状態に切り替える
				onGround_ = false;
			}
		}
	} else {
		// 空中状態の処理
		if (info.landing) {
			// 着地状態に切り替える(落下を止める)
			onGround_ = true;
			// 着地時にX速度を減衰
			velocity_.x *= (1.0f - kAttenuationLoading);
			// Y速度をゼロにする
			velocity_.y = 0.0f;
		}
	}
}

void Player::Update() {

	if (isDead_ == false) {
		// 1移動入力

		// 2 移動量を加味して衝突判定する

		// 3衝突情報を初期化
		CollisionMapInfo collisionMapInfo;
		// 移動量に速度の値をコピー
		collisionMapInfo.MovePoint = velocity_;
		// マップ衝突チェック
		// 4天井に接触している場合の処理

		MapCollisionLeftside(collisionMapInfo);
		MapCollisionRightside(collisionMapInfo);
		MapCollisionUpside(collisionMapInfo);
		isOverheadCollision(collisionMapInfo);
		ReturnMove(collisionMapInfo);
		MapCollisionUnderside(collisionMapInfo);
		GoalCollision(collisionMapInfo);
		goalCollision();
		Move();

		// 判定結果を判定して移動させる
		changeOnGround(collisionMapInfo);

		isUnderFloor();
	}

	// 6接地状態の切り替え

	///

	// 8行列計算
	worldTransform_.UpdateMatrix();

	
};

void Player::DeadMove() { 
	if (DeadMoving_ == false) {
		nowPos_ = worldTransform_.translation_;
		velocity_ = {0};
		DeadMoving_ = true;
	}
	if (completeDeadMove == false) {
		if (worldTransform_.translation_.y < nowPos_.y + 2) {
			worldTransform_.translation_.y += 0.1f;
		} else {
			deadTimer_ += 1.0f / 60;
		}
		if (deadTimer_ >= 0.4f) {
			deadTimer_ = 0.0f;
			completeDeadMove = true;
		}
	}
	if (completeDeadMove == true && deadTimer_ < deadTime_) {
		lrDirection_ = LRDirection::kRight;
		// 左右の自キャラ角度テーブル
		deadTimer_ += 1 / deadTime_ / 60;
		worldTransform_.translation_.x = easeInOut(deadTimer_, worldTransform_.translation_.x, firstPos_.x);
		worldTransform_.translation_.y = easeInOut(deadTimer_, worldTransform_.translation_.y, firstPos_.y);
		float destinationRotationYTable[] = {std::numbers::pi_v<float> / 2.0f, std::numbers::pi_v<float> * 3.0f / 2.0f};
		// 状態に応じた角度を取得する
		float destinationRotationY = destinationRotationYTable[static_cast<uint32_t>(lrDirection_)];
		if (worldTransform_.rotation_.y != destinationRotationY) {
			// 自キャラの角度を設定する
			worldTransform_.rotation_.y = easeInOut(deadTimer_, turnFirstRotationY_, destinationRotationY);
		}
	}
	if (deadTimer_ >= deadTime_) {
		isDead_ = false;
		DeadMoving_ = false;
		completeDeadMove = false;
		deadTimer_ = 0.0f;
		playerDeathNum_++;
	}
	
	worldTransform_.UpdateMatrix();
}


Vector3 Player::GetWorldPosition() {
	// ワールド座標を入れる変数
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得(ワールド座標)
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}

AABB Player::GetAABB() { 
	Vector3 worldPos = GetWorldPosition();

	AABB aabb;

	aabb.min = {worldPos.x - kWidth / 2.0f, worldPos.y - kHeight / 2.0f, worldPos.z - kWidth / 2.0f};
	aabb.max = {worldPos.x + kWidth / 2.0f, worldPos.y + kHeight / 2.0f, worldPos.z + kWidth / 2.0f};

	return aabb;
}

void Player::OnCollision(const Enemy* enemy) {
	(void)enemy;
	// ジャンプ開始(仮処理)


	isDead_ = true;
	
}

void Player::isUnderFloor() { 
	Vector3 worldPos = GetWorldPosition();

	if (worldPos.y < -4) {
		isDead_ = true;
	}
}

void Player::goalCollision() {
	if (goalFlag_ == true) {
		goalTime_ += 1.0f / 60.0f;
	}
	if (goalTime_ >= 2.0f) {
		goal_ = true;
		goalTime_ = 0.0f;
		goalFlag_ = false;
	}
}

void Player::Draw(Model* model) { model->Draw(worldTransform_, *viewProjection_); }

