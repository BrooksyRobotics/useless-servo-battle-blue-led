/*
  ═══════════════════════════════════════════════════════════════════════════════
  USELESS BOX — BATTLE OF THE SERVOS
  ═══════════════════════════════════════════════════════════════════════════════
  Hardware:
    Servo A   → D9   — presses the OFF button (right servo)
    Servo B   → D10  — presses the ON  button (left servo)
    LED       → D6   — **100 ohm resistor
*/

#include <Servo.h>

// ── PINS ─────────────────────────────────────────────────────────────────────
const int SERVO_A_PIN   = 9;
const int SERVO_B_PIN   = 10;
const int LED_PIN       = 6;

// Angles for the regular positions used throughout code 
const int A_HOME  = 90;   // Center resting / staredown
const int A_PRESS = 150;  // ◄ Presses OFF button — calibrate first
const int A_LED   = 70;   // Looking at LED
const int A_READY = 130;

const int B_HOME  = 95;   // Center resting / staredown
const int B_PRESS = 40;   // ◄ Presses ON button — calibrate first
const int B_LED   = 115;  // Looking at LED
const int B_DROOP = 75;   // Dejected — arm hanging away from button
const int B_READY = 60;

// LED brightness levels
const int LED_OFF    = 0;
const int LED_DIM    = 80;
const int LED_BRIGHT = 200;
const int LED_MAX    = 255;

// Globals
Servo servoA;
Servo servoB;
int   ledLevel = LED_OFF;

// Servo movement functions for consistent style and LED control

// Smooth sweep - stepDelay controls timing: 5=quick  15=normal  25=slower (any timing can be used)
void sweep(Servo &srv, int fromAngle, int toAngle, int stepDelay) {
  int step = (fromAngle < toAngle) ? 1 : -1;
  for (int pos = fromAngle; pos != toAngle + step; pos += step) {
    srv.write(pos);
    delay(stepDelay);
  }
}

// Instant move with optional settle time
void snap(Servo &srv, int angle, int settleMs = 150) {
  srv.write(angle);
  delay(settleMs);
}

// Irritation wiggle at a given position
void wiggle(Servo &srv, int centerAngle, int amount, int reps, int speed = 70) {
  for (int i = 0; i < reps; i++) {
    srv.write(centerAngle + amount);
    delay(speed);
    srv.write(centerAngle - amount);
    delay(speed);
  }
  srv.write(centerAngle);
  delay(80);
}

// LED control
void ledOn(int brightness = LED_DIM) {
  ledLevel = brightness;
  analogWrite(LED_PIN, ledLevel);
}
void ledOff() {
  ledLevel = LED_OFF;
  analogWrite(LED_PIN, LED_OFF);
}

// Quick press and return — ledAction: 1=on  -1=off  0=no change
void quickPress(Servo &srv, int pressAngle, int homeAngle,
                int holdMs, int returnMs, int ledAction = 0) {
  srv.write(pressAngle);
  delay(holdMs);
  if (ledAction ==  1) ledOn(LED_DIM);
  if (ledAction == -1) ledOff();
  srv.write(homeAngle);
  delay(returnMs);
}

// Animation Scenes for servo movements and LED control
void scene_1() {
  delay(600);

  sweep(servoB, B_HOME, B_LED, 15);
  delay(900);

  sweep(servoB, B_LED, B_PRESS, 10);
  delay(300);
  ledOn(LED_DIM);  
  sweep(servoB, B_PRESS, B_LED, 5);
  delay(500);
  wiggle(servoB, B_LED, 5, 3, 45);
}

void scene_2() {
  snap(servoA, A_HOME, A_LED);
  delay(200);

  sweep(servoA, A_LED, A_HOME, 10);
  delay(200);

  sweep(servoA, A_HOME, A_LED, 10);
  delay(300);

  snap(servoA, A_LED, A_PRESS);
  ledOff();
  sweep(servoA, A_PRESS, A_HOME, 20);
  delay(300);

  sweep(servoB, B_LED, B_HOME, 10);
  delay(300);

  wiggle(servoB, B_HOME, 5, 3, 20);

  snap(servoB, B_PRESS, 80);
  ledOn(LED_DIM);
  snap(servoB, B_HOME, 100);

  sweep(servoB, B_HOME, B_LED, 14);
  delay(500);
}

void scene_3() {
  for (int r = 3; r >= 0; r--) {
    quickPress(servoA, A_PRESS, A_HOME, 30 * r +100, 30 * r + 150, -1);
    delay(60 * r);
    quickPress(servoB, B_PRESS, B_HOME, 30 * r +100, 30 * r + 150,  1);
    delay(60 * r);
  }
  quickPress(servoA, A_PRESS, A_HOME, 200, 200, -1);

  wiggle(servoB, B_HOME, 12, 3, 65);
  for (int r = 0; r < 4; r++) {
    quickPress(servoB, B_PRESS, B_HOME, 100, 140,  1);
    quickPress(servoA, A_PRESS, A_HOME, 100, 140, -1);
  }

  wiggle(servoA, A_HOME, 14, 3, 55);
  wiggle(servoB, B_HOME, 14, 3, 55);
  for (int r = 0; r < 5; r++) {
    quickPress(servoB, B_PRESS, B_READY, 60, 80,  1);
    quickPress(servoA, A_PRESS, A_READY, 60, 80, -1);
  }

  for (int e = 0; e < 6; e++) {
    servoA.write(e % 2 == 0 ? A_PRESS : A_READY);
    servoB.write(e % 2 == 0 ? B_HOME  : B_READY);
    if (e % 2 == 0) ledOff(); else ledOn(LED_DIM);
    delay(55);
  }

  servoA.write(A_HOME);
  servoB.write(B_HOME);
  ledOff();

  delay(1500);
}

void scene_4() {
  snap(servoB, 150, 100);
  wiggle(servoB, 150, 3, 3);
  delay(200);
  snap(servoA, 30, 100);
  delay(200);  
  sweep(servoB, B_HOME, B_PRESS, 2);
  ledOn(LED_DIM);
  delay(300);

  sweep(servoA, A_HOME, A_LED, 2);
  delay(100);
  sweep(servoA, A_LED, A_HOME, 20);
  delay(200);

  sweep(servoA, A_HOME, A_PRESS, 9);
  wiggle(servoA, A_PRESS - 8, 8, 3, 60);
  sweep(servoA, A_PRESS - 15, A_HOME, 10);
  delay(200);

  sweep(servoA, A_HOME, A_PRESS, 7);
  wiggle(servoA, A_PRESS - 5, 5, 4, 45);
  snap(servoA, A_HOME, 20);
  snap(servoA, A_PRESS, 20);

  for (int b = LED_DIM; b <= LED_BRIGHT; b++) {
    analogWrite(LED_PIN, b);
    delay(5);
  }
  
  for (int f = 0; f < 7; f++) {
    snap(servoA, A_PRESS - 10, 55);
    analogWrite(LED_PIN, LED_BRIGHT);
    delay(55);
    snap(servoA, A_PRESS, 55);
    analogWrite(LED_PIN, 80);
    delay(45);
  }
  analogWrite(LED_PIN, LED_BRIGHT);
  sweep(servoA, A_PRESS, 40, 10);
  snap(servoA, A_PRESS, 20);

  analogWrite(LED_PIN, LED_MAX);
  delay(120);
  analogWrite(LED_PIN, LED_OFF);
  ledLevel = LED_OFF;

  delay(500);
}

void scene_5() {
  delay(400);
  sweep(servoA, A_PRESS,  A_LED, 20);  
  sweep(servoB, B_PRESS, B_LED, 8);

  delay(500);

  sweep(servoA, A_LED, A_HOME, 12);
  delay(100);
  sweep(servoB, B_LED, B_HOME, 12);
  delay(100);

  sweep(servoA, A_HOME, A_LED, 5);
  sweep(servoB, B_HOME, B_LED, 5);
  delay(200);

  sweep(servoB, B_LED, B_PRESS + 15, 5);
  delay(300);
  snap(servoB, B_PRESS);

  snap(servoB, B_PRESS + 15 );
  delay(500);
  snap(servoB, B_PRESS );
  delay(300);

  snap(servoB, B_LED, 200);
  delay(400);
  sweep(servoB, B_LED, B_DROOP, 30);

  sweep(servoA, A_LED, A_HOME, 8);
  delay(900);
  servoA.write(A_HOME);
}

void setup() {
  servoA.attach(SERVO_A_PIN);
  servoB.attach(SERVO_B_PIN);
  pinMode(LED_PIN, OUTPUT);

  analogWrite(LED_PIN, LED_OFF);
  servoA.write(A_HOME);
  servoB.write(B_HOME);

  Serial.begin(9600);
  Serial.println("Battle of the Servos — ready.");
  delay(1200);

  // Functions that control the different scenes
  scene_1();
  scene_2();
  scene_3();
  scene_4();
  scene_5();  
}

void loop() {

}
