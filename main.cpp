/*******************************
**	�@�@�ǂ��Ԃ��傤���@�@�@**
*******************************/
#include "DxLib.h"
#include<stdlib.h>


#define PieceKinds 4 //��̎��

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
	CHICK = 0,	//�q���R(����)
	ELEPHA,		//�]�E(���)
	GIRAF,		//�L����(�p�s)
	LION		//���C�I��(����)
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
typedef struct PieceStatus {
	int x, y;		//��̍��W
	int w, h;		//��̑傫��
	int images;		//��̉摜�f�[�^
	int flg;		//��̗L�����
}PieceSt;

PieceSt Pieces[PieceKinds] = { CHICK,ELEPHA,GIRAF,LION };	//����z��


/***********************************************
 * �ϐ��̐錾
 ***********************************************/
int	OldKey;		//�O��̓��̓L�[
int	NowKey;		//����̓��̓L�[
int	KeyFlg;		//���̓L�[���
int	MouseX;		//�}�E�XX���W
int	MouseY;		//�}�E�XY���W
int	GameState = GAME_TITLE;   //�Q�[�����[�h

int WaitTime = 0;    //	�҂�����

int TitleImage;      //�^�C�g���摜
int StageImage;      //�X�e�[�W�摜
int KomaImage[10];   //�R�}�摜

int Live2D_ModelHandle;

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

int LoadImages(void);      //�摜�Ǎ���
int LoadSounds(void);



/***********************************************
 * �v���O�����̊J�n
 ***********************************************/
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow)
{
	//�^�C�g����ݒ�
	SetMainWindowText("�ǂ��Ԃ��傤��");

	//�E�B���h�E���[�h�ŋN��
	ChangeWindowMode(TRUE);

	//�E�B���h�E�T�C�Y
	//SetGraphMode(600, 700, 32);
	SetGraphMode(800, 700, 32);		//�`�悷��ő�E�B���h�E�T�C�Y��ݒ�
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
	Live2D_Model_SetTranslate(Live2D_ModelHandle, 300, 10);		//Live2D���f���̍��W��ݒ�
	Live2D_Model_SetExtendRate(Live2D_ModelHandle,1.8f, 1.8f);

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

		Live2D_Model_Update(Live2D_ModelHandle, 1 / 300.0f);	//Live2D���f���X�V
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
			PlaySoundMem(StartClick, DX_PLAYTYPE_BACK);
			GameState = GAME_INIT;   //�Q�[���X�^�[�g
		}
	}
}

void GameInit(void)
{
	//��̏�����
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

	//�Q�[�����C��������
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
	SetWindowSize(800, 700);	//�E�B���h�E�T�C�Y�̕ύX

	//�X�e�[�W�摜�\��
	DrawGraph(0, 0, StageImage, FALSE);
	
	DrawGraph(600, 0, StageImage, FALSE);		//Live2D�p�w�i

	Live2D_Model_Draw(Live2D_ModelHandle);		//Live2D���f���`��
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

	DrawFormatString(70, 25, 0x000000, "x:%d  y:%d", MouseX, MouseY);	//�f�o�b�N�p ���W�m�F

}


int LoadImages()
{
	//�^�C�g��
	if ((TitleImage = LoadGraph("images/Title.jpg")) == -1)   return -1;
	//�X�e�[�W
	if ((StageImage = LoadGraph("images/Stage.jpg")) == -1)   return -1;
	//�u���b�N�摜
	if (LoadDivGraph("images/Koma.png", 10, 5, 2, 80, 80, KomaImage) == -1)   return -1;
}
	
void GameEnd(void) {

	DrawFormatString(300, 300, 0x000000,"���[�ނ��イ��傤");

	if(RemoveFontResourceEx(font_path, FR_PRIVATE, NULL)) {
	} else {
		MessageBox(NULL, "remove failure", "", MB_OK);
	}
	Live2D_RenderEnd();		//Live2D�`��̏I��
	Live2D_DeleteModel(Live2D_ModelHandle);	//Live2D���f���폜

}

void GamePause(void) {

	DrawFormatString(300, 300, 0x000000, "�ہ[��");

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


	ChangeVolumeSoundMem(150, TitleBGM);
	ChangeVolumeSoundMem(150, TitleBGM01);
}