# UV-Robot-V2.0

STM32 has a different internal structure than Atmega chips. When programming this using Arduino IDE, latest libraries should be manually replaced
inside "arduino15/packages/stm32duino/hardware/STM32F1/2020.8.9/libraries".

Using Flash Memory as the EEPROM
---------------------------------

Since STM32F103 series microcontrollers do not have dedicated EEPROM, the flash memory can be used as its' EEPROM.
For this, the EEPROM library should be replaced with the STM32 Duino EEPROM library.

To bypass this function to test the code, comment out the Cal_Lamp_Hours() function.
