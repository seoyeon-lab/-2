// Arduino pin assignment
#define PIN_LED  9
#define PIN_TRIG 12   // sonar sensor TRIGGER
#define PIN_ECHO 13   // sonar sensor ECHO

// configurable parameters
#define SND_VEL 346.0     // sound velocity at 24 celsius degree (unit: m/sec)
#define INTERVAL 25       // sampling interval (unit: msec)
#define PULSE_DURATION 10 // ultra-sound Pulse Duration (unit: usec)
#define _DIST_MIN 100.0   // minimum distance to be measured (unit: mm)
#define _DIST_MAX 300.0   // maximum distance to be measured (unit: mm)

#define TIMEOUT ((INTERVAL / 2) * 1000.0) // maximum echo waiting time (unit: usec)
#define SCALE (0.001 * 0.5 * SND_VEL)     // coefficent to convert duration to distance

unsigned long last_sampling_time;   // unit: msec

void setup() {
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_TRIG, OUTPUT); 
  pinMode(PIN_ECHO, INPUT);  
  digitalWrite(PIN_TRIG, LOW);  
  Serial.begin(57600);
}

void loop() { 
  if (millis() < (last_sampling_time + INTERVAL))
    return;

  float distance = USS_measure(PIN_TRIG, PIN_ECHO);

  // 범위를 100~300mm로 제한
  if (distance < _DIST_MIN) distance = _DIST_MIN;
  if (distance > _DIST_MAX) distance = _DIST_MAX;

  // 거리 → LED duty 변환 (삼각형 형태)
  int duty = 255; // 기본 꺼짐
  if (distance <= 200) {
    // 100mm -> 255, 200mm -> 0
    duty = map(distance, 100, 200, 255, 0);
  } else {
    // 200mm -> 0, 300mm -> 255
    duty = map(distance, 200, 300, 0, 255);
  }

  analogWrite(PIN_LED, duty);

  // 디버깅용 출력
  Serial.print("Distance: "); Serial.print(distance);
  Serial.print(" mm, Duty: "); Serial.println(duty);

  last_sampling_time += INTERVAL;
}

// 초음파 측정 함수
float USS_measure(int TRIG, int ECHO) {
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(PULSE_DURATION);
  digitalWrite(TRIG, LOW);
  return pulseIn(ECHO, HIGH, TIMEOUT) * SCALE; // unit: mm
}
