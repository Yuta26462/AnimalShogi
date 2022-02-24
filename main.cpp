/*******************************
**	�@�@�ǂ��Ԃ��傤���@�@�@**
*******************************/
#include "DxLib.h"
#include<stdlib.h>
#include <string.h>		//�ǂݍ��܂Ȃ��Ă������񑀍�֐��g���邪�O�̈�


#define KomaKinds 4 //��̎��
#define Sizex 3
#define Sizey 4
#define XMARGIN 180
#define YMARGIN 140

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
	LION = 0,	//���C�I��(����)
	GIRAF,		//�L����(�p�s)
	ELEPHA,		//�]�E(���)
	CHICK		//�q���R(����
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
}KomaSt;

KomaSt Komas[KomaKinds] = { CHICK,ELEPHA,GIRAF,LION };	//����z��


/***********************************************
 * �ϐ��̐錾
 ***********************************************/
int	OldKey;		//�O��̓��̓L�[
int	NowKey;		//����̓��̓L�[
int	KeyFlg;		//���̓L�[���
int	MouseX;		//�}�E�XX���W
int	MouseY;		//�}�E�XY���W
int	GameState = GAME_TITLE;   //�Q�[�����[�h

int Status = 0;		//�X�e�[�W�̃X�e�[�^�X

int Stage[Sizey][Sizex];		//�X�e�[�W�z��

int WaitTime = 0;    //	�҂�����
int StartTime = GetNowCount();	//�N������̌o�ߎ���

int TitleImage,Live2DStage;      //�^�C�g���摜
int StageImage;      //�X�e�[�W�摜
int KomaImage[10];   //�R�}�摜

int Live2D_ModelHandle, Live2D_ModelHandle2;
int ContentsFont;	//ISendMessege�p�t�H���g
char ms[];	//������n���p

//�T�E���h
int KomaClick, KomaNaru, StartClick;
//BGM
int TitleBGM, TitleBGM01, TitleBGM02, TitleBGM03, TitleBGM04, TitleBGM05, TitleBGM06;

/***********************************************
 * �֐��̃v���g�^�C�v�錾
 ***********************************************/
void GameInit(void);		//�Q�[������������
void GameMain(void);		//�Q�[�����C������
void GamePause(void);
void DrawGameTitle(void);	//�Q�[���^�C�g������
void GameEnd(void);

void DrawStage(void);	    //�X�e�[�W
void StageInit(void);	    //�X�e�[�W��������

void SelectKomas(void);	//��̑I��
void MoveChick(void);		//�q���R�̈ړ�����
void MoveGiraf(void);		//�L�����̈ړ�����
void MoveElepha(void);		//�]�E�̈ړ�����
void MoveLion(void);		//���C�I���̈ړ�����


int LoadImages(void);      //�摜�Ǎ���
int LoadSounds(void);	  //�����ǂݍ���

void ISendMessege(char* Contents, int partner = 0);
void SideBar(void);


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
	//SetGraphMode(800, 700, 32);
	SetGraphMode(1000, 700, 32);		//�`�悷��ő�E�B���h�E�T�C�Y��ݒ�
	SetWindowSizeChangeEnableFlag(FALSE, FALSE);	//�蓮�ŕύX�AFitScreen���I�t�ɂ���B
	SetWindowSize(600, 700);	//�E�B���h�E�T�C�Y��ύX


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
	Live2D_Model_SetTranslate(Live2D_ModelHandle, 400, -50);		//Live2D���f���̍��W��ݒ�
	Live2D_Model_SetTranslate(Live2D_ModelHandle2, -380, 0);		//Live2D���f���̍��W��ݒ�
	Live2D_Model_SetExtendRate(Live2D_ModelHandle,1.8f, 1.8f);		//Live2D���f���̑傫����ݒ�
	Live2D_Model_SetExtendRate(Live2D_ModelHandle2, 2.0f, 2.0f);	//Live2D���f���̑傫����ݒ�
	ContentsFont = CreateFontToHandle("Cherry bomb", 30, 2, DX_CHARSET_DEFAULT);	//ContentsFont�t�H���g�f�[�^���쐬

	// �Q�[�����[�v
	while (ProcessMessage() == 0 && GameState != END && !(KeyFlg & PAD_INPUT_START))
	{


		// ���̓L�[�擾
		OldKey = NowKey;
		NowKey = GetMouseInput();
		KeyFlg = NowKey & ~OldKey;

		//�}�E�X�̈ʒu���擾
		GetMousePoint(&MouseX, &MouseY);


		if (CheckHitKey(KEY_INPUT_Q) == 1) GamePause();			//Q�L�[�ŃQ�[���I��

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
			break;
		case GAME_MAIN:
			GameMain();
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

void DrawGameTitle(void)
{

	//�^�C�g���摜�\��
	DrawGraph(0, 0, TitleImage, FALSE);

	if (CheckSoundMem(TitleBGM) == 0) PlaySoundMem(TitleBGM, DX_PLAYTYPE_BACK);

	//DrawGraph(600, 0, StageImage, FALSE);			//Live2D�p�w�i
	//Live2D_Model_Draw(Live2D_ModelHandle);		//Live2D���f���`��

	//DrawBox(160, 405, 460, 465, 0xffffff, TRUE);

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

	StageInit();		//�X�e�[�W�̏�����

	//�Q�[�����C��������
	GameState = GAME_MAIN;
}

void DrawStage(void)
{

}

void StageInit(void)
{
	//�X�e�[�W�z��̏�����
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
	SetWindowSize(1000, 700);	//�E�B���h�E�T�C�Y�̕ύX

	//�X�e�[�W�摜�\��
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
		SelectKomas();		//��I��
		break;
	case 1:

		break;
	}

	DrawFormatString(270, 25, 0x000000, "x:%d  y:%d", MouseX, MouseY);	//�f�o�b�N�p ���W�m�F

}


int LoadImages()
{
	//�^�C�g��
	if ((TitleImage = LoadGraph("images/Title.jpg")) == -1)   return -1;
	//�X�e�[�W
	if ((StageImage = LoadGraph("images/Stage.jpg")) == -1)   return -1;

	if ((Live2DStage = LoadGraph("images/Live2DStage.png")) == -1)   return -1;
	//�u���b�N�摜
	if (LoadDivGraph("images/Koma.png", 10, 5, 2, 80, 80, KomaImage) == -1)   return -1;
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

}

void GamePause(void) {

	DrawFormatString(500, 300, 0x000000, "�ہ[��");

	WaitTimer(1000);		//1000ms(1�b)�ҋ@
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

	DrawGraph(0, 0, Live2DStage, FALSE);		//Live2D�p�w�i
	DrawGraph(800, 0, Live2DStage, FALSE);		//Live2D�p�w�i

	Live2D_Model_Draw(Live2D_ModelHandle);		//Live2D���f���`��
	Live2D_Model_Draw(Live2D_ModelHandle2);		//Live2D���f���`��
	//Live2D_Model_StartMotion(Live2D_ModelHandle, "FlickDown", 0);

	// �}�E�X���N���b�N����
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

	// �X�e�[�^�X�E���j���[�`��
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 235);
	DrawBox(5, 360, 200, 690, 0xd2b48c, TRUE);
	DrawBox(4, 360, 199, 690, 0x000000, FALSE);
	DrawBox(800, 360, 990, 690, 0xd2b48c, TRUE);
	DrawBox(799, 360, 989, 690, 0x000000, FALSE);

	// �^�C�g���{�^��
	DrawBox(10, 590, 195, 670, 0xf5f5f5, TRUE);
	DrawBox(9, 590, 194, 670, 0x000000, FALSE);
	DrawFormatString(15, 610, 0x000000, "�����Ƃ�");

	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	//// �����o���`��
	//SetDrawBlendMode(DX_BLENDMODE_ALPHA, 150);
	//DrawOval(105, 265, 100, 80, 0x000000, FALSE);
	//DrawOval(105, 265, 99, 79, 0xf0f8ff, TRUE);
	//
	//DrawOval(895, 265, 100, 80, 0x000000, FALSE);
	//DrawOval(895, 265, 99, 79, 0xf0f8ff, TRUE);
	//SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	//ISendMessage�e�X�g�\��	
	char ms[] = "�Ă��Ƃł��B";
	ISendMessege(ms);
	strcpy_s(ms, "���傤��");
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