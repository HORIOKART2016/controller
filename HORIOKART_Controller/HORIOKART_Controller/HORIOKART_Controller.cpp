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




int main(int argc, _TCHAR* argv[])
{
	HANDLE hCom;		//Arduino��COM�|�[�g�̃n���h��
	char arduino_state[1];
	unsigned long len;
	int ret;

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
	}


	return 0;
}

