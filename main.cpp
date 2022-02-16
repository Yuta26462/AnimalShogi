/*****************************************
* 第８章　ミニゲームをつくる（５）
*	Match 3 Puzzle (マッチ３パズル)
* ****************************************/
								//P.15	⑪～
#include"DxLib.h"
#include<stdlib.h>
int hhhh;

typedef enum GAME_MODE {

	GAME_TITLE,
	GAME_INIT,
	GAME_END,
	GAME_MAIN,
	GAME_CLEAR,
	GAME_OVER,
	END = 99
};



const int HEIGHT = 12;
const int WIDTH = 12;
const int BLOCKSIZE = 48;
const int MARGIN = 0;

const int ITEM_MAX = 8;
const int TIMELIMIT = 3600 * 3;



int g_OldKey, g_NowKey, g_Keyflg;
int g_MouseX, g_MouseY;
int g_GameState = GAME_TITLE;

int g_TitleImage;

int g_StageImage;

int g_GameClearImage;
int g_GameOverImage;

int g_BlockImage[10];

int g_NumberImage[10];
int g_Score;
int g_Level;
int g_Mission;
int g_Item[ITEM_MAX];
int g_Time;

int g_Status = 0;		// 0...プレイヤー操作中
int g_SelectX1;		// 1回目に選択したブロックの二次元配列(横)
int g_SelectY1;		// 1回目に選択したブロックの二次元配列(縦)
int g_SelectX2;		// 2回目に選択したブロックの二次元配列(横)
int g_SelectY2;		// 2回目に選択したブロックの二次元配列(縦)

int g_TitleBGM, g_GameClearSE, g_GameOverSE, g_ClickSE, g_FadeOutSE, g_MoveBlockSE;

void GameInit(void);
void GameMain(void);
void DrawGameTitle(void);
void DrawGameClear(void);
void DrawGameOver(void);
void DrawEnd(void);

int LoadImages();

void DrawStage(void);

void StageInit(void);

int RensaCheck(int y, int x);
int RensaCheckH(int y, int x, int* cnt, int* col);
int RensaCheckW(int y, int x, int* cnt, int* col);
void SaveBlock(void);
void RestoreBlock(void);

void SelectBlock(void);

void MoveBlock(void);

void CheckBlock(void);
void CheckClear(void);

void FadeOutBlock(void);

int LoadSounds(void);


struct Object {
	int flg;
	int x, y;
	int w, h;
	int image;
	int backup;
};
struct Object g_Block[HEIGHT][WIDTH];



int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

	SetMainWindowText("Match 3 Puzzle (マッチ３パズル)");

	ChangeWindowMode(TRUE);

	if (DxLib_Init() == -1)return -1;

	SetDrawScreen(DX_SCREEN_BACK);

	if (LoadImages() == -1) return -1;
	if (LoadSounds() == -1) return -1;


	// ゲームループ
	while (ProcessMessage() == 0 && g_GameState != END && !(g_Keyflg & PAD_INPUT_START)) {

		g_OldKey = g_NowKey;
		g_NowKey = GetMouseInput();
		g_Keyflg = g_NowKey & ~g_OldKey;

		GetMousePoint(&g_MouseX, &g_MouseY);

		ClearDrawScreen();

		switch (g_GameState) {

		case GAME_TITLE:
			DrawGameTitle();
			break;

		case GAME_INIT:
			GameInit();
			break;

		case GAME_END:
			DrawEnd();
			break;

		case GAME_MAIN:
			GameMain();
			break;

		case GAME_CLEAR:
			DrawGameClear();
			break;

		case GAME_OVER:
			DrawGameOver();
			break;
		}

		ScreenFlip();
	}
	DxLib_End();

	return 0;
}


void DrawGameTitle(void) {

	if (CheckSoundMem(g_TitleBGM) == 0) PlaySoundMem(g_TitleBGM, DX_PLAYTYPE_BACK);
	DrawGraph(0, 0, g_TitleImage, FALSE);

	if (g_Keyflg & MOUSE_INPUT_LEFT) {
		if (g_MouseX > 120 && g_MouseX < 290 && g_MouseY > 260 && g_MouseY < 315) {
			g_GameState = GAME_INIT;
			StopSoundMem(g_TitleBGM);
		}
		else if (g_MouseX > 120 && g_MouseX < 220 && g_MouseY > 345 && g_MouseY < 400) {
			g_GameState = GAME_END;
			StopSoundMem(g_TitleBGM);
		}
	}
}


void GameInit(void) {

	//g_GameState = GAME_MAIN;

	for (int i = 0; i < HEIGHT; i++) {
		for (int j = 0; j < WIDTH; j++) {
			if (j == 0 || j == WIDTH - 1 || i == HEIGHT - 1 || i == 0) {
				g_Block[i][j].flg = 0;
				g_Block[i][j].image = 0;
			}
			else {
				g_Block[i][j].flg = 1;
				g_Block[i][j].x = (j - 1) * BLOCKSIZE + MARGIN;
				g_Block[i][j].y = (i - 1) * BLOCKSIZE;
				g_Block[i][j].w = BLOCKSIZE;
				g_Block[i][j].h = BLOCKSIZE;
				//g_Block[i][j].image = 1;	// GetRand(7) + 1;
				g_Block[i][j].image = GetRand(7) + 1;	// 1～8までに乱数を生成
			}
		}
	}


	g_Score = 0;
	g_Level = 0;
	g_Mission = 2;			// レベル1は消去するブロック数が3からスタート

	StageInit();
}


void DrawEnd(void) {

	static int WaitTime = 0;

	DrawGraph(0, 0, g_TitleImage, FALSE);

	SetFontSize(24);
	DrawString(360, 480 - 24, "Thank you for Playing", 0x003333, 0);

	if (++WaitTime > 180)g_GameState = END;
}


void GameMain(void) {

	DrawGraph(0, 0, g_StageImage, FALSE);

	DrawStage();

	/*if (g_Keyflg & MOUSE_INPUT_LEFT)g_GameState = GAME_CLEAR;
	if (g_Keyflg & MOUSE_INPUT_RIGHT)g_GameState = GAME_OVER;

	SetFontSize(16);
	DrawString(150, 420, "---- 左クリックでゲームクリアへ ----", 0xffffff, 0);
	DrawString(150, 450, "---- 右クリックでゲームオーバーへ ----", 0xffffff, 0);*/


	switch (g_Status) {

	case 0:
		SelectBlock();
		break;

	case 1:
		FadeOutBlock();
		break;

	case 2:
		MoveBlock();
		break;

	case 3:
		CheckBlock();
		break;

	case 4:
		CheckClear();
		break;
	}


	// タイム変数をデクリメントして、0未満になったらゲームオーバーへ移行する
	if (--g_Time < 0) {
		g_GameState = GAME_OVER;
	}
}


void DrawGameClear(void) {

	static int GameClearFlg = 0;

	if (CheckSoundMem(g_GameClearSE) == 0) {
		if (GameClearFlg++ == 0) {
			PlaySoundMem(g_GameClearSE, DX_PLAYTYPE_BACK);
		}
		else {
			// ステージ初期処理
			StageInit();
			g_GameState = GAME_MAIN;	//改変
			GameClearFlg = 0;
		}
	}
	DrawGraph(0, 0, g_GameClearImage, FALSE);

	//if (g_Keyflg & MOUSE_INPUT_RIGHT) {
	//	//g_GameState = GAME_TITLE;

	//	// ステージ初期処理
	//	StageInit();
	//	g_GameState = GAME_MAIN;
	//}
	////SetFontSize(16);
	////DrawString(150, 450, "---- 右クリックでタイトルへ ----", 0xffffff, 0);
	//SetFontSize(20);
	//DrawString(150, 450, "---- 右クリックで次のステージへ ----", 0xffffff, 0);
}


void DrawGameOver(void) {

	static int GameOverFlg = 0;

	if (CheckSoundMem(g_GameOverSE) == 0) {
		if (GameOverFlg++ == 0) {
			PlaySoundMem(g_GameOverSE, DX_PLAYTYPE_BACK);
		}
		else {
			g_GameState = GAME_TITLE;	//改変
			GameOverFlg = 0;
		}
	}
	DrawGraph(0, 0, g_GameOverImage, FALSE);

	//if (g_Keyflg & MOUSE_INPUT_RIGHT)g_GameState = GAME_TITLE;
	//SetFontSize(16);
	//DrawString(150, 450, "---- 右クリックでタイトルへ ----", 0xffffff, 0);
}


int LoadImages() {

	if ((g_TitleImage = LoadGraph("images/08/title.png")) == -1)return -1;
	if ((g_StageImage = LoadGraph("images/08/stage.png")) == -1)return -1;
	if ((g_GameClearImage = LoadGraph("images/08/gameclear.png")) == -1)return -1;
	if ((g_GameOverImage = LoadGraph("images/08/gameover.png")) == -1)return -1;

	if (LoadDivGraph("images/08/block.png", 10, 10, 1, 48, 48, g_BlockImage) == -1) return -1;

	if (LoadDivGraph("images/08/number.png", 10, 10, 1, 60, 120, g_NumberImage) == -1) return -1;

	return 0;
}


void DrawStage(void) {


	// レベルを描画
	int TmpLevel = g_Level;
	int PosX = 600;
	do {
		DrawRotaGraph(PosX, 80, 0.5f, 0, g_NumberImage[TmpLevel % 10], TRUE);
		TmpLevel /= 10;
		PosX -= 30;
	} while (TmpLevel > 0);


	// スコアを描画
	int TmpScore = g_Score;
	PosX = 620;
	do {
		DrawRotaGraph(PosX, 160, 0.3f, 0, g_NumberImage[TmpScore % 10], TRUE);
		TmpScore /= 10;
		PosX -= 20;
	} while (TmpScore > 0);

	// ミッションを描画
	SetFontSize(20);
	DrawFormatString(590, 211, 0xFFFFFF, "%3d", g_Mission);

	// アイテムの取得個数を描画
	for (int i = 0; i < ITEM_MAX; i++) {
		DrawRotaGraph(540, 245 + i * 30, 0.5f, 0, g_BlockImage[i + 1], TRUE, 0);
		DrawFormatString(580, 235 + i * 30, 0xFFFFFF, "%3d", g_Item[i]);
	}


	// ブロックを描画
	for (int i = 0; i < HEIGHT; i++) {
		for (int j = 0; j < WIDTH; j++) {
			if (g_Block[i][j].flg != 0 && g_Block[i][j].image != 0) {
				DrawGraph(g_Block[i][j].x, g_Block[i][j].y, g_BlockImage[g_Block[i][j].image], TRUE);
			}
		}
	}


	// タイムバーを描画
	DrawBox(491, 469, 509, 469 - g_Time / 60 * 2, 0x0033ff, TRUE);
}


void StageInit(void) {

	int Result = 0;

	// 空のブロックを生成する処理
	for (int i = 1; i < HEIGHT - 1; i++) {
		for (int j = 1; j < WIDTH - 1; j++) {
			if (g_Block[i][j].image == 0) {
				g_Block[i][j].image = GetRand(7) + 1;
			}
		}
	}


	// ブロック連鎖チェック
	for (int i = 1; i < HEIGHT - 1; i++) {
		for (int j = 1; j < WIDTH - 1; j++) {
			Result += RensaCheck(i, j);
		}
	}

	if (Result == 0) {						// P.11 ⑨ 誤りの可能性大！
		g_GameState = GAME_MAIN;
		}
	g_Level += 1;
	g_Mission += 1;
	g_Time = TIMELIMIT;

	//カウント配列の初期化
	for (int i = 0; i < ITEM_MAX; i++) {
		g_Item[i] = 0;
	}
}


int RensaCheck(int y, int x) {

	int CountH = 0;
	int ColorH = 0;

	SaveBlock();
	RensaCheckH(y, x, &CountH, &ColorH);
	if (CountH < 3) {
		RestoreBlock();		// ブロックの連鎖が3個未満ならブロックを戻す
	}


	int CountW = 0;
	int ColorW = 0;
	SaveBlock();
	RensaCheckW(y, x, &CountW, &ColorW);
	if (CountW < 3) {
		RestoreBlock();		// ブロックの連鎖が3個未満ならブロックを戻す
	}


	if ((CountH >= 3 || CountW >= 3)) {
		if (CountH >= 3) {
			g_Item[ColorH - 1] += CountH;
			g_Score += CountH * 10;
			g_Time += 60;
		}

		if (CountW >= 3) {
			g_Item[ColorW - 1] += CountW;
			g_Score += CountW * 10;
			g_Time += 60;
		}
		return 1;
	}
	return 0;

}


int RensaCheckH(int y, int x, int* cnt, int* col) {
	/**************************************************
	* 縦の隣接している同色の個数を調べる(探索後に消す)
	* 戻り値：縦の隣接している同色の個数
	**************************************************/

	// 対象ブロックが外枠の場合 return 0; で処理を拔ける
	if (g_Block[y][x].image == 0) return 0;

	*col = g_Block[y][x].image;
	int c = g_Block[y][x].image;
	g_Block[y][x].image = 0;
	(*cnt)++;

	if (g_Block[y + 1][x].image == c)RensaCheckH(y + 1, x, cnt, col);
	if (g_Block[y - 1][x].image == c)RensaCheckH(y - 1, x, cnt, col);

	return 0;
}


int RensaCheckW(int y, int x, int* cnt, int* col) {
	/**************************************************
	* 横の隣接している同色の個数を調べる(探索後に消す)
	* 戻り値：横の隣接している同色の個数
	**************************************************/

	// 対象ブロックが外枠の場合 return 0; で処理を拔ける
	if (g_Block[y][x].image == 0) return 0;

	*col = g_Block[y][x].image;
	int c = g_Block[y][x].image;
	g_Block[y][x].image = 0;
	(*cnt)++;

	if (g_Block[y][x + 1].image == c)RensaCheckW(y, x + 1, cnt, col);
	if (g_Block[y][x - 1].image == c)RensaCheckW(y, x - 1, cnt, col);

	return 0;
}


void SaveBlock(void) {

	for (int i = 0; i < HEIGHT; i++) {
		for (int j = 0; j < WIDTH; j++) {
			g_Block[i][j].backup = g_Block[i][j].image;
		}
	}
}


void RestoreBlock(void) {

	for (int i = 0; i < HEIGHT; i++) {
		for (int j = 0; j < WIDTH; j++) {
			g_Block[i][j].image = g_Block[i][j].backup;
		}
	}
}

void SelectBlock(void) {

	static int ClickFlg = 0;

	int SelectX = g_MouseX / BLOCKSIZE;
	int SelectY = g_MouseY / BLOCKSIZE;

	// 選択ブロックの範囲を制御
	if (SelectX < 0)SelectX = 0;
	if (SelectX > WIDTH - 3)SelectX = WIDTH - 3;
	if (SelectY < 0)SelectY = 0;
	if (SelectY > HEIGHT - 3)SelectY = HEIGHT - 3;

	// クリックでブロックを選択
	if (g_Keyflg & MOUSE_INPUT_LEFT) {
		PlaySoundMem(g_ClickSE, DX_PLAYTYPE_BACK);
		if (ClickFlg == 0) {
			g_SelectX1 = SelectX;
			g_SelectY1 = SelectY;
			ClickFlg = 1;
		}
		//else if (ClickFlg == 1) {
		else if (ClickFlg == 1 && ((abs(g_SelectX1 - SelectX) == 1 && abs(g_SelectY1 - SelectY) == 0) || (abs(g_SelectX1 - SelectX) == 0 && abs(g_SelectY1 - SelectY) == 1))) {

			g_SelectX2 = SelectX;
			g_SelectY2 = SelectY;
			ClickFlg = 2;
		}
	}

	// 選択ブロックを描画
	DrawGraph(SelectX * BLOCKSIZE, SelectY * BLOCKSIZE, g_BlockImage[9], TRUE);
	if (ClickFlg >= 1) {
		DrawGraph(g_SelectX1 * BLOCKSIZE, g_SelectY1 * BLOCKSIZE, g_BlockImage[9], TRUE);
	}
	// 選択ブロックを交換
	if (ClickFlg == 2) {
		int TmpBlock = g_Block[g_SelectY1 + 1][g_SelectX1 + 1].image;
		g_Block[g_SelectY1 + 1][g_SelectX1 + 1].image = g_Block[g_SelectY2 + 1][g_SelectX2 + 1].image;

		g_Block[g_SelectY2 + 1][g_SelectX2 + 1].image = TmpBlock;


		// 連鎖が3つ以上か調べる
		int Result = 0;
		Result += RensaCheck(g_SelectY1 + 1, g_SelectX1 + 1);
		Result += RensaCheck(g_SelectY2 + 1, g_SelectX2 + 1);

		// 連鎖が3未満なら選択ブロックを元に戻す
		if (Result == 0) {
			TmpBlock = g_Block[g_SelectY1 + 1][g_SelectX1 + 1].image;
			g_Block[g_SelectY1 + 1][g_SelectX1 + 1].image = g_Block[g_SelectY2 + 1][g_SelectX2 + 1].image;
			g_Block[g_SelectY2 + 1][g_SelectX2 + 1].image = TmpBlock;
		}
		else {
			// 連鎖が3つ以上ならブロックを消し、ブロック移動処理へ移行する
			g_Status = 1;
		}


		// 次にクリックができるように、ClickFlg を0にする。
		ClickFlg = 0;
	}
}


void MoveBlock(void) {

	PlaySoundMem(g_MoveBlockSE, DX_PLAYTYPE_BACK);

	// 下へ移動する処理
	for (int i = 1; i < HEIGHT - 1; i++) {
		for (int j = 1; j < WIDTH - 1; j++) {
			if (g_Block[i][j].image == 0) {
				for (int k = i; k > 0; k--) {
					g_Block[k][j].image = g_Block[k - 1][j].image;
					g_Block[k - 1][j].image = 0;
				}
			}
		}
	}

	// 空のブロックを生成する処理
	for (int i = 1; i < HEIGHT - 1; i++) {
		for (int j = 1; j < WIDTH - 1; j++) {
			if (g_Block[i][j].image == 0) {
				g_Block[i][j].image = GetRand(7) + 1;
			}
		}
	}


	// ブロック選択処理へ移行する
	//g_Status = 0;

	// ブロック連鎖チェック処理へ移行する
	g_Status = 3;
}


void CheckBlock(void) {

	int Result = 0;

	for (int i = 1; i < HEIGHT - 1; i++) {
		for (int j = 1; j < WIDTH - 1; j++) {
			Result += RensaCheck(i, j);
		}
	}

	// 連鎖がなくなれば、ブロック選択へ移行する
	// そうでなければ、ブロック移動へ移行して連鎖チェックを継続する
	if (Result == 0) {
		// ブロック選択処理へ移行する
		//g_Status = 0;

		// クリア条件のチェック処理へ移行する
		g_Status = 4;
	}
	else {
		// 連鎖が3つ以上ならブロックを消し、ブロック移行処理へ移行する
		g_Status = 1;
	}
}

void CheckClear(void) {

	// クリア条件のチェック
	// クリア条件フラグを 0 とし、各スクールの消去ブロックが
	// レベルよりもブロック数が少なかったら、チェック処理を中断してゲームを続行する

	int ClearFlg = 0;
	for (int i = 0; i < ITEM_MAX; i++) {
		if (g_Item[i] < g_Mission) {
			ClearFlg = 1;
			break;
		}
	}

	if (ClearFlg == 0) {
		g_GameState = GAME_CLEAR;
	}
	else {
		g_Status = 0;
	}

}


void FadeOutBlock(void) {

	if (CheckSoundMem(g_FadeOutSE) == 0) {
		PlaySoundMem(g_FadeOutSE, DX_PLAYTYPE_BACK);
	}

	static int BlendMode = 255;
	// 描画ブレンドモードをアルファブレンドにする
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, BlendMode);
	for (int i = 1; i < HEIGHT - 1; i++) {
		for (int j = 1; j < WIDTH - 1; j++) {
			if (g_Block[i][j].image == 0) {
				DrawGraph(g_Block[i][j].x, g_Block[i][j].y, g_BlockImage[g_Block[i][j].backup], TRUE);
			}
		}
	}

	// 描画ブレンドモードをノーブレンドにする
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	BlendMode -= 5;
	if (BlendMode <= 0) {
		BlendMode = 255;
		g_Status = 2;
		StopSoundMem(g_FadeOutSE);
	}

}


int LoadSounds(void) {

	if ((g_TitleBGM = LoadSoundMem("sounds/08/muci_hono_r01.mp3")) == -1)return -1;
	if ((g_GameClearSE = LoadSoundMem("sounds/08/muci_fan_06.mp3")) == -1)return -1;
	if ((g_GameOverSE = LoadSoundMem("sounds/08/muci_bara_07.mp3")) == -1)return -1;
	if ((g_ClickSE = LoadSoundMem("sounds/08/ta_ta_warekie02.mp3")) == -1)return -1;
	if ((g_FadeOutSE = LoadSoundMem("sounds/08/ani_ta_biyon02.mp3")) == -1)return -1;
	//if ((g_FadeOutSE = LoadSoundMem("sounds/08/ta_ta_warekie02.mp3")) == -1)return -1;
	if ((g_MoveBlockSE = LoadSoundMem("sounds/08/ta_ta_doron01.mp3")) == -1)return -1;

	return 0;
}
