// HORIOKART_Controller.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"

#include <math.h>
#include <windows.h>
#include <stdlib.h>

#include <ypspur.h>




#define COMPORT "\\\\.\\COM8"

bool isInitialized = false;



/*//////////////////////////////////////////////////////////
initSpur:YPspurの初期化
引数：void
返り値
　0：正常
  1：エラー
*///////////////////////////////////////////////////////////
int initSpur(void){
	// Windows環境で標準出力がバッファリングされないように設定
	setvbuf(stdout, 0, _IONBF, 0);

	// 初期化
	if (Spur_init() < 0)
	{
		fprintf(stderr, "ERROR : cannot open spur.\n");
		return -1;
	}

	return 0;
}

int getArduinoHandle(HANDLE& hComm){
	//シリアルポートを開いてハンドルを取得

	hComm = CreateFile(_T(COMPORT), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hComm == INVALID_HANDLE_VALUE){
		printf("シリアルポートを開くことができませんでした。");
		char z;
		z = getchar();
		return -1;
	}
	//ポートを開けていれば通信設定を行う
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
	
	// ハンドルチェック
	if (!hComm)	return -1;
	// バッファクリア
	memset(sendbuf, 0x00, sizeof(sendbuf));
	// パケット作成
	sendbuf[0] = (unsigned char)state;
	// 通信バッファクリア
	PurgeComm(hComm, PURGE_RXCLEAR);
	// 送信
	ret = WriteFile(hComm, &sendbuf, 1, &len, NULL);
}




int main(int argc, _TCHAR* argv[])
{
	HANDLE hCom;		//ArduinoのCOMポートのハンドル
	char arduino_state[1];
	unsigned long len;
	int ret;

	initSpur();			//ypspurの初期化

	getArduinoHandle(hCom);		//Arduinoのハンドルを取得する

	//ループ内でArduinoと通信を行いボタンのステータスを取得しモータードライバーに指令を送る

	while (1){

		//arduinoからステータスを取得
		// ハンドルチェック
		if (!hCom)	return -1;
		// 通信バッファクリア
		PurgeComm(hCom, PURGE_RXCLEAR);
		// Arduinoからデータを受信
		ret = ReadFile(hCom, &arduino_state, 1, &len, NULL);
	}


	return 0;
}

