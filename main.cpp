/*******************************
**	　　どうぶつしょうぎ　　　**
*******************************/
#include "DxLib.h"
#include<stdlib.h>



#define KomaKinds 10 //駒の種類
#define Sizex 3		//盤面(横)
#define Sizey 4		//盤面(縦)
#define XMARGIN 180	//駒と駒の間隔(横)
#define HXMARGIN 90	//駒とマスの間隔(横)
#define YMARGIN 140	//駒と駒の間隔(縦)
#define HYMARGIN 70	//駒とマスの間隔(縦)

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
	//1P
	LION = 0,	//ライオン(王将)
	GIRAF,		//キリン(角行)
	ELEPHA,		//ゾウ(飛車)
	CHICK,		//ヒヨコ(歩兵)
	CHICKEN,	//ニワトリ(と金)
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
	int pflg;		//駒のプレイヤー情報(1P:1,2P:2)
}KomaSt;

KomaSt Komas[KomaKinds];	//駒情報配列


/***********************************************
 * 変数の宣言
 ***********************************************/
int	OldKey, OldKey2;		//前回の入力キー
int	NowKey, NowKey2;		//今回の入力キー
int	KeyFlg, KeyFlg2;		//入力キー情報
int	MouseX;		//マウスX座標
int	MouseY;		//マウスY座標
int	GameState = GAME_TITLE;   //ゲームモード
bool Pause;		//ポーズ判定

int Status;		//ステージのステータス
static int ClickFlag;	//クリックフラグ(クリックした駒の識別)
static int Mflag;		//移動可能マークフラグ
 int Cflag;		//駒クリックフラグ
static int Pflag;		//プレイヤーフラグ 1P:1,2P;2
static int Branch;		//1Pと2Pの分岐用変数
static int Abranch;		//Branchの反転

int Stage[Sizey][Sizex];		//ステージ配列

int Handrand;		//先手・後手判別用乱数

int WaitTime = 0;    //	待ち時間
int StartTime = GetNowCount();	//起動からの経過時間

int TitleImage,Live2DStage;      //タイトル画像
int StageImage;      //ステージ画像
int KomaImage[10];   //コマ画像
int Flame, Button;//UI画像
int GameClearImage;		//ゲームクリア画像
int HandCount, HandCount2;		//てめカウント用

int Live2D_ModelHandle, Live2D_ModelHandle2;//Live2Dハンドル
int mscount;		//ISendMessage用カウント
bool msdis;			//ISendMessage用表示・非表示フラグ
int ContentsFont;	//ISendMessege用フォント
int MenuFont;		//メニュー用フォント

//サウンド
int KomaClick, KomaNaru, StartClick;
//BGM
int TitleBGM, TitleBGM01,GameClearBGM;

/***********************************************
 * 関数のプロトタイプ宣言
 ***********************************************/
void GameInit(void);		//ゲーム初期化処理
void GameMain(void);		//ゲームメイン処理
void GamePause(void);
void DrawGameTitle(void);	//ゲームタイトル処理
void GameClear(void);		//ゲームクリア
void GameEnd(void);
void StageInit(void);	    //ステージ初期処理

void SelectKomas(void);	//駒の選択
void SelectDisplay(int x, int y);	//選択できる範囲表示
void MoveChick(void);		//ヒヨコの移動処理
void MoveGiraf(void);		//キリンの移動処理
void MoveElepha(void);		//ゾウの移動処理
void MoveLion(void);		//ライオンの移動処理
void ChangeTurn(void);		//自分、相手ターン変更処理
int CheckKoma(int Pf,int Course);			//対象の駒のプレイヤーを調べる

int LoadImages(void);      //画像読込み
int LoadSounds(void);	  //音声読み込み

void ISendMessege(const TCHAR* Contents, int partner = 0);		//メッセージ表示
void SideBar(void);		//サイドメニュー表示


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
	SetGraphMode(1000, 700, 32);		//描画する最大ウィンドウサイズを設定
	SetWindowSizeChangeEnableFlag(FALSE, FALSE);	//手動で変更、FitScreenをオフにする。


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
	Live2D_Model_SetTranslate(Live2D_ModelHandle, 400, -50);		//Live2Dモデルの座標を初期設定
	Live2D_Model_SetTranslate(Live2D_ModelHandle2, -380, 0);		//Live2Dモデルの座標を初期設定
	Live2D_Model_SetExtendRate(Live2D_ModelHandle,1.8f, 1.8f);		//Live2Dモデルの大きさを設定
	Live2D_Model_SetExtendRate(Live2D_ModelHandle2, 2.0f, 2.0f);	//Live2Dモデルの大きさを設定
	ContentsFont = CreateFontToHandle("Cherry bomb", 30, 2, DX_CHARSET_DEFAULT);	//ContentsFontフォントデータ(メッセージ用)を作成
	MenuFont = CreateFontToHandle("Cherry bomb", 45, 2, DX_CHARSET_DEFAULT);	//ContentsFontフォントデータ(メッセージ用)を作成

	// ゲームループ
	while (ProcessMessage() == 0 && GameState != END && !(KeyFlg & PAD_INPUT_START))
	{


		// 入力キー取得
		OldKey = NowKey;
		NowKey = GetMouseInput();
		KeyFlg = NowKey & ~OldKey;

		OldKey2 = NowKey2;
		NowKey2 = GetJoypadInputState(DX_INPUT_KEY_PAD1);
		KeyFlg2 = NowKey2 & ~OldKey2;

		//マウスの位置を取得
		GetMousePoint(&MouseX, &MouseY);


		if (KeyFlg2 && CheckHitKey(KEY_INPUT_Q) == 1) 			//Qキーでぽーずメニューへ
		{
			if (Pause == false) {
				Pause = true;
			}
			else { Pause = false; }
		}

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
		case GAME_MAIN:
			GameMain();
			break;
		case GAME_CLEAR:
			GameClear();
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

int LoadImages()
{
	//タイトル
	if ((TitleImage = LoadGraph("images/Title.png")) == -1)   return -1;
	//ステージ
	if ((StageImage = LoadGraph("images/Stage.jpg")) == -1)   return -1;
	if ((GameClearImage = LoadGraph("images/GameClear.png")) == -1)   return -1;
	if ((Live2DStage = LoadGraph("images/Live2DStage.png")) == -1)   return -1;

	if ((Flame = LoadGraph("images/Flame.png")) == -1)   return -1;
	if ((Button = LoadGraph("images/Button.png")) == -1)   return -1;

	//ブロック画像
	if (LoadDivGraph("images/Koma.png", 10, 5, 2, 80, 80, KomaImage) == -1)   return -1;
}

int LoadSounds(void) {

	if ((KomaClick = LoadSoundMem("sounds/KomaClick.mp3")) == -1)return -1;
	if ((KomaNaru = LoadSoundMem("sounds/KomaNaru.mp3")) == -1)return -1;
	if ((StartClick = LoadSoundMem("sounds/StartClick.mp3")) == -1)return -1;
	if ((TitleBGM = LoadSoundMem("sounds/BGM/TitleBGM.mp3")) == -1)return -1;
	if ((TitleBGM01 = LoadSoundMem("sounds/BGM/TitleBGM01.mp3")) == -1)return -1;
	if ((GameClearBGM = LoadSoundMem("sounds/BGM/GameClearBGM.mp3")) == -1)return -1;


	ChangeVolumeSoundMem(200, TitleBGM);
	ChangeVolumeSoundMem(200, TitleBGM01);
	ChangeVolumeSoundMem(200, GameClearBGM);
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
			Komas[i].pflg = 1;
			break;
		case 1:
			Komas[i].x = 320;
			Komas[i].y = 560;
			Komas[i].pflg = 1;
			break;
		case 2:
			Komas[i].x = 680;
			Komas[i].y = 560;
			Komas[i].pflg = 1;
			break;
		case 3:
			Komas[i].x = 500;
			Komas[i].y = 420;
			Komas[i].pflg = 1;
			break;
		case 5:
			Komas[i].x = 500;
			Komas[i].y = 140;
			Komas[i].pflg = 2;
			break;
		case 6:
			Komas[i].x = 680;
			Komas[i].y = 140;
			Komas[i].pflg = 2;
			break;
		case 7:
			Komas[i].x = 320;
			Komas[i].y = 140;
			Komas[i].pflg = 2;
			break;
		case 8:
			Komas[i].x = 500;
			Komas[i].y = 280;
			Komas[i].pflg = 2;
			break;
		}
		Komas[i].images = KomaImage[i];
		Komas[i].flg = 1;
	}

	StageInit();		//ステージの初期化

	Handrand = GetRand(1);		//先手・後手決定
	mscount = 0;		//メッセージ表示カウントリセット
	msdis = false;		//メッセージ非表示にリセット
	Pause = false;		//ポーズ状態リセット

	HandCount = 0,HandCount2 = 0;	//てめカウント初期化
	ClickFlag = 0;	//クリックフラグ(駒の識別)
	Cflag = 0;	//駒クリックフラグ
	Mflag = 0;	//マーク表示フラグ
	Status = 0;	//ステージの状況
	Pflag = Handrand + 1;	//プレイヤーフラグ
	Branch = 0;	//1Pと2Pの駒の切り替え用変数
	Abranch = 0;
	//ゲームメイン処理へ
	GameState = GAME_MAIN;
}

void DrawGameTitle(void)
{

	//タイトル画像表示
	DrawGraph(0, 0, TitleImage, FALSE);
	if (CheckSoundMem(TitleBGM01) == 1) StopSoundMem(TitleBGM01);
	if (CheckSoundMem(TitleBGM) == 0) PlaySoundMem(TitleBGM, DX_PLAYTYPE_BACK);


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
	if (MouseX > 160 && MouseX < 460 && MouseY>405 && MouseY < 465) {// ホバー時
		DrawString(170, 410, "す た ぁ と", 0xffa500);
	}
}

void StageInit(void)
{
	Stage[0][0] = 8;	Stage[0][1] = 6;	Stage[0][2] = 7;
	Stage[1][0] = 0;	Stage[1][1] = 9;	Stage[1][2] = 0;
	Stage[2][0] = 0;	Stage[2][1] = 4;	Stage[2][2] = 0;
	Stage[3][0] = 2;	Stage[3][1] = 1;	Stage[3][2] = 3;
}


void GameMain(void)
{

	//ステージ画像表示
	DrawGraph(200, 0, StageImage, FALSE);

	SideBar();

	if(CheckSoundMem(TitleBGM01) == 0) PlaySoundMem(TitleBGM01, DX_PLAYTYPE_BACK);


	for (int i = 0; i < KomaKinds; i++) {
		if (i == 4 || i == 9) {
			continue;			//ニワトリは描画しない
		}
		if (Komas[i].flg != 0) {
			DrawRotaGraph(Komas[i].x, Komas[i].y, 1.8, 0, Komas[i].images, TRUE, FALSE);
		}
	}

	//デバッグ用
	/*for (int i = 0; i < Sizey; i++)
	{
		for (int j = 0; j < Sizex; j++)
		{
			DrawFormatString(j*100, i*150, 0xffffff, "%4d", Stage[i][j]);
		}
	}
	DrawFormatString(800, 50, 0x000000, "%3d", Pflag);
	DrawFormatString(800, 100, 0x000000, "%3d", Cflag);*/

	if (Pause == false) {
		switch (Status) {
		case 0:
			SelectKomas();		//駒選択
			break;
		case 1:
			if (Komas[LION + Abranch].flg == 0) {
				GameClear();
			}
			else
			{
				ChangeTurn();		//ターンチェンジ
			}

			break;
		}
	}

	//DrawFormatStringToHandle(270, 25, 0x000000, MenuFont, "x:%d  y:%d", MouseX, MouseY);	//デバック用 座標確認

	GamePause();	// ポーズ画面呼び出し用

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
	DeleteFontToHandle(MenuFont);	//MenuFontデータを削除

}

void GamePause(void) {

	if (Pause == true) {
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 200);
		DrawBox(0, 0, 1000, 700, 0x000000, TRUE);
		DrawFormatString(450, 185, 0xfffffff, "ぽーず");

		// タイトルボタン
		DrawRotaGraph(525, 400, 0.8f, 0, Button, TRUE, FALSE);
		DrawFormatStringToHandle(450, 380, 0x000000, MenuFont, "たいとる");

		DrawRotaGraph(525, 500, 0.8f, 0, Button, TRUE, FALSE);
		DrawFormatStringToHandle(470, 480, 0x000000, MenuFont, "おわる");


		// マウス左クリック判定
		if (KeyFlg & MOUSE_INPUT_LEFT) {
			if (MouseX < 610 && MouseX > 445 && MouseY > 370 && MouseY < 430) {	//タイトル画面ボタン
				PlaySoundMem(StartClick, DX_PLAYTYPE_BACK);
				StopSoundMem(TitleBGM01);
				GameState = GAME_TITLE;
			}

			if (MouseX < 610 && MouseX > 445 && MouseY > 470 && MouseY < 530) {	//おわる画面ボタン
				PlaySoundMem(StartClick, DX_PLAYTYPE_BACK);
				GameState = END;
			}
		}
		if (MouseX < 610 && MouseX > 445 && MouseY > 370 && MouseY < 430) {//タイトル画面ボタン(ホバー時)
			DrawFormatStringToHandle(450, 380, 0xffd700, MenuFont, "たいとる");
		}
		if (MouseX < 610 && MouseX > 445 && MouseY > 470 && MouseY < 530) {	//おわる画面ボタン(ホバー時)
			DrawFormatStringToHandle(470, 480, 0xffd700, MenuFont, "おわる");
		}

		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}
}


void SideBar(void) {

	DrawGraph(0, 0, Live2DStage, FALSE);		//Live2D用背景
	DrawGraph(800, 0, Live2DStage, FALSE);		//Live2D用背景

	Live2D_Model_SetTranslate(Live2D_ModelHandle, Pflag == 1 ? 400 : -380, -50);		//Live2Dモデルの座標を設定
	Live2D_Model_SetTranslate(Live2D_ModelHandle2, Pflag == 1 ? -380 : 400, 0);		//Live2Dモデルの座標を設定

	Live2D_Model_Draw(Live2D_ModelHandle);		//Live2Dモデル描画
	Live2D_Model_Draw(Live2D_ModelHandle2);		//Live2Dモデル描画
	//Live2D_Model_StartMotion(Live2D_ModelHandle, "FlickDown", 0);


	// ステータス・メニュー描画
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 235);
	DrawRotaGraph(110, 520, 2.0f, 0, Flame, TRUE, FALSE);
	DrawRotaGraph(890, 520, 2.0f, 0, Flame, TRUE, FALSE);


	DrawFormatStringToHandle(80, 380, Pflag == 1 ? 0xffff00:0xffd700, MenuFont, Pflag == 1 ? "1P" : "2P");
	DrawFormatStringToHandle(860, 380, Pflag == 1 ? 0xffd700:0xffff00, MenuFont, Pflag == 1 ? "2P" : "1P");

	DrawFormatStringToHandle(50, 450, Pflag == 1 ? 0xff7f50:0xff571a, MenuFont, "%dてめ", Pflag == 1 ? HandCount : HandCount2);
	DrawFormatStringToHandle(835, 450, Pflag == 1 ? 0xff571a:0xff7f50, MenuFont, "%dてめ", Pflag == 1 ? HandCount2 : HandCount);

	// タイトルボタン
	DrawRotaGraph(110, 630, 0.8f, 0, Button, TRUE, FALSE);
	DrawFormatStringToHandle(33, 610, 0x000000, MenuFont, "たいとる");

	DrawRotaGraph(890, 630, 0.8f, 0, Button, TRUE, FALSE);
	DrawFormatStringToHandle(830, 610, 0x000000, MenuFont, "おわる");

	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	// マウス左クリック判定
	if (Pause == false) {
		if (KeyFlg & MOUSE_INPUT_LEFT) {
			if (MouseX < 1000 && MouseX > 800 && MouseY > 20 && MouseY < 700) {
				if(Pflag == 1){
					Live2D_Model_StartMotion(Live2D_ModelHandle, "Tap2", GetRand(5));
				}
				else {
					Live2D_Model_StartMotion(Live2D_ModelHandle2, "Tap", GetRand(5));
				}
				
				PlaySoundMem(KomaNaru, DX_PLAYTYPE_BACK);
			}

			if (MouseX < 220 && MouseX > 0 && MouseY > 20 && MouseY < 700) {
				if (Pflag != 1) {
					Live2D_Model_StartMotion(Live2D_ModelHandle, "Tap2", GetRand(5));
				}
				else {
					Live2D_Model_StartMotion(Live2D_ModelHandle2, "Tap", GetRand(5));
				}

				PlaySoundMem(KomaNaru, DX_PLAYTYPE_BACK);
			}

			if (MouseX < 190 && MouseX > 30 && MouseY > 600 && MouseY < 655) {	//タイトル画面ボタン
				PlaySoundMem(StartClick, DX_PLAYTYPE_BACK);
				StopSoundMem(TitleBGM01);
				GameState = GAME_TITLE;
			}
			if (MouseX < 970 && MouseX > 810 && MouseY > 600 && MouseY < 655) {	//おわる画面ボタン
				PlaySoundMem(StartClick, DX_PLAYTYPE_BACK);
				GameState = END;
			}
		}
		if (MouseX < 190 && MouseX > 30 && MouseY > 600 && MouseY < 655) {	//タイトル画面ボタン(ホバー時)
			DrawFormatStringToHandle(33, 610, 0xffa500, MenuFont, "たいとる");
		}
		if (MouseX < 970 && MouseX > 810 && MouseY > 600 && MouseY < 655) {	//おわる画面ボタン(ホバー時)
			DrawFormatStringToHandle(830, 610, 0xffd700, MenuFont, "おわる");
		}
	}

	//ISendMessageテスト表示	
	char Hand[2][7] = { "せんて","ごて" };
	
	ISendMessege(Hand[Handrand]);
	ISendMessege(Hand[1 - Handrand], 1);
	//ISendMessege("てすとです。");
	//ISendMessege("しょうご", 1);
}

void ISendMessege(const TCHAR* Contents, int partner) {
	msdis = true;
	if (msdis == true && mscount++ < 1080) {
		int DrawWidth = GetDrawStringWidth(Contents, strlen2Dx(Contents));

		switch (partner) {

		case 0:
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 200);
			DrawOval(105, 265, 100, 80, 0x000000, FALSE);
			DrawOval(105, 265, 99, 79, 0xf0f8ff, TRUE);
			DrawFormatStringToHandle(((10 + ((200 - 10) / 2)) - (DrawWidth / 2)), 260, 0x000000, ContentsFont, Contents);
			//DrawFormatStringToHandle(60, 260, 0x000000, ContentsFont, Contents);
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
			break;

		case 1:
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 200);
			DrawOval(895, 265, 100, 80, 0x000000, FALSE);
			DrawOval(895, 265, 99, 79, 0xf0f8ff, TRUE);
			//DrawFormatStringToHandle(850, 260, 0x000000, ContentsFont, Contents);
			DrawFormatStringToHandle(((800 + ((990 - 800) / 2)) - (DrawWidth / 2)), 260, 0x000000, ContentsFont, Contents);
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
			break;
		}
	}
	if (mscount == 1080) { msdis = false; /*mscount = 0;*/ }
}

void SelectKomas(void)
{
	if (Pflag == 1) {		//1Pのとき
		Branch = 0;
		Abranch = 5;
	}
	else if (Pflag == 2) {	//2Pのとき
		Branch = 5;
		Abranch = 0;
	}

	if (KeyFlg & MOUSE_INPUT_LEFT) {
		if (Komas[CHICK + Branch].flg != 0 && MouseX > Komas[CHICK + Branch].x - HXMARGIN && MouseX<Komas[CHICK + Branch].x + HXMARGIN && MouseY>Komas[CHICK + Branch].y - HYMARGIN && MouseY < Komas[CHICK + Branch].y + HYMARGIN) {

			ClickFlag = 1;
		}
		else if (Komas[GIRAF + Branch].flg != 0 && MouseX > Komas[GIRAF + Branch].x - HXMARGIN && MouseX<Komas[GIRAF + Branch].x + HXMARGIN && MouseY>Komas[GIRAF + Branch].y - HYMARGIN && MouseY < Komas[GIRAF + Branch].y + HYMARGIN) {

			ClickFlag = 2;
		}
		else if (Komas[ELEPHA + Branch].flg != 0 && MouseX > Komas[ELEPHA + Branch].x - HXMARGIN && MouseX<Komas[ELEPHA + Branch].x + HXMARGIN && MouseY>Komas[ELEPHA + Branch].y - HYMARGIN && MouseY < Komas[ELEPHA + Branch].y + HYMARGIN) {

			ClickFlag = 3;
		}
		else if (Komas[LION + Branch].flg != 0 && MouseX > Komas[LION + Branch].x - HXMARGIN && MouseX<Komas[LION + Branch].x + HXMARGIN && MouseY>Komas[LION + Branch].y - HYMARGIN && MouseY < Komas[LION + Branch].y + HYMARGIN) {

			ClickFlag = 4;
		}
		if (Cflag == 1) {
			Cflag = 0;
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
	int Sign = 0;		//符号用変数

	if (Pflag == 1) {			//1Pの場合
		Sign = 1;
	}
	else if (Pflag == 2) {		//2Pの場合
		Sign = -1;
	}

	//他の駒がなければ移動可能マークを描画
	//↑
	if ((Komas[CHICK + Branch].y > 140 && Komas[CHICK + Branch].y < 560) && Cflag == 0 && Komas[CHICK + Branch].flg != 0
		&& (Stage[(Komas[CHICK + Branch].y - 140) / YMARGIN - Sign][(Komas[CHICK + Branch].x - 320) / XMARGIN] == 0
			|| Komas[Stage[(Komas[CHICK + Branch].y - 140) / YMARGIN - Sign][(Komas[CHICK + Branch].x - 320) / XMARGIN] - 1].pflg != Pflag))
	{
		SelectDisplay(Komas[CHICK + Branch].x, Komas[CHICK + Branch].y - YMARGIN * Sign);
		Mflag = 1;
	}


	//移動可能マークをクリックしたとき移動
	//↑
	if (KeyFlg & KEY_INPUT_LEFT && Mflag == 1) {
		if (MouseX > Komas[CHICK + Branch].x - HXMARGIN && MouseX < Komas[CHICK + Branch].x + HXMARGIN) {
			if (Pflag == 1) {			//1Pのとき
				if (Komas[CHICK + Branch].y > 140 && MouseY > Komas[CHICK + Branch].y - (YMARGIN + HYMARGIN) && MouseY < Komas[CHICK + Branch].y - HYMARGIN) {
					if (Stage[(Komas[CHICK + Branch].y - 280) / YMARGIN][(Komas[CHICK + Branch].x - 320) / XMARGIN] != 0
						&& Komas[Stage[(Komas[CHICK + Branch].y - 280) / YMARGIN][(Komas[CHICK + Branch].x - 320) / XMARGIN] - 1].pflg != Pflag)
					{
						Komas[Stage[(Komas[CHICK + Branch].y - 280) / YMARGIN][(Komas[CHICK + Branch].x - 320) / XMARGIN] - 1].flg = 0;
					}
					Stage[Komas[CHICK + Branch].y / YMARGIN - 1][(Komas[CHICK + Branch].x - 320) / XMARGIN] = 0;
					Komas[CHICK + Branch].y -= YMARGIN;
					Stage[Komas[CHICK + Branch].y / YMARGIN - 1][(Komas[CHICK + Branch].x - 320) / XMARGIN] = 4 + Branch;

					Status = 1;		//ターンチェンジ関数に移動
					PlaySoundMem(KomaClick, DX_PLAYTYPE_BACK);
				}
			}
			else if (Pflag == 2) {		//2Pのとき
				if (Komas[CHICK + Branch].y < 560 && MouseY > Komas[CHICK + Branch].y + HYMARGIN && MouseY < Komas[CHICK + Branch].y + (YMARGIN + HYMARGIN)) {
					if (Stage[(Komas[CHICK + Branch].y - 280) / YMARGIN + 2][(Komas[CHICK + Branch].x - 320) / XMARGIN] != 0
						&& Komas[Stage[(Komas[CHICK + Branch].y - 280) / YMARGIN + 2][(Komas[CHICK + Branch].x - 320) / XMARGIN] - 1].pflg != Pflag)
					{
						Komas[Stage[(Komas[CHICK + Branch].y - 280) / YMARGIN + 2][(Komas[CHICK + Branch].x - 320) / XMARGIN] - 1].flg = 0;
					}
					Stage[Komas[CHICK + Branch].y / YMARGIN - 1][(Komas[CHICK + Branch].x - 320) / XMARGIN] = 0;
					Komas[CHICK + Branch].y += YMARGIN;
					Stage[Komas[CHICK + Branch].y / YMARGIN - 1][(Komas[CHICK + Branch].x - 320) / XMARGIN] = 4 + Branch;

					Status = 1;		//ターンチェンジ関数に移動
					PlaySoundMem(KomaClick, DX_PLAYTYPE_BACK);
				}
			}

		}

	}
}

void MoveGiraf(void)
{
	//他の駒がなければ移動可能マークを描画
		//↑
	if (Komas[GIRAF + Branch].y > 140 && Cflag == 0 && Komas[GIRAF + Branch].flg != 0
		&& (Stage[(Komas[GIRAF + Branch].y - 280) / YMARGIN][(Komas[GIRAF + Branch].x - 320) / XMARGIN] == 0
		|| Komas[Stage[(Komas[GIRAF + Branch].y - 280) / YMARGIN][(Komas[GIRAF + Branch].x - 320) / XMARGIN] - 1].pflg != Pflag)) 
	{
		SelectDisplay(Komas[GIRAF + Branch].x, Komas[GIRAF + Branch].y - YMARGIN);
		if (Mflag == 0) {
			Mflag = 1;
		}
	}	//→
	if (Komas[GIRAF + Branch].x < 680 && Cflag == 0 && Komas[GIRAF + Branch].flg != 0
		&& (Stage[Komas[GIRAF + Branch].y / YMARGIN - 1][(Komas[GIRAF + Branch].x - 140) / XMARGIN] == 0
		|| Komas[Stage[Komas[GIRAF + Branch].y / YMARGIN - 1][(Komas[GIRAF + Branch].x - 140) / XMARGIN] - 1].pflg != Pflag)) 
	{
		SelectDisplay(Komas[GIRAF + Branch].x + XMARGIN, Komas[GIRAF + Branch].y);
		if (Mflag == 0) {
			Mflag = 1;
		}
	}	//←
	if (Komas[GIRAF + Branch].x > 320 && Cflag == 0 && Komas[GIRAF + Branch].flg != 0
		&& (Stage[Komas[GIRAF + Branch].y / YMARGIN - 1][(Komas[GIRAF + Branch].x - 140) / XMARGIN - 2] == 0
		|| Komas[Stage[Komas[GIRAF + Branch].y / YMARGIN - 1][(Komas[GIRAF + Branch].x - 140) / XMARGIN - 2] - 1].pflg != Pflag)) 
	{
			SelectDisplay(Komas[GIRAF + Branch].x - XMARGIN, Komas[GIRAF + Branch].y);
			if (Mflag == 0) {
				Mflag = 1;
			}
	}	//↓
	if (Komas[GIRAF + Branch].y < 560 && Cflag == 0 && Komas[GIRAF + Branch].flg != 0
		&& (Stage[(Komas[GIRAF + Branch].y - 280) / YMARGIN + 2][(Komas[GIRAF + Branch].x - 320) / XMARGIN] == 0
		|| Komas[Stage[(Komas[GIRAF + Branch].y - 280) / YMARGIN + 2][(Komas[GIRAF + Branch].x - 320) / XMARGIN] - 1].pflg != Pflag)) 
	{
			SelectDisplay(Komas[GIRAF + Branch].x, Komas[GIRAF + Branch].y + YMARGIN);
			if (Mflag == 0) {
				Mflag = 1;
			}
	}
	

	//移動可能マークをクリックしたとき移動
	if (KeyFlg & KEY_INPUT_LEFT && Mflag == 1) {
			//上下
		if (MouseX > Komas[GIRAF + Branch].x - HXMARGIN && MouseX < Komas[GIRAF + Branch].x + HXMARGIN) {
			//↑
			if (Komas[GIRAF + Branch].y > 140 && MouseY > Komas[GIRAF + Branch].y - (YMARGIN + HYMARGIN) && MouseY < Komas[GIRAF + Branch].y - HYMARGIN) {
				if (Stage[(Komas[GIRAF + Branch].y - 280) / YMARGIN][(Komas[GIRAF + Branch].x - 320) / XMARGIN] != 0
					&& Komas[Stage[(Komas[GIRAF + Branch].y - 280) / YMARGIN][(Komas[GIRAF + Branch].x - 320) / XMARGIN] - 1].pflg != Pflag) 
				{
					Komas[Stage[(Komas[GIRAF + Branch].y - 280) / YMARGIN][(Komas[GIRAF + Branch].x - 320) / XMARGIN] - 1].flg = 0;
				}
				Stage[Komas[GIRAF + Branch].y / YMARGIN - 1][(Komas[GIRAF + Branch].x - 320) / XMARGIN] = 0;
				Komas[GIRAF + Branch].y -= YMARGIN;
				Stage[Komas[GIRAF + Branch].y / YMARGIN - 1][(Komas[GIRAF + Branch].x - 320) / XMARGIN] = 2 + Branch;
				
				Status = 1;		//ターンチェンジ関数に移動
				PlaySoundMem(KomaClick, DX_PLAYTYPE_BACK);
			}//↓
			else if (Komas[GIRAF + Branch].y < 560 && MouseY > Komas[GIRAF + Branch].y + HYMARGIN && MouseY < Komas[GIRAF + Branch].y + (YMARGIN + HYMARGIN)) {
				if (Stage[(Komas[GIRAF + Branch].y - 280) / YMARGIN + 2][(Komas[GIRAF + Branch].x - 320) / XMARGIN] != 0
					&& Komas[Stage[(Komas[GIRAF + Branch].y - 280) / YMARGIN + 2][(Komas[GIRAF + Branch].x - 320) / XMARGIN] - 1].pflg != Pflag) {
					Komas[Stage[(Komas[GIRAF + Branch].y - 280) / YMARGIN + 2][(Komas[GIRAF + Branch].x - 320) / XMARGIN] - 1].flg = 0;
				}
				Stage[Komas[GIRAF + Branch].y / YMARGIN - 1][(Komas[GIRAF + Branch].x - 320) / XMARGIN] = 0;
				Komas[GIRAF + Branch].y += YMARGIN;
				Stage[Komas[GIRAF + Branch].y / YMARGIN - 1][(Komas[GIRAF + Branch].x - 320) / XMARGIN] = 2 + Branch;
				
				Status = 1;		//ターンチェンジ関数に移動
				PlaySoundMem(KomaClick, DX_PLAYTYPE_BACK);
			}
		}	//左右
		else if (MouseY > Komas[GIRAF + Branch].y - HYMARGIN && MouseY < Komas[GIRAF + Branch].y + HYMARGIN) {
			//←
			if (Komas[GIRAF + Branch].x > 320 && MouseX > Komas[GIRAF + Branch].x - (XMARGIN + HXMARGIN) && MouseX < Komas[GIRAF + Branch].x - HXMARGIN) {
				if (Stage[Komas[GIRAF + Branch].y / YMARGIN - 1][(Komas[GIRAF + Branch].x - 140) / XMARGIN - 2] != 0
					&& Komas[Stage[Komas[GIRAF + Branch].y / YMARGIN - 1][(Komas[GIRAF + Branch].x - 140) / XMARGIN - 2] - 1].pflg != Pflag) 
				{
					Komas[Stage[Komas[GIRAF + Branch].y / YMARGIN - 1][(Komas[GIRAF + Branch].x - 140) / XMARGIN - 2] - 1].flg = 0;
				}
				Stage[Komas[GIRAF + Branch].y / YMARGIN - 1][(Komas[GIRAF + Branch].x - 320) / XMARGIN] = 0;
				Komas[GIRAF + Branch].x -= XMARGIN;
				Stage[Komas[GIRAF + Branch].y / YMARGIN - 1][(Komas[GIRAF + Branch].x - 320) / XMARGIN] = 2 + Branch;
				
				Status = 1;		//ターンチェンジ関数に移動
				PlaySoundMem(KomaClick, DX_PLAYTYPE_BACK);
			}//→
			else if (Komas[GIRAF + Branch].x < 680 && MouseX > Komas[GIRAF + Branch].x + HXMARGIN && MouseX < Komas[GIRAF + Branch].x + (XMARGIN + HXMARGIN)) {
				if (Stage[Komas[GIRAF + Branch].y / YMARGIN - 1][(Komas[GIRAF + Branch].x - 140) / XMARGIN] != 0
					&& Komas[Stage[Komas[GIRAF + Branch].y / YMARGIN - 1][(Komas[GIRAF + Branch].x - 140) / XMARGIN] - 1].pflg != Pflag) 
				{
					Komas[Stage[Komas[GIRAF + Branch].y / YMARGIN - 1][(Komas[GIRAF + Branch].x - 140) / XMARGIN] - 1].flg = 0;
				}
				Stage[Komas[GIRAF + Branch].y / YMARGIN - 1][(Komas[GIRAF + Branch].x - 320) / XMARGIN] = 0;
				Komas[GIRAF + Branch].x += XMARGIN;
				Stage[Komas[GIRAF + Branch].y / YMARGIN - 1][(Komas[GIRAF + Branch].x - 320) / XMARGIN] = 2 + Branch;
				
				Status = 1;		//ターンチェンジ関数に移動
				PlaySoundMem(KomaClick, DX_PLAYTYPE_BACK);
			}
		}
	}
	
}

//角行の移動処理
void MoveElepha(void)
{
	//他の駒がなければ移動可能マークを描画
	if (Komas[ELEPHA + Branch].y > 140) {
			//左上
		if (Komas[ELEPHA + Branch].x > 320 && Cflag == 0 && Komas[ELEPHA + Branch].flg != 0
			&& (Stage[(Komas[ELEPHA + Branch].y - 280) / YMARGIN][(Komas[ELEPHA + Branch].x - 140) / XMARGIN - 2] == 0
			|| Komas[Stage[(Komas[ELEPHA + Branch].y - 280) / YMARGIN][(Komas[ELEPHA + Branch].x - 140) / XMARGIN - 2] - 1].pflg != Pflag)) 
		{
			SelectDisplay(Komas[ELEPHA + Branch].x - XMARGIN, Komas[ELEPHA + Branch].y - YMARGIN);
			if (Mflag == 0) {
				Mflag = 1;
			}
		}	//右上
		if (Komas[ELEPHA + Branch].x < 680 && Cflag == 0 && Komas[ELEPHA + Branch].flg != 0
			&& (Stage[(Komas[ELEPHA + Branch].y - 280) / YMARGIN][(Komas[ELEPHA + Branch].x - 140) / XMARGIN] == 0
			|| Komas[Stage[(Komas[ELEPHA + Branch].y - 280) / YMARGIN][(Komas[ELEPHA + Branch].x - 140) / XMARGIN] - 1].pflg != Pflag)) 
		{
			SelectDisplay(Komas[ELEPHA + Branch].x + XMARGIN, Komas[ELEPHA + Branch].y - YMARGIN);
			if (Mflag == 0) {
				Mflag = 1;
			}
		}
	}
	if (Komas[ELEPHA + Branch].y < 560) {
			//左下
		if (Komas[ELEPHA + Branch].x > 320 && Cflag == 0 && Komas[ELEPHA + Branch].flg != 0
			&& (Stage[(Komas[ELEPHA + Branch].y - 280) / YMARGIN + 2][(Komas[GIRAF + Branch].x - 140) / XMARGIN - 2] == 0
			|| Komas[Stage[(Komas[ELEPHA + Branch].y - 280) / YMARGIN + 2][(Komas[GIRAF + Branch].x - 140) / XMARGIN - 2] - 1].pflg != Pflag)) 
		{
			SelectDisplay(Komas[ELEPHA + Branch].x - XMARGIN, Komas[ELEPHA + Branch].y + YMARGIN);
			if (Mflag == 0) {
				Mflag = 1;
			}
		}	//右下
		if (Komas[ELEPHA + Branch].x < 680 && Cflag == 0 && Komas[ELEPHA + Branch].flg != 0
			&& (Stage[(Komas[ELEPHA + Branch].y - 280) / YMARGIN + 2][(Komas[ELEPHA + Branch].x - 140) / XMARGIN] == 0
			|| Komas[Stage[(Komas[ELEPHA + Branch].y - 280) / YMARGIN + 2][(Komas[ELEPHA + Branch].x - 140) / XMARGIN] - 1].pflg != Pflag)) 
		{
			SelectDisplay(Komas[ELEPHA + Branch].x + XMARGIN, Komas[ELEPHA + Branch].y + YMARGIN);
			if (Mflag == 0) {
				Mflag = 1;
			}
		}
	}

	//移動可能マークをクリックしたと移動
	if (KeyFlg & KEY_INPUT_LEFT && Mflag == 1) {
		if (Komas[ELEPHA + Branch].y > 140 && MouseY < Komas[ELEPHA + Branch].y - HYMARGIN && MouseY > Komas[ELEPHA + Branch].y - (YMARGIN + HYMARGIN)) {
				//左上
			if (Komas[ELEPHA + Branch].x > 320 && MouseX > Komas[ELEPHA + Branch].x - (XMARGIN + HXMARGIN) && MouseX < Komas[ELEPHA + Branch].x - HXMARGIN) {
				if (Stage[(Komas[ELEPHA + Branch].y - 280) / YMARGIN][(Komas[ELEPHA + Branch].x - 140) / XMARGIN - 2] != 0
					&& Komas[Stage[(Komas[ELEPHA + Branch].y - 280) / YMARGIN][(Komas[ELEPHA + Branch].x - 140) / XMARGIN - 2] - 1].pflg != Pflag) 
				{
					Komas[Stage[(Komas[ELEPHA + Branch].y - 280) / YMARGIN][(Komas[ELEPHA + Branch].x - 140) / XMARGIN - 2] - 1].flg = 0;
				}
				Stage[Komas[ELEPHA + Branch].y / YMARGIN - 1][(Komas[ELEPHA + Branch].x - 320) / XMARGIN] = 0;
				Komas[ELEPHA + Branch].y -= YMARGIN;
				Komas[ELEPHA + Branch].x -= XMARGIN;
				Stage[Komas[ELEPHA + Branch].y / YMARGIN - 1][(Komas[ELEPHA + Branch].x - 320) / XMARGIN] = 3 + Branch;
				Status = 1;		//ターンチェンジ関数に移動
				PlaySoundMem(KomaClick, DX_PLAYTYPE_BACK);
			}	//右上
			else if (Komas[ELEPHA + Branch].x < 680 && MouseX > Komas[ELEPHA + Branch].x + HXMARGIN && MouseX < Komas[ELEPHA + Branch].x + (XMARGIN + HXMARGIN)) {
				if (Stage[(Komas[ELEPHA + Branch].y - 280) / YMARGIN][(Komas[ELEPHA + Branch].x - 140) / XMARGIN] != 0
					&& Komas[Stage[(Komas[ELEPHA + Branch].y - 280) / YMARGIN][(Komas[ELEPHA + Branch].x - 140) / XMARGIN] - 1].pflg != Pflag) 
				{
					Komas[Stage[(Komas[ELEPHA + Branch].y - 280) / YMARGIN][(Komas[ELEPHA + Branch].x - 140) / XMARGIN] - 1].flg = 0;
				}
				Stage[Komas[ELEPHA + Branch].y / YMARGIN - 1][(Komas[ELEPHA + Branch].x - 320) / XMARGIN] = 0;
				Komas[ELEPHA + Branch].y -= YMARGIN;
				Komas[ELEPHA + Branch].x += XMARGIN;
				Stage[Komas[ELEPHA + Branch].y / YMARGIN - 1][(Komas[ELEPHA + Branch].x - 320) / XMARGIN] = 3 + Branch;
				Status = 1;		//ターンチェンジ関数に移動
				PlaySoundMem(KomaClick, DX_PLAYTYPE_BACK);
			}
		}
		else if (Komas[ELEPHA + Branch].y < 560 && MouseY > Komas[ELEPHA + Branch].y + HYMARGIN && MouseY < Komas[ELEPHA + Branch].y + (YMARGIN + HYMARGIN)) {
				//左下
			if (Komas[ELEPHA + Branch].x > 320 && MouseX > Komas[ELEPHA + Branch].x - (XMARGIN + HXMARGIN) && MouseX < Komas[ELEPHA + Branch].x - HXMARGIN) {
				if (Stage[(Komas[ELEPHA + Branch].y - 280) / YMARGIN + 2][(Komas[GIRAF + Branch].x - 140) / XMARGIN - 2] != 0
					&& Komas[Stage[(Komas[ELEPHA + Branch].y - 280) / YMARGIN + 2][(Komas[GIRAF + Branch].x - 140) / XMARGIN - 2] - 1].pflg != Pflag) 
				{
					Komas[Stage[(Komas[ELEPHA + Branch].y - 280) / YMARGIN + 2][(Komas[GIRAF + Branch].x - 140) / XMARGIN - 2] - 1].flg = 0;
				}
				Stage[Komas[ELEPHA + Branch].y / YMARGIN - 1][(Komas[ELEPHA + Branch].x - 320) / XMARGIN] = 0;
				Komas[ELEPHA + Branch].y += YMARGIN;
				Komas[ELEPHA + Branch].x -= XMARGIN;
				Stage[Komas[ELEPHA + Branch].y / YMARGIN - 1][(Komas[ELEPHA + Branch].x - 320) / XMARGIN] = 3 + Branch;
				Status = 1;		//ターンチェンジ関数に移動
				PlaySoundMem(KomaClick, DX_PLAYTYPE_BACK);
			}	//右下
			else if (Komas[ELEPHA + Branch].x < 680 && MouseX > Komas[ELEPHA + Branch].x + HXMARGIN && MouseX < Komas[ELEPHA + Branch].x + (XMARGIN + HXMARGIN)) {
				if (Stage[(Komas[ELEPHA + Branch].y - 280) / YMARGIN + 2][(Komas[ELEPHA + Branch].x - 140) / XMARGIN] != 0
					&& Komas[Stage[(Komas[ELEPHA + Branch].y - 280) / YMARGIN + 2][(Komas[ELEPHA + Branch].x - 140) / XMARGIN] - 1].pflg != Pflag) 
				{
					Komas[Stage[(Komas[ELEPHA + Branch].y - 280) / YMARGIN + 2][(Komas[ELEPHA + Branch].x - 140) / XMARGIN] - 1].flg = 0;
				}
				Stage[Komas[ELEPHA + Branch].y / YMARGIN - 1][(Komas[ELEPHA + Branch].x - 320) / XMARGIN] = 0;
				Komas[ELEPHA + Branch].y += YMARGIN;
				Komas[ELEPHA + Branch].x += XMARGIN;
				Stage[Komas[ELEPHA + Branch].y / YMARGIN - 1][(Komas[ELEPHA + Branch].x - 320) / XMARGIN] = 3 + Branch;
				Status = 1;		//ターンチェンジ関数に移動
				PlaySoundMem(KomaClick, DX_PLAYTYPE_BACK);
			}
		}
	}
}

void MoveLion(void)
{
	//他の駒がなければ移動可能マークを描画
		//↑
	if (Komas[LION + Branch].y > 140 && Cflag == 0 && Komas[LION + Branch].flg != 0
		&& (Stage[(Komas[LION + Branch].y - 280) / YMARGIN][(Komas[LION + Branch].x - 320) / XMARGIN] == 0
			|| Komas[Stage[(Komas[LION + Branch].y - 280) / YMARGIN][(Komas[LION + Branch].x - 320) / XMARGIN] - 1].pflg != Pflag)) 
	{
		SelectDisplay(Komas[LION + Branch].x, Komas[LION + Branch].y - YMARGIN);
		if (Mflag == 0) {
			Mflag = 1;
		}
	}	//→
	if (Komas[LION + Branch].x < 680 && Cflag == 0 && Komas[LION + Branch].flg != 0
		&& (Stage[Komas[LION + Branch].y / YMARGIN - 1][(Komas[LION + Branch].x - 140) / XMARGIN] == 0
			|| Komas[Stage[Komas[LION + Branch].y / YMARGIN - 1][(Komas[LION + Branch].x - 140) / XMARGIN] - 1].pflg != Pflag)) 
	{
		SelectDisplay(Komas[LION + Branch].x + XMARGIN, Komas[LION + Branch].y);
		if (Mflag == 0) {
			Mflag = 1;
		}
	}	//←
	if (Komas[LION + Branch].x > 320 && Cflag == 0 && Komas[LION + Branch].flg != 0
		&& (Stage[Komas[LION + Branch].y / YMARGIN - 1][(Komas[LION + Branch].x - 140) / XMARGIN - 2] == 0
		|| Komas[Stage[Komas[LION + Branch].y / YMARGIN - 1][(Komas[LION + Branch].x - 140) / XMARGIN - 2] - 1].pflg != Pflag)) 
	{
		SelectDisplay(Komas[LION + Branch].x - XMARGIN, Komas[LION + Branch].y);
		if (Mflag == 0) {
			Mflag = 1;
		}
	}	//↓
	if (Komas[LION + Branch].y < 560 && Cflag == 0 && Komas[LION + Branch].flg != 0
		&& (Stage[(Komas[LION + Branch].y - 280) / YMARGIN + 2][(Komas[LION + Branch].x - 320) / XMARGIN] == 0
		|| Komas[Stage[(Komas[LION + Branch].y - 280) / YMARGIN + 2][(Komas[LION + Branch].x - 320) / XMARGIN] - 1].pflg != Pflag)) 
	{
		SelectDisplay(Komas[LION + Branch].x, Komas[LION + Branch].y + YMARGIN);
		if (Mflag == 0) {
			Mflag = 1;
		}
	}
	if (Komas[LION + Branch].y > 140) {
		//左上
		if (Komas[LION + Branch].x > 320 && Cflag == 0 && Komas[LION + Branch].flg != 0
			&& (Stage[(Komas[LION + Branch].y - 280) / YMARGIN][(Komas[LION + Branch].x - 140) / XMARGIN - 2] == 0
			|| Komas[Stage[(Komas[LION + Branch].y - 280) / YMARGIN][(Komas[LION + Branch].x - 140) / XMARGIN - 2] - 1].pflg != Pflag)) 
{
			SelectDisplay(Komas[LION + Branch].x - XMARGIN, Komas[LION + Branch].y - YMARGIN);
			if (Mflag == 0) {
				Mflag = 1;
			}
		}	//右上
		if (Komas[LION + Branch].x < 680 && Cflag == 0 && Komas[LION + Branch].flg != 0
			&& (Stage[(Komas[LION + Branch].y - 280) / YMARGIN][(Komas[LION + Branch].x - 140) / XMARGIN] == 0
			|| Komas[Stage[(Komas[LION + Branch].y - 280) / YMARGIN][(Komas[LION + Branch].x - 140) / XMARGIN] - 1].pflg != Pflag)) 
		{
			SelectDisplay(Komas[LION + Branch].x + XMARGIN, Komas[LION + Branch].y - YMARGIN);
			if (Mflag == 0) {
				Mflag = 1;
			}
		}
	}
	if (Komas[LION + Branch].y < 560) {
		//左下
		if (Komas[LION + Branch].x > 320 && Cflag == 0 && Komas[LION + Branch].flg != 0
			&& (Stage[(Komas[LION + Branch].y - 280) / YMARGIN + 2][(Komas[LION + Branch].x - 140) / XMARGIN - 2] == 0
			|| Komas[Stage[(Komas[LION + Branch].y - 280) / YMARGIN + 2][(Komas[LION + Branch].x - 140) / XMARGIN - 2] - 1].pflg != Pflag)) 
		{
			SelectDisplay(Komas[LION + Branch].x - XMARGIN, Komas[LION + Branch].y + YMARGIN);
			if (Mflag == 0) {
				Mflag = 1;
			}
		}	//右下
		if (Komas[LION + Branch].x < 680 && Cflag == 0 && Komas[LION + Branch].flg != 0
			&& (Stage[(Komas[LION + Branch].y - 280) / YMARGIN + 2][(Komas[LION + Branch].x - 140) / XMARGIN] == 0
			|| Komas[Stage[(Komas[LION + Branch].y - 280) / YMARGIN + 2][(Komas[LION + Branch].x - 140) / XMARGIN] - 1].pflg != Pflag)) 
		{
			SelectDisplay(Komas[LION + Branch].x + XMARGIN, Komas[LION + Branch].y + YMARGIN);
			if (Mflag == 0) {
				Mflag = 1;
			}
		}
	}

	//移動可能マークをクリックしたとき移動
	if (KeyFlg & KEY_INPUT_LEFT && Mflag == 1) {
		//上下
		if (MouseX > Komas[LION + Branch].x - HXMARGIN && MouseX < Komas[LION + Branch].x + HXMARGIN) {
			//↑
			if (Komas[LION + Branch].y > 140 && MouseY > Komas[LION + Branch].y - (YMARGIN + HYMARGIN) && MouseY < Komas[LION + Branch].y - HYMARGIN) {
				if (Stage[(Komas[LION + Branch].y - 280) / YMARGIN][(Komas[LION + Branch].x - 320) / XMARGIN] != 0
					&& Komas[Stage[(Komas[LION + Branch].y - 280) / YMARGIN][(Komas[LION + Branch].x - 320) / XMARGIN] - 1].pflg != Pflag) 
				{
					Komas[Stage[(Komas[LION + Branch].y - 280) / YMARGIN][(Komas[LION + Branch].x - 320) / XMARGIN] - 1].flg = 0;
				}
				Stage[Komas[LION + Branch].y / YMARGIN - 1][(Komas[LION + Branch].x - 320) / XMARGIN] = 0;
				Komas[LION + Branch].y -= YMARGIN;
				Stage[Komas[LION + Branch].y / YMARGIN - 1][(Komas[LION + Branch].x - 320) / XMARGIN] = 1 + Branch;
				Status = 1;		//ターンチェンジ関数に移動
				PlaySoundMem(KomaClick, DX_PLAYTYPE_BACK);
			}//↓
			else if (Komas[LION + Branch].y < 560 && MouseY > Komas[LION + Branch].y + HYMARGIN && MouseY < Komas[LION + Branch].y + (YMARGIN + HYMARGIN)) {
				if (Stage[(Komas[LION + Branch].y - 280) / YMARGIN + 2][(Komas[LION + Branch].x - 320) / XMARGIN] != 0
					&& Komas[Stage[(Komas[LION + Branch].y - 280) / YMARGIN + 2][(Komas[LION + Branch].x - 320) / XMARGIN] - 1].pflg != Pflag) 
				{
					Komas[Stage[(Komas[LION + Branch].y - 280) / YMARGIN + 2][(Komas[LION + Branch].x - 320) / XMARGIN] - 1].flg = 0;
				}
				Stage[Komas[LION + Branch].y / YMARGIN - 1][(Komas[LION + Branch].x - 320) / XMARGIN] = 0;
				Komas[LION + Branch].y += YMARGIN;
				Stage[Komas[LION + Branch].y / YMARGIN - 1][(Komas[LION + Branch].x - 320) / XMARGIN] = 1 + Branch;
				Status = 1;		//ターンチェンジ関数に移動
				PlaySoundMem(KomaClick, DX_PLAYTYPE_BACK);
			} 
		}	//左右
		else if (MouseY > Komas[LION + Branch].y - HYMARGIN && MouseY < Komas[LION + Branch].y + HYMARGIN) {
			//←
			if (Komas[LION + Branch].x > 320 && MouseX > Komas[LION + Branch].x - (XMARGIN + HXMARGIN) && MouseX < Komas[LION + Branch].x - HXMARGIN) {
				if (Stage[Komas[LION + Branch].y / YMARGIN - 1][(Komas[LION + Branch].x - 140) / XMARGIN - 2] != 0
					&& Komas[Stage[Komas[LION + Branch].y / YMARGIN - 1][(Komas[LION + Branch].x - 140) / XMARGIN - 2] - 1].pflg != Pflag) 
				{
					Komas[Stage[Komas[LION + Branch].y / YMARGIN - 1][(Komas[LION + Branch].x - 140) / XMARGIN - 2] - 1].flg = 0;
				}
				Stage[Komas[LION + Branch].y / YMARGIN - 1][(Komas[LION + Branch].x - 320) / XMARGIN] = 0;
				Komas[LION + Branch].x -= XMARGIN;
				Stage[Komas[LION + Branch].y / YMARGIN - 1][(Komas[LION + Branch].x - 320) / XMARGIN] = 1 + Branch;
				Status = 1;		//ターンチェンジ関数に移動
				PlaySoundMem(KomaClick, DX_PLAYTYPE_BACK);
			}//→
			else if (Komas[LION + Branch].x < 680 && MouseX > Komas[LION + Branch].x + HXMARGIN && MouseX < Komas[LION + Branch].x + (XMARGIN + HXMARGIN)) {
				if (Stage[Komas[LION + Branch].y / YMARGIN - 1][(Komas[LION + Branch].x - 140) / XMARGIN] != 0
					&& Komas[Stage[Komas[LION + Branch].y / YMARGIN - 1][(Komas[LION + Branch].x - 140) / XMARGIN] - 1].pflg != Pflag) 
				{
					Komas[Stage[Komas[LION + Branch].y / YMARGIN - 1][(Komas[LION + Branch].x - 140) / XMARGIN] - 1].flg = 0;
				}
				Stage[Komas[LION + Branch].y / YMARGIN - 1][(Komas[LION + Branch].x - 320) / XMARGIN] = 0;
				Komas[LION + Branch].x += XMARGIN;
				Stage[Komas[LION + Branch].y / YMARGIN - 1][(Komas[LION + Branch].x - 320) / XMARGIN] = 1 + Branch;
				Status = 1;		//ターンチェンジ関数に移動
				PlaySoundMem(KomaClick, DX_PLAYTYPE_BACK);
			}
		}

		if (Komas[LION + Branch].y > 140 && MouseY < Komas[LION + Branch].y - HYMARGIN && MouseY > Komas[LION + Branch].y - (YMARGIN + HYMARGIN)) {
			//左上
			if (Komas[LION + Branch].x > 320 && MouseX > Komas[LION + Branch].x - (XMARGIN + HXMARGIN) && MouseX < Komas[LION + Branch].x - HXMARGIN) {
				if (Stage[(Komas[LION + Branch].y - 280) / YMARGIN][(Komas[LION + Branch].x - 140) / XMARGIN - 2] != 0
					&& Komas[Stage[(Komas[LION + Branch].y - 280) / YMARGIN][(Komas[LION + Branch].x - 140) / XMARGIN - 2] - 1].pflg != Pflag) 
				{
					Komas[Stage[(Komas[LION + Branch].y - 280) / YMARGIN][(Komas[LION + Branch].x - 140) / XMARGIN - 2] - 1].flg = 0;
				}
				Stage[Komas[LION + Branch].y / YMARGIN - 1][(Komas[LION + Branch].x - 320) / XMARGIN] = 0;
				Komas[LION + Branch].y -= YMARGIN;
				Komas[LION + Branch].x -= XMARGIN;
				Stage[Komas[LION + Branch].y / YMARGIN - 1][(Komas[LION + Branch].x - 320) / XMARGIN] = 1 + Branch;
				Status = 1;		//ターンチェンジ関数に移動
				PlaySoundMem(KomaClick, DX_PLAYTYPE_BACK);
			}	//右上
			else if (Komas[LION + Branch].x < 680 && MouseX > Komas[LION + Branch].x + HXMARGIN && MouseX < Komas[LION + Branch].x + (XMARGIN + HXMARGIN)) {
				if (Stage[(Komas[LION + Branch].y - 280) / YMARGIN][(Komas[LION + Branch].x - 140) / XMARGIN] != 0
					&& Komas[Stage[(Komas[LION + Branch].y - 280) / YMARGIN][(Komas[LION + Branch].x - 140) / XMARGIN] - 1].pflg != Pflag) 
				{
					Komas[Stage[(Komas[LION + Branch].y - 280) / YMARGIN][(Komas[LION + Branch].x - 140) / XMARGIN] - 1].flg = 0;
				}
				Stage[Komas[LION + Branch].y / YMARGIN - 1][(Komas[LION + Branch].x - 320) / XMARGIN] = 0;
				Komas[LION + Branch].y -= YMARGIN;
				Komas[LION + Branch].x += XMARGIN;
				Stage[Komas[LION + Branch].y / YMARGIN - 1][(Komas[LION + Branch].x - 320) / XMARGIN] = 1 + Branch;
				Status = 1;		//ターンチェンジ関数に移動
				PlaySoundMem(KomaClick, DX_PLAYTYPE_BACK);
			}
		}
		else if (Komas[LION + Branch].y < 560 && MouseY > Komas[LION + Branch].y + HYMARGIN && MouseY < Komas[LION + Branch].y + (YMARGIN + HYMARGIN)) {
			//左下
			if (Komas[LION + Branch].x > 320 && MouseX > Komas[LION + Branch].x - (XMARGIN + HXMARGIN) && MouseX < Komas[LION + Branch].x - HXMARGIN) {
				if (Stage[(Komas[LION + Branch].y - 280) / YMARGIN + 2][(Komas[LION + Branch].x - 140) / XMARGIN - 2] != 0
					&& Komas[Stage[(Komas[LION + Branch].y - 280) / YMARGIN + 2][(Komas[LION + Branch].x - 140) / XMARGIN - 2] - 1].pflg != Pflag) 
				{
					Komas[Stage[(Komas[LION + Branch].y - 280) / YMARGIN + 2][(Komas[LION + Branch].x - 140) / XMARGIN - 2] - 1].flg = 0;
				}
				Stage[Komas[LION + Branch].y / YMARGIN - 1][(Komas[LION + Branch].x - 320) / XMARGIN] = 0;
				Komas[LION + Branch].y += YMARGIN;
				Komas[LION + Branch].x -= XMARGIN;
				Stage[Komas[LION + Branch].y / YMARGIN - 1][(Komas[LION + Branch].x - 320) / XMARGIN] = 1 + Branch;
				Status = 1;		//ターンチェンジ関数に移動
				PlaySoundMem(KomaClick, DX_PLAYTYPE_BACK);
			}	//右下
			else if (Komas[LION + Branch].x < 680 && MouseX > Komas[LION + Branch].x + HXMARGIN && MouseX < Komas[LION + Branch].x + (XMARGIN + HXMARGIN)) {
				if (Stage[(Komas[LION + Branch].y - 280) / YMARGIN + 2][(Komas[LION + Branch].x - 140) / XMARGIN] != 0
					&& Komas[Stage[(Komas[LION + Branch].y - 280) / YMARGIN + 2][(Komas[LION + Branch].x - 140) / XMARGIN] - 1].pflg != Pflag) 
				{
					Komas[Stage[(Komas[LION + Branch].y - 280) / YMARGIN + 2][(Komas[LION + Branch].x - 140) / XMARGIN] - 1].flg = 0;
				}
				Stage[Komas[LION + Branch].y / YMARGIN - 1][(Komas[LION + Branch].x - 320) / XMARGIN] = 0;
				Komas[LION + Branch].y += YMARGIN;
				Komas[LION + Branch].x += XMARGIN;
				Stage[Komas[LION + Branch].y / YMARGIN - 1][(Komas[LION + Branch].x - 320) / XMARGIN] = 1 + Branch;
				Status = 1;		//ターンチェンジ関数に移動
				PlaySoundMem(KomaClick, DX_PLAYTYPE_BACK);
			}
		}
	}
}

void ChangeTurn(void)
{
	if (Pause == false) {
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 200);
		DrawBox(0, 0, 1000, 700, 0x000000, TRUE);
		DrawFormatString(380, 185, 0xfffff00, "たーんこうたい");
		DrawFormatStringToHandle(350, 250, 0xffff00, ContentsFont, "%dP のひとへこうたいしてね！",Pflag == 1 ? 2 : 1);

		// つぎのひとへボタン
		DrawRotaGraph(535, 400, 1.4f, 0, Button, TRUE, FALSE);
		DrawFormatStringToHandle(410, 380, 0x000000, MenuFont, "%dPのひとへ", Pflag == 1 ? 2 : 1);


		// マウス左クリック判定
		if (KeyFlg & MOUSE_INPUT_LEFT) {
			if (MouseX < 680 && MouseX > 390 && MouseY > 345 && MouseY < 450) {	//たーんこうたいボタン
				PlaySoundMem(StartClick, DX_PLAYTYPE_BACK);
				Pflag == 1 ? HandCount++ : HandCount2++;
				//static int i = 1;

				Mflag = 0;
				Cflag = 1;
				if (Pflag == 1) {
					Pflag = 2;
				}
				else if (Pflag == 2) {
					Pflag = 1;
				}
				Status = 0; 
			}
		}
		if (MouseX < 680 && MouseX > 390 && MouseY > 345 && MouseY < 450) {	//たーんこうたいボタン(ホバー時)
			DrawFormatStringToHandle(410, 380, 0xff69b4, MenuFont, "%dPのひとへ", Pflag == 1 ? 2 : 1);
		}

		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}
}

void SelectDisplay(int x, int y) {

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 100);
	DrawBox(x - 60, y - 60, x + 60, y + 60, 0xdc143c, TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void GameClear(void) {

	if (CheckSoundMem(TitleBGM01) == 1) StopSoundMem(TitleBGM01);
	if (CheckSoundMem(GameClearBGM) == 0) PlaySoundMem(GameClearBGM, DX_PLAYTYPE_BACK);

	DrawGraph(0, 0, GameClearImage, TRUE);
	DrawFormatString(310, 260, 0xffd700, "%dP がかちしました！", Pflag == 1 ? 1 : 2);

	// タイトルボタン
	DrawRotaGraph(520, 400, 0.8f, 0, Button, TRUE, FALSE);
	DrawFormatStringToHandle(445, 380, 0x000000, MenuFont, "たいとる");

	DrawRotaGraph(520, 500, 0.8f, 0, Button, TRUE, FALSE);
	DrawFormatStringToHandle(465, 480, 0x000000, MenuFont, "おわる");


	// マウス左クリック判定
	if (KeyFlg & MOUSE_INPUT_LEFT) {
		if (MouseX < 605 && MouseX > 440 && MouseY > 370 && MouseY < 430) {	//タイトル画面ボタン
			PlaySoundMem(StartClick, DX_PLAYTYPE_BACK);
			StopSoundMem(GameClearBGM);
			GameState = GAME_TITLE;
		}

		if (MouseX < 605 && MouseX > 440 && MouseY > 470 && MouseY < 530) {	//おわる画面ボタン
			PlaySoundMem(StartClick, DX_PLAYTYPE_BACK);
			GameState = END;
		}
	}
	if (MouseX < 605 && MouseX > 440 && MouseY > 370 && MouseY < 430) {//タイトル画面ボタン(ホバー時)
		DrawFormatStringToHandle(445, 380, 0xffd700, MenuFont, "たいとる");
	}
	if (MouseX < 605 && MouseX > 440 && MouseY > 470 && MouseY < 530) {	//おわる画面ボタン(ホバー時)
		DrawFormatStringToHandle(465, 480, 0xffd700, MenuFont, "おわる");
	}
}