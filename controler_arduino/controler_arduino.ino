#define button_front 5
#define button_back 2
#define button_left 4
#define button_right 3

#define button_free 6
#define button_stop 7
#define emergency_button 9

int front_state = 0;
int back_state = 0;
int left_state = 0;
int right_state = 0;
int free_state = 0;
int emerg_state = 0;

int state = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(button_front, INPUT_PULLUP);
  pinMode(button_back, INPUT_PULLUP);
  pinMode(button_left, INPUT_PULLUP);
  pinMode(button_right, INPUT_PULLUP);

  pinMode(button_free, INPUT_PULLUP);
  pinMode(button_stop, INPUT_PULLUP);

  pinMode(emergency_button, INPUT_PULLUP);

  Serial.begin(9600);


}


void loop() {
  // put your main code here, to run repeatedly:

  //非常停止ボタンのみ先に読み取り検知したら即座に指令を送る
  if (digitalRead(emergency_button) == HIGH) {
    Serial.write(99);
  }

  //非常停止ボタンが押されていなければコントローラの指令を読む
  else {

    //各ボタンのステータスを取得する
    if (digitalRead(button_front) == LOW) {
      front_state = 1;
    }
    else {
      front_state = 0;
    }

    if (digitalRead(button_back) == LOW) {
      back_state = 1;
    }
    else {
      back_state = 0;
    }

    if (digitalRead(button_left) == LOW) {
      left_state = 1;
    }
    else {
      left_state = 0;
    }

    if (digitalRead(button_right) == LOW) {
      right_state = 1;
    }
    else {
      right_state = 0;
    }

    if (digitalRead(button_free) == LOW) {
      free_state = 1;
    }
    else {
      free_state = 0;
    }

    if (digitalRead(button_stop) == LOW) {
      emerg_state = 1;
    }
    else {
      emerg_state = 0;
    }

    

    //2進数から10進数に変換
    state = 1 * front_state + 2 * back_state + 4 * left_state + 8 * right_state + 16 * free_state + 32 * emerg_state;

    Serial.write(state);
  }
  delay(50);

}
