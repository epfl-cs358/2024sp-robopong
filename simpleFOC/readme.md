# STM32 MCU Setup Tutorial for Arduino IDE

This tutorial will guide you through setting up your Arduino IDE to use STM32 MCU based boards and installing the Simple FOC library to control motor drivers.

## Prerequisites

- Ensure you have the latest version of the Arduino IDE installed on your computer.

## Step 1: Add the STM32 Board Manager URL

1. Open the Arduino IDE.
2. Navigate to `File -> Preferences`.
3. In the "Additional Boards Manager URLs" field, add the following URL:
https://raw.githubusercontent.com/stm32duino/BoardManagerFiles/main/package_stmicroelectronics_index.json
4. Click "OK" to save your changes.

## Step 2: Install STM32 MCU Based Boards

1. Open the Boards Manager by going to `Tools -> Board -> Boards Manager...`
2. In the search bar, type `STM32 MCU`.
3. Find "STM32 MCU based boards by STMicroelectronics" in the list and click the "Install" button.

## Step 3: Install the Simple FOC Library

1. Navigate to `Tools -> Manage Libraries...`
2. In the search bar, type `Simple FOC`.
3. Find the "Simple FOC by simplefoc" library and click the "Install" button.

## Step 4: Select Your Board

1. Go to `Tools -> Board -> STM32 MCU based boards` and select "Discovery".

## Step 5: Set the Board Part Number

1. Navigate to `Tools -> Board part number`.
2. Ensure `B-G431B-ESC1` is selected from the list.

## Step 6: Select the Correct Port

1. Connect your STM32 board to your computer via USB.
2. Go to `Tools -> Port` and select the port that shows your connected board.

## Step 7: Compilation

- Note: The first compilation process might take a longer time. This is normal as the IDE prepares all necessary files.

## Step 8: Uploading Code to Motor Drivers

- Before uploading your code, ensure that the motor drivers are powered by an external supply.
- Proceed with uploading your code as usual via the Arduino IDE.

## Troubleshooting

If you encounter any issues, please verify that all steps were followed correctly, particularly ensuring that the correct board and port are selected. Check also that your motor drivers are properly powered.

## Conclusion

You should now be able to program your STM32 board to control motor drivers using the Simple FOC library. Happy coding!

PS: This small tutorial is based on the summary made by Amene for setting up simpleFOC with stepper motors, then modified/completed to match the new specifications and the text has been rephrased/put in form using ChatGPT.

And the code is provided by Professor Koch.