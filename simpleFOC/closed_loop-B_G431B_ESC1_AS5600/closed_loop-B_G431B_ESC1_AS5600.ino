#include <Arduino.h>
#include <SimpleFOC.h>

BLDCMotor motor = BLDCMotor(7, 0.025); // br3536 kv1200
BLDCDriver6PWM driver = BLDCDriver6PWM(A_PHASE_UH, A_PHASE_UL, A_PHASE_VH, A_PHASE_VL, A_PHASE_WH, A_PHASE_WL);
LowsideCurrentSense currentSense = LowsideCurrentSense(0.003f, -64.0f/7.0f, A_OP1_OUT, A_OP2_OUT, A_OP3_OUT);
MagneticSensorI2C sensor = MagneticSensorI2C(AS5600_I2C);

Commander commander = Commander(Serial);
void onMotion(char* cmd) { commander.motion(&motor, cmd); }

void setup() {
  Wire.setClock(400000);
  sensor.init();

  motor.linkSensor(&sensor);
  
  driver.voltage_power_supply = 12;
  driver.init();
  motor.linkDriver(&driver);
  currentSense.linkDriver(&driver);

  currentSense.init();
  currentSense.skip_align = true;
  motor.linkCurrentSense(&currentSense);

  motor.controller = MotionControlType::angle;

  motor.voltage_limit = 12;
  motor.current_limit = 40;
  motor.velocity_limit = 200;

  motor.PID_velocity.P = 0.2;
  motor.PID_velocity.I = 20;
  motor.PID_velocity.D = 0.001;
  motor.PID_velocity.output_ramp = 1000;
  motor.LPF_velocity.Tf = 0.01;
  
  Serial.begin(115200);

  motor.useMonitoring(Serial);

  //motor.zero_electric_angle = 1.07;
  //motor.sensor_direction = Direction::CCW;

  motor.sensor_offset = 0;


  //motor.target = 0;

  motor.init();
  motor.initFOC();

  commander.add('M', onMotion, "motion control");

  Serial.println(F("Motor ready."));
  Serial.println(F("Set the target angle using serial terminal:"));
  _delay(1000);
}

void loop() {
  motor.loopFOC();
  motor.move();
  commander.run();
}
