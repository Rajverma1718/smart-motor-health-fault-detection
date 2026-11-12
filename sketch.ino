#include <Arduino.h>

// Smart Motor Health & Fault Detection System
// STM32 ADC-based embedded monitoring demonstration.

constexpr uint8_t TEMP_PIN = A0;
constexpr uint8_t VIBRATION_PIN = A1;
constexpr uint8_t CURRENT_PIN = A2;
constexpr uint8_t RPM_PIN = A3;
constexpr uint8_t NORMAL_LED = D2;
constexpr uint8_t WARNING_LED = D3;
constexpr uint8_t FAULT_LED = D4;
constexpr uint8_t BUZZER_PIN = D5;

struct MotorData {
  float temperatureC;
  float vibrationG;
  float currentA;
  uint16_t rpm;
  float health;
  const char* state;
  const char* fault;
};

MotorData motor{};
uint32_t lastSample = 0;
uint32_t sampleCount = 0;

float readScaled(uint8_t pin, float maximum) {
  return analogRead(pin) * maximum / 4095.0f;
}

void calculateMotorHealth() {
  float temperatureRisk = constrain((motor.temperatureC - 70.0f) / 35.0f, 0.0f, 1.0f);
  float vibrationRisk = constrain((motor.vibrationG - 0.25f) / 1.0f, 0.0f, 1.0f);
  float currentRisk = constrain((motor.currentA - 8.0f) / 7.0f, 0.0f, 1.0f);
  float rpmRisk = motor.rpm < 900 ? 0.35f : 0.0f;
  float risk = 0.35f * temperatureRisk + 0.30f * vibrationRisk +
               0.20f * currentRisk + 0.15f * rpmRisk;
  motor.health = 100.0f * (1.0f - risk);

  if (motor.temperatureC >= 95.0f) motor.fault = "OVER_TEMPERATURE";
  else if (motor.vibrationG >= 1.0f) motor.fault = "EXCESS_VIBRATION";
  else if (motor.currentA >= 13.0f) motor.fault = "OVER_CURRENT";
  else if (motor.rpm < 700) motor.fault = "STALL_OR_LOW_SPEED";
  else motor.fault = "NONE";

  if (strcmp(motor.fault, "NONE") != 0) motor.state = "FAULT";
  else if (motor.health < 70.0f) motor.state = "WARNING";
  else motor.state = "NORMAL";
}

void updateIndicators() {
  digitalWrite(NORMAL_LED, strcmp(motor.state, "NORMAL") == 0);
  digitalWrite(WARNING_LED, strcmp(motor.state, "WARNING") == 0);
  digitalWrite(FAULT_LED, strcmp(motor.state, "FAULT") == 0);
  digitalWrite(BUZZER_PIN, strcmp(motor.state, "FAULT") == 0 && ((millis() / 250) % 2));
}

void readMotorSensors() {
  // Potentiometers represent conditioned industrial sensor outputs.
  motor.temperatureC = readScaled(TEMP_PIN, 130.0f);
  motor.vibrationG = readScaled(VIBRATION_PIN, 1.5f);
  motor.currentA = readScaled(CURRENT_PIN, 15.0f);
  motor.rpm = static_cast<uint16_t>(readScaled(RPM_PIN, 3600.0f));
  calculateMotorHealth();
  updateIndicators();
}

void printTelemetry() {
  Serial.printf("MOTOR,t=%lu,temp=%.1fC,vibration=%.2fg,current=%.2fA,rpm=%u,health=%.1f%%,state=%s,fault=%s,samples=%lu\n",
                millis(), motor.temperatureC, motor.vibrationG, motor.currentA,
                motor.rpm, motor.health, motor.state, motor.fault, sampleCount);
}

void setup() {
  Serial.begin(115200);
  pinMode(NORMAL_LED, OUTPUT);
  pinMode(WARNING_LED, OUTPUT);
  pinMode(FAULT_LED, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  analogReadResolution(12);
  Serial.println("SMART MOTOR HEALTH & FAULT DETECTION SYSTEM");
  Serial.println("STM32 ADC monitoring node ready");
}

void loop() {
  if (millis() - lastSample >= 1000) {
    lastSample = millis();
    sampleCount++;
    readMotorSensors();
    printTelemetry();
  }
}
