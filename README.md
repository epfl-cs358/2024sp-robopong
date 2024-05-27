## Wiring
### Motors drivers
#### Encoder
![326759916-8e4414b6-f468-42d3-b0b5-9535e6a64daa](https://github.com/epfl-cs358/2024sp-robopong/assets/55513917/8a89a43a-dad3-49eb-bf74-22bd882c4506) [Schemtic from](https://hackaday.io/project/177578-b-g431b-esc-brushless-servo-controller/log/190978-encoders)

         +--------------------+       +----------------------+
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
To control the driver with an Arduino with Serial, we will need to use the RX/TX ports. We will also connect GND in order to have a shared ground and on one of the two drivers will be connect the 5V to the Vin of the arduibo in order to power the arduino.
![326795159-361b4c3e-4f68-4188-b18f-7fbf75081d51](https://github.com/epfl-cs358/2024sp-robopong/assets/55513917/71b1a199-0911-48a2-8d9a-9b82a49c0b3c) [Schemtic from](https://www.st.com/resource/en/user_manual/um2516-electronic-speed-controller-discovery-kit-for-drones-with-stm32g431cb-stmicroelectronics.pdf)

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
![326795266-6a96b532-8359-4901-882d-a318c4af6d5f](https://github.com/epfl-cs358/2024sp-robopong/assets/55513917/2fdbaf7d-fa63-475b-becb-0ab89f83412c) [Schemtic from](https://www.st.com/resource/en/user_manual/um2516-electronic-speed-controller-discovery-kit-for-drones-with-stm32g431cb-stmicroelectronics.pdf)
To power the drivers, we connect respectively the V- and the V+ to the ground and the 12V of our powersupply.
In our system, we are using some XT-60 connectors, on this connectors the flat side is the ground and the other with a sort of triangle is the +. Additionnaly we connect the femelle side to the power source.
To be able to connect a single power source to the two motor drivers, we make a Y cable.

To connect the motor to the driver, we do it by soldering 3 cables to the 3 big pads on the OUTPUT side.
