// HORIOKART_Controller.cpp : �R���\�[�� �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
//

#include "stdafx.h"

#include <math.h>
#include <windows.h>
#include <stdlib.h>

#include <ypspur.h>




#define COMPORT "\\\\.\\COM8"

bool isInitialized = false;



/*//////////////////////////////////////////////////////////
initSpur:YPspur�̏�����
�����Fvoid
�Ԃ�l
�@0�F����
  1�F�G���[
*///////////////////////////////////////////////////////////
int initSpur(void){
	// Windows���ŕW���o�͂��o�b�t�@�����O����Ȃ��悤�ɐݒ�
	setvbuf(stdout, 0, _IONBF, 0);

	// ������
	if (Spur_init() < 0)
	{
		fprintf(stderr, "ERROR : cannot open spur.\n");
		return -1;
	}

	return 0;
}

int getArduinoHandle(HANDLE& hComm){
	//�V���A���|�[�g���J���ăn���h�����擾

	hComm = CreateFile(_T(COMPORT), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hComm == INVALID_HANDLE_VALUE){
		printf("�V���A���|�[�g���J�����Ƃ��ł��܂���ł����B");
		char z;
		z = getchar();
		return -1;
	}
	//�|�[�g���J���Ă���ΒʐM�ݒ���s��
	else
	{

		printf("port open\n");
		DCB lpTest;
		GetCommState(hComm, &lpTest);
		lpTest.BaudRate = 9600;
		lpTest.ByteSize = 8;
		lpTest.Parity = NOPARITY;
		lpTest.StopBits = ONESTOPBIT;
		SetCommState(hComm, &lpTest);
	}

	return 0;
}


int senddata(HANDLE hComm ,char *state){
	char sendbuf[1];
	int ret;
	unsigned long len;
	
	// �n���h���`�F�b�N
	if (!hComm)	return -1;
	// �o�b�t�@�N���A
	memset(sendbuf, 0x00, sizeof(sendbuf));
	// �p�P�b�g�쐬
	sendbuf[0] = (unsigned char)state;
	// �ʐM�o�b�t�@�N���A
	PurgeComm(hComm, PURGE_RXCLEAR);
	// ���M
	ret = WriteFile(hComm, &sendbuf, 1, &len, NULL);
}


//����1�F10�i���̃X�e�[�^�X�@2�F�e�{�^���̃X�e�[�^�X�̃|�C���^�[
//button_state  0:front 1:back 2:left 3:rigtht 4:free 5:emergency

void read_states(int arduino_state, int *button_state){

	//10�i���Ŏ�M�����X�e�[�^�X��2�i���ɕϊ����{�^���̃X�e�[�^�X����M����
	int i = 0;
	int remainder, inp;

	inp = arduino_state;

	for (i = 0; i < 6;i++){
		
		remainder = inp % 2;	//�]������߂�
		button_state[i] = remainder;		//�{�^���̃X�e�[�^�X���擾
		inp = (inp - remainder) / 2;		//���̌��ʂɂ܂킷����Z

	}
	
}




int main(int argc, _TCHAR* argv[])
{
	HANDLE hCom;		//Arduino��COM�|�[�g�̃n���h��
	char arduino_state[1];
	unsigned long len;
	int ret;
	int state;
	int button_state[6];		
	//0:front 1:back 2:left 3:rigtht 4:free 5:emergency

	int emergency_state = 0;


	initSpur();			//ypspur�̏�����

	getArduinoHandle(hCom);		//Arduino�̃n���h�����擾����

	//���[�v����Arduino�ƒʐM���s���{�^���̃X�e�[�^�X���擾�����[�^�[�h���C�o�[�Ɏw�߂𑗂�

	while (1){

		//arduino����X�e�[�^�X���擾
		// �n���h���`�F�b�N
		if (!hCom)	return -1;
		// �ʐM�o�b�t�@�N���A
		PurgeComm(hCom, PURGE_RXCLEAR);
		// Arduino����f�[�^����M
		ret = ReadFile(hCom, &arduino_state, 1, &len, NULL);

		state = (int)arduino_state;


		//����~�{�^���������ꂽ�Ƃ��̓���
		//�v���O�����ɂ�����~�ł���C�⏕�I����
		//��x�����ꂽ��5�b�ԑ��̓�����󂯕t���Ȃ��C
		//�����ꂽ��t���[�Y��ԂƂȂ�C������x�������܂Ŕ������Ȃ�
		if (button_state[5] == 1){
			if (emergency_state){
				//����~��Ԃŉ����ꂽ�Ƃ��͒�~�w�߂𑗂�����ŕ��A����
				Spur_stop();
				Spur_unfreeze();

				Sleep(1000);
				//������̓{�^���둀��h�~�̂���1�b�Ԓ�~����
			}

			Spur_freeze();
			Sleep(5000);
			
		}

		//�ԗփ��b�N�̉����{�^���̓���
		if (button_state[4] == 1){
			Spur_free();
		}


		//��������ɂ��{�^���ɑ΂��铮�������U��
		//�����������ɕ��򂵒�`�C��ɕʂŉ������Ƃ��̓�����`
		//�ǂ̏����ɂ����Ă͂܂�Ȃ�(else)�̂Ƃ���stop������

		//front/left
		//�������ɓ����ƊO���̎ԗւ̑��x��1�F2��

		//front/right


		//back/left



		//back/right


		//front

		//back


		//left
		//���ɋȂ���悤�ɗ��ւ��t��]

		//right









	}


	return 0;
}

