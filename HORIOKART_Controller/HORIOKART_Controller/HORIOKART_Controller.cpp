// HORIOKART_Controller.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"

#include <math.h>
#include <windows.h>
#include <stdlib.h>

#include <ypspur.h>


#define PI 3.14159265359


#define TIRE_R 0.2900   //タイヤ径[m]

#define MAX_VEL 3.9		//最高速度設定　[km/h]
#define MAX_ACC 2.0		//加速度の設定  [km/h.s]


#define COMPORT "\\\\.\\COM11"

bool isInitialized = false;

double vel = 3.0;			//速度の指定[km/h]



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


//引数1：10進数のステータス　2：各ボタンのステータスのポインター
//button_state  0:front 1:back 2:left 3:rigtht 4:free 5:emergency

void read_states(int arduino_state, int *button_state){

	//10進数で受信したステータスを2進数に変換しボタンのステータスを受信する
	int i = 0;
	int remainder, inp;

	inp = arduino_state;

	for (i = 0; i < 6;i++){
		
		remainder = inp % 2;	//余りを求める
		button_state[i] = remainder;		//ボタンのステータスを取得
		inp = (inp - remainder) / 2;		//次の結果にまわす割り算
		printf("%d\n", remainder);
	}
	
}




int main(int argc, _TCHAR* argv[])
{
	HANDLE hCom;		//ArduinoのCOMポートのハンドル
	unsigned char arduino_state[1];
	unsigned long len;
	int ret;
	int state;
	int button_state[6];		
	//0:front 1:back 2:left 3:rigtht 4:free 5:emergency

	int emergency_state = 0;
	double ang_vel;

	initSpur();			//ypspurの初期化

	getArduinoHandle(hCom);		//Arduinoのハンドルを取得する

	YP_set_wheel_vel((MAX_VEL / 3600) / TIRE_R, (MAX_VEL / 3600) / TIRE_R);

	ang_vel = (vel / 3600) / TIRE_R;

	//ループ内でArduinoと通信を行いボタンのステータスを取得しモータードライバーに指令を送る
	
	while (1){

		//arduinoからステータスを取得
		// ハンドルチェック
		if (!hCom)	{
			printf("error!!\n");
			return -1;
		}


		// 通信バッファクリア
		PurgeComm(hCom, PURGE_RXCLEAR);
		// Arduinoからデータを受信
		ret = ReadFile(hCom, &arduino_state, 1, &len, NULL);

		printf("len :%d\n",len);

		//printf("%c%c\n", arduino_state[0], arduino_state[1]);
		printf("%d\n", (int)arduino_state[0]);
		state = (int)arduino_state[0];
		//printf("state\n");

		//非常停止ボタンが押されたときの動作
		//プログラムによる非常停止であり，補助的役割
		//一度押されたら5秒間他の動作を受け付けない，
		//押された後フリーズ状態となり，もう一度押されるまで反応しない
		if (button_state[5] == 1){
			printf("emergency");
			if (emergency_state){
				//非常停止状態で押されたときは停止指令を送った上で復帰する
				Spur_stop();
				Spur_unfreeze();
				emergency_state = 0;
				Sleep(1000);
				//解除後はボタン誤操作防止のため1秒間停止する
			}

			Spur_freeze();
			emergency_state = 1;
			Sleep(5000);

			
		}

		//車輪ロックの解除ボタンの動作
		if (button_state[4] == 1){
			Spur_free();
		}


		//条件分岐によりボタンに対する動作を割り振る
		//同時押しを先に分岐し定義，後に別で押したときの動作を定義
		//どの条件にも当てはまらない(else)のときはstopさせる

		//front/left
		//左方向に内側と外側の車輪の速度を1：2に
		if ((button_state[0] == 1) && (button_state[2] == 1)){
			YP_wheel_vel(-(MAX_VEL / 3600) / (2*TIRE_R), (MAX_VEL / 3600) / TIRE_R);

		}
		//front/right
		else if((button_state[0] == 1) && (button_state[3] == 1)){
			YP_wheel_vel(-(MAX_VEL / 3600) / (TIRE_R), (MAX_VEL / 3600) / (2*TIRE_R));
		}

		//back/left
		else if ((button_state[1] == 1) && (button_state[2] == 1)){
			YP_wheel_vel((MAX_VEL / 3600) / (2 * TIRE_R), -(MAX_VEL / 3600) / TIRE_R);
		}


		//back/right
		else if ((button_state[1] == 1) && (button_state[3] == 1)){
			YP_wheel_vel(-(MAX_VEL / 3600) / (TIRE_R), (MAX_VEL / 3600) / (2 * TIRE_R));
		}

		//front
		else if (button_state[0] == 1){

			YP_wheel_vel(-(MAX_VEL / 3600) / TIRE_R, (MAX_VEL / 3600) / TIRE_R);

		}

		//back
		else if (button_state[1] == 1){
			YP_wheel_vel((MAX_VEL / 3600) / TIRE_R, -(MAX_VEL / 3600) / TIRE_R);
		}

		//left
		//左に曲がるように両輪を逆回転
		else if (button_state[2] == 1){
			YP_wheel_vel((MAX_VEL / 3600) / (2*TIRE_R), -(MAX_VEL / 3600) / (2*TIRE_R));
		}
		//right
		else if (button_state[3] == 1){
			YP_wheel_vel(+(MAX_VEL / 3600) / (2 * TIRE_R), (MAX_VEL / 3600) / (2 * TIRE_R));
		}

		else{
			Spur_stop();
		}


		
		Sleep(500);
	}


	return 0;
}

