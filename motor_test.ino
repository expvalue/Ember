/*
 * motor_test.ino — Full Diagnostic + 8-Direction Mecanum Test
 * Arduino Mega  •  9600 baud  •  Osoyoo Mecanum Chassis
 *
 * PHASE 1: Each wheel individually (find dead motors)
 * PHASE 2: 8 directions (every 45°) + rotations
 * PHASE 3: Manual serial control
 *
 * PROP THE CAR ON A BOX before uploading!
 *
 * Serial commands (after auto-cycle):
 *     8 = Fwd      9 = Fwd-Right   7 = Fwd-Left
 *     4 = Left     5 = Stop        6 = Right
 *     1 = Bk-Left  2 = Backward    3 = Bk-Right
 *     Z = Rotate Left    X = Rotate Right    S = Stop
 */

#define FL_PWM  10
#define FL_DIR1 26
#define FL_DIR2 28

#define FR_PWM  9
#define FR_DIR1 22
#define FR_DIR2 24

#define RL_PWM  12
#define RL_DIR1 7
#define RL_DIR2 8

#define RR_PWM  11
#define RR_DIR1 5
#define RR_DIR2 6

#define TEST_SPEED  150
#define DIAG_SPEED  120
#define STEP_TIME   2000
#define PAUSE_TIME  800

// ── Per-wheel primitives ─────────────────────────────────────────────────────

void FL_fwd(int s) { digitalWrite(FL_DIR1, HIGH); digitalWrite(FL_DIR2, LOW);  analogWrite(FL_PWM, s); }
void FL_rev(int s) { digitalWrite(FL_DIR1, LOW);  digitalWrite(FL_DIR2, HIGH); analogWrite(FL_PWM, s); }
void FL_off()      { analogWrite(FL_PWM, 0); }

void FR_fwd(int s) { digitalWrite(FR_DIR1, HIGH); digitalWrite(FR_DIR2, LOW);  analogWrite(FR_PWM, s); }
void FR_rev(int s) { digitalWrite(FR_DIR1, LOW);  digitalWrite(FR_DIR2, HIGH); analogWrite(FR_PWM, s); }
void FR_off()      { analogWrite(FR_PWM, 0); }

void RL_fwd(int s) { digitalWrite(RL_DIR1, HIGH); digitalWrite(RL_DIR2, LOW);  analogWrite(RL_PWM, s); }
void RL_rev(int s) { digitalWrite(RL_DIR1, LOW);  digitalWrite(RL_DIR2, HIGH); analogWrite(RL_PWM, s); }
void RL_off()      { analogWrite(RL_PWM, 0); }

void RR_fwd(int s) { digitalWrite(RR_DIR1, HIGH); digitalWrite(RR_DIR2, LOW);  analogWrite(RR_PWM, s); }
void RR_rev(int s) { digitalWrite(RR_DIR1, LOW);  digitalWrite(RR_DIR2, HIGH); analogWrite(RR_PWM, s); }
void RR_off()      { analogWrite(RR_PWM, 0); }

void stop_all() {
  FL_off(); FR_off(); RL_off(); RR_off();
  Serial.println(F(">> STOP"));
}

// ── 8-direction movement ─────────────────────────────────────────────────────

void move_forward(int s)   { FL_fwd(s); FR_fwd(s); RL_fwd(s); RR_fwd(s); Serial.println(F(">> 0° FORWARD")); }
void move_fwd_right(int s) { FL_fwd(s); FR_off();  RL_off();  RR_fwd(s); Serial.println(F(">> 45° FWD-RIGHT")); }
void move_right(int s)     { FL_fwd(s); FR_rev(s); RL_rev(s); RR_fwd(s); Serial.println(F(">> 90° STRAFE RIGHT")); }
void move_bk_right(int s)  { FL_off();  FR_rev(s); RL_rev(s); RR_off();  Serial.println(F(">> 135° BK-RIGHT")); }
void move_backward(int s)  { FL_rev(s); FR_rev(s); RL_rev(s); RR_rev(s); Serial.println(F(">> 180° BACKWARD")); }
void move_bk_left(int s)   { FL_rev(s); FR_off();  RL_off();  RR_rev(s); Serial.println(F(">> 225° BK-LEFT")); }
void move_left(int s)      { FL_rev(s); FR_fwd(s); RL_fwd(s); RR_rev(s); Serial.println(F(">> 270° STRAFE LEFT")); }
void move_fwd_left(int s)  { FL_off();  FR_fwd(s); RL_fwd(s); RR_off();  Serial.println(F(">> 315° FWD-LEFT")); }
void rotate_left(int s)    { FL_rev(s); FR_fwd(s); RL_rev(s); RR_fwd(s); Serial.println(F(">> ROTATE LEFT")); }
void rotate_right(int s)   { FL_fwd(s); FR_rev(s); RL_fwd(s); RR_rev(s); Serial.println(F(">> ROTATE RIGHT")); }

// ── GPIO init ────────────────────────────────────────────────────────────────

void init_GPIO() {
  int pins[] = {
    FL_DIR1, FL_DIR2, FL_PWM,
    FR_DIR1, FR_DIR2, FR_PWM,
    RL_DIR1, RL_DIR2, RL_PWM,
    RR_DIR1, RR_DIR2, RR_PWM
  };
  for (int i = 0; i < 12; i++) pinMode(pins[i], OUTPUT);
  stop_all();
}

void test_step(const char* label, void (*action)(int), int spd) {
  Serial.println(label);
  action(spd);
  delay(STEP_TIME);
  stop_all();
  delay(PAUSE_TIME);
}

// ── Setup ────────────────────────────────────────────────────────────────────

void setup() {
  init_GPIO();
  Serial.begin(9600);

  Serial.println(F(""));
  Serial.println(F("========================================"));
  Serial.println(F("  MOTOR TEST — Osoyoo Mecanum / Mega"));
  Serial.println(F("========================================"));
  Serial.println(F(""));

  // PHASE 1: Individual wheels
  Serial.println(F("== PHASE 1: INDIVIDUAL WHEEL TEST =="));
  Serial.println(F("Starting in 3 sec..."));
  delay(3000);

  Serial.println(F("  [1/4] FRONT LEFT only..."));
  FL_fwd(DIAG_SPEED); delay(1500); FL_off(); delay(600);

  Serial.println(F("  [2/4] FRONT RIGHT only..."));
  FR_fwd(DIAG_SPEED); delay(1500); FR_off(); delay(600);

  Serial.println(F("  [3/4] REAR LEFT only..."));
  RL_fwd(DIAG_SPEED); delay(1500); RL_off(); delay(600);

  Serial.println(F("  [4/4] REAR RIGHT only..."));
  RR_fwd(DIAG_SPEED); delay(1500); RR_off(); delay(600);

  Serial.println(F(""));
  Serial.println(F("All 4 spin? If not, check wiring:"));
  Serial.println(F("  FL: PWM=D10, DIR=26/28"));
  Serial.println(F("  FR: PWM=D9,  DIR=22/24"));
  Serial.println(F("  RL: PWM=D12, DIR=7/8"));
  Serial.println(F("  RR: PWM=D11, DIR=5/6"));
  Serial.println(F(""));
  delay(3000);

  // PHASE 2: 8 directions
  Serial.println(F("== PHASE 2: 8-DIRECTION TEST =="));

  test_step("--- 0° FORWARD ---",        move_forward,    TEST_SPEED);
  test_step("--- 45° FWD-RIGHT ---",      move_fwd_right,  TEST_SPEED);
  test_step("--- 90° STRAFE RIGHT ---",   move_right,      TEST_SPEED);
  test_step("--- 135° BACK-RIGHT ---",    move_bk_right,   TEST_SPEED);
  test_step("--- 180° BACKWARD ---",      move_backward,   TEST_SPEED);
  test_step("--- 225° BACK-LEFT ---",     move_bk_left,    TEST_SPEED);
  test_step("--- 270° STRAFE LEFT ---",   move_left,       TEST_SPEED);
  test_step("--- 315° FWD-LEFT ---",      move_fwd_left,   TEST_SPEED);
  test_step("--- ROTATE LEFT ---",        rotate_left,     TEST_SPEED);
  test_step("--- ROTATE RIGHT ---",       rotate_right,    TEST_SPEED);

  Serial.println(F(""));
  Serial.println(F("========================================"));
  Serial.println(F("  AUTO TEST DONE — MANUAL MODE"));
  Serial.println(F("========================================"));
  Serial.println(F("  8=Fwd  9=FwdR  7=FwdL"));
  Serial.println(F("  4=Left 5=Stop  6=Right"));
  Serial.println(F("  1=BkL  2=Bk    3=BkR"));
  Serial.println(F("  Z=RotL X=RotR  S=Stop"));
  Serial.println(F(""));
}

// ── Loop: manual control ─────────────────────────────────────────────────────

void loop() {
  while (Serial.available()) {
    char c = Serial.read();
    switch (c) {
      case '8': move_forward(TEST_SPEED);    break;
      case '9': move_fwd_right(TEST_SPEED);  break;
      case '6': move_right(TEST_SPEED);      break;
      case '3': move_bk_right(TEST_SPEED);   break;
      case '2': move_backward(TEST_SPEED);   break;
      case '1': move_bk_left(TEST_SPEED);    break;
      case '4': move_left(TEST_SPEED);       break;
      case '7': move_fwd_left(TEST_SPEED);   break;
      case 'Z': rotate_left(TEST_SPEED);     break;
      case 'X': rotate_right(TEST_SPEED);    break;
      case '5':
      case 'S': stop_all();                  break;
    }
  }
}
