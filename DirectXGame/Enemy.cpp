#include "Enemy.h"
#include <cassert>
#include <cmath>
#include <numbers>
#include "MathUtilityForText.h"
#include "Player.h"
#include "MapChipField.h"

void Enemy::Initialize(Model* model, ViewProjection* viewProjection, const Vector3& position){
	assert(model);

	model_ = model;

	// 速度を設定する
	velocity_ = {-kWalkSpeed, 0, 0};
	worldTransform_.Initialize();
	viewProjection_ = viewProjection;
	worldTransform_.translation_ = position;
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / -2.0f;
	walkTimer_ = 0.0f;
}

void Enemy::Update() {



	
		// 衝突情報を初期化
		CollisionMapInfo collisionMapInfo;
		// 移動量に速度の値をコピー
		collisionMapInfo.movePoint = velocity_;

		// マップ諸突チェック
		MapCollision(collisionMapInfo);

		if (isAlive == true) {
			// タイマーを加算
			walkTimer_ += 1.0f / 60.0f;
			// 移動
			if (MoveDirection == 0) {
				worldTransform_.translation_ += velocity_;
				if (lrDirection_ != LRDirection::kLeft) {
					lrDirection_ = LRDirection::kLeft;
					turnFirstRotationY_ = worldTransform_.rotation_.y;
					turnTimer_ = 0.01f;
				}
			} else if (MoveDirection == 1) {
				if (lrDirection_ != LRDirection::kRight) {
					lrDirection_ = LRDirection::kRight;
					turnFirstRotationY_ = worldTransform_.rotation_.y;
					turnTimer_ = 0.01f;
				}
				worldTransform_.translation_ -= velocity_;
			} else {
				MoveDirection = 0;
			}

			// enemy回転
			float param = std::sin(std::numbers::pi_v<float> * 2.0f * walkTimer_ / kWalkMotionTime);
			float radian = kWalkMotionAngleStart + kWalkMotionAngleEnd * (param + 1.0f) / -50.0f;
			worldTransform_.rotation_.x = radian;
			if (turnTimer_ > 0.0f) {
				turnTimer_ += 1 / kTimeTurn / 60;

				// 左右のenemy角度テーブル
				float destinationRotationYTable[] = {std::numbers::pi_v<float> / 2.0f, std::numbers::pi_v<float> * 3.0f / 2.0f};
				// 状態に応じた角度を取得する
				float destinationRotationY = destinationRotationYTable[static_cast<uint32_t>(lrDirection_)];
				// enemyの角度を設定する
				worldTransform_.rotation_.y = easeInOut(turnTimer_, turnFirstRotationY_, destinationRotationY);
			}
			if (turnTimer_ > 1.0f) {
				turnTimer_ = 0.0f;
			}

			worldTransform_.UpdateMatrix();
		}
	

}

Vector3 Enemy::CornerPosition(const Vector3& center, Corner corner) {
	Vector3 offsetTable[kNumCorner] = {
	    {+kWidth / 2.0f, -kHeight / 2.0f, 0}, // kRightBottom
	    {-kWidth / 2.0f, -kHeight / 2.0f, 0}, // kLeftBottom
	    {+kWidth / 2.0f, +kHeight / 2.0f, 0}, // kRightTop
	    {-kWidth / 2.0f, +kHeight / 2.0f, 0}  // kLeftTop
	};
	return center + offsetTable[static_cast<uint32_t>(corner)];
}



void Enemy::isCollisionLeft(CollisionMapInfo& info) {
	// 移動後の4つ角の座標
	std::array<Vector3, kNumCorner> positionNew;

	for (uint32_t i = 0; i < positionNew.size(); ++i) {
		positionNew[i] = CornerPosition(worldTransform_.translation_ + info.movePoint, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	// 左の当たり判定を行う
	MapChipField::IndexSet indexSet;
	// 左上点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapChiptypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::KBlock || mapChipType == MapChipType::kSoil) {
		MoveDirection = 1;
	}
	// 左下点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kLeftBottom]);
	mapChipType = mapChipField_->GetMapChiptypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::KBlock || mapChipType == MapChipType::kSoil) {
		MoveDirection = 1;
	}
}

void Enemy::isCollisionRight(CollisionMapInfo& info) {
	// 移動後の4つ角の座標
	std::array<Vector3, kNumCorner> positionNew;

	for (uint32_t i = 0; i < positionNew.size(); ++i) {
		positionNew[i] = CornerPosition(worldTransform_.translation_ + info.movePoint, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	// 左の当たり判定を行う
	MapChipField::IndexSet indexSet;
	// 左上点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChiptypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::KBlock || mapChipType == MapChipType::kSoil) {
		MoveDirection = 0;
	}
	// 左下点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kRightBottom]);
	mapChipType = mapChipField_->GetMapChiptypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::KBlock || mapChipType == MapChipType::kSoil) {
		MoveDirection = 0;
	}
}

void Enemy::isCollisionUnderLeft(CollisionMapInfo& info) {
	// 移動後の4つ角の座標
	std::array<Vector3, kNumCorner> positionNew;

	for (uint32_t i = 0; i < positionNew.size(); ++i) {
		positionNew[i] = CornerPosition(worldTransform_.translation_ + info.movePoint, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	// 左の当たり判定を行う
	MapChipField::IndexSet indexSet;
	// 左上点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapChiptypeByIndex(indexSet.xIndex, indexSet.yIndex + 1);
	if (mapChipType == MapChipType::kBlank) {
		MoveDirection = 1;
	}
	// 左下点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kLeftBottom]);
	mapChipType = mapChipField_->GetMapChiptypeByIndex(indexSet.xIndex, indexSet.yIndex + 1);
	if (mapChipType == MapChipType::kBlank) {
		MoveDirection = 1;
	}
}

void Enemy::isCollisionUnderRight(CollisionMapInfo& info) {
	// 移動後の4つ角の座標
	std::array<Vector3, kNumCorner> positionNew;

	for (uint32_t i = 0; i < positionNew.size(); ++i) {
		positionNew[i] = CornerPosition(worldTransform_.translation_ + info.movePoint, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	// 左の当たり判定を行う
	MapChipField::IndexSet indexSet;
	// 左上点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChiptypeByIndex(indexSet.xIndex, indexSet.yIndex + 1);
	if (mapChipType == MapChipType::kBlank) {
		MoveDirection = 0;
	}
	// 左下点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kRightBottom]);
	mapChipType = mapChipField_->GetMapChiptypeByIndex(indexSet.xIndex, indexSet.yIndex + 1);
	if (mapChipType == MapChipType::kBlank) {
		MoveDirection = 0;
	}
}


Vector3 Enemy::GetWorldPosition() {
	// ワールド座標を入れる変数
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得(ワールド座標)
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}

AABB Enemy::GetAABB() {
	Vector3 worldPos = GetWorldPosition();

	AABB aabb;

	aabb.min = {worldPos.x - kWidth / 2.0f, worldPos.y - kHeight / 2.0f, worldPos.z - kWidth / 2.0f};
	aabb.max = {worldPos.x + kWidth / 2.0f, worldPos.y + kHeight / 2.0f, worldPos.z + kWidth / 2.0f};

	return aabb;
}

void Enemy::OnCollision(const Player* player) {
	(void)player;
	isAlive = false;
}

void Enemy::MapCollision(CollisionMapInfo& info) { 
	isCollisionLeft(info);
	isCollisionRight(info);
	isCollisionUnderLeft(info);
	isCollisionUnderRight(info);
}



void Enemy::Draw() {
	if (isAlive == true) {
		model_->Draw(worldTransform_, *viewProjection_);
	}
}
