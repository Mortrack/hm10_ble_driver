# HM-10 Bluetooth Module STMicroelectronics C driver library

This library provides the definitions, variables and functions necessary so that its implementer can use and communicate
with a HM-10 Bluetooth device from a STMicroelectronics microcontroller or microprocessor. However, know that, for at
least right now, this driver library contains only the main functions required for it to work in either Central or
Peripheral Mode. For more details about this and to learn how to use this library, feel free to read the
<a href=#>documentation of this project</a>.

# How to explore the project files.
The following will describe the general purpose of the folders that are located in the current directory address:

- **/documentation**:
    - This folder provides the documentation to learn all the details of this library and to know how to use it.
- **/'Inc'**:
    - This folder contains the header files required for this library to work, where you will find the following:
      - <a href=#>The actual HM-10 driver library</a>.
      - Two configuration files for your HM-10 Library:
        - <a href=#>The default configurations file<a/> for the HM-10 device with which this library is used with (this file should not be modified).
        - <a href=#>The application's configurations file</a> for the HM-10 device with which this library is used with (this is the file that should be modified in case that you want to have custom configurations).
- **/'Src'**:
    - This folder contains the <a href=#>source code file for this library</a>. 

## Future additions planned for this library

Although I cannot commit to do the following soon, at some point if this project is useful for other people, then I plan
to continue adding more functions so that this can be a complete library with all the actual features of an HM-10
Bluetooth device.
