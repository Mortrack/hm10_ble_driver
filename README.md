# HM-10 Bluetooth Module C driver library

This repository provides the library with the required the definitions, variables and functions so that its implementer
can use and communicate with an HM-10 Bluetooth Device, but in two different versions: 1) For using it from either a
microcontroller or a microprocessor of the STMicroelectronics device family and, in particular, via the STM32CubeIDE
app, and 2) For using it from a Computer with either a Windows or Linux-based Operative System. However, know that, for
at least right now, these driver library versions contain only the main functions required to make the HM-10 Bluetooth
Device to work in either Central or Peripheral Mode. For more details about this and to learn how to use this library,
feel free to read the documentation provided for each version inside their respective folders.

# How to explore the project files.
The following will describe the general purpose of the folders that are located in the current directory address:

- **/'PC'**:
    - This folder contains the HM-10 Bluetooth Module C driver Library for Computers with either Windows or Linux-based Operative Systems.
- **/STMicroelectronics**:
    - This folder provides the HM-10 Bluetooth Module C driver Library for STMicroelectronics microcontrollers and microprocessors. 

## Future additions planned for this library

Although I cannot commit to do the following soon, at some point if this project is useful for other people, then I plan
to continue adding more functions so that this can be a complete library with all the actual features of an HM-10
Bluetooth device.
