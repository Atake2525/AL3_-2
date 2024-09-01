#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>


void GameScene::GenerateBlocks() {
	// 要素数
	const uint32_t kNumBlockVirtical = mapChipField_->GetNumBlockVirtical();
	const uint32_t kNumBlockHorizontal = mapChipField_->GetNumBlockHorizontal();

	// 要素数を変更する
	// 列数を設定(縦方向のブロック数)
	worldTransformBlocks_.resize(kNumBlockVirtical);
	for (uint32_t i = 0; i < kNumBlockVirtical; i++) {
		// 1列の要素数を設定(横方向のブロック数)
		worldTransformBlocks_[i].resize(kNumBlockHorizontal);
	}
	// ブロックの生成
	for (uint32_t i = 0; i < kNumBlockVirtical; i++) {
		for (uint32_t j = 0; j < kNumBlockHorizontal; j++) {
			if (mapChipField_->GetMapChiptypeByIndex(j, i) == MapChipType::KBlock) {
				WorldTransform* worldTransform = new WorldTransform();
				worldTransform->Initialize();
				worldTransformBlocks_[i][j] = worldTransform;
				worldTransformBlocks_[i][j]->translation_ = mapChipField_->GetMapChipPositionByIndex(j, i);
			}
		}
	}
	// 土ブロック
	worldTransformSoil_.resize(kNumBlockVirtical);
	for (uint32_t i = 0; i < kNumBlockVirtical; i++) {
		// 1列の要素数を設定(横方向のブロック数)
		worldTransformSoil_[i].resize(kNumBlockHorizontal);
	}
	// ブロックの生成
	for (uint32_t i = 0; i < kNumBlockVirtical; i++) {
		for (uint32_t j = 0; j < kNumBlockHorizontal; j++) {
			if (mapChipField_->GetMapChiptypeByIndex(j, i) == MapChipType::kSoil) {
				WorldTransform* worldTransform = new WorldTransform();
				worldTransform->Initialize();
				worldTransformSoil_[i][j] = worldTransform;
				worldTransformSoil_[i][j]->translation_ = mapChipField_->GetMapChipPositionByIndex(j, i);
			}
		}
	}
	// ワールドボーダーブロック
	worldTransformWorldBorder_.resize(kNumBlockVirtical);
	for (uint32_t i = 0; i < kNumBlockVirtical; i++) {
		// 1列の要素数を設定(横方向のブロック数)
		worldTransformWorldBorder_[i].resize(kNumBlockHorizontal);
	}
	// ブロックの生成
	for (uint32_t i = 0; i < kNumBlockVirtical; i++) {
		for (uint32_t j = 0; j < kNumBlockHorizontal; j++) {
			if (mapChipField_->GetMapChiptypeByIndex(j, i) == MapChipType::kWorldBorder) {
				WorldTransform* worldTransform = new WorldTransform();
				worldTransform->Initialize();
				worldTransformWorldBorder_[i][j] = worldTransform;
				worldTransformWorldBorder_[i][j]->translation_ = mapChipField_->GetMapChipPositionByIndex(j, i);
			}
		}
	}
	// ゴール
	worldTransformGoal_.resize(kNumBlockVirtical);
	for (uint32_t i = 0; i < kNumBlockVirtical; i++) {
		worldTransformGoal_[i].resize(kNumBlockHorizontal);
	}
	// ゴールの生成
	for (uint32_t i = 0; i < kNumBlockVirtical; i++) {
		for (uint32_t j = 0; j < kNumBlockHorizontal; j++) {
			if (mapChipField_->GetMapChiptypeByIndex(j, i) == MapChipType::kGoal) {
				WorldTransform* worldTransform = new WorldTransform();
				worldTransform->Initialize();
				worldTransformGoal_[i][j] = worldTransform;
				worldTransformGoal_[i][j]->translation_ = mapChipField_->GetMapChipPositionByIndex(j, i);
			}
		}
	}
}


GameScene::GameScene() {}

GameScene::~GameScene() { 
	delete modelSkydome_; 
	for (std::vector<WorldTransform*>& worldtransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldtransformBlockLine ) {
			delete worldTransformBlock;
		}
	}
	for (std::vector<WorldTransform*>& worldtransformBlockLine : worldTransformGoal_) {
		for (WorldTransform* worldTransformBlock : worldtransformBlockLine) {
			delete worldTransformBlock;
		}
	}
	for (std::vector<WorldTransform*>& worldtransformBlockLine : worldTransformSoil_) {
		for (WorldTransform* worldTransformBlock : worldtransformBlockLine) {
			delete worldTransformBlock;
		}
	}
	for (std::vector<WorldTransform*>& worldtransformBlockLine : worldTransformWorldBorder_) {
		for (WorldTransform* worldTransformBlock : worldtransformBlockLine) {
			delete worldTransformBlock;
		}
	}
	worldTransformBlocks_.clear();
	
	delete skyDome_;
	delete player_;
	delete cameraController_;
	delete debugCamera_;
	delete mapChipField_;
	for (Enemy* enemy : enemies_) {
		delete enemy;
	}
	delete deathParticles_;
}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	model_ = Model::Create();

	modelSkydome_ = Model::CreateFromOBJ("skydome", true);
	//texturehandle_ = TextureManager::Load("inshipPlayer_front_1.png");
	modelPlayer_ = Model::CreateFromOBJ("player", true);

	// 背景クラスの生成
	skyDome_ = new Skydome();
	// 背景クラスの初期化
	skyDome_->Initialize(modelSkydome_, texturehandle_, &viewProjection_);

	// マップチップ作成
	mapChipField_ = new MapChipField;

	modelMapChip_ = Model::CreateFromOBJ("glassfloor", true);
	soilModel_ = Model::CreateFromOBJ("soil", true);
	worldBorderModel_ = Model::CreateFromOBJ("worldBorder", true);

	goalModel_ = Model::CreateFromOBJ("goal", true);

	//mapChipField_->LoadMapChipCsv("Resources/Debug.csv");
	mapChipField_->LoadMapChipCsv("Resources/blocks.csv");
	GenerateBlocks();

	// プレイヤー
	player_ = new Player();
	// 座標をマップチップ番号で指定
	Vector3 playerPosition = mapChipField_->GetMapChipPositionByIndex(1, 16);
	// プレイヤーキャラの初期化
	player_->Initialize(model_, texturehandle_, &viewProjection_, playerPosition);

	player_->SetMapChipField(mapChipField_);

	// 仮の生成物処理
	deathParticles_ = new DeathParticles;
	deathParticleModel_ = Model::CreateFromOBJ("deathParticle", true);
	deathParticles_->Initialize(deathParticleModel_, &viewProjection_, playerPosition);
	// ゲームプレイフェーズから開始
	phase_ = Phase::kPlay;


	// 追従カメラの作成
	cameraController_ = new CameraController;
	// 追従カメラの初期化
	cameraController_->Initialize();
	// 追従対象をセット
	cameraController_->SetTrarget(player_);

	CameraController::Rect cameraArea = {0.0f, 100, 0.0f, 100.0f};
	cameraController_->SetMovableArea(cameraArea);
	// リセット(瞬間合わせ)
	cameraController_->Reset();

	// +3はカメラの高さ
	for (int32_t i = 0; i < 11; i++) {
		Enemy* newEnemy = new Enemy();
		Vector3 enemyPosition = {0, 0};
		if (i == 0) {
			enemyPosition = {10, 4 + 3};
		} else if (i == 1) {
			enemyPosition = {20, 0 + 3};
		} 
		else if (i == 2) {
			enemyPosition = {48, -1 + 3};
		}  
		else if (i == 3) {
			enemyPosition = {70, 6 + 3};
		} 
		else if (i == 4) {
			enemyPosition = {78, 1 + 3};
		} 
		else if (i == 5) {
			enemyPosition = {76, 8 + 3};
		} 
		else if (i == 6) {
			enemyPosition = {88, 9 + 3};
		} 
		else if (i == 7) {
			enemyPosition = {61, 8 + 3};
		} 
		else if (i == 8) {
			enemyPosition = {11, 11 + 3};
		} 
		else if (i == 9) {
			enemyPosition = {22, 11 + 3};
		}
		else if (i == 10) {
			enemyPosition = {30, 9 + 3};
		}
		newEnemy->SetMapChipField(mapChipField_);
		modelEnemy_ = Model::CreateFromOBJ("enemy", true);
		newEnemy->Initialize(modelEnemy_, &viewProjection_, enemyPosition);


		enemies_.push_back(newEnemy);


	}

	SoundDataHandle_ = audio_->LoadWave("Jump.wav");



	// デバッグカメラの作成
	debugCamera_ = new DebugCamera(1280, 720);

	viewProjection_.Initialize();

	


	

	

}

void GameScene::Update() {


	switch (phase_) {
	case Phase::kPlay:
		skyDome_->Update();
		player_->Update();
		for (Enemy* enemy : enemies_) {
			enemy->Update();
		}
		cameraController_->Update();
		// ブロックの更新
		for (std::vector<WorldTransform*>& worldtransformBlockLine : worldTransformBlocks_) {
			for (WorldTransform* worldTransformBlock : worldtransformBlockLine) {
				if (!worldTransformBlock)
					continue;
				// アフィン変換行列の作成
				worldTransformBlock->matWorld_ = MakeAffineMatrix(worldTransformBlock->scale_, worldTransformBlock->rotation_, worldTransformBlock->translation_);
				// 定数バッファに転送する
				worldTransformBlock->TransferMatrix();
			}
		}
		for (std::vector<WorldTransform*>& worldtransformBlockLine : worldTransformGoal_) {
			for (WorldTransform* worldTransformBlock : worldtransformBlockLine) {
				if (!worldTransformBlock)
					continue;
				// アフィン変換行列の作成
				worldTransformBlock->matWorld_ = MakeAffineMatrix(worldTransformBlock->scale_, worldTransformBlock->rotation_, worldTransformBlock->translation_);
				// 定数バッファに転送する
				worldTransformBlock->TransferMatrix();
			}
		}
		for (std::vector<WorldTransform*>& worldtransformBlockLine : worldTransformSoil_) {
			for (WorldTransform* worldTransformBlock : worldtransformBlockLine) {
				if (!worldTransformBlock)
					continue;
				// アフィン変換行列の作成
				worldTransformBlock->matWorld_ = MakeAffineMatrix(worldTransformBlock->scale_, worldTransformBlock->rotation_, worldTransformBlock->translation_);
				// 定数バッファに転送する
				worldTransformBlock->TransferMatrix();
			}
		}
		for (std::vector<WorldTransform*>& worldtransformBlockLine : worldTransformWorldBorder_) {
			for (WorldTransform* worldTransformBlock : worldtransformBlockLine) {
				if (!worldTransformBlock)
					continue;
				// アフィン変換行列の作成
				worldTransformBlock->matWorld_ = MakeAffineMatrix(worldTransformBlock->scale_, worldTransformBlock->rotation_, worldTransformBlock->translation_);
				// 定数バッファに転送する
				worldTransformBlock->TransferMatrix();
			}
		}
		if (input_->TriggerKey(DIK_SPACE)) {
			audio_->PlayWave(SoundDataHandle_);
		}
		// すべての当たり判定を行う
		CheckAllCollisions();
		break;
	case Phase::kDeath:
		skyDome_->Update();
		/*for (Enemy* enemy : enemies_) {
			enemy->Update();
		}*/
		if (deathParticles_) {
			deathParticles_->Update();
		}
		cameraController_->Update();
		// ブロックの更新
		for (std::vector<WorldTransform*>& worldtransformBlockLine : worldTransformBlocks_) {
			for (WorldTransform* worldTransformBlock : worldtransformBlockLine) {
				if (!worldTransformBlock)
					continue;
				// アフィン変換行列の作成
				worldTransformBlock->matWorld_ = MakeAffineMatrix(worldTransformBlock->scale_, worldTransformBlock->rotation_, worldTransformBlock->translation_);
				// 定数バッファに転送する
				worldTransformBlock->TransferMatrix();
			}
		}
		for (std::vector<WorldTransform*>& worldtransformBlockLine : worldTransformGoal_) {
			for (WorldTransform* worldTransformBlock : worldtransformBlockLine) {
				if (!worldTransformBlock)
					continue;
				// アフィン変換行列の作成
				worldTransformBlock->matWorld_ = MakeAffineMatrix(worldTransformBlock->scale_, worldTransformBlock->rotation_, worldTransformBlock->translation_);
				// 定数バッファに転送する
				worldTransformBlock->TransferMatrix();
			}
		}
		for (std::vector<WorldTransform*>& worldtransformBlockLine : worldTransformSoil_) {
			for (WorldTransform* worldTransformBlock : worldtransformBlockLine) {
				if (!worldTransformBlock)
					continue;
				// アフィン変換行列の作成
				worldTransformBlock->matWorld_ = MakeAffineMatrix(worldTransformBlock->scale_, worldTransformBlock->rotation_, worldTransformBlock->translation_);
				// 定数バッファに転送する
				worldTransformBlock->TransferMatrix();
			}
		}
		for (std::vector<WorldTransform*>& worldtransformBlockLine : worldTransformWorldBorder_) {
			for (WorldTransform* worldTransformBlock : worldtransformBlockLine) {
				if (!worldTransformBlock)
					continue;
				// アフィン変換行列の作成
				worldTransformBlock->matWorld_ = MakeAffineMatrix(worldTransformBlock->scale_, worldTransformBlock->rotation_, worldTransformBlock->translation_);
				// 定数バッファに転送する
				worldTransformBlock->TransferMatrix();
			}
		}
		break;
	}



	
	//デバッグカメラの更新
	debugCamera_->Update();

	#ifdef _DEBUG
	if (input_->TriggerKey(DIK_0)) {
		isDebugCameraActive_ = !isDebugCameraActive_;
	}
#endif  DEBUG
	if (isDebugCameraActive_) {
		debugCamera_->Update();
		viewProjection_.matView = debugCamera_->GetViewProjection().matView;
		viewProjection_.matProjection = debugCamera_->GetViewProjection().matProjection;
		//ビュープロジェクション行列の転送
		viewProjection_.TransferMatrix();
	}else
	{
		viewProjection_.matView = cameraController_->GetViewProjection().matView;
		viewProjection_.matProjection = cameraController_->GetViewProjection().matProjection;
	       //ビュープロジェクション行列の更新と転送
		viewProjection_.TransferMatrix();
	}
}

void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>
	//ブロックの描画
	for (std::vector<WorldTransform*>& worldtransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldtransformBlockLine) {
			if (!worldTransformBlock)
				continue;
				modelMapChip_->Draw(*worldTransformBlock, viewProjection_);
		}
	}
	for (std::vector<WorldTransform*>& worldtransformBlockLine : worldTransformGoal_) {
		for (WorldTransform* worldTransformBlock : worldtransformBlockLine) {
			if (!worldTransformBlock)
				continue;
			goalModel_->Draw(*worldTransformBlock, viewProjection_);
		}
	}
	for (std::vector<WorldTransform*>& worldtransformBlockLine : worldTransformSoil_) {
		for (WorldTransform* worldTransformBlock : worldtransformBlockLine) {
			if (!worldTransformBlock)
				continue;
			soilModel_->Draw(*worldTransformBlock, viewProjection_);
		}
	}
	for (std::vector<WorldTransform*>& worldtransformBlockLine : worldTransformWorldBorder_) {
		for (WorldTransform* worldTransformBlock : worldtransformBlockLine) {
			if (!worldTransformBlock)
				continue;
			worldBorderModel_->Draw(*worldTransformBlock, viewProjection_);
		}
	}
	player_->Draw(modelPlayer_);
	GameScene::ChangePhase();
	if (deathParticles_) {
		deathParticles_->Draw(deathParticleModel_);
	}
	for (Enemy* enemy:enemies_) {
		enemy->Draw();
	}
	skyDome_->Draw();

	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}

void GameScene::CheckAllCollisions() {
	#pragma region 自キャラと敵キャラの当たり判定

	// 判定対象1と2の座標
	AABB aabb1, aabb2;

	// 自キャラの座標
	aabb1 = player_->GetAABB();

	// 自キャラと敵弾全ての当たり判定
	for (Enemy* enemy:enemies_) {
		// 敵弾の座標
		aabb2 = enemy->GetAABB();

		// AABB同士の交差判定
		if (IsCollision(aabb1, aabb2)) {
			// 自キャラの衝突時コールバックを呼び出す
			if (enemy->IsAlive() == true) {
				player_->OnCollision(enemy);
			}
			// 敵弾の衝突時コールバックを呼び出す
			enemy->OnCollision(player_);
		}
	}

	#pragma endregion

}

void GameScene::ChangePhase() {
	switch (phase_) {
	case GameScene::Phase::kPlay:
		if (player_->IsDead()) {
			// 死亡演出フェーズに切り替え
			phase_ = Phase::kDeath;
			// 自キャラの座標を取得
			const Vector3& deathParticlesPosition = player_->GetWorldPosition();

			deathParticles_->Initialize(deathParticleModel_, &viewProjection_, deathParticlesPosition);
			deathParticles_->SetIsFinished(false);
		}
		if (player_->GetGoalFlag() == true) {
			finished_ = true;
		}
		break;
	case GameScene::Phase::kDeath:
		if (deathParticles_ && deathParticles_->IsFinished()) {
			player_->SetIsDead(true);
			player_->DeadMove();
			const Vector3& deathParticlesPosition = {-10, -10, 0};
			deathParticles_->Initialize(deathParticleModel_, &viewProjection_, deathParticlesPosition);
			if (player_->IsDead() == false) {
				phase_ = GameScene::Phase::kPlay;
			}
		}
		break;
	case GameScene::Phase::kClear:
		if (player_->GetGoalFlag() == true) {
			finished_ = true;
		}
	}
}
