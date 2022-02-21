/*******************************
**	　　どうぶつしょうぎ　　　**
*******************************/
#include "DxLib.h"
#include<stdlib.h>

#define PieceKinds 4 //駒の種類

LPCSTR font_path = "Fonts/Cherrybomb/Cherrybomb.ttf";

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
	END = 99,
	CHICK = 0,	//ヒヨコ(歩兵)
	ELEPHA,		//ゾウ(飛車)
	GIRAF,		//キリン(角行)
	LION		//ライオン(王将)
};

/***********************************************
 * 定数の宣言
 ***********************************************/
const int HEIGHT = 12;
const int WIDTH = 12;

/***********************************************
 * 構造体の宣言
 ***********************************************/

//駒構造体の宣言
typedef struct PieceStatus {
	int x, y;		//駒の座標
	int w, h;		//駒の大きさ
	int images;		//駒の画像データ
	int flg;		//駒の有無情報
}PieceSt;

PieceSt Pieces[PieceKinds] = { CHICK,ELEPHA,GIRAF,LION };	//駒情報配列


/***********************************************
 * 変数の宣言
 ***********************************************/
int	OldKey;		//前回の入力キー
int	NowKey;		//今回の入力キー
int	KeyFlg;		//入力キー情報
int	MouseX;		//マウスX座標
int	MouseY;		//マウスY座標
int	GameState = GAME_TITLE;   //ゲームモード

int WaitTime = 0;    //	待ち時間

int TitleImage;      //タイトル画像
int StageImage;      //ステージ画像
int KomaImage[10];   //コマ画像

/***********************************************
 * 関数のプロトタイプ宣言
 ***********************************************/
void GameInit(void);		//ゲーム初期化処理
void GameMain(void);		//ゲームメイン処理
void GamePause(void);
void DrawGameTitle(void);	//ゲームタイトル処理
void GameEnd(void);

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
	SetGraphMode(600, 700, 32);

	//ウィンドウモードで起動
	ChangeWindowMode(TRUE);

	// フォント読み込み
	if (AddFontResourceEx(font_path, FR_PRIVATE, NULL) > 0) {
	}
	else {
		// フォント読込エラー処理
		MessageBox(NULL, "フォント読込失敗", "", MB_OK);
	}
	ChangeFont("Cherry bomb", DX_CHARSET_DEFAULT);

	//DXライブラリの初期化処理
	if (DxLib_Init() == -1)   return -1;

	//描画先画面を裏にする
	SetDrawScreen(DX_SCREEN_BACK);

	//画像読込み関数を呼び出し
	if (LoadImages() == -1)   return -1;

	// ゲームループ
	while (ProcessMessage() == 0 && GameState != END && !(KeyFlg & PAD_INPUT_START))
	{
		// 入力キー取得
		OldKey = NowKey;
		NowKey = GetMouseInput();
		KeyFlg = NowKey & ~OldKey;

		//マウスの位置を取得
		GetMousePoint(&MouseX, &MouseY);

		if (GetJoypadInputState(DX_INPUT_PAD1) & KEY_INPUT_PAUSE != 0) {
			GamePause();
		}

		ClearDrawScreen();		// 画面の初期化

		switch (GameState)
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

	//DrawBox(160, 405, 460, 465, 0xffffff, TRUE);

	//文字の表示(点滅)
	if (++WaitTime < 50)
	{
		SetFontSize(50);
		DrawString(170, 410, "す た ぁ と", 0x000000);
	}
	else if (WaitTime > 100)
	{
		WaitTime = 0;
	}

	//ゲームモードを切り替える
	if (KeyFlg & MOUSE_INPUT_LEFT)
	{
		if (MouseX > 160 && MouseX < 460 && MouseY>405 && MouseY < 465)
		{
			GameState = GAME_INIT;   //ゲームスタート
		}
	}
}

void GameInit(void)
{
	//ゲームメイン処理へ
	GameState = GAME_MAIN;
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
	
	for (int i = 0; i < 3; i++) { 
		DrawRotaGraph(120 + (i)*180, 130, 1.8, 0, KomaImage[i], TRUE, FALSE);
		for (int i = 0; i < 3; i++)
			DrawRotaGraph(120 + (i) * 180, 270, 1.8, 0, KomaImage[i+3], TRUE, FALSE);
		for (int i = 0; i < 3; i++)
			DrawRotaGraph(120 + (i) * 180, 420, 1.8, 0, KomaImage[i + 6], TRUE, FALSE);
	}
	//DrawRotaGraph(120, 130, 2.0, 0, KomaImage[0], TRUE, FALSE);


	if (KeyFlg & MOUSE_INPUT_LEFT) {
		if (MouseX < 120 && MouseX > 20 && MouseY > 20 && MouseY < 150) {
			DrawRotaGraph(120, 130, 1.8, 0, KomaImage[9], TRUE, FALSE);
		}
	}
	
}


int LoadImages()
{
	//タイトル
	if ((TitleImage = LoadGraph("images/Title.jpg")) == -1)   return -1;
	//ステージ
	if ((StageImage = LoadGraph("images/Stage.jpg")) == -1)   return -1;
	//ブロック画像
	if (LoadDivGraph("images/Koma.png", 10, 5, 2, 80, 80, KomaImage) == -1)   return -1;
}
	
void GameEnd(void) {

	DrawFormatString(300, 300, 0x000000,"げーむしゅうりょう");

	if(RemoveFontResourceEx(font_path, FR_PRIVATE, NULL)) {
	}
 else {
 MessageBox(NULL, "remove failure", "", MB_OK);
	}
}

void GamePause(void) {

	DrawFormatString(300, 300, 0x000000, "ぽーず");
}