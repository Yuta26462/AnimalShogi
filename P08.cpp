/*****************************************
* ��W�́@�~�j�Q�[��������i�T�j
*	Match 3 Puzzle (�}�b�`�R�p�Y��)
* ****************************************/
								//P.15	�J�`
#include"DxLib.h"
#include<stdlib.h>


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

int g_Status = 0;		// 0...�v���C���[���쒆
int g_SelectX1;		// 1��ڂɑI�������u���b�N�̓񎟌��z��(��)
int g_SelectY1;		// 1��ڂɑI�������u���b�N�̓񎟌��z��(�c)
int g_SelectX2;		// 2��ڂɑI�������u���b�N�̓񎟌��z��(��)
int g_SelectY2;		// 2��ڂɑI�������u���b�N�̓񎟌��z��(�c)

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

	SetMainWindowText("Match 3 Puzzle (�}�b�`�R�p�Y��)");

	ChangeWindowMode(TRUE);

	if (DxLib_Init() == -1)return -1;

	SetDrawScreen(DX_SCREEN_BACK);

	if (LoadImages() == -1) return -1;
	if (LoadSounds() == -1) return -1;


	// �Q�[�����[�v
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
				g_Block[i][j].image = GetRand(7) + 1;	// 1�`8�܂łɗ����𐶐�
			}
		}
	}


	g_Score = 0;
	g_Level = 0;
	g_Mission = 2;			// ���x��1�͏�������u���b�N����3����X�^�[�g

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
	DrawString(150, 420, "---- ���N���b�N�ŃQ�[���N���A�� ----", 0xffffff, 0);
	DrawString(150, 450, "---- �E�N���b�N�ŃQ�[���I�[�o�[�� ----", 0xffffff, 0);*/


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


	// �^�C���ϐ����f�N�������g���āA0�����ɂȂ�����Q�[���I�[�o�[�ֈڍs����
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
			// �X�e�[�W��������
			StageInit();
			g_GameState = GAME_MAIN;	//����
			GameClearFlg = 0;
		}
	}
	DrawGraph(0, 0, g_GameClearImage, FALSE);

	//if (g_Keyflg & MOUSE_INPUT_RIGHT) {
	//	//g_GameState = GAME_TITLE;

	//	// �X�e�[�W��������
	//	StageInit();
	//	g_GameState = GAME_MAIN;
	//}
	////SetFontSize(16);
	////DrawString(150, 450, "---- �E�N���b�N�Ń^�C�g���� ----", 0xffffff, 0);
	//SetFontSize(20);
	//DrawString(150, 450, "---- �E�N���b�N�Ŏ��̃X�e�[�W�� ----", 0xffffff, 0);
}


void DrawGameOver(void) {

	static int GameOverFlg = 0;

	if (CheckSoundMem(g_GameOverSE) == 0) {
		if (GameOverFlg++ == 0) {
			PlaySoundMem(g_GameOverSE, DX_PLAYTYPE_BACK);
		}
		else {
			g_GameState = GAME_TITLE;	//����
			GameOverFlg = 0;
		}
	}
	DrawGraph(0, 0, g_GameOverImage, FALSE);

	//if (g_Keyflg & MOUSE_INPUT_RIGHT)g_GameState = GAME_TITLE;
	//SetFontSize(16);
	//DrawString(150, 450, "---- �E�N���b�N�Ń^�C�g���� ----", 0xffffff, 0);
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


	// ���x����`��
	int TmpLevel = g_Level;
	int PosX = 600;
	do {
		DrawRotaGraph(PosX, 80, 0.5f, 0, g_NumberImage[TmpLevel % 10], TRUE);
		TmpLevel /= 10;
		PosX -= 30;
	} while (TmpLevel > 0);


	// �X�R�A��`��
	int TmpScore = g_Score;
	PosX = 620;
	do {
		DrawRotaGraph(PosX, 160, 0.3f, 0, g_NumberImage[TmpScore % 10], TRUE);
		TmpScore /= 10;
		PosX -= 20;
	} while (TmpScore > 0);

	// �~�b�V������`��
	SetFontSize(20);
	DrawFormatString(590, 211, 0xFFFFFF, "%3d", g_Mission);

	// �A�C�e���̎擾����`��
	for (int i = 0; i < ITEM_MAX; i++) {
		DrawRotaGraph(540, 245 + i * 30, 0.5f, 0, g_BlockImage[i + 1], TRUE, 0);
		DrawFormatString(580, 235 + i * 30, 0xFFFFFF, "%3d", g_Item[i]);
	}


	// �u���b�N��`��
	for (int i = 0; i < HEIGHT; i++) {
		for (int j = 0; j < WIDTH; j++) {
			if (g_Block[i][j].flg != 0 && g_Block[i][j].image != 0) {
				DrawGraph(g_Block[i][j].x, g_Block[i][j].y, g_BlockImage[g_Block[i][j].image], TRUE);
			}
		}
	}


	// �^�C���o�[��`��
	DrawBox(491, 469, 509, 469 - g_Time / 60 * 2, 0x0033ff, TRUE);
}


void StageInit(void) {

	int Result = 0;

	// ��̃u���b�N�𐶐����鏈��
	for (int i = 1; i < HEIGHT - 1; i++) {
		for (int j = 1; j < WIDTH - 1; j++) {
			if (g_Block[i][j].image == 0) {
				g_Block[i][j].image = GetRand(7) + 1;
			}
		}
	}


	// �u���b�N�A���`�F�b�N
	for (int i = 1; i < HEIGHT - 1; i++) {
		for (int j = 1; j < WIDTH - 1; j++) {
			Result += RensaCheck(i, j);
		}
	}

	if (Result == 0) {						// P.11 �H ���̉\����I
		g_GameState = GAME_MAIN;
		}
	g_Level += 1;
	g_Mission += 1;
	g_Time = TIMELIMIT;

	//�J�E���g�z��̏�����
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
		RestoreBlock();		// �u���b�N�̘A����3�����Ȃ�u���b�N��߂�
	}


	int CountW = 0;
	int ColorW = 0;
	SaveBlock();
	RensaCheckW(y, x, &CountW, &ColorW);
	if (CountW < 3) {
		RestoreBlock();		// �u���b�N�̘A����3�����Ȃ�u���b�N��߂�
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
	* �c�̗אڂ��Ă��铯�F�̌��𒲂ׂ�(�T����ɏ���)
	* �߂�l�F�c�̗אڂ��Ă��铯�F�̌�
	**************************************************/

	// �Ώۃu���b�N���O�g�̏ꍇ return 0; �ŏ�����U����
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
	* ���̗אڂ��Ă��铯�F�̌��𒲂ׂ�(�T����ɏ���)
	* �߂�l�F���̗אڂ��Ă��铯�F�̌�
	**************************************************/

	// �Ώۃu���b�N���O�g�̏ꍇ return 0; �ŏ�����U����
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

	// �I���u���b�N�͈̔͂𐧌�
	if (SelectX < 0)SelectX = 0;
	if (SelectX > WIDTH - 3)SelectX = WIDTH - 3;
	if (SelectY < 0)SelectY = 0;
	if (SelectY > HEIGHT - 3)SelectY = HEIGHT - 3;

	// �N���b�N�Ńu���b�N��I��
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

	// �I���u���b�N��`��
	DrawGraph(SelectX * BLOCKSIZE, SelectY * BLOCKSIZE, g_BlockImage[9], TRUE);
	if (ClickFlg >= 1) {
		DrawGraph(g_SelectX1 * BLOCKSIZE, g_SelectY1 * BLOCKSIZE, g_BlockImage[9], TRUE);
	}
	// �I���u���b�N������
	if (ClickFlg == 2) {
		int TmpBlock = g_Block[g_SelectY1 + 1][g_SelectX1 + 1].image;
		g_Block[g_SelectY1 + 1][g_SelectX1 + 1].image = g_Block[g_SelectY2 + 1][g_SelectX2 + 1].image;

		g_Block[g_SelectY2 + 1][g_SelectX2 + 1].image = TmpBlock;


		// �A����3�ȏォ���ׂ�
		int Result = 0;
		Result += RensaCheck(g_SelectY1 + 1, g_SelectX1 + 1);
		Result += RensaCheck(g_SelectY2 + 1, g_SelectX2 + 1);

		// �A����3�����Ȃ�I���u���b�N�����ɖ߂�
		if (Result == 0) {
			TmpBlock = g_Block[g_SelectY1 + 1][g_SelectX1 + 1].image;
			g_Block[g_SelectY1 + 1][g_SelectX1 + 1].image = g_Block[g_SelectY2 + 1][g_SelectX2 + 1].image;
			g_Block[g_SelectY2 + 1][g_SelectX2 + 1].image = TmpBlock;
		}
		else {
			// �A����3�ȏ�Ȃ�u���b�N�������A�u���b�N�ړ������ֈڍs����
			g_Status = 1;
		}


		// ���ɃN���b�N���ł���悤�ɁAClickFlg ��0�ɂ���B
		ClickFlg = 0;
	}
}


void MoveBlock(void) {

	PlaySoundMem(g_MoveBlockSE, DX_PLAYTYPE_BACK);

	// ���ֈړ����鏈��
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

	// ��̃u���b�N�𐶐����鏈��
	for (int i = 1; i < HEIGHT - 1; i++) {
		for (int j = 1; j < WIDTH - 1; j++) {
			if (g_Block[i][j].image == 0) {
				g_Block[i][j].image = GetRand(7) + 1;
			}
		}
	}


	// �u���b�N�I�������ֈڍs����
	//g_Status = 0;

	// �u���b�N�A���`�F�b�N�����ֈڍs����
	g_Status = 3;
}


void CheckBlock(void) {

	int Result = 0;

	for (int i = 1; i < HEIGHT - 1; i++) {
		for (int j = 1; j < WIDTH - 1; j++) {
			Result += RensaCheck(i, j);
		}
	}

	// �A�����Ȃ��Ȃ�΁A�u���b�N�I���ֈڍs����
	// �����łȂ���΁A�u���b�N�ړ��ֈڍs���ĘA���`�F�b�N���p������
	if (Result == 0) {
		// �u���b�N�I�������ֈڍs����
		//g_Status = 0;

		// �N���A�����̃`�F�b�N�����ֈڍs����
		g_Status = 4;
	}
	else {
		// �A����3�ȏ�Ȃ�u���b�N�������A�u���b�N�ڍs�����ֈڍs����
		g_Status = 1;
	}
}

void CheckClear(void) {

	// �N���A�����̃`�F�b�N
	// �N���A�����t���O�� 0 �Ƃ��A�e�X�N�[���̏����u���b�N��
	// ���x�������u���b�N�������Ȃ�������A�`�F�b�N�����𒆒f���ăQ�[���𑱍s����

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
	// �`��u�����h���[�h���A���t�@�u�����h�ɂ���
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, BlendMode);
	for (int i = 1; i < HEIGHT - 1; i++) {
		for (int j = 1; j < WIDTH - 1; j++) {
			if (g_Block[i][j].image == 0) {
				DrawGraph(g_Block[i][j].x, g_Block[i][j].y, g_BlockImage[g_Block[i][j].backup], TRUE);
			}
		}
	}

	// �`��u�����h���[�h���m�[�u�����h�ɂ���
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