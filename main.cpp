/*******************************
**	　　どうぶつしょうぎ　　　**
*******************************/
#include "DxLib.h"
#include<stdlib.h>

/***********************************************
 * 列挙体の宣言
 ***********************************************/
typedef enum GAME_MODE
{
	GAME_TITLE,
	GAME_INIT,
	GAME_END,
	GAME_MAIN,
	GAME_CLEAR,
	GAME_OVER,
	END = 99
};

/***********************************************
 * 定数の宣言
 ***********************************************/
const int HEIGHT = 12;
const int WIDTH = 12;

/***********************************************
 * 変数の宣言
 ***********************************************/
int	g_OldKey;		//前回の入力キー
int	g_NowKey;		//今回の入力キー
int	g_KeyFlg;		//入力キー情報
int	g_MouseX;		//マウスX座標
int	g_MouseY;		//マウスY座標
int	g_GameState = GAME_TITLE;   //ゲームモード

int TitleImage;       //タイトル画像
int StageImage;       //ステージ画像
int KomaImage[10];   //コマ画像

/***********************************************
 * 関数のプロトタイプ宣言
 ***********************************************/
void GameInit(void);		//ゲーム初期化処理
void GameMain(void);		//ゲームメイン処理
void DrawGameTitle(void);	//ゲームタイトル処理

void DrawStage(void);	    //ステージ
void StageInit(void);	    //ステージ初期処理

int LoadImages(void);      //画像読込み

/***********************************************
 * プログラムの開始
 ***********************************************/
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow)
{
	//タイトルを設定
	SetMainWindowText("どうぶつしょうぎ");

	//ウィンドウサイズ
	SetGraphMode(640, 640, 32);

	//ウィンドウモードで起動
	ChangeWindowMode(TRUE);

	//DXライブラリの初期化処理
	if (DxLib_Init() == -1)   return -1;

	//描画先画面を裏にする
	SetDrawScreen(DX_SCREEN_BACK);

	//画像読込み関数を呼び出し
	if (LoadImages() == -1)   return -1;

	// ゲームループ
	while (ProcessMessage() == 0 && g_GameState != END && !(g_KeyFlg & PAD_INPUT_START))
	{
		// 入力キー取得
		g_OldKey = g_NowKey;
		g_NowKey = GetMouseInput();
		g_KeyFlg = g_NowKey & ~g_OldKey;

		//マウスの位置を取得
		GetMousePoint(&g_MouseX, &g_MouseY);

		ClearDrawScreen();		// 画面の初期化

		switch (g_GameState)
		{
		case GAME_TITLE:
			DrawGameTitle();
			break;
		case GAME_INIT:
			GameInit();
			break;
			break;
		case GAME_MAIN:
			GameMain();
			break;
		}
		ScreenFlip();			//裏画面の内容を表画面に反映
	}

	DxLib_End();	//DXライブラリ使用の終了処理

	return 0;	//ソフトの終了
}

void DrawGameTitle(void)
{
	//タイトル画像表示
	DrawGraph(0, 0, TitleImage, FALSE);

	DrawBox(200, 400, 460, 460, 0xffffff, TRUE);

	//ゲームモードを切り替える
	if (g_KeyFlg & MOUSE_INPUT_LEFT)
	{
		if (g_MouseX > 200 && g_MouseX < 460 && g_MouseY>400 && g_MouseY < 465)
		{
			g_GameState = GAME_INIT;   //ゲームスタート
		}
	}
}

void GameInit(void)
{
	//ゲームメイン処理へ
	g_GameState = GAME_MAIN;

	//ステージの初期化
	StageInit();
}

void DrawStage(void)
{

}

void StageInit(void)
{

}


void GameMain(void)
{
	//ステージ画像表示
	DrawGraph(0, 0, StageImage, FALSE);
}


int LoadImages()
{
	//タイトル
	if ((TitleImage = LoadGraph("images/Title.jpg")) == -1)   return -1;
	//ステージ
	if ((StageImage = LoadGraph("images/Stage.jpg")) == -1)   return -1;
	//ブロック画像
	//if (LoadDivGraph("images/Free/1Koma.gif", 10, 5, 2, 80, 80, KomaImage) == -1)   return -1;
}