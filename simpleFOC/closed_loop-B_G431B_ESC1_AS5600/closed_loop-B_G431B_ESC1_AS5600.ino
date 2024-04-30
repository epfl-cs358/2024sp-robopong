#include <Arduino.h>
#include <SimpleFOC.h>

//BLDCMotor motor = BLDCMotor(7, 0.033); // br3536 kv950
BLDCMotor motor = BLDCMotor(7, 0.025); // br3536 kv1200
BLDCDriver6PWM driver = BLDCDriver6PWM(A_PHASE_UH, A_PHASE_UL, A_PHASE_VH, A_PHASE_VL, A_PHASE_WH, A_PHASE_WL);
LowsideCurrentSense currentSense = LowsideCurrentSense(0.003f, -64.0f/7.0f, A_OP1_OUT, A_OP2_OUT, A_OP3_OUT);
MagneticSensorI2C sensor = MagneticSensorI2C(AS5600_I2C);

Commander command = Commander(Serial);
void doTarget(char* cmd) { command.motion(&motor, cmd); }

void setup() {

  // initialise magnetic sensor hardware
  Wire.setClock(400000);
  Wire.begin();
  sensor.init(&Wire);

  motor.linkSensor(&sensor);
  
  driver.voltage_power_supply = 12;
  driver.init();
  motor.linkDriver(&driver);
  currentSense.linkDriver(&driver);

  currentSense.init();
  currentSense.skip_align = true;
  motor.linkCurrentSense(&currentSense);

  motor.controller = MotionControlType::angle;

// motor.voltage_limit = 12;
  motor.current_limit = 35;
  motor.velocity_limit = 1000;

  motor.PID_velocity.P = 0.2;
  motor.PID_velocity.I = 20;
  //motor.PID_velocity.D = 0;
  motor.PID_velocity.output_ramp = 1000;
  motor.LPF_velocity.Tf = 0.01;
  motor.P_angle.P = 20;

  motor.target = 0;

  Serial.begin(115200);
  motor.useMonitoring(Serial);

  motor.init();
  motor.initFOC();

  command.add('T', doTarget, "target angle");

  Serial.println(F("Motor ready."));
  Serial.println(F("Set the target angle using serial terminal:"));
  _delay(1000);
}

void loop() {
  motor.loopFOC();
  motor.move();
  command.run();
}
