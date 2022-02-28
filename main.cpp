/*******************************
**	�@�@�ǂ��Ԃ��傤���@�@�@**
*******************************/
#include "DxLib.h"
#include<stdlib.h>



#define KomaKinds 10 //��̎��
#define Sizex 3		//�Ֆ�(��)
#define Sizey 4		//�Ֆ�(�c)
#define XMARGIN 180	//��Ƌ�̊Ԋu(��)
#define HXMARGIN 90	//��ƃ}�X�̊Ԋu(��)
#define YMARGIN 140	//��Ƌ�̊Ԋu(�c)
#define HYMARGIN 70	//��ƃ}�X�̊Ԋu(�c)

LPCSTR font_path = "Fonts/Cherrybomb/Cherrybomb.ttf";

/***********************************************
 * �񋓑̂̐錾
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
	LION = 0,	//���C�I��(����)
	GIRAF,		//�L����(�p�s)
	ELEPHA,		//�]�E(���)
	CHICK,		//�q���R(����)
	CHICKEN,	//�j���g��(�Ƌ�)
};

/***********************************************
 * �萔�̐錾
 ***********************************************/
const int HEIGHT = 12;
const int WIDTH = 12;

/***********************************************
 * �\���̂̐錾
 ***********************************************/

//��\���̂̐錾
typedef struct KomaStatus {
	int x, y;		//��̍��W
	int w, h;		//��̑傫��
	int images;		//��̉摜�f�[�^
	int flg;		//��̗L�����
	int pflg;		//��̃v���C���[���(1P:1,2P:2)
}KomaSt;

KomaSt Komas[KomaKinds];	//����z��


/***********************************************
 * �ϐ��̐錾
 ***********************************************/
int	OldKey, OldKey2;		//�O��̓��̓L�[
int	NowKey, NowKey2;		//����̓��̓L�[
int	KeyFlg, KeyFlg2;		//���̓L�[���
int	MouseX;		//�}�E�XX���W
int	MouseY;		//�}�E�XY���W
int	GameState = GAME_TITLE;   //�Q�[�����[�h
bool Pause;		//�|�[�Y����

int Status;		//�X�e�[�W�̃X�e�[�^�X
static int ClickFlag;	//�N���b�N�t���O(�N���b�N������̎���)
static int Mflag;		//�ړ��\�}�[�N�t���O
 int Cflag;		//��N���b�N�t���O
static int Pflag;		//�v���C���[�t���O 1P:1,2P;2
static int Branch;		//1P��2P�̕���p�ϐ�
static int Abranch;		//Branch�̔��]

int Stage[Sizey][Sizex];		//�X�e�[�W�z��

int Handrand;		//���E��蔻�ʗp����

int WaitTime = 0;    //	�҂�����
int StartTime = GetNowCount();	//�N������̌o�ߎ���

int TitleImage,Live2DStage;      //�^�C�g���摜
int StageImage;      //�X�e�[�W�摜
int KomaImage[10];   //�R�}�摜
int Flame, Button;//UI�摜
int GameClearImage;		//�Q�[���N���A�摜
int HandCount, HandCount2;		//�Ă߃J�E���g�p

int Live2D_ModelHandle, Live2D_ModelHandle2;//Live2D�n���h��
int mscount;		//ISendMessage�p�J�E���g
bool msdis;			//ISendMessage�p�\���E��\���t���O
int ContentsFont;	//ISendMessege�p�t�H���g
int MenuFont;		//���j���[�p�t�H���g

//�T�E���h
int KomaClick, KomaNaru, StartClick;
//BGM
int TitleBGM, TitleBGM01,GameClearBGM;

/***********************************************
 * �֐��̃v���g�^�C�v�錾
 ***********************************************/
void GameInit(void);		//�Q�[������������
void GameMain(void);		//�Q�[�����C������
void GamePause(void);
void DrawGameTitle(void);	//�Q�[���^�C�g������
void GameClear(void);		//�Q�[���N���A
void GameEnd(void);
void StageInit(void);	    //�X�e�[�W��������

void SelectKomas(void);	//��̑I��
void SelectDisplay(int x, int y);	//�I���ł���͈͕\��
void MoveChick(void);		//�q���R�̈ړ�����
void MoveGiraf(void);		//�L�����̈ړ�����
void MoveElepha(void);		//�]�E�̈ړ�����
void MoveLion(void);		//���C�I���̈ړ�����
void ChangeTurn(void);		//�����A����^�[���ύX����
int CheckKoma(int Pf,int Course);			//�Ώۂ̋�̃v���C���[�𒲂ׂ�

int LoadImages(void);      //�摜�Ǎ���
int LoadSounds(void);	  //�����ǂݍ���

void ISendMessege(const TCHAR* Contents, int partner = 0);		//���b�Z�[�W�\��
void SideBar(void);		//�T�C�h���j���[�\��


/***********************************************
 * �v���O�����̊J�n
 ***********************************************/
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow)
{
	//�^�C�g����ݒ�
	SetMainWindowText("�ǂ��Ԃ��傤��");

	SetWindowIconID(01);	//�A�C�R����ݒ�

	//�E�B���h�E���[�h�ŋN��
	ChangeWindowMode(TRUE);

	//�E�B���h�E�T�C�Y
	SetGraphMode(1000, 700, 32);		//�`�悷��ő�E�B���h�E�T�C�Y��ݒ�
	SetWindowSizeChangeEnableFlag(FALSE, FALSE);	//�蓮�ŕύX�AFitScreen���I�t�ɂ���B


	// �t�H���g�ǂݍ���
	if (AddFontResourceEx(font_path, FR_PRIVATE, NULL) > 0) {
	}
	else {
		// �t�H���g�Ǎ��G���[����
		MessageBox(NULL, "�t�H���g�Ǎ����s", "", MB_OK);
	}
	ChangeFont("Cherry bomb", DX_CHARSET_DEFAULT);


	// Live2D Cubism Core DLL �̓ǂݍ���( 64bit �A�v���̏ꍇ�� 32bit �A�v���̏ꍇ�œǂݍ��� dll ��ύX )
#ifdef _WIN64
	Live2D_SetCubism4CoreDLLPath("dll/live2d/x86_64/Live2DCubismCore.dll");
#else
	Live2D_SetCubism4CoreDLLPath("dll/live2d/x86/Live2DCubismCore.dll");
#endif



	//DX���C�u�����̏���������
	if (DxLib_Init() == -1)   return -1;

	//�`����ʂ𗠂ɂ���
	SetDrawScreen(DX_SCREEN_BACK);

	
	if (LoadImages() == -1)   return -1;	//�摜�Ǎ��݊֐����Ăяo��
	if (LoadSounds() == -1)   return -1;	//�����ǂݍ���	

	Live2D_ModelHandle = Live2D_LoadModel("dll/hiyori_free_jp/runtime/hiyori_free_t08.model3.json");	//Live2D���f���ǂݍ���
	Live2D_ModelHandle2 = Live2D_LoadModel("dll/21miku_street/21miku_street.model3.json");	//Live2D���f���ǂݍ���
	Live2D_Model_SetTranslate(Live2D_ModelHandle, 400, -50);		//Live2D���f���̍��W�������ݒ�
	Live2D_Model_SetTranslate(Live2D_ModelHandle2, -380, 0);		//Live2D���f���̍��W�������ݒ�
	Live2D_Model_SetExtendRate(Live2D_ModelHandle,1.8f, 1.8f);		//Live2D���f���̑傫����ݒ�
	Live2D_Model_SetExtendRate(Live2D_ModelHandle2, 2.0f, 2.0f);	//Live2D���f���̑傫����ݒ�
	ContentsFont = CreateFontToHandle("Cherry bomb", 30, 2, DX_CHARSET_DEFAULT);	//ContentsFont�t�H���g�f�[�^(���b�Z�[�W�p)���쐬
	MenuFont = CreateFontToHandle("Cherry bomb", 45, 2, DX_CHARSET_DEFAULT);	//ContentsFont�t�H���g�f�[�^(���b�Z�[�W�p)���쐬

	// �Q�[�����[�v
	while (ProcessMessage() == 0 && GameState != END && !(KeyFlg & PAD_INPUT_START))
	{


		// ���̓L�[�擾
		OldKey = NowKey;
		NowKey = GetMouseInput();
		KeyFlg = NowKey & ~OldKey;

		OldKey2 = NowKey2;
		NowKey2 = GetJoypadInputState(DX_INPUT_KEY_PAD1);
		KeyFlg2 = NowKey2 & ~OldKey2;

		//�}�E�X�̈ʒu���擾
		GetMousePoint(&MouseX, &MouseY);


		if (KeyFlg2 && CheckHitKey(KEY_INPUT_Q) == 1) 			//Q�L�[�łہ[�����j���[��
		{
			if (Pause == false) {
				Pause = true;
			}
			else { Pause = false; }
		}

		if (GetWindowUserCloseFlag() != 0)	GameState = END;		//���C���E�B���h�E�̕���{�^������������Q�[���I��

		ClearDrawScreen();		// ��ʂ̏�����

		//	���[�V�����Đ����I��������Idle���[�V�����������_���Đ�
		if (Live2D_Model_IsMotionFinished(Live2D_ModelHandle) == TRUE) {
			Live2D_Model_StartMotion(Live2D_ModelHandle, "Idle", GetRand(3));
		}
		if (Live2D_Model_IsMotionFinished(Live2D_ModelHandle2) == TRUE) {
			Live2D_Model_StartMotion(Live2D_ModelHandle2, "Idle", GetRand(4));
		}

		Live2D_Model_Update(Live2D_ModelHandle, 1 / 200.0f);	//Live2D���f���X�V
		Live2D_Model_Update(Live2D_ModelHandle2, 1 / 200.0f);	//Live2D���f���X�V
		Live2D_RenderBegin();	//Live2D���f���`��J�n����


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
		ScreenFlip();			//����ʂ̓��e��\��ʂɔ��f
	}

	DxLib_End();	//DX���C�u�����g�p�̏I������

	return 0;	//�\�t�g�̏I��
}

int LoadImages()
{
	//�^�C�g��
	if ((TitleImage = LoadGraph("images/Title.png")) == -1)   return -1;
	//�X�e�[�W
	if ((StageImage = LoadGraph("images/Stage.jpg")) == -1)   return -1;
	if ((GameClearImage = LoadGraph("images/GameClear.png")) == -1)   return -1;
	if ((Live2DStage = LoadGraph("images/Live2DStage.png")) == -1)   return -1;

	if ((Flame = LoadGraph("images/Flame.png")) == -1)   return -1;
	if ((Button = LoadGraph("images/Button.png")) == -1)   return -1;

	//�u���b�N�摜
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
	//��̏�����
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

	StageInit();		//�X�e�[�W�̏�����

	Handrand = GetRand(1);		//���E��茈��
	mscount = 0;		//���b�Z�[�W�\���J�E���g���Z�b�g
	msdis = false;		//���b�Z�[�W��\���Ƀ��Z�b�g
	Pause = false;		//�|�[�Y��ԃ��Z�b�g

	HandCount = 0,HandCount2 = 0;	//�Ă߃J�E���g������
	ClickFlag = 0;	//�N���b�N�t���O(��̎���)
	Cflag = 0;	//��N���b�N�t���O
	Mflag = 0;	//�}�[�N�\���t���O
	Status = 0;	//�X�e�[�W�̏�
	Pflag = Handrand + 1;	//�v���C���[�t���O
	Branch = 0;	//1P��2P�̋�̐؂�ւ��p�ϐ�
	Abranch = 0;
	//�Q�[�����C��������
	GameState = GAME_MAIN;
}

void DrawGameTitle(void)
{

	//�^�C�g���摜�\��
	DrawGraph(0, 0, TitleImage, FALSE);
	if (CheckSoundMem(TitleBGM01) == 1) StopSoundMem(TitleBGM01);
	if (CheckSoundMem(TitleBGM) == 0) PlaySoundMem(TitleBGM, DX_PLAYTYPE_BACK);


	//�����̕\��(�_��)
	if (++WaitTime < 50)
	{
		SetFontSize(50);
		DrawString(170, 410, "�� �� �� ��", 0x000000);
	}
	else if (WaitTime > 100)
	{
		WaitTime = 0;
	}

	//�Q�[�����[�h��؂�ւ���
	if (KeyFlg & MOUSE_INPUT_LEFT)
	{
		if (MouseX > 160 && MouseX < 460 && MouseY>405 && MouseY < 465)
		{
			StopSoundMem(TitleBGM);
			PlaySoundMem(StartClick, DX_PLAYTYPE_BACK);
			GameState = GAME_INIT;   //�Q�[���X�^�[�g
		}
	}
	if (MouseX > 160 && MouseX < 460 && MouseY>405 && MouseY < 465) {// �z�o�[��
		DrawString(170, 410, "�� �� �� ��", 0xffa500);
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

	//�X�e�[�W�摜�\��
	DrawGraph(200, 0, StageImage, FALSE);

	SideBar();

	if(CheckSoundMem(TitleBGM01) == 0) PlaySoundMem(TitleBGM01, DX_PLAYTYPE_BACK);


	for (int i = 0; i < KomaKinds; i++) {
		if (i == 4 || i == 9) {
			continue;			//�j���g���͕`�悵�Ȃ�
		}
		if (Komas[i].flg != 0) {
			DrawRotaGraph(Komas[i].x, Komas[i].y, 1.8, 0, Komas[i].images, TRUE, FALSE);
		}
	}

	//�f�o�b�O�p
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
			SelectKomas();		//��I��
			break;
		case 1:
			if (Komas[LION + Abranch].flg == 0) {
				GameClear();
			}
			else
			{
				ChangeTurn();		//�^�[���`�F���W
			}

			break;
		}
	}

	//DrawFormatStringToHandle(270, 25, 0x000000, MenuFont, "x:%d  y:%d", MouseX, MouseY);	//�f�o�b�N�p ���W�m�F

	GamePause();	// �|�[�Y��ʌĂяo���p

}
	
void GameEnd(void) {

	DrawFormatString(500, 300, 0x000000,"���[�ނ��イ��傤");

	if(RemoveFontResourceEx(font_path, FR_PRIVATE, NULL)) {
	} else {
		MessageBox(NULL, "remove failure", "", MB_OK);
	}
	Live2D_RenderEnd();		//Live2D�`��̏I��
	Live2D_DeleteModel(Live2D_ModelHandle);	//Live2D���f���폜
	Live2D_DeleteModel(Live2D_ModelHandle2);	//Live2D���f���폜
	DeleteFontToHandle(ContentsFont);	//ContentsFont�f�[�^���폜
	DeleteFontToHandle(MenuFont);	//MenuFont�f�[�^���폜

}

void GamePause(void) {

	if (Pause == true) {
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 200);
		DrawBox(0, 0, 1000, 700, 0x000000, TRUE);
		DrawFormatString(450, 185, 0xfffffff, "�ہ[��");

		// �^�C�g���{�^��
		DrawRotaGraph(525, 400, 0.8f, 0, Button, TRUE, FALSE);
		DrawFormatStringToHandle(450, 380, 0x000000, MenuFont, "�����Ƃ�");

		DrawRotaGraph(525, 500, 0.8f, 0, Button, TRUE, FALSE);
		DrawFormatStringToHandle(470, 480, 0x000000, MenuFont, "�����");


		// �}�E�X���N���b�N����
		if (KeyFlg & MOUSE_INPUT_LEFT) {
			if (MouseX < 610 && MouseX > 445 && MouseY > 370 && MouseY < 430) {	//�^�C�g����ʃ{�^��
				PlaySoundMem(StartClick, DX_PLAYTYPE_BACK);
				StopSoundMem(TitleBGM01);
				GameState = GAME_TITLE;
			}

			if (MouseX < 610 && MouseX > 445 && MouseY > 470 && MouseY < 530) {	//������ʃ{�^��
				PlaySoundMem(StartClick, DX_PLAYTYPE_BACK);
				GameState = END;
			}
		}
		if (MouseX < 610 && MouseX > 445 && MouseY > 370 && MouseY < 430) {//�^�C�g����ʃ{�^��(�z�o�[��)
			DrawFormatStringToHandle(450, 380, 0xffd700, MenuFont, "�����Ƃ�");
		}
		if (MouseX < 610 && MouseX > 445 && MouseY > 470 && MouseY < 530) {	//������ʃ{�^��(�z�o�[��)
			DrawFormatStringToHandle(470, 480, 0xffd700, MenuFont, "�����");
		}

		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}
}


void SideBar(void) {

	DrawGraph(0, 0, Live2DStage, FALSE);		//Live2D�p�w�i
	DrawGraph(800, 0, Live2DStage, FALSE);		//Live2D�p�w�i

	Live2D_Model_SetTranslate(Live2D_ModelHandle, Pflag == 1 ? 400 : -380, -50);		//Live2D���f���̍��W��ݒ�
	Live2D_Model_SetTranslate(Live2D_ModelHandle2, Pflag == 1 ? -380 : 400, 0);		//Live2D���f���̍��W��ݒ�

	Live2D_Model_Draw(Live2D_ModelHandle);		//Live2D���f���`��
	Live2D_Model_Draw(Live2D_ModelHandle2);		//Live2D���f���`��
	//Live2D_Model_StartMotion(Live2D_ModelHandle, "FlickDown", 0);


	// �X�e�[�^�X�E���j���[�`��
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 235);
	DrawRotaGraph(110, 520, 2.0f, 0, Flame, TRUE, FALSE);
	DrawRotaGraph(890, 520, 2.0f, 0, Flame, TRUE, FALSE);


	DrawFormatStringToHandle(80, 380, Pflag == 1 ? 0xffff00:0xffd700, MenuFont, Pflag == 1 ? "1P" : "2P");
	DrawFormatStringToHandle(860, 380, Pflag == 1 ? 0xffd700:0xffff00, MenuFont, Pflag == 1 ? "2P" : "1P");

	DrawFormatStringToHandle(50, 450, Pflag == 1 ? 0xff7f50:0xff571a, MenuFont, "%d�Ă�", Pflag == 1 ? HandCount : HandCount2);
	DrawFormatStringToHandle(835, 450, Pflag == 1 ? 0xff571a:0xff7f50, MenuFont, "%d�Ă�", Pflag == 1 ? HandCount2 : HandCount);

	// �^�C�g���{�^��
	DrawRotaGraph(110, 630, 0.8f, 0, Button, TRUE, FALSE);
	DrawFormatStringToHandle(33, 610, 0x000000, MenuFont, "�����Ƃ�");

	DrawRotaGraph(890, 630, 0.8f, 0, Button, TRUE, FALSE);
	DrawFormatStringToHandle(830, 610, 0x000000, MenuFont, "�����");

	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	// �}�E�X���N���b�N����
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

			if (MouseX < 190 && MouseX > 30 && MouseY > 600 && MouseY < 655) {	//�^�C�g����ʃ{�^��
				PlaySoundMem(StartClick, DX_PLAYTYPE_BACK);
				StopSoundMem(TitleBGM01);
				GameState = GAME_TITLE;
			}
			if (MouseX < 970 && MouseX > 810 && MouseY > 600 && MouseY < 655) {	//������ʃ{�^��
				PlaySoundMem(StartClick, DX_PLAYTYPE_BACK);
				GameState = END;
			}
		}
		if (MouseX < 190 && MouseX > 30 && MouseY > 600 && MouseY < 655) {	//�^�C�g����ʃ{�^��(�z�o�[��)
			DrawFormatStringToHandle(33, 610, 0xffa500, MenuFont, "�����Ƃ�");
		}
		if (MouseX < 970 && MouseX > 810 && MouseY > 600 && MouseY < 655) {	//������ʃ{�^��(�z�o�[��)
			DrawFormatStringToHandle(830, 610, 0xffd700, MenuFont, "�����");
		}
	}

	//ISendMessage�e�X�g�\��	
	char Hand[2][7] = { "�����","����" };
	
	ISendMessege(Hand[Handrand]);
	ISendMessege(Hand[1 - Handrand], 1);
	//ISendMessege("�Ă��Ƃł��B");
	//ISendMessege("���傤��", 1);
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
	if (Pflag == 1) {		//1P�̂Ƃ�
		Branch = 0;
		Abranch = 5;
	}
	else if (Pflag == 2) {	//2P�̂Ƃ�
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
	int Sign = 0;		//�����p�ϐ�

	if (Pflag == 1) {			//1P�̏ꍇ
		Sign = 1;
	}
	else if (Pflag == 2) {		//2P�̏ꍇ
		Sign = -1;
	}

	//���̋�Ȃ���Έړ��\�}�[�N��`��
	//��
	if ((Komas[CHICK + Branch].y > 140 && Komas[CHICK + Branch].y < 560) && Cflag == 0 && Komas[CHICK + Branch].flg != 0
		&& (Stage[(Komas[CHICK + Branch].y - 140) / YMARGIN - Sign][(Komas[CHICK + Branch].x - 320) / XMARGIN] == 0
			|| Komas[Stage[(Komas[CHICK + Branch].y - 140) / YMARGIN - Sign][(Komas[CHICK + Branch].x - 320) / XMARGIN] - 1].pflg != Pflag))
	{
		SelectDisplay(Komas[CHICK + Branch].x, Komas[CHICK + Branch].y - YMARGIN * Sign);
		Mflag = 1;
	}


	//�ړ��\�}�[�N���N���b�N�����Ƃ��ړ�
	//��
	if (KeyFlg & KEY_INPUT_LEFT && Mflag == 1) {
		if (MouseX > Komas[CHICK + Branch].x - HXMARGIN && MouseX < Komas[CHICK + Branch].x + HXMARGIN) {
			if (Pflag == 1) {			//1P�̂Ƃ�
				if (Komas[CHICK + Branch].y > 140 && MouseY > Komas[CHICK + Branch].y - (YMARGIN + HYMARGIN) && MouseY < Komas[CHICK + Branch].y - HYMARGIN) {
					if (Stage[(Komas[CHICK + Branch].y - 280) / YMARGIN][(Komas[CHICK + Branch].x - 320) / XMARGIN] != 0
						&& Komas[Stage[(Komas[CHICK + Branch].y - 280) / YMARGIN][(Komas[CHICK + Branch].x - 320) / XMARGIN] - 1].pflg != Pflag)
					{
						Komas[Stage[(Komas[CHICK + Branch].y - 280) / YMARGIN][(Komas[CHICK + Branch].x - 320) / XMARGIN] - 1].flg = 0;
					}
					Stage[Komas[CHICK + Branch].y / YMARGIN - 1][(Komas[CHICK + Branch].x - 320) / XMARGIN] = 0;
					Komas[CHICK + Branch].y -= YMARGIN;
					Stage[Komas[CHICK + Branch].y / YMARGIN - 1][(Komas[CHICK + Branch].x - 320) / XMARGIN] = 4 + Branch;

					Status = 1;		//�^�[���`�F���W�֐��Ɉړ�
					PlaySoundMem(KomaClick, DX_PLAYTYPE_BACK);
				}
			}
			else if (Pflag == 2) {		//2P�̂Ƃ�
				if (Komas[CHICK + Branch].y < 560 && MouseY > Komas[CHICK + Branch].y + HYMARGIN && MouseY < Komas[CHICK + Branch].y + (YMARGIN + HYMARGIN)) {
					if (Stage[(Komas[CHICK + Branch].y - 280) / YMARGIN + 2][(Komas[CHICK + Branch].x - 320) / XMARGIN] != 0
						&& Komas[Stage[(Komas[CHICK + Branch].y - 280) / YMARGIN + 2][(Komas[CHICK + Branch].x - 320) / XMARGIN] - 1].pflg != Pflag)
					{
						Komas[Stage[(Komas[CHICK + Branch].y - 280) / YMARGIN + 2][(Komas[CHICK + Branch].x - 320) / XMARGIN] - 1].flg = 0;
					}
					Stage[Komas[CHICK + Branch].y / YMARGIN - 1][(Komas[CHICK + Branch].x - 320) / XMARGIN] = 0;
					Komas[CHICK + Branch].y += YMARGIN;
					Stage[Komas[CHICK + Branch].y / YMARGIN - 1][(Komas[CHICK + Branch].x - 320) / XMARGIN] = 4 + Branch;

					Status = 1;		//�^�[���`�F���W�֐��Ɉړ�
					PlaySoundMem(KomaClick, DX_PLAYTYPE_BACK);
				}
			}

		}

	}
}

void MoveGiraf(void)
{
	//���̋�Ȃ���Έړ��\�}�[�N��`��
		//��
	if (Komas[GIRAF + Branch].y > 140 && Cflag == 0 && Komas[GIRAF + Branch].flg != 0
		&& (Stage[(Komas[GIRAF + Branch].y - 280) / YMARGIN][(Komas[GIRAF + Branch].x - 320) / XMARGIN] == 0
		|| Komas[Stage[(Komas[GIRAF + Branch].y - 280) / YMARGIN][(Komas[GIRAF + Branch].x - 320) / XMARGIN] - 1].pflg != Pflag)) 
	{
		SelectDisplay(Komas[GIRAF + Branch].x, Komas[GIRAF + Branch].y - YMARGIN);
		if (Mflag == 0) {
			Mflag = 1;
		}
	}	//��
	if (Komas[GIRAF + Branch].x < 680 && Cflag == 0 && Komas[GIRAF + Branch].flg != 0
		&& (Stage[Komas[GIRAF + Branch].y / YMARGIN - 1][(Komas[GIRAF + Branch].x - 140) / XMARGIN] == 0
		|| Komas[Stage[Komas[GIRAF + Branch].y / YMARGIN - 1][(Komas[GIRAF + Branch].x - 140) / XMARGIN] - 1].pflg != Pflag)) 
	{
		SelectDisplay(Komas[GIRAF + Branch].x + XMARGIN, Komas[GIRAF + Branch].y);
		if (Mflag == 0) {
			Mflag = 1;
		}
	}	//��
	if (Komas[GIRAF + Branch].x > 320 && Cflag == 0 && Komas[GIRAF + Branch].flg != 0
		&& (Stage[Komas[GIRAF + Branch].y / YMARGIN - 1][(Komas[GIRAF + Branch].x - 140) / XMARGIN - 2] == 0
		|| Komas[Stage[Komas[GIRAF + Branch].y / YMARGIN - 1][(Komas[GIRAF + Branch].x - 140) / XMARGIN - 2] - 1].pflg != Pflag)) 
	{
			SelectDisplay(Komas[GIRAF + Branch].x - XMARGIN, Komas[GIRAF + Branch].y);
			if (Mflag == 0) {
				Mflag = 1;
			}
	}	//��
	if (Komas[GIRAF + Branch].y < 560 && Cflag == 0 && Komas[GIRAF + Branch].flg != 0
		&& (Stage[(Komas[GIRAF + Branch].y - 280) / YMARGIN + 2][(Komas[GIRAF + Branch].x - 320) / XMARGIN] == 0
		|| Komas[Stage[(Komas[GIRAF + Branch].y - 280) / YMARGIN + 2][(Komas[GIRAF + Branch].x - 320) / XMARGIN] - 1].pflg != Pflag)) 
	{
			SelectDisplay(Komas[GIRAF + Branch].x, Komas[GIRAF + Branch].y + YMARGIN);
			if (Mflag == 0) {
				Mflag = 1;
			}
	}
	

	//�ړ��\�}�[�N���N���b�N�����Ƃ��ړ�
	if (KeyFlg & KEY_INPUT_LEFT && Mflag == 1) {
			//�㉺
		if (MouseX > Komas[GIRAF + Branch].x - HXMARGIN && MouseX < Komas[GIRAF + Branch].x + HXMARGIN) {
			//��
			if (Komas[GIRAF + Branch].y > 140 && MouseY > Komas[GIRAF + Branch].y - (YMARGIN + HYMARGIN) && MouseY < Komas[GIRAF + Branch].y - HYMARGIN) {
				if (Stage[(Komas[GIRAF + Branch].y - 280) / YMARGIN][(Komas[GIRAF + Branch].x - 320) / XMARGIN] != 0
					&& Komas[Stage[(Komas[GIRAF + Branch].y - 280) / YMARGIN][(Komas[GIRAF + Branch].x - 320) / XMARGIN] - 1].pflg != Pflag) 
				{
					Komas[Stage[(Komas[GIRAF + Branch].y - 280) / YMARGIN][(Komas[GIRAF + Branch].x - 320) / XMARGIN] - 1].flg = 0;
				}
				Stage[Komas[GIRAF + Branch].y / YMARGIN - 1][(Komas[GIRAF + Branch].x - 320) / XMARGIN] = 0;
				Komas[GIRAF + Branch].y -= YMARGIN;
				Stage[Komas[GIRAF + Branch].y / YMARGIN - 1][(Komas[GIRAF + Branch].x - 320) / XMARGIN] = 2 + Branch;
				
				Status = 1;		//�^�[���`�F���W�֐��Ɉړ�
				PlaySoundMem(KomaClick, DX_PLAYTYPE_BACK);
			}//��
			else if (Komas[GIRAF + Branch].y < 560 && MouseY > Komas[GIRAF + Branch].y + HYMARGIN && MouseY < Komas[GIRAF + Branch].y + (YMARGIN + HYMARGIN)) {
				if (Stage[(Komas[GIRAF + Branch].y - 280) / YMARGIN + 2][(Komas[GIRAF + Branch].x - 320) / XMARGIN] != 0
					&& Komas[Stage[(Komas[GIRAF + Branch].y - 280) / YMARGIN + 2][(Komas[GIRAF + Branch].x - 320) / XMARGIN] - 1].pflg != Pflag) {
					Komas[Stage[(Komas[GIRAF + Branch].y - 280) / YMARGIN + 2][(Komas[GIRAF + Branch].x - 320) / XMARGIN] - 1].flg = 0;
				}
				Stage[Komas[GIRAF + Branch].y / YMARGIN - 1][(Komas[GIRAF + Branch].x - 320) / XMARGIN] = 0;
				Komas[GIRAF + Branch].y += YMARGIN;
				Stage[Komas[GIRAF + Branch].y / YMARGIN - 1][(Komas[GIRAF + Branch].x - 320) / XMARGIN] = 2 + Branch;
				
				Status = 1;		//�^�[���`�F���W�֐��Ɉړ�
				PlaySoundMem(KomaClick, DX_PLAYTYPE_BACK);
			}
		}	//���E
		else if (MouseY > Komas[GIRAF + Branch].y - HYMARGIN && MouseY < Komas[GIRAF + Branch].y + HYMARGIN) {
			//��
			if (Komas[GIRAF + Branch].x > 320 && MouseX > Komas[GIRAF + Branch].x - (XMARGIN + HXMARGIN) && MouseX < Komas[GIRAF + Branch].x - HXMARGIN) {
				if (Stage[Komas[GIRAF + Branch].y / YMARGIN - 1][(Komas[GIRAF + Branch].x - 140) / XMARGIN - 2] != 0
					&& Komas[Stage[Komas[GIRAF + Branch].y / YMARGIN - 1][(Komas[GIRAF + Branch].x - 140) / XMARGIN - 2] - 1].pflg != Pflag) 
				{
					Komas[Stage[Komas[GIRAF + Branch].y / YMARGIN - 1][(Komas[GIRAF + Branch].x - 140) / XMARGIN - 2] - 1].flg = 0;
				}
				Stage[Komas[GIRAF + Branch].y / YMARGIN - 1][(Komas[GIRAF + Branch].x - 320) / XMARGIN] = 0;
				Komas[GIRAF + Branch].x -= XMARGIN;
				Stage[Komas[GIRAF + Branch].y / YMARGIN - 1][(Komas[GIRAF + Branch].x - 320) / XMARGIN] = 2 + Branch;
				
				Status = 1;		//�^�[���`�F���W�֐��Ɉړ�
				PlaySoundMem(KomaClick, DX_PLAYTYPE_BACK);
			}//��
			else if (Komas[GIRAF + Branch].x < 680 && MouseX > Komas[GIRAF + Branch].x + HXMARGIN && MouseX < Komas[GIRAF + Branch].x + (XMARGIN + HXMARGIN)) {
				if (Stage[Komas[GIRAF + Branch].y / YMARGIN - 1][(Komas[GIRAF + Branch].x - 140) / XMARGIN] != 0
					&& Komas[Stage[Komas[GIRAF + Branch].y / YMARGIN - 1][(Komas[GIRAF + Branch].x - 140) / XMARGIN] - 1].pflg != Pflag) 
				{
					Komas[Stage[Komas[GIRAF + Branch].y / YMARGIN - 1][(Komas[GIRAF + Branch].x - 140) / XMARGIN] - 1].flg = 0;
				}
				Stage[Komas[GIRAF + Branch].y / YMARGIN - 1][(Komas[GIRAF + Branch].x - 320) / XMARGIN] = 0;
				Komas[GIRAF + Branch].x += XMARGIN;
				Stage[Komas[GIRAF + Branch].y / YMARGIN - 1][(Komas[GIRAF + Branch].x - 320) / XMARGIN] = 2 + Branch;
				
				Status = 1;		//�^�[���`�F���W�֐��Ɉړ�
				PlaySoundMem(KomaClick, DX_PLAYTYPE_BACK);
			}
		}
	}
	
}

//�p�s�̈ړ�����
void MoveElepha(void)
{
	//���̋�Ȃ���Έړ��\�}�[�N��`��
	if (Komas[ELEPHA + Branch].y > 140) {
			//����
		if (Komas[ELEPHA + Branch].x > 320 && Cflag == 0 && Komas[ELEPHA + Branch].flg != 0
			&& (Stage[(Komas[ELEPHA + Branch].y - 280) / YMARGIN][(Komas[ELEPHA + Branch].x - 140) / XMARGIN - 2] == 0
			|| Komas[Stage[(Komas[ELEPHA + Branch].y - 280) / YMARGIN][(Komas[ELEPHA + Branch].x - 140) / XMARGIN - 2] - 1].pflg != Pflag)) 
		{
			SelectDisplay(Komas[ELEPHA + Branch].x - XMARGIN, Komas[ELEPHA + Branch].y - YMARGIN);
			if (Mflag == 0) {
				Mflag = 1;
			}
		}	//�E��
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
			//����
		if (Komas[ELEPHA + Branch].x > 320 && Cflag == 0 && Komas[ELEPHA + Branch].flg != 0
			&& (Stage[(Komas[ELEPHA + Branch].y - 280) / YMARGIN + 2][(Komas[GIRAF + Branch].x - 140) / XMARGIN - 2] == 0
			|| Komas[Stage[(Komas[ELEPHA + Branch].y - 280) / YMARGIN + 2][(Komas[GIRAF + Branch].x - 140) / XMARGIN - 2] - 1].pflg != Pflag)) 
		{
			SelectDisplay(Komas[ELEPHA + Branch].x - XMARGIN, Komas[ELEPHA + Branch].y + YMARGIN);
			if (Mflag == 0) {
				Mflag = 1;
			}
		}	//�E��
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

	//�ړ��\�}�[�N���N���b�N�����ƈړ�
	if (KeyFlg & KEY_INPUT_LEFT && Mflag == 1) {
		if (Komas[ELEPHA + Branch].y > 140 && MouseY < Komas[ELEPHA + Branch].y - HYMARGIN && MouseY > Komas[ELEPHA + Branch].y - (YMARGIN + HYMARGIN)) {
				//����
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
				Status = 1;		//�^�[���`�F���W�֐��Ɉړ�
				PlaySoundMem(KomaClick, DX_PLAYTYPE_BACK);
			}	//�E��
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
				Status = 1;		//�^�[���`�F���W�֐��Ɉړ�
				PlaySoundMem(KomaClick, DX_PLAYTYPE_BACK);
			}
		}
		else if (Komas[ELEPHA + Branch].y < 560 && MouseY > Komas[ELEPHA + Branch].y + HYMARGIN && MouseY < Komas[ELEPHA + Branch].y + (YMARGIN + HYMARGIN)) {
				//����
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
				Status = 1;		//�^�[���`�F���W�֐��Ɉړ�
				PlaySoundMem(KomaClick, DX_PLAYTYPE_BACK);
			}	//�E��
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
				Status = 1;		//�^�[���`�F���W�֐��Ɉړ�
				PlaySoundMem(KomaClick, DX_PLAYTYPE_BACK);
			}
		}
	}
}

void MoveLion(void)
{
	//���̋�Ȃ���Έړ��\�}�[�N��`��
		//��
	if (Komas[LION + Branch].y > 140 && Cflag == 0 && Komas[LION + Branch].flg != 0
		&& (Stage[(Komas[LION + Branch].y - 280) / YMARGIN][(Komas[LION + Branch].x - 320) / XMARGIN] == 0
			|| Komas[Stage[(Komas[LION + Branch].y - 280) / YMARGIN][(Komas[LION + Branch].x - 320) / XMARGIN] - 1].pflg != Pflag)) 
	{
		SelectDisplay(Komas[LION + Branch].x, Komas[LION + Branch].y - YMARGIN);
		if (Mflag == 0) {
			Mflag = 1;
		}
	}	//��
	if (Komas[LION + Branch].x < 680 && Cflag == 0 && Komas[LION + Branch].flg != 0
		&& (Stage[Komas[LION + Branch].y / YMARGIN - 1][(Komas[LION + Branch].x - 140) / XMARGIN] == 0
			|| Komas[Stage[Komas[LION + Branch].y / YMARGIN - 1][(Komas[LION + Branch].x - 140) / XMARGIN] - 1].pflg != Pflag)) 
	{
		SelectDisplay(Komas[LION + Branch].x + XMARGIN, Komas[LION + Branch].y);
		if (Mflag == 0) {
			Mflag = 1;
		}
	}	//��
	if (Komas[LION + Branch].x > 320 && Cflag == 0 && Komas[LION + Branch].flg != 0
		&& (Stage[Komas[LION + Branch].y / YMARGIN - 1][(Komas[LION + Branch].x - 140) / XMARGIN - 2] == 0
		|| Komas[Stage[Komas[LION + Branch].y / YMARGIN - 1][(Komas[LION + Branch].x - 140) / XMARGIN - 2] - 1].pflg != Pflag)) 
	{
		SelectDisplay(Komas[LION + Branch].x - XMARGIN, Komas[LION + Branch].y);
		if (Mflag == 0) {
			Mflag = 1;
		}
	}	//��
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
		//����
		if (Komas[LION + Branch].x > 320 && Cflag == 0 && Komas[LION + Branch].flg != 0
			&& (Stage[(Komas[LION + Branch].y - 280) / YMARGIN][(Komas[LION + Branch].x - 140) / XMARGIN - 2] == 0
			|| Komas[Stage[(Komas[LION + Branch].y - 280) / YMARGIN][(Komas[LION + Branch].x - 140) / XMARGIN - 2] - 1].pflg != Pflag)) 
{
			SelectDisplay(Komas[LION + Branch].x - XMARGIN, Komas[LION + Branch].y - YMARGIN);
			if (Mflag == 0) {
				Mflag = 1;
			}
		}	//�E��
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
		//����
		if (Komas[LION + Branch].x > 320 && Cflag == 0 && Komas[LION + Branch].flg != 0
			&& (Stage[(Komas[LION + Branch].y - 280) / YMARGIN + 2][(Komas[LION + Branch].x - 140) / XMARGIN - 2] == 0
			|| Komas[Stage[(Komas[LION + Branch].y - 280) / YMARGIN + 2][(Komas[LION + Branch].x - 140) / XMARGIN - 2] - 1].pflg != Pflag)) 
		{
			SelectDisplay(Komas[LION + Branch].x - XMARGIN, Komas[LION + Branch].y + YMARGIN);
			if (Mflag == 0) {
				Mflag = 1;
			}
		}	//�E��
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

	//�ړ��\�}�[�N���N���b�N�����Ƃ��ړ�
	if (KeyFlg & KEY_INPUT_LEFT && Mflag == 1) {
		//�㉺
		if (MouseX > Komas[LION + Branch].x - HXMARGIN && MouseX < Komas[LION + Branch].x + HXMARGIN) {
			//��
			if (Komas[LION + Branch].y > 140 && MouseY > Komas[LION + Branch].y - (YMARGIN + HYMARGIN) && MouseY < Komas[LION + Branch].y - HYMARGIN) {
				if (Stage[(Komas[LION + Branch].y - 280) / YMARGIN][(Komas[LION + Branch].x - 320) / XMARGIN] != 0
					&& Komas[Stage[(Komas[LION + Branch].y - 280) / YMARGIN][(Komas[LION + Branch].x - 320) / XMARGIN] - 1].pflg != Pflag) 
				{
					Komas[Stage[(Komas[LION + Branch].y - 280) / YMARGIN][(Komas[LION + Branch].x - 320) / XMARGIN] - 1].flg = 0;
				}
				Stage[Komas[LION + Branch].y / YMARGIN - 1][(Komas[LION + Branch].x - 320) / XMARGIN] = 0;
				Komas[LION + Branch].y -= YMARGIN;
				Stage[Komas[LION + Branch].y / YMARGIN - 1][(Komas[LION + Branch].x - 320) / XMARGIN] = 1 + Branch;
				Status = 1;		//�^�[���`�F���W�֐��Ɉړ�
				PlaySoundMem(KomaClick, DX_PLAYTYPE_BACK);
			}//��
			else if (Komas[LION + Branch].y < 560 && MouseY > Komas[LION + Branch].y + HYMARGIN && MouseY < Komas[LION + Branch].y + (YMARGIN + HYMARGIN)) {
				if (Stage[(Komas[LION + Branch].y - 280) / YMARGIN + 2][(Komas[LION + Branch].x - 320) / XMARGIN] != 0
					&& Komas[Stage[(Komas[LION + Branch].y - 280) / YMARGIN + 2][(Komas[LION + Branch].x - 320) / XMARGIN] - 1].pflg != Pflag) 
				{
					Komas[Stage[(Komas[LION + Branch].y - 280) / YMARGIN + 2][(Komas[LION + Branch].x - 320) / XMARGIN] - 1].flg = 0;
				}
				Stage[Komas[LION + Branch].y / YMARGIN - 1][(Komas[LION + Branch].x - 320) / XMARGIN] = 0;
				Komas[LION + Branch].y += YMARGIN;
				Stage[Komas[LION + Branch].y / YMARGIN - 1][(Komas[LION + Branch].x - 320) / XMARGIN] = 1 + Branch;
				Status = 1;		//�^�[���`�F���W�֐��Ɉړ�
				PlaySoundMem(KomaClick, DX_PLAYTYPE_BACK);
			} 
		}	//���E
		else if (MouseY > Komas[LION + Branch].y - HYMARGIN && MouseY < Komas[LION + Branch].y + HYMARGIN) {
			//��
			if (Komas[LION + Branch].x > 320 && MouseX > Komas[LION + Branch].x - (XMARGIN + HXMARGIN) && MouseX < Komas[LION + Branch].x - HXMARGIN) {
				if (Stage[Komas[LION + Branch].y / YMARGIN - 1][(Komas[LION + Branch].x - 140) / XMARGIN - 2] != 0
					&& Komas[Stage[Komas[LION + Branch].y / YMARGIN - 1][(Komas[LION + Branch].x - 140) / XMARGIN - 2] - 1].pflg != Pflag) 
				{
					Komas[Stage[Komas[LION + Branch].y / YMARGIN - 1][(Komas[LION + Branch].x - 140) / XMARGIN - 2] - 1].flg = 0;
				}
				Stage[Komas[LION + Branch].y / YMARGIN - 1][(Komas[LION + Branch].x - 320) / XMARGIN] = 0;
				Komas[LION + Branch].x -= XMARGIN;
				Stage[Komas[LION + Branch].y / YMARGIN - 1][(Komas[LION + Branch].x - 320) / XMARGIN] = 1 + Branch;
				Status = 1;		//�^�[���`�F���W�֐��Ɉړ�
				PlaySoundMem(KomaClick, DX_PLAYTYPE_BACK);
			}//��
			else if (Komas[LION + Branch].x < 680 && MouseX > Komas[LION + Branch].x + HXMARGIN && MouseX < Komas[LION + Branch].x + (XMARGIN + HXMARGIN)) {
				if (Stage[Komas[LION + Branch].y / YMARGIN - 1][(Komas[LION + Branch].x - 140) / XMARGIN] != 0
					&& Komas[Stage[Komas[LION + Branch].y / YMARGIN - 1][(Komas[LION + Branch].x - 140) / XMARGIN] - 1].pflg != Pflag) 
				{
					Komas[Stage[Komas[LION + Branch].y / YMARGIN - 1][(Komas[LION + Branch].x - 140) / XMARGIN] - 1].flg = 0;
				}
				Stage[Komas[LION + Branch].y / YMARGIN - 1][(Komas[LION + Branch].x - 320) / XMARGIN] = 0;
				Komas[LION + Branch].x += XMARGIN;
				Stage[Komas[LION + Branch].y / YMARGIN - 1][(Komas[LION + Branch].x - 320) / XMARGIN] = 1 + Branch;
				Status = 1;		//�^�[���`�F���W�֐��Ɉړ�
				PlaySoundMem(KomaClick, DX_PLAYTYPE_BACK);
			}
		}

		if (Komas[LION + Branch].y > 140 && MouseY < Komas[LION + Branch].y - HYMARGIN && MouseY > Komas[LION + Branch].y - (YMARGIN + HYMARGIN)) {
			//����
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
				Status = 1;		//�^�[���`�F���W�֐��Ɉړ�
				PlaySoundMem(KomaClick, DX_PLAYTYPE_BACK);
			}	//�E��
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
				Status = 1;		//�^�[���`�F���W�֐��Ɉړ�
				PlaySoundMem(KomaClick, DX_PLAYTYPE_BACK);
			}
		}
		else if (Komas[LION + Branch].y < 560 && MouseY > Komas[LION + Branch].y + HYMARGIN && MouseY < Komas[LION + Branch].y + (YMARGIN + HYMARGIN)) {
			//����
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
				Status = 1;		//�^�[���`�F���W�֐��Ɉړ�
				PlaySoundMem(KomaClick, DX_PLAYTYPE_BACK);
			}	//�E��
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
				Status = 1;		//�^�[���`�F���W�֐��Ɉړ�
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
		DrawFormatString(380, 185, 0xfffff00, "���[�񂱂�����");
		DrawFormatStringToHandle(350, 250, 0xffff00, ContentsFont, "%dP �̂ЂƂւ����������ĂˁI",Pflag == 1 ? 2 : 1);

		// ���̂ЂƂփ{�^��
		DrawRotaGraph(535, 400, 1.4f, 0, Button, TRUE, FALSE);
		DrawFormatStringToHandle(410, 380, 0x000000, MenuFont, "%dP�̂ЂƂ�", Pflag == 1 ? 2 : 1);


		// �}�E�X���N���b�N����
		if (KeyFlg & MOUSE_INPUT_LEFT) {
			if (MouseX < 680 && MouseX > 390 && MouseY > 345 && MouseY < 450) {	//���[�񂱂������{�^��
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
		if (MouseX < 680 && MouseX > 390 && MouseY > 345 && MouseY < 450) {	//���[�񂱂������{�^��(�z�o�[��)
			DrawFormatStringToHandle(410, 380, 0xff69b4, MenuFont, "%dP�̂ЂƂ�", Pflag == 1 ? 2 : 1);
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
	DrawFormatString(310, 260, 0xffd700, "%dP ���������܂����I", Pflag == 1 ? 1 : 2);

	// �^�C�g���{�^��
	DrawRotaGraph(520, 400, 0.8f, 0, Button, TRUE, FALSE);
	DrawFormatStringToHandle(445, 380, 0x000000, MenuFont, "�����Ƃ�");

	DrawRotaGraph(520, 500, 0.8f, 0, Button, TRUE, FALSE);
	DrawFormatStringToHandle(465, 480, 0x000000, MenuFont, "�����");


	// �}�E�X���N���b�N����
	if (KeyFlg & MOUSE_INPUT_LEFT) {
		if (MouseX < 605 && MouseX > 440 && MouseY > 370 && MouseY < 430) {	//�^�C�g����ʃ{�^��
			PlaySoundMem(StartClick, DX_PLAYTYPE_BACK);
			StopSoundMem(GameClearBGM);
			GameState = GAME_TITLE;
		}

		if (MouseX < 605 && MouseX > 440 && MouseY > 470 && MouseY < 530) {	//������ʃ{�^��
			PlaySoundMem(StartClick, DX_PLAYTYPE_BACK);
			GameState = END;
		}
	}
	if (MouseX < 605 && MouseX > 440 && MouseY > 370 && MouseY < 430) {//�^�C�g����ʃ{�^��(�z�o�[��)
		DrawFormatStringToHandle(445, 380, 0xffd700, MenuFont, "�����Ƃ�");
	}
	if (MouseX < 605 && MouseX > 440 && MouseY > 470 && MouseY < 530) {	//������ʃ{�^��(�z�o�[��)
		DrawFormatStringToHandle(465, 480, 0xffd700, MenuFont, "�����");
	}
}