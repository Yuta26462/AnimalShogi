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



/***********************************************
 * �v���O�����̊J�n
 ***********************************************/
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow)
{
	//�^�C�g����ݒ�
	SetMainWindowText("�ǂ��Ԃ��傤��");

	//�E�B���h�E�T�C�Y
	SetGraphMode(600, 700, 32);

	//�E�B���h�E���[�h�ŋN��
	ChangeWindowMode(TRUE);

	// �t�H���g�ǂݍ���
	if (AddFontResourceEx(font_path, FR_PRIVATE, NULL) > 0) {
	}
	else {
		// �t�H���g�Ǎ��G���[����
		MessageBox(NULL, "�t�H���g�Ǎ����s", "", MB_OK);
	}
	ChangeFont("Cherry bomb", DX_CHARSET_DEFAULT);

	//DX���C�u�����̏���������
	if (DxLib_Init() == -1)   return -1;

	//�`����ʂ𗠂ɂ���
	SetDrawScreen(DX_SCREEN_BACK);

	//�摜�Ǎ��݊֐����Ăяo��
	if (LoadImages() == -1)   return -1;

	// �Q�[�����[�v
	while (ProcessMessage() == 0 && GameState != END && !(KeyFlg & PAD_INPUT_START))
	{
		// ���̓L�[�擾
		OldKey = NowKey;
		NowKey = GetMouseInput();
		KeyFlg = NowKey & ~OldKey;

		//�}�E�X�̈ʒu���擾
		GetMousePoint(&MouseX, &MouseY);

		if (GetJoypadInputState(DX_INPUT_PAD1) & KEY_INPUT_PAUSE != 0) {
			GamePause();
		}

		ClearDrawScreen();		// ��ʂ̏�����

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
		ScreenFlip();			//����ʂ̓��e��\��ʂɔ��f
	}

	DxLib_End();	//DX���C�u�����g�p�̏I������

	return 0;	//�\�t�g�̏I��
}

void DrawGameTitle(void)
{
	//�^�C�g���摜�\��
	DrawGraph(0, 0, TitleImage, FALSE);

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
			GameState = GAME_INIT;   //�Q�[���X�^�[�g
		}
	}
}

void GameInit(void)
{
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
	//�X�e�[�W�摜�\��
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
	}
 else {
 MessageBox(NULL, "remove failure", "", MB_OK);
	}
}

void GamePause(void) {

	DrawFormatString(300, 300, 0x000000, "�ہ[��");
}