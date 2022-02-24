/*******************************
**	　　どうぶつしょうぎ　　　**
*******************************/
#include "DxLib.h"
#include<stdlib.h>
#include <string.h>		//読み込まなくても文字列操作関数使えるが念の為


#define KomaKinds 4 //駒の種類
#define Sizex 3
#define Sizey 4
#define XMARGIN 180
#define YMARGIN 140

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
	LION = 0,	//ライオン(王将)
	GIRAF,		//キリン(角行)
	ELEPHA,		//ゾウ(飛車)
	CHICK		//ヒヨコ(歩兵
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
typedef struct KomaStatus {
	int x, y;		//駒の座標
	int w, h;		//駒の大きさ
	int images;		//駒の画像データ
	int flg;		//駒の有無情報
}KomaSt;

KomaSt Komas[KomaKinds] = { CHICK,ELEPHA,GIRAF,LION };	//駒情報配列


/***********************************************
 * 変数の宣言
 ***********************************************/
int	OldKey;		//前回の入力キー
int	NowKey;		//今回の入力キー
int	KeyFlg;		//入力キー情報
int	MouseX;		//マウスX座標
int	MouseY;		//マウスY座標
int	GameState = GAME_TITLE;   //ゲームモード

int Status = 0;		//ステージのステータス

int Stage[Sizey][Sizex];		//ステージ配列

int WaitTime = 0;    //	待ち時間
int StartTime = GetNowCount();	//起動からの経過時間

int TitleImage,Live2DStage;      //タイトル画像
int StageImage;      //ステージ画像
int KomaImage[10];   //コマ画像

int Live2D_ModelHandle, Live2D_ModelHandle2;
int ContentsFont;	//ISendMessege用フォント
char ms[];	//文字列渡し用

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

void SelectKomas(void);	//駒の選択
void MoveChick(void);		//ヒヨコの移動処理
void MoveGiraf(void);		//キリンの移動処理
void MoveElepha(void);		//ゾウの移動処理
void MoveLion(void);		//ライオンの移動処理


int LoadImages(void);      //画像読込み
int LoadSounds(void);	  //音声読み込み

void ISendMessege(char* Contents, int partner = 0);
void SideBar(void);


/***********************************************
 * プログラムの開始
 ***********************************************/
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow)
{
	//タイトルを設定
	SetMainWindowText("どうぶつしょうぎ");

	SetWindowIconID(01);	//アイコンを設定

	//ウィンドウモードで起動
	ChangeWindowMode(TRUE);

	//ウィンドウサイズ
	//SetGraphMode(800, 700, 32);
	SetGraphMode(1000, 700, 32);		//描画する最大ウィンドウサイズを設定
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
	Live2D_ModelHandle2 = Live2D_LoadModel("dll/21miku_street/21miku_street.model3.json");	//Live2Dモデル読み込み
	Live2D_Model_SetTranslate(Live2D_ModelHandle, 400, -50);		//Live2Dモデルの座標を設定
	Live2D_Model_SetTranslate(Live2D_ModelHandle2, -380, 0);		//Live2Dモデルの座標を設定
	Live2D_Model_SetExtendRate(Live2D_ModelHandle,1.8f, 1.8f);		//Live2Dモデルの大きさを設定
	Live2D_Model_SetExtendRate(Live2D_ModelHandle2, 2.0f, 2.0f);	//Live2Dモデルの大きさを設定
	ContentsFont = CreateFontToHandle("Cherry bomb", 30, 2, DX_CHARSET_DEFAULT);	//ContentsFontフォントデータを作成

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
		if (Live2D_Model_IsMotionFinished(Live2D_ModelHandle2) == TRUE) {
			Live2D_Model_StartMotion(Live2D_ModelHandle2, "Idle", GetRand(4));
		}

		Live2D_Model_Update(Live2D_ModelHandle, 1 / 200.0f);	//Live2Dモデル更新
		Live2D_Model_Update(Live2D_ModelHandle2, 1 / 200.0f);	//Live2Dモデル更新
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
			StopSoundMem(TitleBGM);
			PlaySoundMem(StartClick, DX_PLAYTYPE_BACK);
			GameState = GAME_INIT;   //ゲームスタート
		}
	}
}

void GameInit(void)
{
	//駒の初期化
	for (int i = 0; i < KomaKinds; i++)
	{
		switch (i)
		{
		case 0:
			Komas[i].x = 500;
			Komas[i].y = 560;
			break;
		case 1:
			Komas[i].x = 320;
			Komas[i].y = 560;
			break;
		case 2:
			Komas[i].x = 680;
			Komas[i].y = 560;
			break;
		case 3:
			Komas[i].x = 500;
			Komas[i].y = 420;
			break;
		}
		Komas[i].images = KomaImage[i];
		Komas[i].flg = 1;
	}

	StageInit();		//ステージの初期化

	//ゲームメイン処理へ
	GameState = GAME_MAIN;
}

void DrawStage(void)
{

}

void StageInit(void)
{
	//ステージ配列の初期化
	for (int i = 0; i < Sizey; i++)
	{
		for (int j = 0; j < Sizex; j++)
		{
			if (i == 0 || (i == 1 && j == 1) || i == 3 || (i == 2 && j == 1)) {
				Stage[i][j] = 1;
			}
			else
			{
				Stage[i][j] = 0;
			}
			
		}
	}
	
}


void GameMain(void)
{
	SetWindowSize(1000, 700);	//ウィンドウサイズの変更

	//ステージ画像表示
	DrawGraph(200, 0, StageImage, FALSE);

	SideBar();

	if(CheckSoundMem(TitleBGM01) == 0) PlaySoundMem(TitleBGM01, DX_PLAYTYPE_BACK);

	for (int i = 0; i < 4; i++) {
		DrawRotaGraph(Komas[i].x, Komas[i].y, 1.8, 0, Komas[i].images, TRUE, FALSE);
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
		if (MouseX < 405 && MouseX > 230 && MouseY > 55 && MouseY < 200) {
			int i = GetRand(10);
			StartTime = GetNowCount();
			while (GetNowCount() - StartTime < 500)
			{
				DrawRotaGraph(315, 130, 1.8, 0, KomaImage[i], TRUE, FALSE);
				if (ProcessMessage() == -1)break;
				ScreenFlip();
			}
			PlaySoundMem(KomaClick, DX_PLAYTYPE_BACK);
		}
	}

	for (int i = 0; i < Sizey; i++)
	{
		for (int j = 0; j < Sizex; j++)
		{
			DrawFormatString(j*100, i*150, 0xffffff, "%4d", Stage[i][j]);
		}
	}
	

	switch (Status) {
	case 0:
		SelectKomas();		//駒選択
		break;
	case 1:

		break;
	}

	DrawFormatString(270, 25, 0x000000, "x:%d  y:%d", MouseX, MouseY);	//デバック用 座標確認

}


int LoadImages()
{
	//タイトル
	if ((TitleImage = LoadGraph("images/Title.jpg")) == -1)   return -1;
	//ステージ
	if ((StageImage = LoadGraph("images/Stage.jpg")) == -1)   return -1;

	if ((Live2DStage = LoadGraph("images/Live2DStage.png")) == -1)   return -1;
	//ブロック画像
	if (LoadDivGraph("images/Koma.png", 10, 5, 2, 80, 80, KomaImage) == -1)   return -1;
}
	
void GameEnd(void) {

	DrawFormatString(500, 300, 0x000000,"げーむしゅうりょう");

	if(RemoveFontResourceEx(font_path, FR_PRIVATE, NULL)) {
	} else {
		MessageBox(NULL, "remove failure", "", MB_OK);
	}
	Live2D_RenderEnd();		//Live2D描画の終了
	Live2D_DeleteModel(Live2D_ModelHandle);	//Live2Dモデル削除
	Live2D_DeleteModel(Live2D_ModelHandle2);	//Live2Dモデル削除
	DeleteFontToHandle(ContentsFont);	//ContentsFontデータを削除

}

void GamePause(void) {

	DrawFormatString(500, 300, 0x000000, "ぽーず");

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


	ChangeVolumeSoundMem(200, TitleBGM);
	ChangeVolumeSoundMem(200, TitleBGM01);
}


void SideBar(void) {

	DrawGraph(0, 0, Live2DStage, FALSE);		//Live2D用背景
	DrawGraph(800, 0, Live2DStage, FALSE);		//Live2D用背景

	Live2D_Model_Draw(Live2D_ModelHandle);		//Live2Dモデル描画
	Live2D_Model_Draw(Live2D_ModelHandle2);		//Live2Dモデル描画
	//Live2D_Model_StartMotion(Live2D_ModelHandle, "FlickDown", 0);

	// マウス左クリック判定
	if (KeyFlg & MOUSE_INPUT_LEFT) {
		if (MouseX < 1000 && MouseX > 800 && MouseY > 20 && MouseY < 700) {
			Live2D_Model_StartMotion(Live2D_ModelHandle, "Tap2", GetRand(5));
			PlaySoundMem(KomaNaru, DX_PLAYTYPE_BACK);
		}

		if (MouseX < 220 && MouseX > 0 && MouseY > 20 && MouseY < 700) {
			Live2D_Model_StartMotion(Live2D_ModelHandle2, "Tap", GetRand(5));
			PlaySoundMem(KomaNaru, DX_PLAYTYPE_BACK);
		}

		if (MouseX < 195 && MouseX > 10 && MouseY > 590 && MouseY < 670) {
			GameState = GAME_TITLE;
			PlaySoundMem(StartClick, DX_PLAYTYPE_BACK);
			StopSoundMem(TitleBGM01);
			SetWindowSize(600, 700);
		}
	}

	// ステータス・メニュー描画
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 235);
	DrawBox(5, 360, 200, 690, 0xd2b48c, TRUE);
	DrawBox(4, 360, 199, 690, 0x000000, FALSE);
	DrawBox(800, 360, 990, 690, 0xd2b48c, TRUE);
	DrawBox(799, 360, 989, 690, 0x000000, FALSE);

	// タイトルボタン
	DrawBox(10, 590, 195, 670, 0xf5f5f5, TRUE);
	DrawBox(9, 590, 194, 670, 0x000000, FALSE);
	DrawFormatString(15, 610, 0x000000, "たいとる");

	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	//// 吹き出し描画
	//SetDrawBlendMode(DX_BLENDMODE_ALPHA, 150);
	//DrawOval(105, 265, 100, 80, 0x000000, FALSE);
	//DrawOval(105, 265, 99, 79, 0xf0f8ff, TRUE);
	//
	//DrawOval(895, 265, 100, 80, 0x000000, FALSE);
	//DrawOval(895, 265, 99, 79, 0xf0f8ff, TRUE);
	//SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	//ISendMessageテスト表示	
	char ms[] = "てすとです。";
	ISendMessege(ms);
	strcpy_s(ms, "しょうご");
	ISendMessege(ms, 1);
}

void ISendMessege(char* Contents, int partner) {

		switch (partner) {

		case 0:
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 200);
			DrawOval(105, 265, 100, 80, 0x000000, FALSE);
			DrawOval(105, 265, 99, 79, 0xf0f8ff, TRUE);
			DrawFormatStringToHandle(40, 260, 0x000000, ContentsFont, Contents);
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
			break;

		case 1:
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 200);
			DrawOval(895, 265, 100, 80, 0x000000, FALSE);
			DrawOval(895, 265, 99, 79, 0xf0f8ff, TRUE);
			DrawFormatStringToHandle(830, 260, 0x000000, ContentsFont, Contents);
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
			break;

		}
	}

void SelectKomas(void)
{
	static int ClickFlag = 0;

	/*int SelectX = MouseX/180;
	int SelectY = MouseY/140;*/

	//if (KeyFlg & MOUSE_INPUT_LEFT) {
	//	if (ClickFlag == 0) {
	//		ClickFlag = 1;

	//	}
	//	//Status = 1;
	//}
	//if (ClickFlag==1)
	//{
	//	DrawString(Pieces[CHICK].x, Pieces[CHICK].y - 140, "test", 0x000000, TRUE);
	//}

	if (KeyFlg & MOUSE_INPUT_LEFT /*& Stage[SelectX / 300][SelectY / 140]*/) {
		if (MouseX > Komas[CHICK].x - 70 && MouseX<Komas[CHICK].x + 70 && MouseY>Komas[CHICK].y - 70 && MouseY < Komas[CHICK].y + 70) {

			ClickFlag = 1;
			/*DrawString(Pieces[CHICK].x, Pieces[CHICK].y - 140,"test", 0x000000, TRUE);*/
		}
		else if (MouseX > Komas[GIRAF].x - 70 && MouseX<Komas[GIRAF].x + 70 && MouseY>Komas[GIRAF].y - 70 && MouseY < Komas[GIRAF].y + 70) {

			ClickFlag = 2;
		}
		else if (MouseX > Komas[ELEPHA].x - 70 && MouseX<Komas[ELEPHA].x + 70 && MouseY>Komas[ELEPHA].y - 70 && MouseY < Komas[ELEPHA].y + 70) {

			ClickFlag = 3;
		}
		else if (MouseX > Komas[LION].x - 70 && MouseX<Komas[LION].x + 70 && MouseY>Komas[LION].y - 70 && MouseY < Komas[LION].y + 70) {

			ClickFlag = 4;
		}
	}

	switch (ClickFlag)
	{
	case 1:
		MoveChick();
		break;
	case 2:
		MoveGiraf();
		break;
	case 3:
		MoveElepha();
		break;
	case 4:
		MoveLion();
		break;
	}

}

void MoveChick(void)
{
	if (Stage[(Komas[CHICK].y - 280) / YMARGIN][(Komas[CHICK].x - 320) / XMARGIN] == 0) {
		DrawCircle(Komas[CHICK].x, Komas[CHICK].y - YMARGIN, 30, 0x000000, TRUE);
	}

}

void MoveGiraf(void)
{
	if (Stage[(Komas[GIRAF].y - 280) / YMARGIN][(Komas[GIRAF].x - 320) / XMARGIN] == 0) {
		DrawCircle(Komas[GIRAF].x, Komas[GIRAF].y - YMARGIN, 30, 0x000000, TRUE);
	}
	if (Stage[Komas[GIRAF].y / YMARGIN - 1][(Komas[GIRAF].x - 140) / XMARGIN] == 0) {
		DrawCircle(Komas[GIRAF].x + XMARGIN, Komas[GIRAF].y, 30, 0x000000, TRUE);
	}
	if (Komas[GIRAF].x > 320) {
		if (Stage[Komas[GIRAF].y / YMARGIN - 1][(Komas[GIRAF].x - 140) / XMARGIN - 2] == 0) {
			DrawCircle(Komas[GIRAF].x - XMARGIN, Komas[GIRAF].y, 30, 0x000000, TRUE);
		}
	}
	if (Komas[GIRAF].y < 560) {
		if (Stage[Komas[GIRAF].y -280 / YMARGIN + 2][(Komas[GIRAF].x - 320) / XMARGIN - 2] == 0)
			DrawCircle(Komas[GIRAF].x, Komas[GIRAF].y + YMARGIN, 30, 0x000000, TRUE);
	}
	

	
}

void MoveElepha(void)
{
	DrawCircle(Komas[ELEPHA].x, Komas[ELEPHA].y - 140, 30, 0x000000, TRUE);
}

void MoveLion(void)
{
	DrawCircle(Komas[LION].x, Komas[LION].y - 140, 30, 0x000000, TRUE);
}