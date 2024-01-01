/**@file
 * @brief	HM-10 Bluetooth Device's driver Header file.
 *
 * @defgroup hm10_ble HM-10 Bluetooth module
 * @{
 *
 * @brief   This module provides the functions, definitions, structures and variables required to enable an
 *          STMicroelectronics MCU/MPU's application to be able to Configure, Initialize and Send and/or Receive data
 *          to/from a HM-10 Bluetooth (BT) Device.
 *
 * @details As for now, this @ref hm10_ble provides the necessary things so that the HM-10 BT Device can be used either
 *          in Central or in Peripheral Mode.
 *
 * @note    This module will only work as expected if your BT Device is a
 *          <a href=http://www.jnhuamao.cn/bluetooth.asp>Genuine HM-10 BT Device</a>. There are several sellers
 *          distributing clone modules. Therefore, it is up to the implementer of this module to use it on the right
 *          Device.
 * @note    Another thing to highlight is that this @ref hm10_ble has included the "stm32f1xx_hal.h" header file
 *          to be able to use the UART in this module. However, this header file is specifically meant for the STM32F1
 *          series devices. If yours is from a different type, then you will have to substitute the right one here for
 *          your particular STMicroelectronics device. However, if you cant figure out what the name of that header file
 *          is, then simply substitute that line of code from this @ref hm10_ble by: #include "main.h"
 *
 * @details While it is not necessary to use all the entirety of the functions provided to use the HM-10 BT Device in
 *          either Central or Peripheral Mode, the following code examples together demonstrate how to use that Device
 *          in each of those Modes and how to use each of the functions provided in this module in case that its
 *          implementer requires an application reference:
 *
 * @details <b><u>Code Example for configuring an HM-10 BT Device in Central Mode and, for sending and receiving data
 *          to/from a certain Remote BT Device:</u></b>
 *
 * @code
  #include <stdio.h>	// Library from which "printf" is located at.
  #include <stdint.h> // This library contains the aliases: uint8_t, uint16_t, uint32_t, etc.
  #include "hm10_ble_driver.h" // This custom Mortrack's library contains the HM-10 BT Driver Library.

  // ########################################################## //
  // ##### INITIALIZATION OF HM-10 AS A CENTRAL BLUETOOTH ##### //
  // ########################################################## //
  // Initializing the HM-10 Bluetooth module.
  init_hm10_module(&huart3); // Initializing the HM-10 Bluetooth module with the UART of your preference, where I used UART3 as an example.
  const uint16_t GPIO_hm10_state_Pin = GPIO_PIN_15; // Label for the GPIO Pin 15 towards which the GPIO Pin PC15 in Input Mode is at, which is used to read the STATE Pin of the HM-10 BT Device. The following are the possible values of that STATE Pin:<br><br>* 0 (i.e., Low State) = The HM-10 BT Device is not connected with an external BT Device.<br>* 1 (i.e., High State) = The HM-10 BT Device is currently connected with an external BT Device.
  uint8_t ret; // Local variable used to hold the exception code values returned by functions of the HM-10 Bluetooth module.

  // Making sure to disconnect the HM-10 from a currently on-going Bluetooth Connection
  printf("DEBUG: Running the disconnect_hm10_from_bt_address() function.\r\n");
  ret = disconnect_hm10_from_bt_address();
  printf("DEBUG: disconnect_hm10_from_bt_address() function returned code = %d.\r\n", ret);

  // Sending Test Command.
  printf("DEBUG: Running the send_hm10_test_cmd() function.\r\n");
  ret = send_hm10_test_cmd();
  printf("DEBUG: send_hm10_test_cmd() function returned code = %d.\r\n", ret);

  // Sending Renew Command to Restore HM-10 BT Device to Factory Setup.
  printf("DEBUG: Running the send_hm10_renew_cmd() function.\r\n");
  ret = send_hm10_renew_cmd();
  printf("DEBUG: send_hm10_renew_cmd() function returned code = %d.\r\n", ret);

  // Setting BT Name in HM-10 BT Device.
  printf("DEBUG: Running the set_hm10_name() function.\r\n");
  uint8_t name_size = 11;
  uint8_t name[] = {'M', 'i', 'r', 'a', 'n', 'd', 'a', '_', 'B', 'L', 'E'}; // Remember that max BT Name size is @ref HM10_MAX_BLE_NAME_SIZE .
  ret = set_hm10_name(name, name_size); // Setting the BT Name up to the first 8 ASCII Characters contained in the "name" array.
  printf("DEBUG: set_hm10_name() function returned code = %d.\r\n", ret);

  // Getting BT Name from HM-10 BT Device.
  printf("DEBUG: Running the get_hm10_name() function.\r\n");
  uint8_t getname_size = 0;
  uint8_t getname[HM10_MAX_BLE_NAME_SIZE] = {'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X'};
  ret = get_hm10_name(getname, &getname_size); // Should get back the BT Name of "CesarBLE".
  printf("DEBUG: get_hm10_name() function returned code = %d.\r\n", ret);
  printf("DEBUG: getname = ");
  for (int i=0; i<getname_size; i++)
  {
    printf("%c", getname[i]);
  }
  printf("_ASCII, where the size of the obtained BT Name is %d.\r\n", getname_size);

  // Setting Role in BT Device.
  printf("DEBUG: Running the set_hm10_role() function.\r\n");
  ret = set_hm10_role(HM10_Role_Central);
  printf("DEBUG: set_hm10_role() function returned code = %d.\r\n", ret);

  // Getting Role from BT Device.
  uint8_t ble_role = 7; // Setting an initial value that should not be within the possibly returned values by @ref get_hm10_role function just to make sure that it works correctly.
  printf("DEBUG: Running the get_hm10_role() function.\r\n");
  ret = get_hm10_role(&ble_role);
  printf("DEBUG: get_hm10_role() function returned code = %d.\r\n", ret);
  printf("DEBUG: ble_role value obtained = %c_ASCII\r\n", ble_role);

  // Setting a Pin in the BT Device.
  printf("DEBUG: Running the set_hm10_pin() function.\r\n");
  uint8_t set_pin[HM10_PIN_VALUE_SIZE] = {'0', '0', '1', '1', '2', '2'};
  ret = set_hm10_pin(set_pin);
  printf("DEBUG: set_hm10_pin() function returned code = %d.\r\n", ret);

  // Getting Pin from BT Device.
  printf("DEBUG: Running the get_hm10_pin() function.\r\n");
  uint8_t get_pin[HM10_PIN_VALUE_SIZE] = {'X', 'X', 'X', 'X', 'X', 'X'};
  ret = get_hm10_pin(get_pin);
  printf("DEBUG: get_hm10_pin() function returned code = %d.\r\n", ret);
  printf("DEBUG: get_pin = ");
  for (int i=0; i<HM10_PIN_VALUE_SIZE; i++)
  {
    printf("%c", get_pin[i]);
  }
  printf("_ASCII\r\n");

  // Setting a Pin Code Mode in the BT Device.
  printf("DEBUG: Running the set_hm10_pin_code_mode() function.\r\n");
  ret = set_hm10_pin_code_mode(HM10_Pin_Code_DISABLED);
  printf("DEBUG: set_hm10_pin_code_mode() function returned code = %d.\r\n", ret);

  // Getting the Pin Code Mode from the BT Device.
  printf("DEBUG: Running the get_hm10_pin_code_mode() function.\r\n");
  HM10_Pin_Code_Mode pin_code_mode = 7; // Setting an initial value that should not be within the possibly returned values by @ref get_hm10_pin_code_mode function just to make sure that it works correctly.
  ret = get_hm10_pin_code_mode(&pin_code_mode);
  printf("DEBUG: get_hm10_pin_code_mode() function returned code = %d.\r\n", ret);
  printf("DEBUG: pin_code_mode = %c_ASCII\r\n", pin_code_mode);

  // Setting the Module Work Type in the BT Device.
  printf("DEBUG: Running the set_hm10_module_work_type() function.\r\n");
  ret = set_hm10_module_work_type(HM10_Module_Work_Type_1);
  printf("DEBUG: set_hm10_module_work_type() function returned code = %d.\r\n", ret);

  //Getting the Module Work Type from the BT Device.
  printf("DEBUG: Running the get_hm10_pin_code_mode() function.\r\n");
  HM10_Module_Work_Type module_work_type = 7; // Setting an initial value that should not be within the possibly returned values by @ref get_hm10_pin_code_mode function just to make sure that it works correctly.
  ret = get_hm10_module_work_type(&module_work_type);
  printf("DEBUG: get_hm10_module_work_type() function returned code = %d.\r\n", ret);
  printf("DEBUG: module_work_type = %c_ASCII\r\n", module_work_type);

  // Setting the Notify Information Mode to enabled in the BT Device.
  printf("DEBUG: Running the set_hm10_notify_information_mode() function.\r\n");
  ret = set_hm10_notify_information_mode(HM10_Notify_ENABLED);
  printf("DEBUG: set_hm10_notify_information_mode() function returned code = %d.\r\n", ret);

  // Getting the Notify Information Mode from the BT Device.
  printf("DEBUG: Running the get_hm10_pin_code_mode() function.\r\n");
  HM10_Notify_Information_Mode notify_mode = 7; // Setting an initial value that should not be within the possibly returned values by @ref get_hm10_pin_code_mode function just to make sure that it works correctly.
  ret = get_hm10_notify_information_mode(&notify_mode);
  printf("DEBUG: get_hm10_notify_information_mode() function returned code = %d.\r\n", ret);
  printf("DEBUG: notify_mode = %c_ASCII\r\n", notify_mode);

  // Setting a Module Work Mode in the BT Device.
  printf("DEBUG: Running the set_hm10_module_work_mode() function.\r\n");
  ret = set_hm10_module_work_mode(HM10_Transmission_Mode);
  printf("DEBUG: set_hm10_module_work_mode() function returned code = %d.\r\n", ret);

  // Getting the Module Work Mode in the BT Device.
  printf("DEBUG: Running the get_hm10_module_work_mode() function.\r\n");
  HM10_Module_Work_Mode module_work_mode = 7; // Setting an initial value that should not be within the possibly returned values by @ref get_hm10_module_work_mode function just to make sure that it works correctly.
  ret = get_hm10_module_work_mode(&module_work_mode);
  printf("DEBUG: get_hm10_module_work_mode() function returned code = %d.\r\n", ret);
  printf("DEBUG: module_work_mode = %c_ASCII\r\n", module_work_mode);

  // Resetting the BT Device.
  printf("DEBUG: Running the send_hm10_reset_cmd() function.\r\n");
  ret = send_hm10_reset_cmd();
  printf("DEBUG: send_hm10_reset_cmd() function returned code = %d.\r\n", ret);

  // ###################################### //
  // ##### RECEIVING AND SENDING DATA ##### //
  // ###################################### //
  uint8_t ble_ota_data[1024]; // This Local Variable will hold the data send from the Peripheral BT Device to our Central BT Device, up to a maximum of 1024 ASCI Characters at a time.
  uint8_t ble_ota_byte = 0; // This Local Variable will hold the current value to be send to the Peripheral BT Device from our Central BT Device.
  uint16_t size = 0; // This Local Variable will serve as a counter of the bytes that have been received in our Central BT Device from the Peripheral BT Device at a time.
  // NOTE: Change the Bluetooth Address contained in the following "bt_addr" variable to whatever Remote BT Device you want to connect to.
  char bt_addr[12] = {'0', '0', '1', '7', 'E', 'A', '0', '9', '0', '9', '0', '9'}; // Static MAC Address of Remote BT Device = 00:17:EA:09:09:09
  printf("DEBUG: Starting a BT Connection and wait for receiving some data...\r\n");
  printf("DEBUG: Running the connect_hm10_to_bt_address() function.\r\n");
  ret = connect_hm10_to_bt_address(HM10_BT_Static_MAC, bt_addr);
  printf("DEBUG: connect_hm10_to_bt_address() function returned code = %d.\r\n", ret);
  do
  {
      printf("DEBUG: Waiting to receive some data...\r\n");
      // Waiting to receive up to 1024 ASCI characters of data OTA at a time (i.e., uninterruptedly).
      while (get_hm10_ota_data(ble_ota_data, 1, 1000) != HM10_EC_OK);
      size = 0;
      do
      {
        size++;
      }
      while(get_hm10_ota_data(&ble_ota_data[size], 1, 1000) == HM10_EC_OK);

      // Showing received data via UART.
      printf("\r\n\r\n");
      printf("DEBUG: The BT data received OTA with a size of %d is the following:\r\n", size);
      for (uint16_t i=0; i<size-1; i++)
      {
        printf("0x%08X, ", ble_ota_data[i]);
      }
      printf("0x%08X.\r\n", ble_ota_data[size-1]);
      printf("\r\n\r\n");

      // Sending some custom data OTA to the Peripheral BT Device after having received something from it.
      // NOTE:  The following code will send several numbers from 0 up to 255 and will repeat again from 0 to 255
      //        again and again until 1024 bytes have been send back to the slave BT device as a response (this
      //        is just for demonstrative purposes to see that we can send and receive data via the BT device).
      ble_ota_byte = 0;
      for (uint16_t i=0; i<1024; i++)
      {
          ble_ota_byte++;
          ret = send_hm10_ota_data(&ble_ota_byte, 1, 1000);
          if (ret != HM10_EC_OK)
          {
              printf("DEBUG-ERROR: Data could not be send OTA.");
              break;
          }
      }
  }
  while(1);
 * @endcode
 *
 * @details <b><u>Code Example for configuring an HM-10 BT Device in Peripheral Mode and, for sending and receiving data
 *          to/from a certain Remote BT Device:</u></b>
 *
 * @code
  #include <stdio.h>	// Library from which "printf" is located at.
  #include <stdint.h> // This library contains the aliases: uint8_t, uint16_t, uint32_t, etc.
  #include "hm10_ble_driver.h" // This custom Mortrack's library contains the HM-10 BT Driver Library.

  // ############################################################# //
  // ##### INITIALIZATION OF HM-10 AS A PERIPHERAL BLUETOOTH ##### //
  // ############################################################# //
  // Initializing the HM-10 Bluetooth module.
  init_hm10_module(&huart3); // Initializing the HM-10 Bluetooth module with the UART of your preference, where I used UART3 as an example.
  const uint16_t GPIO_hm10_state_Pin = GPIO_PIN_15; // Label for the GPIO Pin 15 towards which the GPIO Pin PC15 in Input Mode is at, which is used to read the STATE Pin of the HM-10 BT Device. The following are the possible values of that STATE Pin:<br><br>* 0 (i.e., Low State) = The HM-10 BT Device is not connected with an external BT Device.<br>* 1 (i.e., High State) = The HM-10 BT Device is currently connected with an external BT Device.
  uint8_t ret; // Local variable used to hold the exception code values returned by functions of the HM-10 Bluetooth module.

  // Making sure to disconnect the HM-10 from a currently on-going Bluetooth Connection
  printf("DEBUG: Running the disconnect_hm10_from_bt_address() function.\r\n");
  ret = disconnect_hm10_from_bt_address();
  printf("DEBUG: disconnect_hm10_from_bt_address() function returned code = %d.\r\n", ret);

  // Sending Test Command.
  printf("DEBUG: Running the send_hm10_test_cmd() function.\r\n");
  ret = send_hm10_test_cmd();
  printf("DEBUG: send_hm10_test_cmd() function returned code = %d.\r\n", ret);

  // Sending Renew Command to Restore HM-10 BT Device to Factory Setup.
  printf("DEBUG: Running the send_hm10_renew_cmd() function.\r\n");
  ret = send_hm10_renew_cmd();
  printf("DEBUG: send_hm10_renew_cmd() function returned code = %d.\r\n", ret);

  // Setting BT Name in HM-10 BT Device.
  printf("DEBUG: Running the set_hm10_name() function.\r\n");
  uint8_t name_size = 6;
  uint8_t name[] = {'C', 'e', 's', 'a', 'r', 'B', 'L', 'E', 'z', 'x', 'y', 'w'}; // Remember that max BT Name size is @ref HM10_MAX_BLE_NAME_SIZE .
  ret = set_hm10_name(name, name_size); // Setting the BT Name up to the first 8 ASCII Characters contained in the "name" array.
  printf("DEBUG: set_hm10_name() function returned code = %d.\r\n", ret);

  // Getting BT Name from HM-10 BT Device.
  printf("DEBUG: Running the get_hm10_name() function.\r\n");
  uint8_t getname_size = 0;
  uint8_t getname[HM10_MAX_BLE_NAME_SIZE] = {'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X'};
  ret = get_hm10_name(getname, &getname_size); // Should get back the BT Name of "CesarBLE".
  printf("DEBUG: get_hm10_name() function returned code = %d.\r\n", ret);
  printf("DEBUG: getname = ");
  for (int i=0; i<getname_size; i++)
  {
    printf("%c", getname[i]);
  }
  printf("_ASCII, where the size of the obtained BT Name is %d.\r\n", getname_size);

  // Setting Role in BT Device.
  printf("DEBUG: Running the set_hm10_role() function.\r\n");
  ret = set_hm10_role(HM10_Role_Peripheral);
  printf("DEBUG: set_hm10_role() function returned code = %d.\r\n", ret);

  // Getting Role from BT Device.
  uint8_t ble_role = 7; // Setting an initial value that should not be within the possibly returned values by @ref get_hm10_role function just to make sure that it works correctly.
  printf("DEBUG: Running the get_hm10_role() function.\r\n");
  ret = get_hm10_role(&ble_role);
  printf("DEBUG: get_hm10_role() function returned code = %d.\r\n", ret);
  printf("DEBUG: ble_role value obtained = %c_ASCII\r\n", ble_role);

  // Setting a Pin in the BT Device.
  printf("DEBUG: Running the set_hm10_pin() function.\r\n");
  uint8_t set_pin[HM10_PIN_VALUE_SIZE] = {'0', '0', '1', '1', '2', '2'};
  ret = set_hm10_pin(set_pin);
  printf("DEBUG: set_hm10_pin() function returned code = %d.\r\n", ret);

  // Getting Pin from BT Device.
  printf("DEBUG: Running the get_hm10_pin() function.\r\n");
  uint8_t get_pin[HM10_PIN_VALUE_SIZE] = {'X', 'X', 'X', 'X', 'X', 'X'};
  ret = get_hm10_pin(get_pin);
  printf("DEBUG: get_hm10_pin() function returned code = %d.\r\n", ret);
  printf("DEBUG: get_pin = ");
  for (int i=0; i<HM10_PIN_VALUE_SIZE; i++)
  {
    printf("%c", get_pin[i]);
  }
  printf("_ASCII\r\n");

  // Setting a Pin Code Mode in the BT Device.
  printf("DEBUG: Running the set_hm10_pin_code_mode() function.\r\n");
  ret = set_hm10_pin_code_mode(HM10_Pin_Code_DISABLED);
  printf("DEBUG: set_hm10_pin_code_mode() function returned code = %d.\r\n", ret);

  // Getting the Pin Code Mode from the BT Device.
  printf("DEBUG: Running the get_hm10_pin_code_mode() function.\r\n");
  HM10_Pin_Code_Mode pin_code_mode = 7; // Setting an initial value that should not be within the possibly returned values by @ref get_hm10_pin_code_mode function just to make sure that it works correctly.
  ret = get_hm10_pin_code_mode(&pin_code_mode);
  printf("DEBUG: get_hm10_pin_code_mode() function returned code = %d.\r\n", ret);
  printf("DEBUG: pin_code_mode = %c_ASCII\r\n", pin_code_mode);

  // Setting a Module Work Mode in the BT Device.
  printf("DEBUG: Running the set_hm10_module_work_mode() function.\r\n");
  ret = set_hm10_module_work_mode(HM10_Transmission_Mode);
  printf("DEBUG: set_hm10_module_work_mode() function returned code = %d.\r\n", ret);

  // Getting the Module Work Mode in the BT Device.
  printf("DEBUG: Running the get_hm10_module_work_mode() function.\r\n");
  HM10_Module_Work_Mode module_work_mode = 7; // Setting an initial value that should not be within the possibly returned values by @ref get_hm10_module_work_mode function just to make sure that it works correctly.
  ret = get_hm10_module_work_mode(&module_work_mode);
  printf("DEBUG: get_hm10_module_work_mode() function returned code = %d.\r\n", ret);
  printf("DEBUG: module_work_mode = %c_ASCII\r\n", module_work_mode);

  // Resetting the BT Device.
  printf("DEBUG: Running the send_hm10_reset_cmd() function.\r\n");
  ret = send_hm10_reset_cmd();
  printf("DEBUG: send_hm10_reset_cmd() function returned code = %d.\r\n", ret);

  // ###################################### //
  // ##### RECEIVING AND SENDING DATA ##### //
  // ###################################### //
  uint8_t ble_ota_data[1024]; // This Local Variable will hold the data send from the Central BT Device to our Peripheral BT Device, up to a maximum of 1024 ASCI Characters at a time.
  uint8_t ble_ota_byte = 0; // This Local Variable will hold the current value to be send to the Central BT Device from our Peripheral BT Device.
  uint16_t size = 0; // This Local Variable will serve as a counter of the bytes that have been received in our Peripheral BT Device from the Central BT Device at a time.
  printf("DEBUG: Start a BT Connection and wait for receiving some data...\r\n");
  do
  {
	  // Receiving up to 1024 ASCI characters of data OTA at a time (i.e., uninterruptedly).
	  if (get_hm10_ota_data(ble_ota_data, 1, 1000) == HM10_EC_OK)
	  {
		  size = 0;
		  do
		  {
			  size++;
		  }
		  while(get_hm10_ota_data(&ble_ota_data[size], 1, 1000) == HM10_EC_OK);

		  // Showing received data via UART.
		  printf("\r\n\r\n");
		  printf("DEBUG: The BT data received OTA with a size of %d is the following:\r\n", size);
		  for (uint16_t i=0; i<size-1; i++)
		  {
			  printf("0x%08X, ", ble_ota_data[i]);
		  }
		  printf("0x%08X.\r\n", ble_ota_data[size-1]);
		  printf("\r\n\r\n");

		  // Sending some custom data OTA to the Central BT Device after having received something from it.
		  // NOTE:  The following code will send several numbers from 0 up to 255 and will repeat again from 0 to 255
			//        again and again until 1024 bytes have been send back to the master BT device as a response (this
			//        is just for demonstrative purposes to see that we can send and receive data via the BT device).
		  ble_ota_byte = 0;
		  for (uint16_t i=0; i<1024; i++)
		  {
			  ble_ota_byte++;
			  ret = send_hm10_ota_data(&ble_ota_byte, 1, 1000);
			  if (ret != HM10_EC_OK)
			  {
				  printf("DEBUG-ERROR: Data could not be send OTA.");
				  break;
			  }
		  }
	  }
  }
  while(1);
 * @endcode
 *
 * @details Although the previous code examples show the general idea of how to configure and use the HM-10 BT Device,
 *          more than that, they show how to also use all the available functions of this module. Therefore, the
 *          following information will give the concrete suggested steps to follow in order to initialize and connect to
 *          a remote BT Device:<br><br>
 *          <b><u>Initialization steps to configure the HM-10 BT Device in Peripheral Mode:</u></b>
 *          <ol>
 *              <li>
 *                  Make sure that the HM-10 Device is not currently in an on-going Bluetooth Connection (via the @ref
 *                  disconnect_hm10_from_bt_address function)
 *              </li>
 *              <li>Restore all setup values to factory setup (via the @ref send_hm10_renew_cmd function)</li>
 *              <li>Set the desired BT Name (via the @ref set_hm10_name function)</li>
 *              <li>Set the BT Role to Peripheral Mode (via the @ref set_hm10_role function)</li>
 *              <li>
 *                  If it is desired to set a Pin to the HM-10 BT Device, then:
 *                  <ol>
 *                      <li>Set the desired Pin Value (via the @ref set_hm10_pin function)</li>
 *                      <li>Set the desired Pin Code Mode (via the @ref set_hm10_pin_code_mode function)</li>
 *                  </ol>
 *                  Otherwise, skip this step.
 *              </li>
 *              <li>Set the desired Module Work Mode (via the @ref set_hm10_module_work_mode function)</li>
 *              <li>Reset the HM-10 Device (via the @ref send_hm10_reset_cmd function)</li>
 *          </ol>
 *
 * @note    It is very important to have in mind NOT to initialize the HM-10 over and over if it is not required since
 *          that will wear off the life of the Device. This is because the Device is persisting the values changed to it
 *          even after reset, which means that there is some sort of EEPPROM, Flash Memory (most likely) or something
 *          like that where the Bluetooth Configuration settings of the HM-10 are being stored at.
 *
 * @details <br><br>
 *          <b><u>Initialization steps to configure the HM-10 BT Device in Central Mode:</u></b>
 *          <ol>
 *              <li>
 *                  Make sure that the HM-10 Device is not currently in an on-going Bluetooth Connection (via the @ref
 *                  disconnect_hm10_from_bt_address function)
 *              </li>
 *              <li>Restore all setup values to factory setup (via the @ref send_hm10_renew_cmd function)</li>
 *              <li>Set the BT Role to Central Mode (via the @ref set_hm10_role function)</li>
 *              <li>Set the desired Module Work Type (via the @ref set_hm10_module_work_type function)</li>
 *              <li>Set the Notify Information Mode to enabled (via @ref set_hm10_notify_information_mode function)</li>
 *              <li>Set the desired Module Work Mode (via the @ref set_hm10_module_work_mode function)</li>
 *              <li>Reset the HM-10 Device (via the @ref send_hm10_reset_cmd function)</li>
 *          </ol>
 *
 * @note    It is very important to have in mind NOT to initialize the HM-10 over and over if it is not required since
 *          that will wear off the life of the Device. This is because the Device is persisting the values changed to it
 *          even after reset, which means that there is some sort of EEPPROM, Flash Memory (most likely) or something
 *          like that where the Bluetooth Configuration settings of the HM-10 are being stored at.
 *
 * @details <br><br>
 *          <b><u>Steps to connect the HM-10 BT Device in Central Mode to a desired Remote BT Device in Peripheral Mode:</u></b>
 *          <ol>
 *              <li>
 *                  Make sure that the HM-10 Device is not currently in an on-going Bluetooth Connection (via the @ref
 *                  disconnect_hm10_from_bt_address function)
 *              </li>
 *              <li>
 *                  If the desired Remote BT Device has a certain Pin Code Mode and Pin Value, then:
 *                  <ol>
 *                      <li>
 *                          Configure the HM-10 Device so that it has the same Pin Code Mode and Pin Value
 *                          configurations as the Remote BT Device (via the @ref set_hm10_pin and @ref
 *                          set_hm10_pin_code_mode functions).
 *                      </li>
 *                      <li>Reset the HM-10 Device (via the @ref send_hm10_reset_cmd function)</li>
 *                  </ol>
 *                  Otherwise, skip this step.
 *              </li>
 *              <li>Connect to the desired Remote BT Device (via the @ref connect_hm10_to_bt_address function)</li>
 *          </ol>
 *
 * @author 	Cesar Miranda Meza (cmirandameza3@hotmail.com)
 * @date	December 12, 2023.
 */

#ifndef HM10_BLE_DRIVER_H_
#define HM10_BLE_DRIVER_H_

#include "stm32f1xx_hal.h" // This is the HAL Driver Library for the STM32F1 series devices. If yours is from a different type, then you will have to substitute the right one here for your particular STMicroelectronics device. However, if you cant figure out what the name of that header file is, then simply substitute this line of code by: #include "main.h"
#include <stdio.h>	// Library from which "printf" is located at.
#include "etx_ota_config.h" // Custom Library used for configuring the ETX OTA protocol.
#include <stdint.h> // This library contains the aliases: uint8_t, uint16_t, uint32_t, etc.

#define HM10_MAX_BLE_NAME_SIZE          (12)		/**< @brief Total maximum bytes that the BT Name of the HM-10 BT Device can have. */
#define HM10_PIN_VALUE_SIZE             (6)			/**< @brief Length in bytes of the Pin value in a HM-10 BT device. */

/**@brief	HM-10 Exception codes.
 *
 * @details	These Exception Codes are returned by the functions of the @ref hm10_ble to indicate the resulting status of
 *          having executed the process contained in each of those functions. For example, to indicate that the process
 *          executed by a certain function was successful or that it has failed.
 */
typedef enum
{
	HM10_EC_OK      = 0U,    //!< HM-10 Process was successful. @note The code from the @ref HAL_ret_handler function contemplates that this value will match the one given for \c HAL_OK from @ref HAL_StatusTypeDef .
	HM10_EC_STOP    = 1U,    //!< HM-10 Process has been stopped.
	HM10_EC_NR	    = 2U,	 //!< HM-10 Process has concluded with no response from HM-10 Device.
	HM10_EC_NA      = 3U,    //!< HM-10 Data received or to be received Not Applicable.
	HM10_EC_ERR     = 4U     //!< HM-10 Process has failed.
} HM10_Status;

/**@brief	HM-10 Bluetooth Connection Status.
 *
 * @details	These Status Codes are used to indicate whether the @ref disconnect_hm10_from_bt_address function was able
 *          to Disconnect the HM-10 Bluetooth Device from a currently on-going Bluetooth Connection or not.
 */
typedef enum
{
    HM10_BT_Connection_Status_Unknown   = 0U,    //!< HM-10 Device's Bluetooth Connection Status could not be determined because the @ref disconnect_hm10_from_bt_address function could not be completed successfully.
    HM10_BT_No_Connection               = 1U,    //!< HM-10 Device was not in a Bluetooth Connection.
    HM10_BT_Connection_Lost             = 2U     //!< HM-10 Device Bluetooth Connection has been lost.
} HM10_BT_Connection_Status;

/**@brief	HM-10 BT Role definitions.
 *
 * @details These definitions define the available BT Roles that we can use for our HM-10 BT Device.
 *
 * @note	In the Bluetooth Protocol, devices may play specific roles that dictate how they will interact within the
 *          Bluetooth ecosystem. The two primary roles, and the ones supported by the HM-10 BT Devices are the
 *          Peripheral and Central Roles.
 * @note    <b>Peripheral Devices</b> are the devices that advertise their presence to other devices in their vicinity
 *          and that, if they establish a BT connection with a Central device, they will provide data (i.e., They act
 *          as "servers") in that BT communication framework.
 * @note    <b>Central Devices</b> are the devices that scan for Peripheral Devices, initiate connections with them and
 *          consume the data provided by Peripheral Devices (i.e., Central Devices act as "clients").
 */
typedef enum
{
	HM10_Role_Peripheral	= 48U,	//!< HM-10 Peripheral Role (i.e., Slave mode). @note \f$48_d = 0_{ASCII}\f$.
	HM10_Role_Central  	    = 49U   //!< HM-10 Central Role (i.e., Master mode). @note \f$49_d = 1_{ASCII}\f$.
} HM10_Role;

/**@brief	HM-10 Pin Code mode definitions.
 *
 * @details These definitions define the available Pin Code modes in a HM-10 BT Device.
 *
 * @note	In the Bluetooth Protocol, there is not really a Pin Code mode. However, within the HM-10 BT Devices in
 *          particular, there is such a thing. For these type of BT Devices, there are two Pin Code modes during any
 *          bonding process that they may have with other BT Devices: 1) Pin Code enabled, or 2) Pin Code disabled.
 */
typedef enum
{
	HM10_Pin_Code_DISABLED  	= 48U,   //!< HM-10 Pin Code disabled during a authentication process with other BT devices. @note \f$48_d = 0_{ASCII}\f$.
	HM10_Pin_Code_ENABLED		= 50U	 //!< HM-10 Pin Code enabled during a authentication process with other BT devices. @note \f$50_d = 2_{ASCII}\f$.
} HM10_Pin_Code_Mode;

/**@brief	HM-10 Module Work Mode definitions.
 *
 * @details These definitions define the available Module Work modes in a HM-10 BT Device.
 *
 * @note	In the Bluetooth Protocol, there is not really a Module Work mode. However, within the HM-10 BT Devices in
 *          particular, there is such a thing. For these type of BT Devices, there are three Module Work modes that
 *          define the amount of control over our end HM-10 that our module will allow to the remote end to change after
 *          a BT Connection has been established, which are the following:
 */
typedef enum
{
    HM10_Transmission_Mode  	                    = 48U,   //!< HM-10 Transmission Mode, where the HM-10 module allows to:<br><br>- Use AT Commands before establishing a BT Connection through UART.<br>- Send data to remote BT side from each other after establishing a BT Connection.<br> @note \f$48_d = 0_{ASCII}\f$.
    HM10_PIO_Collection_and_Transmission_Mode		= 49U,   //!< HM-10 PIO Collection and Transmission Mode, where the HM-10 module allows to:<br><br>- Use AT Commands before establishing a BT Connection through UART.<br>- Send data to remote BT side from each other after establishing a BT Connection.<br> - Remote side can send AT Command Configurations to this end (i.e., to our HM-10 BT device) after BT Connection.<br> - Remote side can Collect PIO4 to the PIO11 pins input state of our end HM-10 after BT Connection.<br> - Remote side can Control PIO2, PIO3 pins output state of our end HM-10 after BT Connection.<br> - Remote side can send data to our end HM-10's UART port (not include any AT command and per package must be less than 20 bytes) after BT Connection. @note \f$49_d = 1_{ASCII}\f$.
    HM10_PIO_Remote_Control_and_Transmission_Mode   = 50U	 //!< HM-10 PIO Remote Control and Transmission Mode, where the HM-10 module allows to:<br><br>- Use AT Commands before establishing a BT Connection through UART.<br>- Send data to remote BT side from each other after establishing a BT Connection.<br> - Remote side can send AT Command Configurations to this end (i.e., to our HM-10 BT device) after BT Connection.<br> - Remote side can Control PIO2 to PIO11 pins output state of our end HM-10 after BT Connection.<br> - Remote side can send data to our end HM-10's UART port (not include any AT command and per package must be less than 20 bytes) after BT Connection. @note \f$50_d = 2_{ASCII}\f$.
} HM10_Module_Work_Mode;

/**@brief	HM-10 Module Work Type definitions.
 *
 * @details These definitions define the available Module Work types in a HM-10 BT Device.
 *
 * @note	In the Bluetooth Protocol, there is not really a Module Work type. However, within the HM-10 BT Devices in
 *          particular, there is such a thing. For these type of BT Devices, there are two Module Work types that
 *          define the the actions that the HM-10 Device will take right after powering-on, which are the following:
 */
typedef enum
{
    HM10_Module_Work_Type_0  	= 48U,    //!< HM-10 Module Work Type 0, where the HM-10 module will start working immediately (i.e., It will directly attempt to connect to whatever BT it was last configured to connect to and, in case such a BT Connection is given, then our end HM-10 Device will not even give the opportunity to receive AT Commands). @note \f$48_d = 0_{ASCII}\f$.
    HM10_Module_Work_Type_1     = 49U     //!< HM-10 Module Work Type 1, where the HM-10 module will only respond to AT Commands after powering-on and will not do anything else until AT+START, AT+CON or AT+CONNL Commands are given to it. @note \f$49_d = 1_{ASCII}\f$.
} HM10_Module_Work_Type;

/**@brief	HM-10 Notify Information Mode definitions.
 *
 * @details These definitions define the available Notify Information Modes in a HM-10 BT Device whenever a BT
 *          connection gives place between that Device and another remote/external BT Device.
 */
typedef enum
{
    HM10_Notify_DISABLED  	= 48U,    //!< HM-10 Notify Information Mode disabled. @note \f$48_d = 0_{ASCII}\f$.
    HM10_Notify_ENABLED     = 49U     //!< HM-10 Notify Information Mode enabled. @note \f$49_d = 1_{ASCII}\f$.
} HM10_Notify_Information_Mode;

/**@brief	HM-10 Bluetooth Address Type definitions.
 *
 * @details These definitions define the available Bluetooth Address Types in a HM-10 BT Device.
 *
 * @note	In the Bluetooth Protocol, all Bluetooth Devices possess an Address with which the devices can differentiate
 *          and use to decide between which devices a Bluetooth Connection is to be made. There are different types of
 *          Addresses that can be used for these purposes mentioned, where the HM-10 BT Device is capable of
 *          distinguishing four of them: 1) The Static MAC, 2) The Static Random MAC, 3) The Random MAC, and 4) The
 *          Normal addresses types.
 * @note    The Address Type with which you can identify or connect to a HM-10 BT Device is that of a Static MAC Address
 *          Type.
 */
typedef enum
{
    HM10_BT_Static_MAC		    = 48U,   //!< HM-10 Bluetooth Static MAC Address Type. @note \f$48_d = 0_{ASCII}\f$.
    HM10_BT_Static_Random_MAC   = 49U,	 //!< HM-10 Bluetooth Static Random MAC Address Type. @note \f$49_d = 1_{ASCII}\f$.
    HM10_BT_Random_MAC          = 50U,	 //!< HM-10 Bluetooth Random MAC Address Type. @note \f$50_d = 2_{ASCII}\f$.
    HM10_BT_Normal_Address  	= 78U    //!< HM-10 Bluetooth Normal Address Type. @note \f$78_d = N_{ASCII}\f$.
} HM10_BT_Address_Type;

/**@brief	HM-10 GPIO Definition parameters structure.
 *
 * @details This contains all the fields required to associate a certain GPIO pin to either the STATE pin of the HM-10
 *          BT Device Hardware or the MCU pin from which it can be requested to reset the settings of the HM-10 BT
 *          Device.
 *
 * @note    This structure is meant to be used whenever this module is implemented at the ETX OTA Protocol library made
 *          by Mortrack. Therefore, if you do not use this module together with that protocol, feel free to remove this
 *          struct in case you want to free memory from your MCU/MPU.
 */
typedef struct __attribute__ ((__packed__)) {
	GPIO_TypeDef *GPIO_Port;	//!< Type Definition of the GPIO peripheral port to which this @ref HM10_GPIO_def_t structure will be associated with.
	uint16_t GPIO_Pin;			//!< Pin number of the GPIO peripheral from to this @ref HM10_GPIO_def_t structure will be associated with.
} HM10_GPIO_def_t;

/**@brief	Sends a Test Command to the HM-10 BT Device.
 *
 * @details The primary use of this function is to identify if the HM-10 BT Device is active and/or operational
 *          whenever this function returns the @ref HM10_EC_OK Exception Code. Conversely, if any other Exception Code
 *          is given instead, then the HM-10 is not activate and/or operational.
 * @details The other use for this function is to disconnect the HM-10 BT Device from a currently on-going BT
 *          connection whenever this HM-10 BT Device is working in Central Mode and you desire to disconnect it from a
 *          currently on-going BT connection.
 *
 * @retval	HM10_EC_OK	if the Test Command was successfully sent to the HM-10 BT Device and if an OK Response was
 *                      received from it subsequently.
 * @retval  HM10_EC_NR  if there was no response from the HM-10 BT Device.
 * @retval  HM10_EC_ERR if, after sending the Test Command, the validation of the expected OK Response from the HM-10
 *                      BT Device was unsuccessful, or if anything else went wrong.
 *
 * @author	César Miranda Meza (cmirandameza3@hotmail.com)
 * @date	December 08, 2023
 */
HM10_Status send_hm10_test_cmd();

/**@brief	Sends a Reset Command to the HM-10 BT Device.
 *
 * @retval	HM10_EC_OK	if the Reset Command was successfully sent to the HM-10 BT Device and if an OK Response was
 *                      received from it subsequently.
 * @retval  HM10_EC_NR  if there was no response from the HM-10 BT Device.
 * @retval  HM10_EC_ERR if, after sending the Reset Command, the validation of the expected OK Response from the HM-10
 *                      BT Device was unsuccessful, or if anything else went wrong.
 *
 * @author	César Miranda Meza (cmirandameza3@hotmail.com)
 * @date	December 08, 2023
 */
HM10_Status send_hm10_reset_cmd();

/**@brief	Sends a Renew Command to the HM-10 BT Device.
 *
 * @retval	HM10_EC_OK	if the Renew Command was successfully sent to the HM-10 BT Device and if a Renew Response was
 *                      received from it subsequently.
 * @retval  HM10_EC_NR  if there was no response from the HM-10 BT Device.
 * @retval  HM10_EC_ERR if, after sending the Renew Command, the validation of the expected Renew Response from the
 *                      HM-10 BT Device was unsuccessful, or if anything else went wrong.
 *
 * @author	César Miranda Meza (cmirandameza3@hotmail.com)
 * @date	December 09, 2023
 */
HM10_Status send_hm10_renew_cmd();

/**@brief	Sends a Set Name Command to the HM-10 BT Device and sets a desired BT Name to that Device.
 *
 * @param[in] hm10_name Pointer to the ASCII Code data representing the desired BT Name that wants to be given to the
 *                      HM-10 BT Device.
 * @param size          Length in bytes of the name towards which the \p hm10_name param points to, which must be
 *                      @ref HM10_MAX_BLE_NAME_SIZE at the most.
 *
 * @retval	HM10_EC_OK	if the Name Command was successfully sent to the HM-10 BT Device and if the Name Response was
 *                      received from it subsequently.
 * @retval  HM10_EC_NR  if there was no response from the HM-10 BT Device.
 * @retval  HM10_EC_ERR <ul>
 *                          <li>
 *                              If, after having send the Name Command, the validation of the expected OK
 *                              Response from the HM-10 BT Device was unsuccessful.
 *                          </li>
 *                          <li>
 *                              If the \p size param has a value greater than @ref HM10_MAX_BLE_NAME_SIZE .
 *                          </li>
 *                          <li>
 *                              If anything else went wrong.
 *                          </li>
 *                      </ul>
 *
 * @author	César Miranda Meza (cmirandameza3@hotmail.com)
 * @date	December 08, 2023
 */
HM10_Status set_hm10_name(uint8_t *hm10_name, uint8_t size);

/**@brief	Sends a Get Name Command to the HM-10 BT Device and gets the BT Name of that Device.
 *
 * @param[out] hm10_name    Pointer to where it is desired to store the ASCII Code data representing the BT Name that
 *                          the HM-10 BT Device has.
 * @param[out] size         Pointer to where it is desired to store the length in bytes of the name towards which the
 *                          \p hm10_name param points to.
 *
 * @retval	HM10_EC_OK	if the BT Name was successfully received from the HM-10 BT Device and if its size did not
 *                      exceed @ref HM10_MAX_BLE_NAME_SIZE .
 * @retval  HM10_EC_NR  if there was no response from the HM-10 BT Device.
 * @retval  HM10_EC_ERR <ul>
 *                          <li>
 *                              If, after sending the Get Name Command, the validation of the expected Get Name Response
 *                              from the HM-10 BT Device was unsuccessful.
 *                          </li>
 *                          <li>
 *                              If the \p size param has a value greater than @ref HM10_MAX_BLE_NAME_SIZE .
 *                          </li>
 *                          <li>
 *                              If anything else went wrong.
 *                          </li>
 *                      </ul>
 *
 * @author	César Miranda Meza (cmirandameza3@hotmail.com)
 * @date	December 11, 2023
 */
HM10_Status get_hm10_name(uint8_t *hm10_name, uint8_t *size);

/**@brief	Sends a Set Role Command to the HM-10 BT Device and sets a desired BT Role to that Device.
 *
 * @param	ble_role	BT Role that wants to be set in the HM-10 BT Device.
 *
 * @retval	HM10_EC_OK	if the Role Command was successfully sent to the HM-10 BT Device and if the Role Response was
 *                      received from it subsequently.
 * @retval  HM10_EC_NR  if there was no response from the HM-10 BT Device.
 * @retval  HM10_EC_ERR <ul>
 *                          <li>
 *                              If, after sending the Role Command, the validation of the expected Role Response from
 *                              the HM-10 BT Device was unsuccessful.
 *                          </li>
 *                          <li>
 *                              If the \p ble_role param has an invalid value.
 *                          </li>
 *                          <li>
 *                              If anything else went wrong.
 *                          </li>
 *                      </ul>
 *
 * @author	César Miranda Meza (cmirandameza3@hotmail.com)
 * @date	December 08, 2023
 */
HM10_Status set_hm10_role(HM10_Role ble_role);

/**@brief	Sends a Get Role Command to the HM-10 BT Device and gets the Role of that Device.
 *
 * @param[out] ble_role	Pointer to the 1 byte of data into which this function will write the BT Role value given by
 *                      the HM-10 BT Device. Note that the possible values written are @ref HM10_Role .
 *
 * @retval	HM10_EC_OK	if the BT Role was successfully received from the HM-10 BT Device and if its given value is
 *                      among the recognized/expected ones, which are described in @ref HM10_Role .
 * @retval  HM10_EC_NR  if there was no response from the HM-10 BT Device.
 * @retval  HM10_EC_ERR <ul>
 *                          <li>
 *                              If, after having send the Get Role Command, the validation of the expected Get Role
 *                              Response from the HM-10 BT Device was unsuccessful.
 *                          </li>
 *                          <li>
 *                              If anything else went wrong.
 *                          </li>
 *                      </ul>
 *
 * @author	César Miranda Meza (cmirandameza3@hotmail.com)
 * @date	December 08, 2023
 */
HM10_Status get_hm10_role(HM10_Role *ble_role);

/**@brief	Sends a Set Pin Command to the HM-10 BT Device and sets a desired BT Pin to that Device.
 *
 * @param[in] pin	Pointer to the ASCII Code data representing the desired BT Pin that wants to be given to the HM-10
 *                  BT Device. This pin data must consist of exactly @ref HM10_PIN_VALUE_SIZE bytes of data, where each
 *                  byte must stand for any number character in ASCII Code.
 *
 * @retval	HM10_EC_OK	if the Pin Command was successfully sent to the HM-10 BT Device and if the Pin Response was
 *                      received from it subsequently.
 * @retval  HM10_EC_NR  if there was no response from the HM-10 BT Device.
 * @retval  HM10_EC_ERR <ul>
 *                          <li>
 *                              If, after sending the Pin Command, the validation of the expected Pin Response from the
 *                              HM-10 BT Device was unsuccessful.
 *                          </li>
 *                          <li>
 *                              If the \p pin param points to data where one of its bytes of data does not correspond to
 *                              a number character in ASCII Code.
 *                          </li>
 *                          <li>
 *                              If anything else went wrong.
 *                          </li>
 *                      </ul>
 *
 * @author	César Miranda Meza (cmirandameza3@hotmail.com)
 * @date	December 08, 2023
 */
HM10_Status set_hm10_pin(uint8_t *pin);

/**@brief	Sends a Get Pin Command to the HM-10 BT Device and gets the Pin of that Device.
 *
 * @param[out] pin	Pointer to the ASCII Code data representing the received BT Pin from the HM-10 BT Device.
 *                  This pin data consists of a size of exactly @ref HM10_PIN_VALUE_SIZE bytes, where each byte must
 *                  stand for any number character in ASCII Code.
 *
 * @retval	HM10_EC_OK	if the BT Pin was successfully received from the HM-10 BT Device and if each its given bytes
 *                      of data correspond to a number character in ASCII Code.
 * @retval  HM10_EC_NR  if there was no response from the HM-10 BT Device.
 * @retval  HM10_EC_ERR <ul>
 *                          <li>
 *                              If, after sending the Get Pin Command, the validation of the expected Get Pin Response
 *                              from the HM-10 BT Device was unsuccessful.
 *                          </li>
 *                          <li>
 *                              If anything else went wrong.
 *                          </li>
 *                      </ul>
 *
 * @author	César Miranda Meza (cmirandameza3@hotmail.com)
 * @date	December 08, 2023
 */
HM10_Status get_hm10_pin(uint8_t *pin);

/**@brief	Sends a Set Type Command to the HM-10 BT Device and sets a desired Pin Code Mode to that Device.
 *
 * @param pin_code_mode Pin Code Mode that is desired to set in the HM-10 BT Device.
 *
 * @retval	HM10_EC_OK	if the Type Command was successfully sent to the HM-10 BT Device and if the Type Response was
 *                      received from it subsequently.
 * @retval  HM10_EC_NR  if there was no response from the HM-10 BT Device.
 * @retval  HM10_EC_ERR <ul>
 *                          <li>
 *                              If, after sending the Type Command, the validation of the expected Type Response from
 *                              the HM-10 BT Device was unsuccessful.
 *                          </li>
 *                          <li>
 *                              If the \p pin_code_mode param contains an invalid value (see @ref HM10_Pin_Code_Mode for
 *                              valid values).
 *                          </li>
 *                          <li>
 *                              If anything else went wrong.
 *                          </li>
 *                      </ul>
 *
 * @author	César Miranda Meza (cmirandameza3@hotmail.com)
 * @date	December 08, 2023
 */
HM10_Status set_hm10_pin_code_mode(HM10_Pin_Code_Mode pin_code_mode);

/**@brief	Sends a Get Type Command to the HM-10 BT Device and gets the Pin Code Mode that is currently configured in
 *          that Device.
 *
 * @param[out] pin_code_mode    Pointer to the Memory Location Address where it is desired to store the Pin Code Mode
 *                              value that the HM-10 BT Device currently has configured in it.
 *
 * @retval	HM10_EC_OK	if the Pin Code Mode was successfully received from the HM-10 BT Device and if its value
 *                      corresponds to a @ref HM10_Pin_Code_Mode type.
 * @retval  HM10_EC_NR  if there was no response from the HM-10 BT Device.
 * @retval  HM10_EC_ERR <ul>
 *                          <li>
 *                              If, after sending the Get Type Command, the validation of the expected Get Type Response
 *                              from the HM-10 BT Device was unsuccessful.
 *                          </li>
 *                          <li>
 *                              If anything else went wrong.
 *                          </li>
 *                      </ul>
 *
 * @author	César Miranda Meza (cmirandameza3@hotmail.com)
 * @date	December 11, 2023
 */
HM10_Status get_hm10_pin_code_mode(HM10_Pin_Code_Mode *pin_code_mode);

/**@brief	Sends a Set Mode Command to the HM-10 BT Device and sets a desired Module Work Mode to that Device.
 *
 * @param module_work_mode Module Work Mode that is desired to set in the HM-10 BT Device.
 *
 * @retval	HM10_EC_OK	if the Mode Command was successfully sent to the HM-10 BT Device and if the Mode Response was
 *                      received from it subsequently.
 * @retval  HM10_EC_NR  if there was no response from the HM-10 BT Device.
 * @retval  HM10_EC_ERR <ul>
 *                          <li>
 *                              If, after sending the Mode Command, the validation of the expected Mode Response from
 *                              the HM-10 BT Device was unsuccessful.
 *                          </li>
 *                          <li>
 *                              If the \p module_work_mode param contains an invalid value (see @ref
 *                              HM10_Module_Work_Mode for valid values).
 *                          </li>
 *                          <li>
 *                              If anything else went wrong.
 *                          </li>
 *                      </ul>
 *
 * @author	César Miranda Meza (cmirandameza3@hotmail.com)
 * @date	December 10, 2023
 */
HM10_Status set_hm10_module_work_mode(HM10_Module_Work_Mode module_work_mode);

/**@brief	Sends a Get Mode Command to the HM-10 BT Device and gets the Module Work Mode that is currently configured
 *          in that Device.
 *
 * @param[out] module_work_mode Pointer to the Memory Location Address where it is desired to store the Module Work Mode
 *                              value that the HM-10 BT Device currently has configured in it.
 *
 * @retval	HM10_EC_OK	if the Module Work Mode was successfully received from the HM-10 BT Device and if its value
 *                      corresponds to a @ref HM10_Module_Work_Mode type.
 * @retval  HM10_EC_NR  if there was no response from the HM-10 BT Device.
 * @retval  HM10_EC_ERR <ul>
 *                          <li>
 *                              If, after sending the Get Mode Command, the validation of the expected Get Mode Response
 *                              from the HM-10 BT Device was unsuccessful.
 *                          </li>
 *                          <li>
 *                              If anything else went wrong.
 *                          </li>
 *                      </ul>
 *
 * @author	César Miranda Meza (cmirandameza3@hotmail.com)
 * @date	December 11, 2023
 */
HM10_Status get_hm10_module_work_mode(HM10_Module_Work_Mode *module_work_mode);

/**@brief	Sends a Set IMME Command to the HM-10 BT Device and sets a desired Module Work Type to that Device.
 *
 * @param module_work_type Module Work Type that is desired to set in the HM-10 BT Device.
 *
 * @retval	HM10_EC_OK	if the IMME Command was successfully sent to the HM-10 BT Device and if the IMME Response was
 *                      received from it subsequently.
 * @retval  HM10_EC_NR  if there was no response from the HM-10 BT Device.
 * @retval  HM10_EC_ERR <ul>
 *                          <li>
 *                              If, after sending the IMME Command, the validation of the expected IMME Response from
 *                              the HM-10 BT Device was unsuccessful.
 *                          </li>
 *                          <li>
 *                              If the \p module_work_type param contains an invalid value (see @ref
 *                              HM10_Module_Work_Type for valid values).
 *                          </li>
 *                          <li>
 *                              If anything else went wrong.
 *                          </li>
 *                      </ul>
 *
 * @author	César Miranda Meza (cmirandameza3@hotmail.com)
 * @date	December 11, 2023
 */
HM10_Status set_hm10_module_work_type(HM10_Module_Work_Type module_work_type);

/**@brief	Sends a Get IMME Command to the HM-10 BT Device and gets the Module Work Type that is currently configured
 *          in that Device.
 *
 * @param[out] module_work_type Pointer to the Memory Location Address where it is desired to store the Module Work Type
 *                              value that the HM-10 BT Device currently has configured in it.
 *
 * @retval	HM10_EC_OK	if the Module Work Type was successfully received from the HM-10 BT Device and if its value
 *                      corresponds to a @ref HM10_Module_Work_Type type.
 * @retval  HM10_EC_NR  if there was no response from the HM-10 BT Device.
 * @retval  HM10_EC_ERR <ul>
 *                          <li>
 *                              If, after sending the Get IMME Command, the validation of the expected Get IMME Response
 *                              from the HM-10 BT Device was unsuccessful.
 *                          </li>
 *                          <li>
 *                              If anything else went wrong.
 *                          </li>
 *                      </ul>
 *
 * @author	César Miranda Meza (cmirandameza3@hotmail.com)
 * @date	December 11, 2023
 */
HM10_Status get_hm10_module_work_type(HM10_Module_Work_Type *module_work_type);

/**@brief	Sends a Set NOTI Command to the HM-10 BT Device and sets a desired Notify Information Mode to that Device.
 *
 * @param notify_mode   Notify Information Mode that is desired to set in the HM-10 BT Device.
 *
 * @retval	HM10_EC_OK	if the NOTI Command was successfully sent to the HM-10 BT Device and if the NOTI Response was
 *                      received from it subsequently.
 * @retval  HM10_EC_NR  if there was no response from the HM-10 BT Device.
 * @retval  HM10_EC_ERR <ul>
 *                          <li>
 *                              If, after sending the NOTI Command, the validation of the expected NOTI Response from
 *                              the HM-10 BT Device was unsuccessful.
 *                          </li>
 *                          <li>
 *                              If the \p notify_mode param contains an invalid value (see @ref
 *                              HM10_Notify_Information_Mode for valid values).
 *                          </li>
 *                          <li>
 *                              If anything else went wrong.
 *                          </li>
 *                      </ul>
 *
 * @author	César Miranda Meza (cmirandameza3@hotmail.com)
 * @date	December 11, 2023
 */
HM10_Status set_hm10_notify_information_mode(HM10_Notify_Information_Mode notify_mode);

/**@brief	Sends a Get NOTI Command to the HM-10 BT Device and gets the Notify Information Mode that is currently
 *          configured in that Device.
 *
 * @param[out] notify_mode  Pointer to the Memory Location Address where it is desired to store the Notify Information
 *                          Mode value that the HM-10 BT Device currently has configured in it.
 *
 * @retval	HM10_EC_OK	if the Notify Information Mode was successfully received from the HM-10 BT Device and if its
 *                      value corresponds to a @ref HM10_Notify_Information_Mode type.
 * @retval  HM10_EC_NR  if there was no response from the HM-10 BT Device.
 * @retval  HM10_EC_ERR <ul>
 *                          <li>
 *                              If, after sending the Get NOTI Command, the validation of the expected Get NOTI Response
 *                              from the HM-10 BT Device was unsuccessful.
 *                          </li>
 *                          <li>
 *                              If anything else went wrong.
 *                          </li>
 *                      </ul>
 *
 * @author	César Miranda Meza (cmirandameza3@hotmail.com)
 * @date	December 11, 2023
 */
HM10_Status get_hm10_notify_information_mode(HM10_Notify_Information_Mode *notify_mode);

/**@brief	Sends a Connect-To-Address Command to the HM-10 BT Device (must be configured in Central Mode) and connects
 *          that Device with a desired Remote Bluetooth Device that should have already been configured in Peripheral
 *          Mode.
 *
 * @details Before calling this function, our end HM-10 BT Device must have already been configured with the right Pin
 *          Code Mode (via the @ref set_hm10_pin_code_mode function) and with the right Pin Value (via the @ref
 *          set_hm10_pin function) correspondingly so that it matches the configuration of the Remote Bluetooth Device
 *          configured in Peripheral Mode.
 *
 * @param bt_addr_t     Bluetooth Address Type of the Remote Bluetooth Device with which it is desired to establish a
 *                      Bluetooth Connection with our end HM-10 Device.
 * @param[in] bt_addr   Pointer to where the data of the ASCII Characters standing for the actual Bluetooth Address, of
 *                      the Remote Bluetooth Device, is located at. An example of how to populate this data is, for
 *                      example, if the Remote BT Device has the MAC Address of 00:17:EA:09:09:09, then the pointer \p
 *                      bt_addr should point to the data in the following way:
 *                      \code bt_addr[12] = {'0', '0', '1', '7', 'E', 'A', '0', '9', '0', '9', '0', '9'}; \endcode ,
 *                      where it is important to know before hand that all Bluetooth Addresses must must have a MAC
 *                      Address with a length of twelve bytes.
 *
 * @retval	HM10_EC_OK	if the Connect-To-Address Command was successfully sent to the HM-10 BT Device and if the whole
 *                      Connect-To-Address Response was received from it subsequently.
 * @retval  HM10_EC_NR  if there was no response from the HM-10 BT Device.
 * @retval  HM10_EC_ERR <ul>
 *                          <li>
 *                              If, after sending the Connect-To-Address Command, the validation of the expected whole
 *                              Connect-To-Address Response from the HM-10 BT Device was unsuccessful.
 *                          </li>
 *                          <li>
 *                              If, after having received and successfully validated the first part of the
 *                              Connect-To-Address Response from the HM-10 BT Device, no data from the expected second
 *                              part of the Connect-To-Address Response is received from the HM-10 BT Device.<br><br>If
 *                              this happens, then this will mean that our end HM-10 BT Device tried to connect to the
 *                              desired Remote BT Device but could not achieve a successful connection for some reason.
 *                              Some ideas to fix this could be to make sure that both devices Pin Code Mode and Pin
 *                              values match between both of them. Another idea could be to try to electrically reset
 *                              both devices and try to establish a BT Connection between them again. Something else
 *                              that could be attempted is to make sure that both Devices are within a reasonable
 *                              distance and that they do not have something interfering with the BT Wireless signals
 *                              between them.
 *                          </li>
 *                          <li>
 *                              If the \p bt_addr_t param contains an invalid value (see @ref HM10_BT_Address_Type for
 *                              valid values).
 *                          </li>
 *                          <li>
 *                              If anything else went wrong.
 *                          </li>
 *                      </ul>
 *
 * @author	César Miranda Meza (cmirandameza3@hotmail.com)
 * @date	December 12, 2023
 */
HM10_Status connect_hm10_to_bt_address(HM10_BT_Address_Type bt_addr_t, char bt_addr[12]);

/**@brief	Sends a Test Command to the HM-10 BT Device and will disconnect an on-going Bluetooth Connection between our
 *          end HM-10 Device and a Remote BT Device, in case that there is such a connection.
 *
 * @details In case that there is not an on-going Bluetooth Connection and if this function processes complete
 *          successfully, then our end HM-10 Device should only respond back to our MCU/MPU with an OK Response.
 * @details However, if there is such a Connection and this function processes complete successfully, then our end HM-10
 *          Device is expected to respond back to our MCU/MPU with a OK+LOST Response.
 *
 * @retval	HM10_BT_Connection_Lost             if an on-going Bluetooth Connection between our end HM-10 BT Device and
 *                                              a Remote BT Device has been successfully terminated.
 * @retval  HM10_BT_No_Connection               if there was no Bluetooth Connection between our end HM-10 BT Device and
 *                                              any other BT Device.
 * @retval  HM10_BT_Connection_Status_Unknown   if something went wrong when trying to successfully complete this
 *                                              function's processes. Therefore, this would mean that the Bluetooth
 *                                              Connection Status of the HM-10 BT Device could not be determined.
 *
 * @author	César Miranda Meza (cmirandameza3@hotmail.com)
 * @date	December 12, 2023
 */
HM10_BT_Connection_Status disconnect_hm10_from_bt_address();

/**@brief   Sends some desired data Over the Air (OTA) via the HM-10 BT Device to whatever other BT Device it is
 *          connected to Point-to-Point, if there is such a connection.
 *
 * @note    If there is no BT connection between the HM-10 BT Device and any other BT Device, the HM-10 BT Device
 *          will do nothing.
 *
 * @param[out] ble_ota_data Pointer to the data that is desired to send OTA via the HM-10 BT Device.
 * @param size              Length in bytes of the data towards which the \p ble_ota_data param points to.
 * @param timeout           Timeout duration for waiting to send the requested data OTA via the HM-10 BT Device.
 *
 * @retval	HM10_EC_OK	if the requested data was successfully send OTA via the HM-10 BT Device.
 * @retval  HM10_EC_NR  if there was no response from the HM-10 BT Device whenever attempting to send the requested
 *                      data OTA.
 * @retval  HM10_EC_ERR otherwise.
 *
 * @author	César Miranda Meza (cmirandameza3@hotmail.com)
 * @date	December 06, 2023
 */
HM10_Status send_hm10_ota_data(uint8_t *ble_ota_data, uint16_t size, uint32_t timeout);

/**@brief   Gets the HM-10 Device's BT data that is received Over the Air (OTA), if there is any within the
 *          specified timeout.
 *
 * @note    Remember that whenever sending data from a BT Device in Central mode to the HM-10 BT Device with
 *          which this @ref hm10_ble is being used on, the default Service Name is 0xFFE0 (which is modifiable
 *          with the AT+UUID Command that has yet to be implemented in this module) and the default Characteristic Name
 *          is 0xFFE1 (which is modifiable with the AT+CHAR Command that has yet to be implemented in this module).
 *
 * @param[out] ble_ota_data Pointer to the Memory Address into which the received data from the HM-10 BT Device will be
 *                          stored.
 * @param size              Length in bytes of the BT data that is expected to be received OTA from the HM-10 BT
 *                          Device.
 * @param timeout           Timeout duration for waiting to receive BT data OTA from the HM-10 BT Device.
 *
 * @retval	HM10_EC_OK	if some BT data was successfully received OTA from the HM-10 BT Device.
 * @retval  HM10_EC_NR  if no BT data was received OTA from the HM-10 BT Device.
 * @retval  HM10_EC_ERR otherwise.
 *
 * @author	César Miranda Meza (cmirandameza3@hotmail.com)
 * @date	December 06, 2023
 */
HM10_Status get_hm10_ota_data(uint8_t *ble_ota_data, uint16_t size, uint32_t timeout);

/**@brief	Initializes the @ref hm10_ble in order to be able to use its provided functions.
 *
 * @details This function stores in the @ref p_huart Global Static Pointer the address of the UART Handle Structure of
 *          the UART that is desired to be used by the @ref hm10_ble to send/receive data to/from the HM-10 BT Device.
 *
 * @param[in] huart	Pointer to the UART Handle Structure of the UART that it is desired to use in the @ref hm10_ble to
 *                  send/receive data to/from the HM-10 BT Device.
 *
 * @author	César Miranda Meza (cmirandameza3@hotmail.com)
 * @date	December 06, 2023
 */
void init_hm10_module(UART_HandleTypeDef *huart);

#endif /* HM10_BLE_DRIVER_H_ */

/** @} */
