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
- Belt
- MDF
- Velcro
- Metal rod for camera

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

## General Assembly
First, laser cut and 3D print the necessary parts.
To laser cut:
- Box walls
- 2x potentiometer case walls
- Game board (from black plexiglass)

  
To 3D print:
- 2x paddle-belt-rail system (includes one paddle mount, one paddle, two belt attachments)
- Gutter system
- 2x joystick rails and joystick
- 6x wall liner in TPU
- 2x pulleys
- 2x motor encoder case
- 2x backing plates for buttons
- 2x motor reduction gears
- 2x magnet case
- 2x rail holders
- Arduino Mega mount
- Power supply mount
- Camera mount
- Screen mount


First, assemble the game board shell using the laser cut walls as shown in the 3D model (exploded view in [cad/README.md](https://github.com/epfl-cs358/2024sp-robopong/tree/main/cad)). Leave the outermost walls on the left and right side to attach after motor assembly. 

### Motor system assembly
To assemble the motor and belt system, first construct the motor reduction gear by cutting an appropriate length of belt and gluing it into the notch on the gear. Screw this gear on to the motor, followed by the magnet case with the magnet in it (this should be glued in place), followed by the motor encoder holder. Place the motor encoder into its holder.


<img width="185" alt="image" src="https://github.com/epfl-cs358/2024sp-robopong/assets/69027178/d7228f6c-c6bb-432f-9afa-34a9aaf3bb82">

<img width="133" alt="image" src="https://github.com/epfl-cs358/2024sp-robopong/assets/69027178/3df32c89-8f19-4c25-b0d3-a7237de88b51">

The motor can then be screwed into the base of the game board using the corresponding screw holes. Then, insert linear bearings into the appropriate clasps on each paddle mount print. Use screws (four per paddle mount) to tighten the linear rail clasps. 

<img width="341" alt="image" src="https://github.com/epfl-cs358/2024sp-robopong/assets/69027178/5ab42a3d-71e6-4be4-87eb-d1c89c6c3f4a">
<img width="257" alt="image" src="https://github.com/epfl-cs358/2024sp-robopong/assets/69027178/fc83de0e-23ce-4d74-ba31-5f218d621d33">


Screw the belt attachments onto the paddle mount and attach the belt to one end by first placing a nut into the hexagonal indent on the belt attachment

<img width="284" alt="image" src="https://github.com/epfl-cs358/2024sp-robopong/assets/69027178/8dc5bf63-29d5-41ba-a180-39459e1f7c69">

pinching the belt into the notch, and turning a screw down the center. 

<img width="307" alt="image" src="https://github.com/epfl-cs358/2024sp-robopong/assets/69027178/fdaba931-94b2-405d-add4-9799321feed9">


To insert the rails, first screw the rail holder into the side of the box with the corresponding screw holes. Push both rails through the rail holders attached to the box wall and clamp with screws.

<img width="310" alt="image" src="https://github.com/epfl-cs358/2024sp-robopong/assets/69027178/b3215e7b-6e87-4bed-84d4-d2ebe5ebd1cf">

Continue pushing the rails through the paddle mount with linear bearings and the corresponding rail holes on the opposite side of the game board. Loop the belt around the motor reduction gear and pulley on the opposing side and attach the other end of the belt to the paddle mount with the belt attachmentsimilar to before. 

<img width="968" alt="image" src="https://github.com/epfl-cs358/2024sp-robopong/assets/69027178/1dd267c8-50a2-4e04-913a-af48c3573032">

The paddle mount should now be connected to the motor via the belt and should slide along the rails. Repeat on the other side. Then, wire the encoders and microcontrollers to the Arduino as shown above.

### Game board assembly
To assemble the game board, first sand the plexiglass to prevent glare from interfering with the computer vision. Then, run the ARUCO marker generation script (`scripts/gen_aruco.py`) and print the four markers. Attach these to the sanded game board, making sure that the detected corners align with the corners of the game board. Attach velcro strips along the back and front walls of the game board as well as the TPU wall linings. Ensure that the furry side of a velcro strip is attached on the back wall of the game board

<img width="728" alt="image" src="https://github.com/epfl-cs358/2024sp-robopong/assets/69027178/6b381caf-af8b-4eb4-b1bd-d83de4ba6092">

as this will ensure that the game board is able to be removed to access electronics.

### Gutters
Glue the gutter system together as shown in the 3D model. Insert the entire gutter system into the game box.

### Peripherals
Both buttons come with a plastic "nut" that holds the button together. On the front of the board, unscrew the nut on each button and insert the button through the hole in the front of the box. On the inside wall, place the button backing plate and screw the plastic nut back on. The button's nut should now be compressing both the backing plate and the frontmost panel of the game box, securing the buttons in place. Additionally, screw the screen into the screen mount and attach to the top of the game board. Wire the screen and both buttons as mentioned above. 

To construct the joysticks, first assemble the laser cut finger joint box. Then, snap the bottom half of the joystick onto the slider.

<img width="278" alt="image" src="https://github.com/epfl-cs358/2024sp-robopong/assets/69027178/7bda0055-8d0a-4185-9368-2462aee69354">

The top half of the joystick can then be glued on, forming the complete handle.

<img width="306" alt="image" src="https://github.com/epfl-cs358/2024sp-robopong/assets/69027178/27280725-53fa-4be6-928f-c0208e4ae23d">

On the top plate, screw in both rails leaving one screw undone. Insert the slider between the rails, and secure the final screw. 

<img width="425" alt="image" src="https://github.com/epfl-cs358/2024sp-robopong/assets/69027178/f7583d82-614f-41b7-afa4-805d3afd1bbf">

Then, attach the potentiometer to the joystick and screw the potentiometer into the top plate. Feed the potentiometer wires through the hole in the back plate, and connect to the Arduino as mentioned above. 

For the camera, first insert the metal rod through the corresponding holes on the rear upper panel and bottom panel of the game box.

<img width="310" alt="image" src="https://github.com/epfl-cs358/2024sp-robopong/assets/69027178/247be053-2779-4c6b-afdd-202a0b6ccde6">

Attach the camera to the printed hinge mount and push this mount onto the top of the metal rod. Secure the rod in place.

Once all wiring is complete, attach the Arduino Mega to its mount and secure the mount to the bottom of the board. Place the power supply brick in its mount and secure to the bottom of the board as well. Glue the gutter system together and fit it along the inside walls of the game box. The game board can now be inserted, the wall linings attached, and the product is ready to play. 

### Game and Computer Vision

To play the game, we must first launch the Python computer vision code. The Arduino should be connected to the laptop, and the port used should be specified in the first line of the main method. Currently, it is set to COM5. The camera must be connected to any USB port on the laptop.

Once the code is launched, it starts by calibrating the camera: it first detects the four Aruco Markers and then computes the transformation matrix for the frame. Next, motor calibration takes place by detecting the red triangular-shaped paddles. The system sends "Go UP" and "Go DOWN" commands to the Arduino through the serial port. Once the boundaries are reached, a stop command is sent to the Arduino. This step is crucial to establish concrete bounds of the board and to save the zero and maximum positions of each motor.

After reaching all four edges of the board, the code enters the main infinite loop of our algorithm. It detects the white ball and red paddles by color detection, predicts the trajectory of the ball, and then sends the motors to the correct positions at the right time to hit the ball in computer player mode. Concurrently, the algorithm checks if a goal has occurred by performing the following checks:

If the ball is close enough to an edge, moving toward it, and then it is not visible for three consecutive frames, it is considered a goal.
In the event of a goal, a "goal 0" or "goal 1" command is sent to the Arduino depending on which player scored.

To exit the algorithm, press the 'q' button.


![CV](https://github.com/epfl-cs358/2024sp-robopong/assets/90309632/7c35c8de-7ec2-4e15-b59f-937b538845b6)



