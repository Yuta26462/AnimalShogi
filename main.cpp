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

int Live2D_ModelHandle;

//サウンド
int KomaClick, KomaNaru, StartClick;
//BGM
int TitleBGM, TitleBGM01, TitleBGM02, TitleBGM03, TitleBGM04, TitleBGM05, TitleBGM06;

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
int LoadSounds(void);



/***********************************************
 * プログラムの開始
 ***********************************************/
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow)
{
	//タイトルを設定
	SetMainWindowText("どうぶつしょうぎ");

	//ウィンドウモードで起動
	ChangeWindowMode(TRUE);

	//ウィンドウサイズ
	//SetGraphMode(600, 700, 32);
	SetGraphMode(800, 700, 32);		//描画する最大ウィンドウサイズを設定
	SetWindowSizeChangeEnableFlag(FALSE, FALSE);	//手動で変更、FitScreenをオフにする。
	SetWindowSize(600, 700);	//ウィンドウサイズを変更


	// フォント読み込み
	if (AddFontResourceEx(font_path, FR_PRIVATE, NULL) > 0) {
	}
	else {
		// フォント読込エラー処理
		MessageBox(NULL, "フォント読込失敗", "", MB_OK);
	}
	ChangeFont("Cherry bomb", DX_CHARSET_DEFAULT);


	// Live2D Cubism Core DLL の読み込み( 64bit アプリの場合と 32bit アプリの場合で読み込む dll を変更 )
#ifdef _WIN64
	Live2D_SetCubism4CoreDLLPath("dll/live2d/x86_64/Live2DCubismCore.dll");
#else
	Live2D_SetCubism4CoreDLLPath("dll/live2d/x86/Live2DCubismCore.dll");
#endif



	//DXライブラリの初期化処理
	if (DxLib_Init() == -1)   return -1;

	//描画先画面を裏にする
	SetDrawScreen(DX_SCREEN_BACK);

	
	if (LoadImages() == -1)   return -1;	//画像読込み関数を呼び出し
	if (LoadSounds() == -1)   return -1;	//音声読み込み	

	Live2D_ModelHandle = Live2D_LoadModel("dll/hiyori_free_jp/runtime/hiyori_free_t08.model3.json");	//Live2Dモデル読み込み
	Live2D_Model_SetTranslate(Live2D_ModelHandle, 300, 10);		//Live2Dモデルの座標を設定
	Live2D_Model_SetExtendRate(Live2D_ModelHandle,1.8f, 1.8f);

	// ゲームループ
	while (ProcessMessage() == 0 && GameState != END && !(KeyFlg & PAD_INPUT_START))
	{


		// 入力キー取得
		OldKey = NowKey;
		NowKey = GetMouseInput();
		KeyFlg = NowKey & ~OldKey;

		//マウスの位置を取得
		GetMousePoint(&MouseX, &MouseY);


		if (CheckHitKey(KEY_INPUT_Q) == 1) GamePause();			//Qキーでゲーム終了

		if (GetWindowUserCloseFlag() != 0)	GameState = END;		//メインウィンドウの閉じるボタンを押したらゲーム終了

		ClearDrawScreen();		// 画面の初期化

		//	モーション再生が終了したらIdleモーションをランダム再生
		if (Live2D_Model_IsMotionFinished(Live2D_ModelHandle) == TRUE) {
			Live2D_Model_StartMotion(Live2D_ModelHandle, "Idle", GetRand(3));
		}

		Live2D_Model_Update(Live2D_ModelHandle, 1 / 300.0f);	//Live2Dモデル更新
		Live2D_RenderBegin();	//Live2Dモデル描画開始準備


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
		case GAME_END:
			GameEnd();
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

	if (CheckSoundMem(TitleBGM) == 0) PlaySoundMem(TitleBGM, DX_PLAYTYPE_BACK);

	//DrawGraph(600, 0, StageImage, FALSE);			//Live2D用背景
	//Live2D_Model_Draw(Live2D_ModelHandle);		//Live2Dモデル描画

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
			PlaySoundMem(StartClick, DX_PLAYTYPE_BACK);
			GameState = GAME_INIT;   //ゲームスタート
		}
	}
}

void GameInit(void)
{
	//駒の初期化
	for (int i = 0; i < PieceKinds; i++)
	{
		switch (i)
		{
		case 0:
			Pieces[i].x = 300;
			Pieces[i].y = 560;
			break;
		case 1:
			Pieces[i].x = 120;
			Pieces[i].y = 560;
			break;
		case 2:
			Pieces[i].x = 480;
			Pieces[i].y = 560;
			break;
		case 3:
			Pieces[i].x = 300;
			Pieces[i].y = 420;
			break;
		}
		Pieces[i].images = KomaImage[i];
		Pieces[i].flg = 1;
	}

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
	SetWindowSize(800, 700);	//ウィンドウサイズの変更

	//ステージ画像表示
	DrawGraph(0, 0, StageImage, FALSE);
	
	DrawGraph(600, 0, StageImage, FALSE);		//Live2D用背景

	Live2D_Model_Draw(Live2D_ModelHandle);		//Live2Dモデル描画
	//Live2D_Model_StartMotion(Live2D_ModelHandle, "FlickDown", 0);

	if(CheckSoundMem(TitleBGM01) == 0) PlaySoundMem(TitleBGM01, DX_PLAYTYPE_BACK);

	for (int i = 0; i < 4; i++) {
		DrawRotaGraph(Pieces[i].x, Pieces[i].y, 1.8, 0, Pieces[i].images, TRUE, FALSE);
	}

	/*for (int i = 0; i < 3; i++) { 
		DrawRotaGraph(120 + (i)*180, 130, 1.8, 0, KomaImage[i], TRUE, FALSE);
		for (int i = 0; i < 3; i++)
			DrawRotaGraph(120 + (i) * 180, 270, 1.8, 0, KomaImage[i+3], TRUE, FALSE);
		for (int i = 0; i < 3; i++)
			DrawRotaGraph(120 + (i) * 180, 420, 1.8, 0, KomaImage[i + 6], TRUE, FALSE);
	}*/
	//DrawRotaGraph(120, 130, 2.0, 0, KomaImage[0], TRUE, FALSE);

	if (KeyFlg & MOUSE_INPUT_LEFT) {
		if (MouseX < 120 && MouseX > 20 && MouseY > 20 && MouseY < 150) {
			DrawRotaGraph(120, 130, 1.8, 0, KomaImage[9], TRUE, FALSE);
			PlaySoundMem(KomaClick, DX_PLAYTYPE_BACK);
		}

		if (KeyFlg & MOUSE_INPUT_LEFT) {
			if (MouseX < 800 && MouseX > 600 && MouseY > 35 && MouseY < 670) {
				Live2D_Model_StartMotion(Live2D_ModelHandle, "Tap2", GetRand(5));
				PlaySoundMem(KomaNaru, DX_PLAYTYPE_BACK);
			}
		}
	}

	DrawFormatString(70, 25, 0x000000, "x:%d  y:%d", MouseX, MouseY);	//デバック用 座標確認

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
	} else {
		MessageBox(NULL, "remove failure", "", MB_OK);
	}
	Live2D_RenderEnd();		//Live2D描画の終了
	Live2D_DeleteModel(Live2D_ModelHandle);	//Live2Dモデル削除

}

void GamePause(void) {

	DrawFormatString(300, 300, 0x000000, "ぽーず");

	WaitTimer(1000);		//1000ms(1秒)待機
	GameState = END;
}


int LoadSounds(void) {

	if ((KomaClick = LoadSoundMem("sounds/KomaClick.mp3")) == -1)return -1;
	if ((KomaNaru = LoadSoundMem("sounds/KomaNaru.mp3")) == -1)return -1;
	if ((StartClick = LoadSoundMem("sounds/StartClick.mp3")) == -1)return -1;
	if ((TitleBGM = LoadSoundMem("sounds/BGM/TitleBGM.mp3")) == -1)return -1;
	if ((TitleBGM01 = LoadSoundMem("sounds/BGM/TitleBGM01.mp3")) == -1)return -1;
	if ((TitleBGM02 = LoadSoundMem("sounds/BGM/TitleBGM02.mp3")) == -1)return -1;
	if ((TitleBGM03 = LoadSoundMem("sounds/BGM/TitleBGM03.mp3")) == -1)return -1;
	if ((TitleBGM04 = LoadSoundMem("sounds/BGM/TitleBGM04.mp3")) == -1)return -1;
	if ((TitleBGM05 = LoadSoundMem("sounds/BGM/TitleBGM05.mp3")) == -1)return -1;


	ChangeVolumeSoundMem(150, TitleBGM);
	ChangeVolumeSoundMem(150, TitleBGM01);
}