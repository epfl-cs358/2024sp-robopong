## BOM
- 2x b-g431b-esc1 motor driver
- 2x br3536 1200kv brushless motor
- 2x AS5600 magnetic encoder
- Arduino Mega
- 12V power supply
- 2x slide potentiometers
- LCD 16x2 screen
- I2C converter
- Webcam Logitech C270
- 2x buttons with LED
- 2x bearings for supporting the motor
- 4x linear bearings
- 4x 8mm by 40cm metal rails
- some belt
- MDF

## Wiring
### Motors drivers
#### Encoder
![326759916-8e4414b6-f468-42d3-b0b5-9535e6a64daa](https://github.com/epfl-cs358/2024sp-robopong/assets/55513917/8a89a43a-dad3-49eb-bf74-22bd882c4506) [Schematic from](https://hackaday.io/project/177578-b-g431b-esc-brushless-servo-controller/log/190978-encoders)

To connect the AS5600 encoder to the B-G431B-ESC1 motor drivers, we need to make the following connection :

         +--------------------+       +----------------------+
         |  B-G431B-ESC1      |       |      AS5600          |
         |                    |       |                      |
         |     Z (SCL)     <--+-------+--> SCL               |
         |     B (SDA)     <--+-------+--> SDA               |
         |          5V     <--+-------+--> VCC               |
         |         GND     <--+-------+--> GND               |
         +--------------------+       +----------------------+
The pins on the AS5600 are well labeled
#### Arduino
To control the driver with an Arduino with Serial, we will need to use the RX/TX ports. We will also connect GND in order to have a shared ground and on one of the two drivers will be connect the 5V to the VIN of the Arduino in order to power the arduino.
![326795159-361b4c3e-4f68-4188-b18f-7fbf75081d51](https://github.com/epfl-cs358/2024sp-robopong/assets/55513917/71b1a199-0911-48a2-8d9a-9b82a49c0b3c) [Schematic from](https://www.st.com/resource/en/user_manual/um2516-electronic-speed-controller-discovery-kit-for-drones-with-stm32g431cb-stmicroelectronics.pdf)

         +--------------------+       +----------------------+
         |  B-G431B-ESC1      |       |     Arduino          |
         |  motor 0           |       |                      | 
         |                    |       |                      |
         |          GND    <--+-------+--> GND               |
         |           5V    <--+-------+--> 5V                |
         |           TX    <--+-------+--> RX (Pin 8)        |
         |           RX    <--+-------+--> TX (Pin 9)        |
         +--------------------+       +----------------------+

         +--------------------+       +----------------------+
         |  B-G431B-ESC1      |       |     Arduino          |
         |  motor 1           |       |                      | 
         |                    |       |                      |
         |          GND    <--+-------+--> GND               |
         |           TX    <--+-------+--> RX (Pin 10)       |
         |           RX    <--+-------+--> TX (Pin 11)       |
         +--------------------+       +----------------------+
#### Power and motor
![326795266-6a96b532-8359-4901-882d-a318c4af6d5f](https://github.com/epfl-cs358/2024sp-robopong/assets/55513917/2fdbaf7d-fa63-475b-becb-0ab89f83412c) [Schematic from](https://www.st.com/resource/en/user_manual/um2516-electronic-speed-controller-discovery-kit-for-drones-with-stm32g431cb-stmicroelectronics.pdf)
To power the drivers, we connect respectively the V- and the V+ to the ground and the 12V of our powersupply.
In our system, we are using some XT-60 connectors, on this connectors the flat side is the ground and the other with a sort of triangle is the +. Additionnaly we connect the female side to the power source.
To be able to connect a single power source to the two motor drivers, we make a Y cable.

To connect the motor to the driver, we solder 3 cables to the 3 big pads on the OUTPUT side.

### Potentiometers
To control the motors manually, we need to connect two potentiometers to the Arduino. Both potentiometers send output through the analog pins. We use the following connection (the potentiometer pins are labeled 1-3):

         +------------------------+       +----------------------+
         |  Slide potentiometer 0 |       |       Arduino        |
         |                        |       |                      | 
         |                        |       |                      |
         |          1          <--+-------+-->      GND          |
         |          2          <--+-------+-->      A0           |
         |          3          <--+-------+-->      5V           |
         +------------------------+       +----------------------+

         +------------------------+       +----------------------+
         |  Slide potentiometer 1 |       |       Arduino        |
         |                        |       |                      | 
         |                        |       |                      |
         |          1          <--+-------+-->      GND          |
         |          2          <--+-------+-->      A1           |
         |          3          <--+-------+-->      5V           |
         +------------------------+       +----------------------+


### Buttons and screen
To allow the users to progress through the game flow, we need to connect two buttons (with corresponding LEDs) and an LCD screen. Both + and NC for the LED and button respectively utilize the digital pins. We use the following connections:

         +---------------------+       +----------------------+
         |       Button 0      |       |       Arduino        |
         |   (next/player 0)   |       |                      | 
         |                     |       |                      |
         |     - (LED)      <--+-------+-->      GND          |
         |     GND (BUTTON) <--+-------+-->      GND          |
         |     + (LED)      <--+-------+-->      12           |
         |     NC (BUTTON)  <--+-------+-->      6            |
         +---------------------+       +----------------------+

         +---------------------+       +----------------------+
         |      Button 1       |       |       Arduino        |
         |   (enter/player 1)  |       |                      | 
         |                     |       |                      |
         |     - (LED)      <--+-------+-->      GND          |
         |     GND (BUTTON) <--+-------+-->      GND          |
         |     + (LED)      <--+-------+-->      13           |
         |     NC (BUTTON)  <--+-------+-->      7            |
         +---------------------+       +----------------------+

For the screen, we first solder the I2C converter to the back of the LCD screen, making sure the labeled 16 pins on the converter align with the correspondingly labeled holes on the screen. Then, we use the following connections:

         +---------------------+       +----------------------+
         |    I2C converter    |       |       Arduino        |
         |                     |       |                      | 
         |                     |       |                      |
         |         GND      <--+-------+-->      GND          |
         |         VCC      <--+-------+-->      5V           |
         |         SDA      <--+-------+-->      SDA          |
         |         SCL      <--+-------+-->      SCL          |
         +---------------------+       +----------------------+

### Game and Computer Vision

To play the game, we must first launch the Python computer vision code. The Arduino should be connected to COM5 on the laptop. If a different port is used, it can be easily changed in the first line of the main method. The camera must be connected to any USB port on the laptop.

Once the code is launched, it starts by calibrating the camera: it first detects the four Aruco Markers and then computes the transformation matrix for the frame. Next, motor calibration takes place by detecting the red triangular-shaped paddles. The system sends "Go UP" and "Go DOWN" commands to the Arduino through the serial port. Once the boundaries are reached, a stop command is sent to the Arduino. This step is crucial to establish concrete bounds of the board and to save the zero and maximum positions of each motor.

After reaching all four edges of the board, the code enters the main infinite loop of our algorithm. It detects the white ball and red paddles by color detection, predicts the trajectory of the ball, and then sends the motors to the correct positions at the right time to hit the ball in computer player mode. Concurrently, the algorithm checks if a goal has occurred by performing the following checks:

If the ball is close enough to an edge, moving toward it, and then it is not visible for three consecutive frames, it is considered a goal.
In the event of a goal, a "goal 0" or "goal 1" command is sent to the Arduino depending on which player scored.

To exit the algorithm, press the 'q' button.


![CV](https://github.com/epfl-cs358/2024sp-robopong/assets/90309632/7c35c8de-7ec2-4e15-b59f-937b538845b6)

