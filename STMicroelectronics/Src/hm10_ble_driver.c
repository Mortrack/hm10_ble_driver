/** @addtogroup hm10_ble
 * @{
 */

#include "hm10_ble_driver.h"
#include <string.h>	// Library from which "memset()" and "memcpy()" are located at.

#define HM10_MAX_AT_COMMAND_SIZE							(19)       /**< @brief Total maximum bytes in a Tx/Rx AT Command of the HM-10 BT Device. */
#define HM10_MAX_PACKET_SIZE								(19)       /**< @brief Total maximum bytes in a Tx/Rx package/Payload to/from the HM-10 BT Device. @note The documentation of the HM-10 BT Device states that there is a restriction of sending data from one HM-10 BT Device to another, whenever they establish a connection, of 19 bytes per request. Therefore, to manage things homogeneously, both the transmit and receive requests will be handled by this @ref hm10_ble with the same size limit of 19 bytes. */
#define HM10_TEST_CMD_SIZE								    (2)        /**< @brief	Length in bytes of a Test Command in the HM-10 BT device. */
#define HM10_RESET_CMD_SIZE								    (8)        /**< @brief	Length in bytes of a Reset Command in the HM-10 BT device. */
#define HM10_RENEW_CMD_SIZE								    (8)        /**< @brief	Length in bytes of a Renew Command in the HM-10 BT device. */
#define HM10_SET_NAME_RESPONSE_SIZE_WITHOUT_REQUESTED_NAME  (7)        /**< @brief	Length in bytes of a Set Name Response from the HM-10 BT device but without considering the length of the requested name. */
#define HM10_GET_NAME_RESPONSE_SIZE_WITHOUT_REQUESTED_NAME	(8)        /**< @brief	Length in bytes of a Get Name Response from the HM-10 BT device but without considering the length of the requested name. */
#define HM10_GET_NAME_CMD_SIZE							    (8)        /**< @brief	Length in bytes of a Get Name Command in the HM-10 BT device. */
#define HM10_SET_ROLE_CMD_SIZE							    (8)        /**< @brief	Length in bytes of a Set Role Command in the HM-10 BT device. */
#define HM10_GET_ROLE_CMD_SIZE							    (8)        /**< @brief	Length in bytes of a Get Role Command in the HM-10 BT device. */
#define HM10_ROLE_RESPONSE_SIZE							    (8)        /**< @brief	Length in bytes of either a Get or a Set Role Command's Response in the HM-10 BT device. */
#define HM10_SET_PIN_CMD_SIZE								(13)       /**< @brief	Length in bytes of the Set Pin Command of a HM-10 BT device. */
#define HM10_GET_PIN_CMD_SIZE								(8)        /**< @brief	Length in bytes of the Get Pin Command of a HM-10 BT device. */
#define HM10_PIN_RESPONSE_SIZE							    (13)       /**< @brief	Length in bytes of either a Get or a Set Pin Command's Response in the HM-10 BT device. */
#define HM10_SET_TYPE_CMD_SIZE							    (8)        /**< @brief	Length in bytes of the Set Type Command of a HM-10 BT device. */
#define HM10_GET_TYPE_CMD_SIZE							    (8)        /**< @brief	Length in bytes of the Get Type Command of a HM-10 BT device. */
#define HM10_TYPE_RESPONSE_SIZE							    (8)        /**< @brief	Length in bytes of either a Get or a Set Type Command's Response in the HM-10 BT device. */
#define HM10_SET_MODE_CMD_SIZE							    (8)        /**< @brief	Length in bytes of the Set Mode Command of a HM-10 BT device. */
#define HM10_GET_MODE_CMD_SIZE							    (8)        /**< @brief	Length in bytes of the Get Mode Command of a HM-10 BT device. */
#define HM10_MODE_RESPONSE_SIZE							    (8)        /**< @brief	Length in bytes of either a Get or a Set Mode Command's Response in the HM-10 BT device. */
#define HM10_SET_IMME_CMD_SIZE							    (8)        /**< @brief	Length in bytes of the Set IMME Command of a HM-10 BT device. */
#define HM10_GET_IMME_CMD_SIZE							    (8)        /**< @brief	Length in bytes of the Get IMME Command of a HM-10 BT device. */
#define HM10_IMME_RESPONSE_SIZE							    (8)        /**< @brief	Length in bytes of either a Get or a Set IMME Command's Response in the HM-10 BT device. */
#define HM10_SET_NOTI_CMD_SIZE							    (8)        /**< @brief	Length in bytes of the Set NOTI Command of a HM-10 BT device. */
#define HM10_GET_NOTI_CMD_SIZE							    (8)        /**< @brief	Length in bytes of the Get NOTI Command of a HM-10 BT device. */
#define HM10_NOTI_RESPONSE_SIZE							    (8)        /**< @brief	Length in bytes of either a Get or a Set NOTI Command's Response in the HM-10 BT device. */
#define HM10_CONNECT_TO_ADDRESS_CMD_SIZE				    (18)       /**< @brief	Length in bytes of the Connect-To-Address Command of a HM-10 BT device. */
#define HM10_CONNECT_TO_ADDRESS_RESPONSE1_SIZE				(8)        /**< @brief	Length in bytes of the first part of the Connect-To-Address Command's Response in the HM-10 BT device. */
#define HM10_CONNECT_TO_ADDRESS_RESPONSE2_SIZE				(7)        /**< @brief	Length in bytes of the second part of the Connect-To-Address Command's Response in the HM-10 BT device. */
#define HM10_RESET_RESPONSE_SIZE							(8)        /**< @brief	Length in bytes of a Reset Response from the HM-10 BT device. */
#define HM10_RENEW_RESPONSE_SIZE							(8)        /**< @brief	Length in bytes of a Renew Response from the HM-10 BT device. */
#define HM10_OK_RESPONSE_SIZE								(2)        /**< @brief	Length in bytes of a OK Response from the HM-10 BT device. */
#define HM10_OK_LOST_RESPONSE_SIZE                          (7)        /**< @brief	Length in bytes of a whole OK+LOST Response from the HM-10 BT device. */
#define HM10_OK_LOST_RESPONSE_SIZE_WITHOUT_THE_OK_PART      (5)        /**< @brief	Length in bytes of a OK+LOST Response from the HM-10 BT device, but without the OK part. */
#define HM10_CONNECT_TO_ADDRESS_TIMEOUT				        (11000U)   /**< @brief	Time in milliseconds that out MCU/MPU will wait for the HM-10 BT device's Connect-To-Address Response after sending a Connect-To-Address Command to it. */

static UART_HandleTypeDef *p_huart;												                                                  /**< @brief Pointer to the UART Handle Structure of the UART that will be used in this @ref hm10_ble to communicate with the HM-10 BT device. @details This pointer's value is defined in the @ref init_hm10_module function. */
static uint8_t TxRx_Buffer[HM10_MAX_AT_COMMAND_SIZE];					                                                          /**< @brief Global buffer that will be used by our MCU/MPU to hold the whole data of a received response or a request to be send from/to the HM-10 BT Device. */
static char HM10_Set_Name_resp_without_name_value[] = {'O', 'K', '+', 'S', 'e', 't', ':'};	          /**< @brief Pointer to the equivalent data of the BT Name Response that the HM-10 BT device sends back to our MCU/MPU whenever a Set Name request to the HM-10 BT device is processed successfully, but without the name value. */
static char HM10_Get_Name_resp_without_name_value[] = {'O', 'K', '+', 'N', 'A', 'M', 'E', ':'};    /**< @brief Pointer to the equivalent data of a BT Name Response that the HM-10 BT device sends back to our MCU/MPU whenever a Get Name request to the HM-10 BT device is processed successfully, but without the name value. */
static char *HM10_Set_Role_resp_without_role_value = HM10_Set_Name_resp_without_name_value;	                                      /**< @brief Pointer to the equivalent data of a BT Role Response that the HM-10 BT device sends back to our MCU/MPU whenever a Set Role request to the HM-10 BT device is processed successfully, but without the role value. */
static char HM10_Get_Role_resp_without_role_value[] = {'O', 'K', '+', 'G', 'e', 't', ':'};	          /**< @brief Pointer to the equivalent data of a BT Role Response that the HM-10 BT device sends back to our MCU/MPU whenever a Get Role request to the HM-10 BT device is processed successfully, but without the role value. */
static char *HM10_Set_Pin_resp_without_pin_value = HM10_Set_Name_resp_without_name_value;			                              /**< @brief Pointer to the equivalent data of a BT Pin Response that the HM-10 BT device sends back to our MCU/MPU whenever a Set Pin request to the HM-10 BT device is processed successfully, but without the pin value. */
static char *HM10_Get_Pin_resp_without_pin_value = HM10_Get_Role_resp_without_role_value;			                              /**< @brief Pointer to the equivalent data of a BT Pin Response that the HM-10 BT device sends back to our MCU/MPU whenever a Get Pin request to the HM-10 BT device is processed successfully, but without the pin value. */
static char *HM10_Set_Type_resp_without_type_value = HM10_Set_Name_resp_without_name_value;	                                      /**< @brief Pointer to the equivalent data of a BT Type Response that the HM-10 BT device sends back to our MCU/MPU whenever a Set Type request to the HM-10 BT device is processed successfully, but without the type value. */
static char *HM10_Get_Type_resp_without_type_value = HM10_Get_Role_resp_without_role_value;	                                      /**< @brief Pointer to the equivalent data of a BT Type Response that the HM-10 BT device sends back to our MCU/MPU whenever a Get Type request to the HM-10 BT device is processed successfully, but without the type value. */
static char *HM10_Set_Mode_resp_without_module_work_mode_value = HM10_Set_Name_resp_without_name_value;	                          /**< @brief Pointer to the equivalent data of a BT Mode Response that the HM-10 BT device sends back to our MCU/MPU whenever a Set Mode request to the HM-10 BT device is processed successfully, but without the Module Work Mode value. */
static char *HM10_Get_Mode_resp_without_module_work_mode_value = HM10_Get_Role_resp_without_role_value;	                          /**< @brief Pointer to the equivalent data of a BT Mode Response that the HM-10 BT device sends back to our MCU/MPU whenever a Get Mode request to the HM-10 BT device is processed successfully, but without the Module Work Mode value. */
static char *HM10_Set_IMME_resp_without_module_work_type_value = HM10_Set_Name_resp_without_name_value;	                          /**< @brief Pointer to the equivalent data of a BT IMME Response that the HM-10 BT device sends back to our MCU/MPU whenever a Set IMME request to the HM-10 BT device is processed successfully, but without the Module Work Type value. */
static char *HM10_Get_IMME_resp_without_module_work_type_value = HM10_Get_Role_resp_without_role_value;	                          /**< @brief Pointer to the equivalent data of a BT IMME Response that the HM-10 BT device sends back to our MCU/MPU whenever a Get IMME request to the HM-10 BT device is processed successfully, but without the Module Work Type value. */
static char *HM10_Set_NOTI_resp_without_notify_mode_value = HM10_Set_Name_resp_without_name_value;	                              /**< @brief Pointer to the equivalent data of a BT NOTI Response that the HM-10 BT device sends back to our MCU/MPU whenever a Set NOTI request to the HM-10 BT device is processed successfully, but without the Notify Information Mode value. */
static char *HM10_Get_NOTI_resp_without_notify_mode_value = HM10_Get_Role_resp_without_role_value;	                              /**< @brief Pointer to the equivalent data of a BT NOTI Response that the HM-10 BT device sends back to our MCU/MPU whenever a Get NOTI request to the HM-10 BT device is processed successfully, but without the Notify Information Mode value. */
static char HM10_Connect_To_Address_response1[] = {'O', 'K', '+', 'C', 'O', '\0', '\0', 'A'};      /**< @brief Pointer to the equivalent data of a successful connecting BT Connect-To-Address Response that the HM-10 BT device sends back to our MCU/MPU whenever a Connect-To-Address request to the HM-10 BT device is processed successfully and the device is trying to connect to a remote BT. @note The two bytes containing a \c NULL value in this variable's declaration (i.e., indexes 5 and 6) must be changed to the corresponding Bluetooth Address Type with which the Connect-To-Address Command was formulated with. */
static char HM10_Connect_To_Address_response2[] = {'O', 'K', '+', 'C', 'O', 'N', 'N'};                 /**< @brief Pointer to the equivalent data of a successful connected BT Connect-To-Address Response that the HM-10 BT device sends back to our MCU/MPU whenever a Connect-To-Address request to the HM-10 BT device is processed successfully and the device has been able to successfully connect to a remote BT. */
static char HM10_Reset_resp[] = {'O', 'K', '+', 'R', 'E', 'S', 'E', 'T'};				          /**< @brief Pointer to the equivalent data of a Reset Response that the HM-10 BT device sends back to our MCU/MPU whenever a Software Reset request sent to the HM-10 BT device is processed successfully. */
static char HM10_Renew_resp[] = {'O', 'K', '+', 'R', 'E', 'N', 'E', 'W'};				          /**< @brief Pointer to the equivalent data of a Renew Response that the HM-10 BT device sends back to our MCU/MPU whenever a Restore to Factory Setup Request sent to the HM-10 BT device is processed successfully. */
static char HM10_OK_LOST_resp[] = {'O', 'K', '+', 'L', 'O', 'S', 'T'};                                 /**< @brief Pointer to the equivalent data of an OK+LOST Response that the HM-10 BT device sends back to our MCU/MPU whenever, during a Bluetooth Connection, a test request sent to the HM-10 BT device is processed successfully. */
static char HM10_OK_resp[] = {'O', 'K'};				                                                                  /**< @brief Pointer to the equivalent data of an OK Response that the HM-10 BT device sends back to our MCU/MPU whenever a test request sent to the HM-10 BT device is processed successfully. */

/**@brief	Numbers in ASCII code definitions.
 *
 * @details	These definitions contain the decimal equivalent value for each of the number characters available in the
 *          ASCII code table.
 *
 * @note    These definitions are used whenever sending and setting a pin value to the HM-10 BT Device. In particular,
 *          they are used for validating that the pin requested to be set on that Device contains only number characters
 *          available in the ASCII code table.
 */
typedef enum
{
	Number_0_in_ASCII	= 48U,    //!< \f$0_{ASCII} = 48_d\f$.
	Number_1_in_ASCII	= 49U,    //!< \f$1_{ASCII} = 49_d\f$.
	Number_2_in_ASCII	= 50U,    //!< \f$2_{ASCII} = 50_d\f$.
	Number_3_in_ASCII	= 51U,    //!< \f$3_{ASCII} = 51_d\f$.
	Number_4_in_ASCII	= 52U,    //!< \f$4_{ASCII} = 52_d\f$.
	Number_5_in_ASCII	= 53U,    //!< \f$5_{ASCII} = 53_d\f$.
	Number_6_in_ASCII	= 54U,    //!< \f$6_{ASCII} = 54_d\f$.
	Number_7_in_ASCII	= 55U,    //!< \f$7_{ASCII} = 55_d\f$.
	Number_8_in_ASCII	= 56U,    //!< \f$8_{ASCII} = 56_d\f$.
	Number_9_in_ASCII	= 57U     //!< \f$9_{ASCII} = 57_d\f$.
} Numbers_in_ASCII;

/**@brief	Flushes the RX of the UART towards which the @ref p_huart Global Pointer points to.
 *
 * @details This function will poll-receive one byte from the RX of the UART previously mentioned with a timeout of @ref
 *          HM10_CUSTOM_HAL_TIMEOUT over and over until a @ref HAL_TIMEOUT HAL Status is received.
 *
 * @author	César Miranda Meza (cmirandameza3@hotmail.com)
 * @date    November 30, 2023.
 */
static void HAL_uart_rx_flush();

/**@brief	Gets the corresponding @ref HM10_Status value depending on the given @ref HAL_StatusTypeDef value.
 *
 * @param HAL_status	HAL Status value (see @ref HAL_StatusTypeDef ) that wants to be converted into its equivalent
 * 						of a @ref HM10_Status value.
 *
 * @retval				HM10_EC_NR if \p HAL_status param equals \c HAL_BUSY or \c ETX_CUSTOM_HAL_TIMEOUT .
 * @retval				HM10_EC_ERR if \p HAL_status param equals \c HAL_ERROR .
 * @retval				HAL_status param otherwise.
 *
 * @note	For more details on the returned values listed, see @ref HM10_Status and @ref HAL_StatusTypeDef .
 *
 * @author	César Miranda Meza (cmirandameza3@hotmail.com)
 * @date October 17, 2023.
 */
static HM10_Status HAL_ret_handler(HAL_StatusTypeDef HAL_status);

void init_hm10_module(UART_HandleTypeDef *huart)
{
	p_huart = huart;
}

HM10_Status send_hm10_test_cmd()
{
    /** <b>Local variable ret:</b> Return value of either a HAL function or a @ref HM10_Status function type. */
    int16_t  ret;

	/* Flush the UART's RX before starting. */
	HAL_uart_rx_flush();

	/* Populate the HM-10 Device's Test Command into the Tx/Rx Buffer. */
	#if ETX_OTA_VERBOSE
		printf("Sending Test Command to HM-10 BT Device...\r\n");
	#endif
	TxRx_Buffer[0] = 'A';
	TxRx_Buffer[1] = 'T';

	/* Send the HM-10 Device's Test Command. */
	ret = HAL_UART_Transmit(p_huart, TxRx_Buffer, HM10_TEST_CMD_SIZE, HM10_CUSTOM_HAL_TIMEOUT);
	ret = HAL_ret_handler(ret);
	if (ret != HAL_OK)
	{
		#if ETX_OTA_VERBOSE
			printf("ERROR: The transmission of the Test Command to HM-10 BT Device has failed.\r\n");
		#endif
		return ret;
	}

	/* Receive the HM-10 Device's Test Response. */
	ret = HAL_UART_Receive(p_huart, TxRx_Buffer, HM10_OK_RESPONSE_SIZE, HM10_CUSTOM_HAL_TIMEOUT);
	ret = HAL_ret_handler(ret);
	if (ret != HAL_OK)
	{
		#if ETX_OTA_VERBOSE
			printf("ERROR: An OK Response from the HM-10 BT Device was expected, but none was received (HM-10 Exception code = %d)\r\n", ret);
		#endif
		return ret;
	}

	/* Validate the HM-10 Device's Test Response. */
	for (int i=0; i<HM10_OK_RESPONSE_SIZE; i++)
	{
		if (TxRx_Buffer[i] != HM10_OK_resp[i])
		{
			#if ETX_OTA_VERBOSE
				printf("ERROR: An OK Response from the HM-10 BT Device was expected, but something else was received instead.\r\n");
			#endif
			return HM10_EC_ERR;
		}
	}
	#if ETX_OTA_VERBOSE
		printf("DONE: A Test Command has been successfully sent to the HM-10 BT Device.\r\n");
	#endif

	return HM10_EC_OK;
}

HM10_Status send_hm10_reset_cmd()
{
	/** <b>Local variable ret:</b> Return value of either a HAL function or a @ref HM10_Status function type. */
	int16_t  ret;

    /* Flush the UART's RX before starting. */
    HAL_uart_rx_flush();

	/* Populate the HM-10 Device's Reset Command into the Tx/Rx Buffer. */
	#if ETX_OTA_VERBOSE
		printf("Sending Reset Command to HM-10 BT Device...\r\n");
	#endif
	TxRx_Buffer[0] = 'A';
	TxRx_Buffer[1] = 'T';
	TxRx_Buffer[2] = '+';
	TxRx_Buffer[3] = 'R';
	TxRx_Buffer[4] = 'E';
	TxRx_Buffer[5] = 'S';
	TxRx_Buffer[6] = 'E';
	TxRx_Buffer[7] = 'T';

	/* Send the HM-10 Device's Reset Command. */
	ret = HAL_UART_Transmit(p_huart, TxRx_Buffer, HM10_RESET_CMD_SIZE, HM10_CUSTOM_HAL_TIMEOUT);
	ret = HAL_ret_handler(ret);
	if (ret != HAL_OK)
	{
		#if ETX_OTA_VERBOSE
			printf("ERROR: The transmission of the Reset Command to HM-10 BT Device has failed.\r\n");
		#endif
		return ret;
	}

	/* Receive the HM-10 Device's Reset Response. */
	ret = HAL_UART_Receive(p_huart, TxRx_Buffer, HM10_RESET_RESPONSE_SIZE, HM10_CUSTOM_HAL_TIMEOUT);
	ret = HAL_ret_handler(ret);
	if (ret != HAL_OK)
	{
		#if ETX_OTA_VERBOSE
			printf("ERROR: A Reset Response from the HM-10 BT Device was expected, but none was received (HM-10 Exception code = %d)\r\n", ret);
		#endif
		return ret;
	}

	/* Validate the HM-10 Device's Reset Response. */
	for (int i=0; i<HM10_RESET_RESPONSE_SIZE; i++)
	{
		if (TxRx_Buffer[i] != HM10_Reset_resp[i])
		{
			#if ETX_OTA_VERBOSE
				printf("ERROR: A Reset Response from the HM-10 BT Device was expected, but something else was received instead.\r\n");
			#endif
			return HM10_EC_ERR;
		}
	}
	#if ETX_OTA_VERBOSE
		printf("DONE: A Reset Command has been successfully sent to the HM-10 BT Device.\r\n");
	#endif

	/* Generating Delay to allow the HM-10 BT Device to finish resetting correctly before any other action is request to it. */
	HAL_Delay(HM10_RESET_AND_RENEW_CMDS_DELAY);

	return HM10_EC_OK;
}

HM10_Status send_hm10_renew_cmd()
{
    /** <b>Local variable ret:</b> Return value of either a HAL function or a @ref HM10_Status function type. */
    int16_t  ret;

    /* Flush the UART's RX before starting. */
    HAL_uart_rx_flush();

    /* Populate the HM-10 Device's Renew Command into the Tx/Rx Buffer. */
    #if ETX_OTA_VERBOSE
        printf("Sending Renew Command to HM-10 BT Device...\r\n");
    #endif
    TxRx_Buffer[0] = 'A';
    TxRx_Buffer[1] = 'T';
    TxRx_Buffer[2] = '+';
    TxRx_Buffer[3] = 'R';
    TxRx_Buffer[4] = 'E';
    TxRx_Buffer[5] = 'N';
    TxRx_Buffer[6] = 'E';
    TxRx_Buffer[7] = 'W';

    /* Send the HM-10 Device's Renew Command. */
    ret = HAL_UART_Transmit(p_huart, TxRx_Buffer, HM10_RENEW_CMD_SIZE, HM10_CUSTOM_HAL_TIMEOUT);
    ret = HAL_ret_handler(ret);
    if (ret != HAL_OK)
    {
        #if ETX_OTA_VERBOSE
            printf("ERROR: The transmission of the Renew Command to HM-10 BT Device has failed.\r\n");
        #endif
        return ret;
    }

    /* Receive the HM-10 Device's Renew Response. */
    ret = HAL_UART_Receive(p_huart, TxRx_Buffer, HM10_RENEW_RESPONSE_SIZE, HM10_CUSTOM_HAL_TIMEOUT);
    ret = HAL_ret_handler(ret);
    if (ret != HAL_OK)
    {
        #if ETX_OTA_VERBOSE
            printf("ERROR: A Renew Response from the HM-10 BT Device was expected, but none was received (HM-10 Exception code = %d)\r\n", ret);
        #endif
        return ret;
    }

    /* Validate the HM-10 Device's Renew Response. */
    for (int i=0; i<HM10_RENEW_RESPONSE_SIZE; i++)
    {
        if (TxRx_Buffer[i] != HM10_Renew_resp[i])
        {
            #if ETX_OTA_VERBOSE
                printf("ERROR: A Renew Response from the HM-10 BT Device was expected, but something else was received instead.\r\n");
            #endif
            return HM10_EC_ERR;
        }
    }
	#if ETX_OTA_VERBOSE
        printf("DONE: A Renew Command has been successfully sent to the HM-10 BT Device.\r\n");
    #endif

    /* Generating Delay to allow the HM-10 BT Device to finish renewing correctly before any other action is request to it. */
    HAL_Delay(HM10_RESET_AND_RENEW_CMDS_DELAY);

    return HM10_EC_OK;
}

HM10_Status set_hm10_name(uint8_t *hm10_name, uint8_t size)
{
	/* Validating given name. */
	if (size > HM10_MAX_BLE_NAME_SIZE)
	{
		#if ETX_OTA_VERBOSE
			printf("ERROR: Requested BT Name must not exceed a length of %d bytes (i.e., %d ASCII Characters).\r\n", HM10_MAX_BLE_NAME_SIZE, HM10_MAX_BLE_NAME_SIZE);
		#endif
		return HM10_EC_ERR;
	}

	/** <b>Local variable ret:</b> Return value of either a HAL function or a @ref HM10_Status function type. */
	int16_t  ret;
	/** <b>Local variable bytes_populated_in_TxRx_Buffer:</b> Currently populated bytes of data into the Tx/Rx Global Buffer. */
	uint8_t bytes_populated_in_TxRx_Buffer = 0;

    /* Flush the UART's RX before starting. */
    HAL_uart_rx_flush();

	/* Populate the HM-10 Device's Set Name Command into the Tx/Rx Buffer. */
	#if ETX_OTA_VERBOSE
		printf("Sending Set Name Command to HM-10 BT Device...\r\n");
	#endif
	TxRx_Buffer[bytes_populated_in_TxRx_Buffer++] = 'A';
	TxRx_Buffer[bytes_populated_in_TxRx_Buffer++] = 'T';
	TxRx_Buffer[bytes_populated_in_TxRx_Buffer++] = '+';
	TxRx_Buffer[bytes_populated_in_TxRx_Buffer++] = 'N';
	TxRx_Buffer[bytes_populated_in_TxRx_Buffer++] = 'A';
	TxRx_Buffer[bytes_populated_in_TxRx_Buffer++] = 'M';
	TxRx_Buffer[bytes_populated_in_TxRx_Buffer++] = 'E';

	/** <b>Local variable size_with_offset:</b> Either the total size in bytes stated at the \p size param plus the bytes populated in the Tx/Rx Buffer for only the Name Command, or the total size in bytes stated at the \p size param plus the bytes read from the Tx/Rx Buffer for only the Name Response. */
	uint8_t size_with_offset = size + bytes_populated_in_TxRx_Buffer;
	for (uint8_t i=0; bytes_populated_in_TxRx_Buffer<size_with_offset; i++)
	{
		TxRx_Buffer[bytes_populated_in_TxRx_Buffer++] = hm10_name[i];
	}

	/* Send the HM-10 Device's Set Name Command. */
	ret = HAL_UART_Transmit(p_huart, TxRx_Buffer, bytes_populated_in_TxRx_Buffer, HM10_CUSTOM_HAL_TIMEOUT);
	ret = HAL_ret_handler(ret);
	if (ret != HAL_OK)
	{
		#if ETX_OTA_VERBOSE
			printf("ERROR: The transmission of the Set Name Command to HM-10 BT Device has failed.\r\n");
		#endif
		return ret;
	}

	/* Receive the HM-10 Device's Set Name Response. */
	bytes_populated_in_TxRx_Buffer = HM10_SET_NAME_RESPONSE_SIZE_WITHOUT_REQUESTED_NAME + size;
	ret = HAL_UART_Receive(p_huart, TxRx_Buffer, bytes_populated_in_TxRx_Buffer, HM10_CUSTOM_HAL_TIMEOUT);
	ret = HAL_ret_handler(ret);
	if (ret != HAL_OK)
	{
		#if ETX_OTA_VERBOSE
			printf("ERROR: A Set Name Response from the HM-10 BT Device was expected, but none was received (HM-10 Exception code = %d)\r\n", ret);
		#endif
		return ret;
	}

	/* Validate the HM-10 Device's Set Name Response. */
	/** <b>Local variable bytes_compared:</b> Counter for the bytes that have been compared and validated to match between the received Set Name Response (which should be stored in @ref TxRx_Buffer buffer ) and the expected Set Name Response (i.e., @ref HM10_Set_Name_resp_without_name_value ). */
	uint8_t bytes_compared = 0;
	for (; bytes_compared<HM10_SET_NAME_RESPONSE_SIZE_WITHOUT_REQUESTED_NAME; bytes_compared++)
	{
		if (TxRx_Buffer[bytes_compared] != HM10_Set_Name_resp_without_name_value[bytes_compared])
		{
			#if ETX_OTA_VERBOSE
				printf("ERROR: A Set Name Response from the HM-10 BT Device was expected, but something else was received instead.\r\n");
			#endif
			return HM10_EC_ERR;
		}
	}
	size_with_offset = size + bytes_compared;
	for (uint8_t i=0; bytes_compared<size_with_offset; i++)
	{
		if (TxRx_Buffer[bytes_compared++] != hm10_name[i])
		{
			#if ETX_OTA_VERBOSE
				printf("ERROR: A Set Name Response from the HM-10 BT Device was expected, but something else was received instead.\r\n");
			#endif
			return HM10_EC_ERR;
		}
	}
	#if ETX_OTA_VERBOSE
		printf("DONE: A BT Name has been successfully set in the HM-10 BT Device.\r\n");
	#endif

	return HM10_EC_OK;
}

HM10_Status get_hm10_name(uint8_t *hm10_name, uint8_t *size)
{
	/** <b>Local variable ret:</b> Return value of either a HAL function or a @ref HM10_Status function type. */
	int16_t  ret;

    /* Flush the UART's RX before starting. */
    HAL_uart_rx_flush();

	/* Populate the HM-10 Device's Get Name Command into the Tx/Rx Buffer. */
	#if ETX_OTA_VERBOSE
		printf("Sending Get Name Command to HM-10 BT Device...\r\n");
	#endif
	TxRx_Buffer[0] = 'A';
	TxRx_Buffer[1] = 'T';
	TxRx_Buffer[2] = '+';
	TxRx_Buffer[3] = 'N';
	TxRx_Buffer[4] = 'A';
	TxRx_Buffer[5] = 'M';
	TxRx_Buffer[6] = 'E';
	TxRx_Buffer[7] = '?';

	/* Send the HM-10 Device's Get Name Command. */
	ret = HAL_UART_Transmit(p_huart, TxRx_Buffer, HM10_GET_NAME_CMD_SIZE, HM10_CUSTOM_HAL_TIMEOUT);
	ret = HAL_ret_handler(ret);
	if (ret != HAL_OK)
	{
		#if ETX_OTA_VERBOSE
			printf("ERROR: The transmission of the Get Name Command to HM-10 BT Device has failed.\r\n");
		#endif
		return ret;
	}

	/* Receive the HM-10 Device's Get Name Response but just before the BT Name bytes. */
	ret = HAL_UART_Receive(p_huart, TxRx_Buffer, HM10_GET_NAME_RESPONSE_SIZE_WITHOUT_REQUESTED_NAME, HM10_CUSTOM_HAL_TIMEOUT);
	ret = HAL_ret_handler(ret);
	if (ret != HAL_OK)
	{
		#if ETX_OTA_VERBOSE
			printf("ERROR: A Get Name Response from the HM-10 BT Device was expected, but none was received (HM-10 Exception code = %d)\r\n", ret);
		#endif
		return ret;
	}

	/* Validate the HM-10 Device's Get Name Response part that was just received (i.e., The received data just before the BT Name bytes part). */
	/** <b>Local variable bytes_populated_in_TxRx_Buffer:</b> Currently populated bytes of data into the Tx/Rx Global Buffer. */
	uint8_t bytes_populated_in_TxRx_Buffer = 0;
	for (; bytes_populated_in_TxRx_Buffer<HM10_GET_NAME_RESPONSE_SIZE_WITHOUT_REQUESTED_NAME; bytes_populated_in_TxRx_Buffer++)
	{
		if (TxRx_Buffer[bytes_populated_in_TxRx_Buffer] != HM10_Get_Name_resp_without_name_value[bytes_populated_in_TxRx_Buffer])
		{
			#if ETX_OTA_VERBOSE
				printf("ERROR: A Get Name Response from the HM-10 BT Device was expected, but something else was received instead.\r\n");
			#endif
			return HM10_EC_ERR;
		}
	}

	/* Receive the BT Name bytes part from the HM-10 Device's Get Name Response. */
	*size = 0;
	do
	{
		/* Receive the next byte from the BT Name. */
		ret = HAL_UART_Receive(p_huart, &TxRx_Buffer[bytes_populated_in_TxRx_Buffer++], 1, HM10_CUSTOM_HAL_TIMEOUT);
		(*size)++;
		ret = HAL_ret_handler(ret);
		if (ret != HAL_OK)
		{
			#if ETX_OTA_VERBOSE
				printf("ERROR: A BT Name from the HM-10 BT Device was expected, but none was received (HM-10 Exception code = %d)\r\n", ret);
            #endif
            *size = 0;
			return ret;
		}

		/* Check if the BT Name has been completely received. */
		if (TxRx_Buffer[bytes_populated_in_TxRx_Buffer-1] == '\0') // NOTE: '\0' == NULL
		{
            if (*size == 1)
            {
                *size = 0;
            }
            else
            {
                (*size)--;
            }
			break;
		}

		/* Validate the BT Name bytes that have been received so far. */
		if (bytes_populated_in_TxRx_Buffer == (HM10_MAX_BLE_NAME_SIZE + HM10_GET_NAME_RESPONSE_SIZE_WITHOUT_REQUESTED_NAME + 1)) // NOTE: The +1 is for the NULL value that is expected be at the end of the response.
		{
			ret = HM10_EC_ERR;
			#if ETX_OTA_VERBOSE
				printf("ERROR: A BT Name, with a maximum size of %d, from the HM-10 BT Device was expected, but a larger name was received instead.\r\n", HM10_MAX_BLE_NAME_SIZE);
			#endif
			return ret;
		}
	}
	while (1);

	/* Pass the BT Name from the Buffer that is storing it into the \p hm10_name param. */
	memcpy(hm10_name, &TxRx_Buffer[HM10_GET_NAME_RESPONSE_SIZE_WITHOUT_REQUESTED_NAME], *size);

	#if ETX_OTA_VERBOSE
		printf("DONE: The BT Name has been successfully received from the HM-10 BT Device.\r\n");
	#endif

	return HM10_EC_OK;
}

HM10_Status set_hm10_role(HM10_Role ble_role)
{
	/* Validating given role. */
	switch (ble_role)
	{
		case HM10_Role_Peripheral:
		case HM10_Role_Central:
			break;
		default:
			#if ETX_OTA_VERBOSE
				printf("ERROR: Requested BT Role %d is not recognized.\r\n", ble_role);
			#endif
			return HM10_EC_ERR;
	}

	/** <b>Local variable ret:</b> Return value of either a HAL function or a @ref HM10_Status function type. */
	int16_t  ret;

    /* Flush the UART's RX before starting. */
    HAL_uart_rx_flush();

	/* Populate the HM-10 Device's Set Role Command into the Tx/Rx Buffer. */
	#if ETX_OTA_VERBOSE
		printf("Sending Set Role Command to HM-10 BT Device...\r\n");
	#endif
	TxRx_Buffer[0] = 'A';
	TxRx_Buffer[1] = 'T';
	TxRx_Buffer[2] = '+';
	TxRx_Buffer[3] = 'R';
	TxRx_Buffer[4] = 'O';
	TxRx_Buffer[5] = 'L';
	TxRx_Buffer[6] = 'E';
	TxRx_Buffer[7] = ble_role;

	/* Send the HM-10 Device's Set Role Command. */
	ret = HAL_UART_Transmit(p_huart, TxRx_Buffer, HM10_SET_ROLE_CMD_SIZE, HM10_CUSTOM_HAL_TIMEOUT);
	ret = HAL_ret_handler(ret);
	if (ret != HAL_OK)
	{
		#if ETX_OTA_VERBOSE
			printf("ERROR: The transmission of the Set Role Command to HM-10 BT Device has failed.\r\n");
		#endif
		return ret;
	}

	/* Receive the HM-10 Device's Set Role Response. */
	ret = HAL_UART_Receive(p_huart, TxRx_Buffer, HM10_ROLE_RESPONSE_SIZE, HM10_CUSTOM_HAL_TIMEOUT);
	ret = HAL_ret_handler(ret);
	if (ret != HAL_OK)
	{
		#if ETX_OTA_VERBOSE
			printf("ERROR: A Set Role Response from the HM-10 BT Device was expected, but none was received (HM-10 Exception code = %d)\r\n", ret);
		#endif
		return ret;
	}

	/* Validate the HM-10 Device's Set Role Response. */
	/** <b>Local variable role_resp_size_without_role_value:</b> Size in bytes of the Role Response from the HM-10 BT device but without considering the length of the requested role value. */
	uint8_t role_resp_size_without_role_value = HM10_ROLE_RESPONSE_SIZE - 1;
	/** <b>Local variable bytes_compared:</b> Counter for the bytes that have been compared and validated to match between the received Set Role Response (which should be stored in @ref TxRx_Buffer buffer ) and the expected Set Role Response (i.e., @ref HM10_Set_Role_resp_without_role_value ). */
	uint8_t bytes_compared = 0;
	for (; bytes_compared<role_resp_size_without_role_value; bytes_compared++)
	{
		if (TxRx_Buffer[bytes_compared] != HM10_Set_Role_resp_without_role_value[bytes_compared])
		{
			#if ETX_OTA_VERBOSE
				printf("ERROR: A Set Role Response from the HM-10 BT Device was expected, but something else was received instead.\r\n");
			#endif
			return HM10_EC_ERR;
		}
	}
	if (TxRx_Buffer[bytes_compared] != ble_role)
	{
		#if ETX_OTA_VERBOSE
			printf("ERROR: A Set Role Response from the HM-10 BT Device was expected, but something else was received instead.\r\n");
		#endif
		return HM10_EC_ERR;
	}
	#if ETX_OTA_VERBOSE
		printf("DONE: A Role has been successfully set in the HM-10 BT Device.\r\n");
	#endif

	return HM10_EC_OK;
}

HM10_Status get_hm10_role(HM10_Role *ble_role)
{
	/** <b>Local variable ret:</b> Return value of either a HAL function or a @ref HM10_Status function type. */
	int16_t  ret;

    /* Flush the UART's RX before starting. */
    HAL_uart_rx_flush();

    /* Populate the HM-10 Device's Get Role Command into the Tx/Rx Buffer. */
	#if ETX_OTA_VERBOSE
		printf("Sending Get Role Command to HM-10 BT Device...\r\n");
	#endif
	TxRx_Buffer[0] = 'A';
	TxRx_Buffer[1] = 'T';
	TxRx_Buffer[2] = '+';
	TxRx_Buffer[3] = 'R';
	TxRx_Buffer[4] = 'O';
	TxRx_Buffer[5] = 'L';
	TxRx_Buffer[6] = 'E';
	TxRx_Buffer[7] = '?';

	/* Send the HM-10 Device's Get Role Command. */
	ret = HAL_UART_Transmit(p_huart, TxRx_Buffer, HM10_GET_ROLE_CMD_SIZE, HM10_CUSTOM_HAL_TIMEOUT);
	ret = HAL_ret_handler(ret);
	if (ret != HAL_OK)
	{
		#if ETX_OTA_VERBOSE
			printf("ERROR: The transmission of the Get Role Command to HM-10 BT Device has failed.\r\n");
		#endif
		return ret;
	}

	/* Receive the HM-10 Device's Get Role Response. */
	ret = HAL_UART_Receive(p_huart, TxRx_Buffer, HM10_ROLE_RESPONSE_SIZE, HM10_CUSTOM_HAL_TIMEOUT);
	ret = HAL_ret_handler(ret);
	if (ret != HAL_OK)
	{
		#if ETX_OTA_VERBOSE
			printf("ERROR: A Get Role Response from the HM-10 BT Device was expected, but none was received (HM-10 Exception code = %d)\r\n", ret);
		#endif
		return ret;
	}

	/* Validate the HM-10 Device's Get Role Response. */
	/** <b>Local variable role_resp_size_without_role_value:</b> Size in bytes of the Role Response from the HM-10 BT device but without considering the length of the role value. */
	uint8_t role_resp_size_without_role_value = HM10_ROLE_RESPONSE_SIZE - 1;
	/** <b>Local variable bytes_compared:</b> Counter for the bytes that have been compared and validated to match between the received Get Role Response (which should be stored in @ref TxRx_Buffer buffer ) and the expected Get Role Response (i.e., @ref HM10_Get_Role_resp_without_role_value ). */
	uint8_t bytes_compared = 0;
	for (; bytes_compared<role_resp_size_without_role_value; bytes_compared++)
	{
		if (TxRx_Buffer[bytes_compared] != HM10_Get_Role_resp_without_role_value[bytes_compared])
		{
			#if ETX_OTA_VERBOSE
				printf("ERROR: A Get Role Response from the HM-10 BT Device was expected, but something else was received instead.\r\n");
			#endif
			return HM10_EC_ERR;
		}
	}
	switch (TxRx_Buffer[bytes_compared])
	{
		case HM10_Role_Peripheral:
		case HM10_Role_Central:
			break;
		default:
			#if ETX_OTA_VERBOSE
				printf("ERROR: Received BT Role %d is not recognized.\r\n", TxRx_Buffer[bytes_compared]);
			#endif
			return HM10_EC_ERR;
	}
	*ble_role = TxRx_Buffer[bytes_compared];

	#if ETX_OTA_VERBOSE
		printf("DONE: The BT Role has been successfully received from the HM-10 BT Device.\r\n");
	#endif

	return HM10_EC_OK;
}

HM10_Status set_hm10_pin(uint8_t *pin)
{
	/* Validating given pin. */
	for (uint8_t current_pin_character=0; current_pin_character<HM10_PIN_VALUE_SIZE; current_pin_character++)
	{
		switch (pin[current_pin_character])
		{
			case Number_0_in_ASCII:
			case Number_1_in_ASCII:
			case Number_2_in_ASCII:
			case Number_3_in_ASCII:
			case Number_4_in_ASCII:
			case Number_5_in_ASCII:
			case Number_6_in_ASCII:
			case Number_7_in_ASCII:
			case Number_8_in_ASCII:
			case Number_9_in_ASCII:
				break;
			default:
				#if ETX_OTA_VERBOSE
					printf("ERROR: Expected a number character value in ASCII code on given pin value at index %d, but the following ASCII value was given instead: %c.\r\n", current_pin_character, pin[current_pin_character]);
				#endif
				return HM10_EC_ERR;
		}
	}

	/** <b>Local variable ret:</b> Return value of either a HAL function or a @ref HM10_Status function type. */
	int16_t  ret;
	/** <b>Local variable bytes_populated_in_TxRx_Buffer:</b> Currently populated bytes of data into the Tx/Rx Global Buffer. */
	uint8_t bytes_populated_in_TxRx_Buffer = 0;

    /* Flush the UART's RX before starting. */
    HAL_uart_rx_flush();

	/* Populate the HM-10 Device's Set Pin Command into the Tx/Rx Buffer. */
	#if ETX_OTA_VERBOSE
		printf("Sending Set Pin Command to HM-10 BT Device...\r\n");
	#endif
	TxRx_Buffer[bytes_populated_in_TxRx_Buffer++] = 'A';
	TxRx_Buffer[bytes_populated_in_TxRx_Buffer++] = 'T';
	TxRx_Buffer[bytes_populated_in_TxRx_Buffer++] = '+';
	TxRx_Buffer[bytes_populated_in_TxRx_Buffer++] = 'P';
	TxRx_Buffer[bytes_populated_in_TxRx_Buffer++] = 'A';
	TxRx_Buffer[bytes_populated_in_TxRx_Buffer++] = 'S';
    TxRx_Buffer[bytes_populated_in_TxRx_Buffer++] = 'S';
	for (uint8_t current_pin_character=0; bytes_populated_in_TxRx_Buffer<HM10_SET_PIN_CMD_SIZE; current_pin_character++)
	{
		TxRx_Buffer[bytes_populated_in_TxRx_Buffer++] = pin[current_pin_character];
	}

	/* Send the HM-10 Device's Set Pin Command. */
	ret = HAL_UART_Transmit(p_huart, TxRx_Buffer, HM10_SET_PIN_CMD_SIZE, HM10_CUSTOM_HAL_TIMEOUT);
	ret = HAL_ret_handler(ret);
	if (ret != HAL_OK)
	{
		#if ETX_OTA_VERBOSE
			printf("ERROR: The transmission of the Set Pin Command to HM-10 BT Device has failed.\r\n");
		#endif
		return ret;
	}

	/* Receive the HM-10 Device's Set Pin Response. */
	ret = HAL_UART_Receive(p_huart, TxRx_Buffer, HM10_PIN_RESPONSE_SIZE, HM10_CUSTOM_HAL_TIMEOUT);
	ret = HAL_ret_handler(ret);
	if (ret != HAL_OK)
	{
		#if ETX_OTA_VERBOSE
			printf("ERROR: A Set Pin Response from the HM-10 BT Device was expected, but none was received (HM-10 Exception code = %d)\r\n", ret);
		#endif
		return ret;
	}

	/* Validate the HM-10 Device's Set Pin Response. */
	/** <b>Local variable pin_resp_size_without_pin_value:</b> Size in bytes of the Pin Response from the HM-10 BT device but without considering the length of the pin value. */
	uint8_t pin_resp_size_without_pin_value = HM10_PIN_RESPONSE_SIZE - HM10_PIN_VALUE_SIZE;
	/** <b>Local variable bytes_compared:</b> Counter for the bytes that have been compared and validated to match between the received Set Pin Response (which should be stored in @ref TxRx_Buffer buffer ) and the expected Set Pin Response (i.e., @ref HM10_Set_Pin_resp_without_pin_value ). */
	uint8_t bytes_compared = 0;
	for (; bytes_compared<pin_resp_size_without_pin_value; bytes_compared++)
	{
		if (TxRx_Buffer[bytes_compared] != HM10_Set_Pin_resp_without_pin_value[bytes_compared])
		{
			#if ETX_OTA_VERBOSE
				printf("ERROR: A Set Pin Response from the HM-10 BT Device was expected, but something else was received instead.\r\n");
			#endif
			return HM10_EC_ERR;
		}
	}
	for (uint8_t current_pin_character=0; bytes_compared<HM10_PIN_RESPONSE_SIZE; current_pin_character++)
	{
		if (TxRx_Buffer[bytes_compared++] != pin[current_pin_character])
		{
			#if ETX_OTA_VERBOSE
				printf("ERROR: A Set Pin Response from the HM-10 BT Device was expected, but something else was received instead.\r\n");
			#endif
			return HM10_EC_ERR;
		}
	}

	#if ETX_OTA_VERBOSE
		printf("DONE: A BT Pin has been successfully set in the HM-10 BT Device.\r\n");
	#endif

	return HM10_EC_OK;
}

HM10_Status get_hm10_pin(uint8_t *pin)
{
	/** <b>Local variable ret:</b> Return value of either a HAL function or a @ref HM10_Status function type. */
	int16_t  ret;

    /* Flush the UART's RX before starting. */
    HAL_uart_rx_flush();

	/* Populate the HM-10 Device's Get Pin Command into the Tx/Rx Buffer. */
	#if ETX_OTA_VERBOSE
		printf("Sending Get Pin Command to HM-10 BT Device...\r\n");
	#endif
	TxRx_Buffer[0] = 'A';
	TxRx_Buffer[1] = 'T';
	TxRx_Buffer[2] = '+';
	TxRx_Buffer[3] = 'P';
	TxRx_Buffer[4] = 'A';
	TxRx_Buffer[5] = 'S';
	TxRx_Buffer[6] = 'S';
	TxRx_Buffer[7] = '?';

	/* Send the HM-10 Device's Get Pin Command. */
	ret = HAL_UART_Transmit(p_huart, TxRx_Buffer, HM10_GET_PIN_CMD_SIZE, HM10_CUSTOM_HAL_TIMEOUT);
	ret = HAL_ret_handler(ret);
	if (ret != HAL_OK)
	{
		#if ETX_OTA_VERBOSE
			printf("ERROR: The transmission of the Get Pin Command to HM-10 BT Device has failed.\r\n");
		#endif
		return ret;
	}

	/* Receive the HM-10 Device's Get Pin Response. */
	ret = HAL_UART_Receive(p_huart, TxRx_Buffer, HM10_PIN_RESPONSE_SIZE, HM10_CUSTOM_HAL_TIMEOUT);
	ret = HAL_ret_handler(ret);
	if (ret != HAL_OK)
	{
		#if ETX_OTA_VERBOSE
			printf("ERROR: A Get Pin Response from the HM-10 BT Device was expected, but none was received (HM-10 Exception code = %d)\r\n", ret);
		#endif
		return ret;
	}

	/* Validate the HM-10 Device's Get Pin Response. */
	/** <b>Local variable pin_resp_size_without_pin_value:</b> Size in bytes of the Pin Response from the HM-10 BT device but without considering the length of the pin value. */
	uint8_t pin_resp_size_without_pin_value = HM10_PIN_RESPONSE_SIZE - HM10_PIN_VALUE_SIZE;
	/** <b>Local variable bytes_compared:</b> Counter for the bytes that have been compared and validated to match between the received Get Pin Response (which should be stored in @ref TxRx_Buffer buffer ) and the expected Get Pin Response (i.e., @ref HM10_Get_Pin_resp_without_pin_value ). */
	uint8_t bytes_compared = 0;
	for (; bytes_compared<pin_resp_size_without_pin_value; bytes_compared++)
	{
		if (TxRx_Buffer[bytes_compared] != HM10_Get_Pin_resp_without_pin_value[bytes_compared])
		{
			#if ETX_OTA_VERBOSE
				printf("ERROR: A Get Pin Response from the HM-10 BT Device was expected, but something else was received instead.\r\n");
			#endif
			return HM10_EC_ERR;
		}
	}
	for (uint8_t current_pin_character=0; current_pin_character<HM10_PIN_VALUE_SIZE; current_pin_character++)
	{
		switch (TxRx_Buffer[bytes_compared])
		{
			case Number_0_in_ASCII:
			case Number_1_in_ASCII:
			case Number_2_in_ASCII:
			case Number_3_in_ASCII:
			case Number_4_in_ASCII:
			case Number_5_in_ASCII:
			case Number_6_in_ASCII:
			case Number_7_in_ASCII:
			case Number_8_in_ASCII:
			case Number_9_in_ASCII:
				break;
			default:
				#if ETX_OTA_VERBOSE
					printf("ERROR: Expected a number character value in ASCII code on index %d of TxRx-Buffer, where the following ASCII value was received instead: %c.\r\n", bytes_compared, TxRx_Buffer[bytes_compared]);
				#endif
				return HM10_EC_ERR;
		}
		bytes_compared++;
	}

	/* Pass the BT Pin from the Buffer that is storing it into the \p pin param. */
	memcpy(pin, &TxRx_Buffer[pin_resp_size_without_pin_value], HM10_PIN_VALUE_SIZE);

	#if ETX_OTA_VERBOSE
		printf("DONE: The BT Pin has been successfully received from the HM-10 BT Device.\r\n");
	#endif

	return HM10_EC_OK;
}

HM10_Status set_hm10_pin_code_mode(HM10_Pin_Code_Mode pin_code_mode)
{
	/* Validating given pin code mode. */
	switch (pin_code_mode)
	{
		case HM10_Pin_Code_DISABLED:
		case HM10_Pin_Code_ENABLED:
			break;
		default:
			#if ETX_OTA_VERBOSE
				printf("ERROR: An invalid pin code mode value has been given: %c_ASCII.\r\n", pin_code_mode);
			#endif
			return HM10_EC_ERR;
	}

	/** <b>Local variable ret:</b> Return value of either a HAL function or a @ref HM10_Status function type. */
	int16_t  ret;

    /* Flush the UART's RX before starting. */
    HAL_uart_rx_flush();

	/* Populate the HM-10 Device's Set Type Command into the Tx/Rx Buffer. */
	#if ETX_OTA_VERBOSE
		printf("Sending Set Type Command to HM-10 BT Device...\r\n");
	#endif
	TxRx_Buffer[0] = 'A';
	TxRx_Buffer[1] = 'T';
	TxRx_Buffer[2] = '+';
	TxRx_Buffer[3] = 'T';
	TxRx_Buffer[4] = 'Y';
	TxRx_Buffer[5] = 'P';
	TxRx_Buffer[6] = 'E';
	TxRx_Buffer[7] = pin_code_mode;

	/* Send the HM-10 Device's Set Type Command. */
	ret = HAL_UART_Transmit(p_huart, TxRx_Buffer, HM10_SET_TYPE_CMD_SIZE, HM10_CUSTOM_HAL_TIMEOUT);
	ret = HAL_ret_handler(ret);
	if (ret != HAL_OK)
	{
		#if ETX_OTA_VERBOSE
			printf("ERROR: The transmission of the Set Type Command to HM-10 BT Device has failed.\r\n");
		#endif
		return ret;
	}

	/* Receive the HM-10 Device's Set Type Response. */
	ret = HAL_UART_Receive(p_huart, TxRx_Buffer, HM10_TYPE_RESPONSE_SIZE, HM10_CUSTOM_HAL_TIMEOUT);
	ret = HAL_ret_handler(ret);
	if (ret != HAL_OK)
	{
		#if ETX_OTA_VERBOSE
			printf("ERROR: A Set Type Response from the HM-10 BT Device was expected, but none was received (HM-10 Exception code = %d)\r\n", ret);
		#endif
		return ret;
	}

	/* Validate the HM-10 Device's Set Type Response. */
	/** <b>Local variable type_resp_size_without_pin_mode_value:</b> Size in bytes of the Type Response from the HM-10 BT device but without considering the length of the Type value. */
	uint8_t type_resp_size_without_pin_mode_value = HM10_TYPE_RESPONSE_SIZE - 1;
	/** <b>Local variable bytes_compared:</b> Counter for the bytes that have been compared and validated to match between the received Set Type Response (which should be stored in @ref TxRx_Buffer buffer ) and the expected Set Type Response (i.e., @ref HM10_Set_Type_resp_without_type_value ). */
	uint8_t bytes_compared = 0;
	for (; bytes_compared<type_resp_size_without_pin_mode_value; bytes_compared++)
	{
		if (TxRx_Buffer[bytes_compared] != HM10_Set_Type_resp_without_type_value[bytes_compared])
		{
			#if ETX_OTA_VERBOSE
				printf("ERROR: A Set Type Response from the HM-10 BT Device was expected, but something else was received instead at index %d. The received value was %c_ASCII and the expected value is %c_ASCII.\r\n", bytes_compared, TxRx_Buffer[bytes_compared], HM10_Set_Type_resp_without_type_value[bytes_compared]);
			#endif
			return HM10_EC_ERR;
		}
	}
	if (TxRx_Buffer[bytes_compared] != pin_code_mode)
	{
		#if ETX_OTA_VERBOSE
		printf("ERROR: A Set Type Response from the HM-10 BT Device was expected, but something else was received instead at index %d. The received value was %c_ASCII and the expected value is %c_ASCII.\r\n", bytes_compared, TxRx_Buffer[bytes_compared], pin_code_mode);
		#endif
		return HM10_EC_ERR;
	}

	#if ETX_OTA_VERBOSE
		printf("DONE: The desired Pin Code Mode has been successfully set in the HM-10 BT Device.\r\n");
	#endif

	return HM10_EC_OK;
}

HM10_Status get_hm10_pin_code_mode(HM10_Pin_Code_Mode *pin_code_mode)
{
	/** <b>Local variable ret:</b> Return value of either a HAL function or a @ref HM10_Status function type. */
	int16_t  ret;

    /* Flush the UART's RX before starting. */
    HAL_uart_rx_flush();

	/* Populate the HM-10 Device's Get Type Command into the Tx/Rx Buffer. */
	#if ETX_OTA_VERBOSE
		printf("Sending Get Type Command to HM-10 BT Device...\r\n");
	#endif
	TxRx_Buffer[0] = 'A';
	TxRx_Buffer[1] = 'T';
	TxRx_Buffer[2] = '+';
	TxRx_Buffer[3] = 'T';
	TxRx_Buffer[4] = 'Y';
	TxRx_Buffer[5] = 'P';
	TxRx_Buffer[6] = 'E';
	TxRx_Buffer[7] = '?';

	/* Send the HM-10 Device's Get Type Command. */
	ret = HAL_UART_Transmit(p_huart, TxRx_Buffer, HM10_GET_TYPE_CMD_SIZE, HM10_CUSTOM_HAL_TIMEOUT);
	ret = HAL_ret_handler(ret);
	if (ret != HAL_OK)
	{
		#if ETX_OTA_VERBOSE
			printf("ERROR: The transmission of the Get Type Command to HM-10 BT Device has failed.\r\n");
		#endif
		return ret;
	}

	/* Receive the HM-10 Device's Get Type Response. */
	ret = HAL_UART_Receive(p_huart, TxRx_Buffer, HM10_TYPE_RESPONSE_SIZE, HM10_CUSTOM_HAL_TIMEOUT);
	ret = HAL_ret_handler(ret);
	if (ret != HAL_OK)
	{
		#if ETX_OTA_VERBOSE
			printf("ERROR: A Get Type Response from the HM-10 BT Device was expected, but none was received (HM-10 Exception code = %d)\r\n", ret);
		#endif
		return ret;
	}

	/* Validate the HM-10 Device's Get Type Response. */
	/** <b>Local variable type_resp_size_without_type_value:</b> Size in bytes of the Type Response from the HM-10 BT device but without considering the length of the Type value. */
	uint8_t type_resp_size_without_type_value = HM10_TYPE_RESPONSE_SIZE - 1;
	/** <b>Local variable bytes_compared:</b> Counter for the bytes that have been compared and validated to match between the received Get Type Response (which should be stored in @ref TxRx_Buffer buffer ) and the expected Get Type Response (i.e., @ref HM10_Get_Type_resp_without_type_value ). */
	uint8_t bytes_compared = 0;
	for (; bytes_compared<type_resp_size_without_type_value; bytes_compared++)
	{
		if (TxRx_Buffer[bytes_compared] != HM10_Get_Type_resp_without_type_value[bytes_compared])
		{
			#if ETX_OTA_VERBOSE
				printf("ERROR: A Get Type Response from the HM-10 BT Device was expected, but something else was received instead.\r\n");
			#endif
			return HM10_EC_ERR;
		}
	}
	switch (TxRx_Buffer[bytes_compared])
	{
		case HM10_Pin_Code_DISABLED:
		case HM10_Pin_Code_ENABLED:
			break;
		default:
			#if ETX_OTA_VERBOSE
				printf("ERROR: An invalid pin code mode value has been received from the HM-10 BT Device: %c_ASCII.\r\n", TxRx_Buffer[bytes_compared]);
			#endif
			return HM10_EC_ERR;
	}

	/* Pass the BT Pin Code Mode from the Buffer that is storing it into the \p pin_code_mode param. */
	*pin_code_mode = TxRx_Buffer[type_resp_size_without_type_value];

	#if ETX_OTA_VERBOSE
		printf("DONE: The BT Pin Code Mode has been successfully received from the HM-10 BT Device.\r\n");
	#endif

	return HM10_EC_OK;
}

HM10_Status set_hm10_module_work_mode(HM10_Module_Work_Mode module_work_mode)
{
    /* Validating given module work mode. */
    switch (module_work_mode)
    {
        case HM10_Transmission_Mode:
        case HM10_PIO_Collection_and_Transmission_Mode:
        case HM10_PIO_Remote_Control_and_Transmission_Mode:
            break;
        default:
            #if ETX_OTA_VERBOSE
                printf("ERROR: An invalid module work mode value has been given: %c_ASCII.\r\n", module_work_mode);
            #endif
            return HM10_EC_ERR;
    }

    /** <b>Local variable ret:</b> Return value of either a HAL function or a @ref HM10_Status function type. */
    int16_t  ret;

    /* Flush the UART's RX before starting. */
    HAL_uart_rx_flush();

    /* Populate the HM-10 Device's Set Mode Command into the Tx/Rx Buffer. */
    #if ETX_OTA_VERBOSE
        printf("Sending Set Mode Command to HM-10 BT Device...\r\n");
    #endif
    TxRx_Buffer[0] = 'A';
    TxRx_Buffer[1] = 'T';
    TxRx_Buffer[2] = '+';
    TxRx_Buffer[3] = 'M';
    TxRx_Buffer[4] = 'O';
    TxRx_Buffer[5] = 'D';
    TxRx_Buffer[6] = 'E';
    TxRx_Buffer[7] = module_work_mode;

    /* Send the HM-10 Device's Set Mode Command. */
    ret = HAL_UART_Transmit(p_huart, TxRx_Buffer, HM10_SET_MODE_CMD_SIZE, HM10_CUSTOM_HAL_TIMEOUT);
    ret = HAL_ret_handler(ret);
    if (ret != HAL_OK)
    {
        #if ETX_OTA_VERBOSE
            printf("ERROR: The transmission of the Set Mode Command to HM-10 BT Device has failed.\r\n");
        #endif
        return ret;
    }

    /* Receive the HM-10 Device's Set Mode Response. */
    ret = HAL_UART_Receive(p_huart, TxRx_Buffer, HM10_MODE_RESPONSE_SIZE, HM10_CUSTOM_HAL_TIMEOUT);
    ret = HAL_ret_handler(ret);
    if (ret != HAL_OK)
    {
        #if ETX_OTA_VERBOSE
            printf("ERROR: A Set Mode Response from the HM-10 BT Device was expected, but none was received (HM-10 Exception code = %d)\r\n", ret);
        #endif
        return ret;
    }

    /* Validate the HM-10 Device's Set Mode Response. */
    /** <b>Local variable mode_resp_size_without_module_work_mode_value:</b> Size in bytes of the Mode Response from the HM-10 BT device but without considering the length of the Module Work Mode value. */
    uint8_t mode_resp_size_without_module_work_mode_value = HM10_MODE_RESPONSE_SIZE - 1;
    /** <b>Local variable bytes_compared:</b> Counter for the bytes that have been compared and validated to match between the received Set Mode Response (which should be stored in @ref TxRx_Buffer buffer ) and the expected Set Mode Response (i.e., @ref HM10_Set_Mode_resp_without_module_work_mode_value ). */
    uint8_t bytes_compared = 0;
    for (; bytes_compared<mode_resp_size_without_module_work_mode_value; bytes_compared++)
    {
        if (TxRx_Buffer[bytes_compared] != HM10_Set_Mode_resp_without_module_work_mode_value[bytes_compared])
        {
            #if ETX_OTA_VERBOSE
                printf("ERROR: A Set Mode Response from the HM-10 BT Device was expected, but something else was received instead at index %d. The received value was %c_ASCII and the expected value is %c_ASCII.\r\n", bytes_compared, TxRx_Buffer[bytes_compared], HM10_Set_Mode_resp_without_module_work_mode_value[bytes_compared]);
            #endif
            return HM10_EC_ERR;
        }
    }
    if (TxRx_Buffer[bytes_compared] != module_work_mode)
    {
        #if ETX_OTA_VERBOSE
            printf("ERROR: A Set Mode Response from the HM-10 BT Device was expected, but something else was received instead at index %d. The received value was %c_ASCII and the expected value is %c_ASCII.\r\n", bytes_compared, TxRx_Buffer[bytes_compared], module_work_mode);
        #endif
        return HM10_EC_ERR;
    }
    #if ETX_OTA_VERBOSE
        printf("DONE: The desired Module Work Mode has been successfully set in the HM-10 BT Device.\r\n");
    #endif

    return HM10_EC_OK;
}

HM10_Status get_hm10_module_work_mode(HM10_Module_Work_Mode *module_work_mode)
{
    /** <b>Local variable ret:</b> Return value of either a HAL function or a @ref HM10_Status function type. */
    int16_t  ret;

    /* Flush the UART's RX before starting. */
    HAL_uart_rx_flush();

    /* Populate the HM-10 Device's Get Mode Command into the Tx/Rx Buffer. */
    #if ETX_OTA_VERBOSE
        printf("Sending Get Mode Command to HM-10 BT Device...\r\n");
    #endif
    TxRx_Buffer[0] = 'A';
    TxRx_Buffer[1] = 'T';
    TxRx_Buffer[2] = '+';
    TxRx_Buffer[3] = 'M';
    TxRx_Buffer[4] = 'O';
    TxRx_Buffer[5] = 'D';
    TxRx_Buffer[6] = 'E';
    TxRx_Buffer[7] = '?';

    /* Send the HM-10 Device's Get Type Command. */
    ret = HAL_UART_Transmit(p_huart, TxRx_Buffer, HM10_GET_MODE_CMD_SIZE, HM10_CUSTOM_HAL_TIMEOUT);
    ret = HAL_ret_handler(ret);
    if (ret != HAL_OK)
    {
        #if ETX_OTA_VERBOSE
            printf("ERROR: The transmission of the Get Mode Command to HM-10 BT Device has failed.\r\n");
        #endif
        return ret;
    }

    /* Receive the HM-10 Device's Get Mode Response. */
    ret = HAL_UART_Receive(p_huart, TxRx_Buffer, HM10_MODE_RESPONSE_SIZE, HM10_CUSTOM_HAL_TIMEOUT);
    ret = HAL_ret_handler(ret);
    if (ret != HAL_OK)
    {
        #if ETX_OTA_VERBOSE
            printf("ERROR: A Get Mode Response from the HM-10 BT Device was expected, but none was received (HM-10 Exception code = %d)\r\n", ret);
        #endif
        return ret;
    }

    /* Validate the HM-10 Device's Get Mode Response. */
    /** <b>Local variable mode_resp_size_without_module_work_mode_value:</b> Size in bytes of the Mode Response from the HM-10 BT device but without considering the length of the Module Work Mode value. */
    uint8_t mode_resp_size_without_module_work_mode_value = HM10_MODE_RESPONSE_SIZE - 1;
    /** <b>Local variable bytes_compared:</b> Counter for the bytes that have been compared and validated to match between the received Get Mode Response (which should be stored in @ref TxRx_Buffer buffer ) and the expected Get Mode Response (i.e., @ref HM10_Get_Mode_resp_without_module_work_mode_value ). */
    uint8_t bytes_compared = 0;
    for (; bytes_compared<mode_resp_size_without_module_work_mode_value; bytes_compared++)
    {
        if (TxRx_Buffer[bytes_compared] != HM10_Get_Mode_resp_without_module_work_mode_value[bytes_compared])
        {
            #if ETX_OTA_VERBOSE
                printf("ERROR: A Get Mode Response from the HM-10 BT Device was expected, but something else was received instead.\r\n");
            #endif
            return HM10_EC_ERR;
        }
    }
    switch (TxRx_Buffer[bytes_compared])
    {
        case HM10_Transmission_Mode:
        case HM10_PIO_Collection_and_Transmission_Mode:
        case HM10_PIO_Remote_Control_and_Transmission_Mode:
            break;
        default:
            #if ETX_OTA_VERBOSE
                printf("ERROR: An invalid Module Work Mode value has been received from the HM-10 BT Device: %c_ASCII.\r\n", TxRx_Buffer[bytes_compared]);
            #endif
            return HM10_EC_ERR;
    }

    /* Pass the HM-10's Module Work Mode from the Buffer that is storing it into the \p module_work_mode param. */
    *module_work_mode = TxRx_Buffer[mode_resp_size_without_module_work_mode_value];

    #if ETX_OTA_VERBOSE
        printf("DONE: The Module Work Mode has been successfully received from the HM-10 BT Device.\r\n");
    #endif

    return HM10_EC_OK;
}

HM10_Status set_hm10_module_work_type(HM10_Module_Work_Type module_work_type)
{
    /* Validating given module work type. */
    switch (module_work_type)
    {
        case HM10_Module_Work_Type_0:
        case HM10_Module_Work_Type_1:
            break;
        default:
            #if ETX_OTA_VERBOSE
                printf("ERROR: An invalid module work type value has been given: %c_ASCII.\r\n", module_work_type);
            #endif
            return HM10_EC_ERR;
    }

    /** <b>Local variable ret:</b> Return value of either a HAL function or a @ref HM10_Status function type. */
    int16_t  ret;

    /* Flush the UART's RX before starting. */
    HAL_uart_rx_flush();

    /* Populate the HM-10 Device's Set IMME Command into the Tx/Rx Buffer. */
    #if ETX_OTA_VERBOSE
        printf("Sending Set IMME Command to HM-10 BT Device...\r\n");
    #endif
    TxRx_Buffer[0] = 'A';
    TxRx_Buffer[1] = 'T';
    TxRx_Buffer[2] = '+';
    TxRx_Buffer[3] = 'I';
    TxRx_Buffer[4] = 'M';
    TxRx_Buffer[5] = 'M';
    TxRx_Buffer[6] = 'E';
    TxRx_Buffer[7] = module_work_type;

    /* Send the HM-10 Device's Set IMME Command. */
    ret = HAL_UART_Transmit(p_huart, TxRx_Buffer, HM10_SET_IMME_CMD_SIZE, HM10_CUSTOM_HAL_TIMEOUT);
    ret = HAL_ret_handler(ret);
    if (ret != HAL_OK)
    {
        #if ETX_OTA_VERBOSE
            printf("ERROR: The transmission of the Set IMME Command to HM-10 BT Device has failed.\r\n");
        #endif
        return ret;
    }

    /* Receive the HM-10 Device's Set IMME Response. */
    ret = HAL_UART_Receive(p_huart, TxRx_Buffer, HM10_IMME_RESPONSE_SIZE, HM10_CUSTOM_HAL_TIMEOUT);
    ret = HAL_ret_handler(ret);
    if (ret != HAL_OK)
    {
        #if ETX_OTA_VERBOSE
            printf("ERROR: A Set IMME Response from the HM-10 BT Device was expected, but none was received (HM-10 Exception code = %d)\r\n", ret);
        #endif
        return ret;
    }

    /* Validate the HM-10 Device's Set IMME Response. */
    /** <b>Local variable imme_resp_size_without_module_work_type_value:</b> Size in bytes of the IMME Response from the HM-10 BT device but without considering the length of the Module Work Type value. */
    uint8_t imme_resp_size_without_module_work_type_value = HM10_IMME_RESPONSE_SIZE - 1;
    /** <b>Local variable bytes_compared:</b> Counter for the bytes that have been compared and validated to match between the received Set IMME Response (which should be stored in @ref TxRx_Buffer buffer ) and the expected Set IMME Response (i.e., @ref HM10_Set_IMME_resp_without_module_work_type_value ). */
    uint8_t bytes_compared = 0;
    for (; bytes_compared<imme_resp_size_without_module_work_type_value; bytes_compared++)
    {
        if (TxRx_Buffer[bytes_compared] != HM10_Set_IMME_resp_without_module_work_type_value[bytes_compared])
        {
            #if ETX_OTA_VERBOSE
                printf("ERROR: A Set IMME Response from the HM-10 BT Device was expected, but something else was received instead at index %d. The received value was %c_ASCII and the expected value is %c_ASCII.\r\n", bytes_compared, TxRx_Buffer[bytes_compared], HM10_Set_IMME_resp_without_module_work_type_value[bytes_compared]);
            #endif
            return HM10_EC_ERR;
        }
    }
    if (TxRx_Buffer[bytes_compared] != module_work_type)
    {
        #if ETX_OTA_VERBOSE
            printf("ERROR: A Set IMME Response from the HM-10 BT Device was expected, but something else was received instead at index %d. The received value was %c_ASCII and the expected value is %c_ASCII.\r\n", bytes_compared, TxRx_Buffer[bytes_compared], module_work_type);
        #endif
        return HM10_EC_ERR;
    }
    #if ETX_OTA_VERBOSE
        printf("DONE: The desired Module Work Type has been successfully set in the HM-10 BT Device.\r\n");
    #endif

    return HM10_EC_OK;
}

HM10_Status get_hm10_module_work_type(HM10_Module_Work_Type *module_work_type)
{
    /** <b>Local variable ret:</b> Return value of either a HAL function or a @ref HM10_Status function type. */
    int16_t  ret;

    /* Flush the UART's RX before starting. */
    HAL_uart_rx_flush();

    /* Populate the HM-10 Device's Get IMME Command into the Tx/Rx Buffer. */
    #if ETX_OTA_VERBOSE
        printf("Sending Get IMME Command to HM-10 BT Device...\r\n");
    #endif
    TxRx_Buffer[0] = 'A';
    TxRx_Buffer[1] = 'T';
    TxRx_Buffer[2] = '+';
    TxRx_Buffer[3] = 'I';
    TxRx_Buffer[4] = 'M';
    TxRx_Buffer[5] = 'M';
    TxRx_Buffer[6] = 'E';
    TxRx_Buffer[7] = '?';

    /* Send the HM-10 Device's Get IMME Command. */
    ret = HAL_UART_Transmit(p_huart, TxRx_Buffer, HM10_GET_IMME_CMD_SIZE, HM10_CUSTOM_HAL_TIMEOUT);
    ret = HAL_ret_handler(ret);
    if (ret != HAL_OK)
    {
        #if ETX_OTA_VERBOSE
            printf("ERROR: The transmission of the Get IMME Command to HM-10 BT Device has failed.\r\n");
        #endif
        return ret;
    }

    /* Receive the HM-10 Device's Get IMME Response. */
    ret = HAL_UART_Receive(p_huart, TxRx_Buffer, HM10_IMME_RESPONSE_SIZE, HM10_CUSTOM_HAL_TIMEOUT);
    ret = HAL_ret_handler(ret);
    if (ret != HAL_OK)
    {
        #if ETX_OTA_VERBOSE
            printf("ERROR: A Get IMME Response from the HM-10 BT Device was expected, but none was received (HM-10 Exception code = %d)\r\n", ret);
        #endif
        return ret;
    }

    /* Validate the HM-10 Device's Get IMME Response. */
    /** <b>Local variable imme_resp_size_without_module_work_type_value:</b> Size in bytes of the IMME Response from the HM-10 BT device but without considering the length of the Module Work Type value. */
    uint8_t imme_resp_size_without_module_work_type_value = HM10_IMME_RESPONSE_SIZE - 1;
    /** <b>Local variable bytes_compared:</b> Counter for the bytes that have been compared and validated to match between the received Get IMME Response (which should be stored in @ref TxRx_Buffer buffer ) and the expected Get IMME Response (i.e., @ref HM10_Get_IMME_resp_without_module_work_type_value ). */
    uint8_t bytes_compared = 0;
    for (; bytes_compared<imme_resp_size_without_module_work_type_value; bytes_compared++)
    {
        if (TxRx_Buffer[bytes_compared] != HM10_Get_IMME_resp_without_module_work_type_value[bytes_compared])
        {
            #if ETX_OTA_VERBOSE
                printf("ERROR: A Get IMME Response from the HM-10 BT Device was expected, but something else was received instead.\r\n");
            #endif
            return HM10_EC_ERR;
        }
    }
    switch (TxRx_Buffer[bytes_compared])
    {
        case HM10_Module_Work_Type_0:
        case HM10_Module_Work_Type_1:
            break;
        default:
            #if ETX_OTA_VERBOSE
                printf("ERROR: An invalid Module Work Type value has been received from the HM-10 BT Device: %c_ASCII.\r\n", TxRx_Buffer[bytes_compared]);
            #endif
            return HM10_EC_ERR;
    }

    /* Pass the HM-10's Module Work Type from the Buffer that is storing it into the \p module_work_type param. */
    *module_work_type = TxRx_Buffer[imme_resp_size_without_module_work_type_value];
    #if ETX_OTA_VERBOSE
        printf("DONE: The Module Work Type has been successfully received from the HM-10 BT Device.\r\n");
    #endif

    return HM10_EC_OK;
}

HM10_Status set_hm10_notify_information_mode(HM10_Notify_Information_Mode notify_mode)
{
    /* Validating given Notify Information Mode. */
    switch (notify_mode)
    {
        case HM10_Notify_DISABLED:
        case HM10_Notify_ENABLED:
            break;
        default:
            #if ETX_OTA_VERBOSE
                printf("ERROR: An invalid Notify Information Mode value has been given: %c_ASCII.\r\n", notify_mode);
            #endif
            return HM10_EC_ERR;
    }

    /** <b>Local variable ret:</b> Return value of either a HAL function or a @ref HM10_Status function type. */
    int16_t  ret;

    /* Flush the UART's RX before starting. */
    HAL_uart_rx_flush();

    /* Populate the HM-10 Device's Set NOTI Command into the Tx/Rx Buffer. */
    #if ETX_OTA_VERBOSE
        printf("Sending Set NOTI Command to HM-10 BT Device...\r\n");
    #endif
    TxRx_Buffer[0] = 'A';
    TxRx_Buffer[1] = 'T';
    TxRx_Buffer[2] = '+';
    TxRx_Buffer[3] = 'N';
    TxRx_Buffer[4] = 'O';
    TxRx_Buffer[5] = 'T';
    TxRx_Buffer[6] = 'I';
    TxRx_Buffer[7] = notify_mode;

    /* Send the HM-10 Device's Set NOTI Command. */
    ret = HAL_UART_Transmit(p_huart, TxRx_Buffer, HM10_SET_NOTI_CMD_SIZE, HM10_CUSTOM_HAL_TIMEOUT);
    ret = HAL_ret_handler(ret);
    if (ret != HAL_OK)
    {
        #if ETX_OTA_VERBOSE
            printf("ERROR: The transmission of the Set NOTI Command to HM-10 BT Device has failed.\r\n");
        #endif
        return ret;
    }

    /* Receive the HM-10 Device's Set NOTI Response. */
    ret = HAL_UART_Receive(p_huart, TxRx_Buffer, HM10_NOTI_RESPONSE_SIZE, HM10_CUSTOM_HAL_TIMEOUT);
    ret = HAL_ret_handler(ret);
    if (ret != HAL_OK)
    {
        #if ETX_OTA_VERBOSE
            printf("ERROR: A Set NOTI Response from the HM-10 BT Device was expected, but none was received (HM-10 Exception code = %d)\r\n", ret);
        #endif
        return ret;
    }

    /* Validate the HM-10 Device's Set NOTI Response. */
    /** <b>Local variable noti_resp_size_without_notify_mode_value:</b> Size in bytes of the NOTI Response from the HM-10 BT device but without considering the length of the Notify Information Mode value. */
    uint8_t noti_resp_size_without_notify_mode_value = HM10_NOTI_RESPONSE_SIZE - 1;
    /** <b>Local variable bytes_compared:</b> Counter for the bytes that have been compared and validated to match between the received Set NOTI Response (which should be stored in @ref TxRx_Buffer buffer ) and the expected Set NOTI Response (i.e., @ref HM10_Set_NOTI_resp_without_notify_mode_value ). */
    uint8_t bytes_compared = 0;
    for (; bytes_compared<noti_resp_size_without_notify_mode_value; bytes_compared++)
    {
        if (TxRx_Buffer[bytes_compared] != HM10_Set_NOTI_resp_without_notify_mode_value[bytes_compared])
        {
            #if ETX_OTA_VERBOSE
                printf("ERROR: A Set NOTI Response from the HM-10 BT Device was expected, but something else was received instead at index %d. The received value was %c_ASCII and the expected value is %c_ASCII.\r\n", bytes_compared, TxRx_Buffer[bytes_compared], HM10_Set_NOTI_resp_without_notify_mode_value[bytes_compared]);
            #endif
            return HM10_EC_ERR;
        }
    }
    if (TxRx_Buffer[bytes_compared] != notify_mode)
    {
        #if ETX_OTA_VERBOSE
            printf("ERROR: A Set NOTI Response from the HM-10 BT Device was expected, but something else was received instead at index %d. The received value was %c_ASCII and the expected value is %c_ASCII.\r\n", bytes_compared, TxRx_Buffer[bytes_compared], notify_mode);
        #endif
        return HM10_EC_ERR;
    }
    #if ETX_OTA_VERBOSE
        printf("DONE: The desired Notify Information Mode has been successfully set in the HM-10 BT Device.\r\n");
    #endif

    return HM10_EC_OK;
}

HM10_Status get_hm10_notify_information_mode(HM10_Notify_Information_Mode *notify_mode)
{
    /** <b>Local variable ret:</b> Return value of either a HAL function or a @ref HM10_Status function type. */
    int16_t  ret;

    /* Flush the UART's RX before starting. */
    HAL_uart_rx_flush();

    /* Populate the HM-10 Device's Get NOTI Command into the Tx/Rx Buffer. */
    #if ETX_OTA_VERBOSE
        printf("Sending Get NOTI Command to HM-10 BT Device...\r\n");
    #endif
    TxRx_Buffer[0] = 'A';
    TxRx_Buffer[1] = 'T';
    TxRx_Buffer[2] = '+';
    TxRx_Buffer[3] = 'N';
    TxRx_Buffer[4] = 'O';
    TxRx_Buffer[5] = 'T';
    TxRx_Buffer[6] = 'I';
    TxRx_Buffer[7] = '?';

    /* Send the HM-10 Device's Get NOTI Command. */
    ret = HAL_UART_Transmit(p_huart, TxRx_Buffer, HM10_GET_NOTI_CMD_SIZE, HM10_CUSTOM_HAL_TIMEOUT);
    ret = HAL_ret_handler(ret);
    if (ret != HAL_OK)
    {
        #if ETX_OTA_VERBOSE
            printf("ERROR: The transmission of the Get NOTI Command to HM-10 BT Device has failed.\r\n");
        #endif
        return ret;
    }

    /* Receive the HM-10 Device's Get NOTI Response. */
    ret = HAL_UART_Receive(p_huart, TxRx_Buffer, HM10_NOTI_RESPONSE_SIZE, HM10_CUSTOM_HAL_TIMEOUT);
    ret = HAL_ret_handler(ret);
    if (ret != HAL_OK)
    {
        #if ETX_OTA_VERBOSE
            printf("ERROR: A Get NOTI Response from the HM-10 BT Device was expected, but none was received (HM-10 Exception code = %d)\r\n", ret);
        #endif
        return ret;
    }

    /* Validate the HM-10 Device's Get NOTI Response. */
    /** <b>Local variable noti_resp_size_without_notify_mode_value:</b> Size in bytes of the NOTI Response from the HM-10 BT device but without considering the length of the Notify Information Mode value. */
    uint8_t noti_resp_size_without_notify_mode_value = HM10_NOTI_RESPONSE_SIZE - 1;
    /** <b>Local variable bytes_compared:</b> Counter for the bytes that have been compared and validated to match between the received Get NOTI Response (which should be stored in @ref TxRx_Buffer buffer ) and the expected Get NOTI Response (i.e., @ref HM10_Get_NOTI_resp_without_notify_mode_value ). */
    uint8_t bytes_compared = 0;
    for (; bytes_compared<noti_resp_size_without_notify_mode_value; bytes_compared++)
    {
        if (TxRx_Buffer[bytes_compared] != HM10_Get_NOTI_resp_without_notify_mode_value[bytes_compared])
        {
            #if ETX_OTA_VERBOSE
                printf("ERROR: A Get NOTI Response from the HM-10 BT Device was expected, but something else was received instead.\r\n");
            #endif
            return HM10_EC_ERR;
        }
    }
    switch (TxRx_Buffer[bytes_compared])
    {
        case HM10_Notify_DISABLED:
        case HM10_Notify_ENABLED:
            break;
        default:
            #if ETX_OTA_VERBOSE
                printf("ERROR: An invalid Notify Information MOde value has been received from the HM-10 BT Device: %c_ASCII.\r\n", TxRx_Buffer[bytes_compared]);
            #endif
            return HM10_EC_ERR;
    }

    /* Pass the HM-10's Notify Information Mode from the Buffer that is storing it into the \p notify_mode param. */
    *notify_mode = TxRx_Buffer[noti_resp_size_without_notify_mode_value];
    #if ETX_OTA_VERBOSE
        printf("DONE: The Notify Information Mode has been successfully received from the HM-10 BT Device.\r\n");
    #endif

    return HM10_EC_OK;
}

HM10_Status connect_hm10_to_bt_address(HM10_BT_Address_Type bt_addr_t, char bt_addr[12])
{
    /* Validating given Bluetooth Address Type. */
    switch (bt_addr_t)
    {
        case HM10_BT_Static_MAC:
        case HM10_BT_Static_Random_MAC:
        case HM10_BT_Random_MAC:
        case HM10_BT_Normal_Address:
            break;
        default:
            #if ETX_OTA_VERBOSE
                printf("ERROR: An invalid Bluetooth Address Type value has been given: %c_ASCII.\r\n", bt_addr_t);
            #endif
            return HM10_EC_ERR;
    }

    /** <b>Local variable ret:</b> Return value of either a HAL function or a @ref HM10_Status function type. */
    int16_t  ret;

    /* Flush the UART's RX before starting. */
    HAL_uart_rx_flush();

    /* Populate the HM-10 Device's Connect-To-Address Command into the Tx/Rx Buffer. */
    #if ETX_OTA_VERBOSE
        printf("Sending Connect-To-Address Command to HM-10 BT Device...\r\n");
    #endif
    TxRx_Buffer[0] = 'A';
    TxRx_Buffer[1] = 'T';
    TxRx_Buffer[2] = '+';
    TxRx_Buffer[3] = 'C';
    TxRx_Buffer[4] = 'O';
    TxRx_Buffer[5] = bt_addr_t;
    TxRx_Buffer[6] = bt_addr[0];
    TxRx_Buffer[7] = bt_addr[1];
    TxRx_Buffer[8] = bt_addr[2];
    TxRx_Buffer[9] = bt_addr[3];
    TxRx_Buffer[10] = bt_addr[4];
    TxRx_Buffer[11] = bt_addr[5];
    TxRx_Buffer[12] = bt_addr[6];
    TxRx_Buffer[13] = bt_addr[7];
    TxRx_Buffer[14] = bt_addr[8];
    TxRx_Buffer[15] = bt_addr[9];
    TxRx_Buffer[16] = bt_addr[10];
    TxRx_Buffer[17] = bt_addr[11];

    /* Send the HM-10 Device's Connect-To-Address Command. */
    ret = HAL_UART_Transmit(p_huart, TxRx_Buffer, HM10_CONNECT_TO_ADDRESS_CMD_SIZE, HM10_CUSTOM_HAL_TIMEOUT);
    ret = HAL_ret_handler(ret);
    if (ret != HAL_OK)
    {
        #if ETX_OTA_VERBOSE
            printf("ERROR: The transmission of the Connect-To-Address Command to HM-10 BT Device has failed.\r\n");
        #endif
        return ret;
    }

    /* Receive the part one of the HM-10 Device's Connect-To-Address Response. */
    ret = HAL_UART_Receive(p_huart, TxRx_Buffer, HM10_CONNECT_TO_ADDRESS_RESPONSE1_SIZE, HM10_CUSTOM_HAL_TIMEOUT);
    ret = HAL_ret_handler(ret);
    if (ret != HAL_OK)
    {
        #if ETX_OTA_VERBOSE
            printf("ERROR: The part one of the Connect-To-Address Response from the HM-10 BT Device was expected, but none was received (HM-10 Exception code = %d)\r\n", ret);
        #endif
        return ret;
    }

    /* Validate the part one of the HM-10 Device's Connect-To-Address Response. */
    /** <b>Local variable bytes_compared:</b> Counter for the bytes that have been compared and validated to match between the received Connect-To-Address Response (which should be stored in @ref TxRx_Buffer buffer ) and the expected Connect-To-Address Response (i.e., either @ref HM10_Connect_To_Address_response1 or @ref HM10_Connect_To_Address_response2 correspondingly). */
    uint8_t bytes_compared = 0;
    HM10_Connect_To_Address_response1[5] = bt_addr_t;
    HM10_Connect_To_Address_response1[6] = bt_addr_t;
    for (; bytes_compared<HM10_CONNECT_TO_ADDRESS_RESPONSE1_SIZE; bytes_compared++)
    {
        if (TxRx_Buffer[bytes_compared] != HM10_Connect_To_Address_response1[bytes_compared])
        {
            #if ETX_OTA_VERBOSE
                printf("ERROR: The first part of the Connect-To-Address Response from the HM-10 BT Device was expected, but something else was received instead at index %d. The received value was %c_ASCII and the expected value is %c_ASCII.\r\n", bytes_compared, TxRx_Buffer[bytes_compared], HM10_Connect_To_Address_response1[bytes_compared]);
            #endif
            return HM10_EC_ERR;
        }
    }

    /* Receive the part two of the HM-10 Device's Connect-To-Address Response. */
    ret = HAL_UART_Receive(p_huart, TxRx_Buffer, HM10_CONNECT_TO_ADDRESS_RESPONSE2_SIZE, HM10_CONNECT_TO_ADDRESS_TIMEOUT);
    ret = HAL_ret_handler(ret);
    if (ret != HAL_OK)
    {
        #if ETX_OTA_VERBOSE
            printf("ERROR: The part two of the Connect-To-Address Response from the HM-10 BT Device was expected, but none was received (HM-10 Exception code = %d)\r\n", ret);
        #endif
        return HM10_EC_ERR;
    }

    /* Validate the part two of the HM-10 Device's Connect-To-Address Response. */
    bytes_compared = 0;
    for (; bytes_compared<HM10_CONNECT_TO_ADDRESS_RESPONSE2_SIZE; bytes_compared++)
    {
        if (TxRx_Buffer[bytes_compared] != HM10_Connect_To_Address_response2[bytes_compared])
        {
            #if ETX_OTA_VERBOSE
                printf("ERROR: The second part of the Connect-To-Address Response from the HM-10 BT Device was expected, but something else was received instead at index %d. The received value was %c_ASCII and the expected value is %c_ASCII.\r\n", bytes_compared, TxRx_Buffer[bytes_compared], HM10_Connect_To_Address_response2[bytes_compared]);
            #endif
            return HM10_EC_ERR;
        }
    }
    #if ETX_OTA_VERBOSE
        printf("DONE: The HM-10 BT Device has successfully connected to the remote BT that has the requested BT Address of ");
        for (uint8_t i=0; i<10; i+=2)
        {
            printf("%c", bt_addr[i]);
            printf("%c", bt_addr[i+1]);
            printf(":");
        }
        printf("%c", bt_addr[10]);
        printf("%c\r\n", bt_addr[11]);
    #endif

    return HM10_EC_OK;
}

HM10_BT_Connection_Status disconnect_hm10_from_bt_address()
{
    /** <b>Local variable ret:</b> Return value of either a HAL function or a @ref HM10_Status function type. */
    int16_t  ret;

    /* Flush the UART's RX before starting. */
    HAL_uart_rx_flush();

    /* Populate the HM-10 Device's Lost-Connection Command into the Tx/Rx Buffer. */
    #if ETX_OTA_VERBOSE
        printf("Sending Lost-Connection Command to HM-10 BT Device...\r\n");
    #endif
    TxRx_Buffer[0] = 'A';
    TxRx_Buffer[1] = 'T';

    /* Send the HM-10 Device's Lost-Connection Command. */
    ret = HAL_UART_Transmit(p_huart, TxRx_Buffer, HM10_TEST_CMD_SIZE, HM10_CUSTOM_HAL_TIMEOUT);
    ret = HAL_ret_handler(ret);
    if (ret != HAL_OK)
    {
        #if ETX_OTA_VERBOSE
            printf("ERROR: The transmission of the Lost-Connection Command to HM-10 BT Device has failed.\r\n");
        #endif
        return HM10_BT_Connection_Status_Unknown;
    }

    /* Receive the first part of the HM-10 Device's Lost-Connection Response. */
    ret = HAL_UART_Receive(p_huart, TxRx_Buffer, HM10_OK_RESPONSE_SIZE, HM10_CUSTOM_HAL_TIMEOUT);
    ret = HAL_ret_handler(ret);
    if (ret != HAL_OK)
    {
        #if ETX_OTA_VERBOSE
            printf("ERROR: The first part of the Lost-Connection Response from the HM-10 BT Device was expected, but none was received (HM-10 Exception code = %d)\r\n", ret);
        #endif
        return HM10_BT_Connection_Status_Unknown;
    }

    /* Validate the first part of the HM-10 Device's Lost-Connection Response. */
    /** <b>Local variable bytes_compared:</b> Counter for the bytes that have been compared and validated to match between the received Connection-Lost Response (which should be stored in @ref TxRx_Buffer buffer ) and the expected Connection-Lost Response (i.e., @ref HM10_OK_LOST_resp ). */
    uint8_t bytes_compared = 0;
    for ( ; bytes_compared<HM10_OK_RESPONSE_SIZE; bytes_compared++)
    {
        if (TxRx_Buffer[bytes_compared] != HM10_OK_LOST_resp[bytes_compared])
        {
            #if ETX_OTA_VERBOSE
                printf("ERROR: The first part of the Lost-Connection Response from the HM-10 BT Device was expected, but something else was received instead.\r\n");
            #endif
            return HM10_BT_Connection_Status_Unknown;
        }
    }

    /* Receive the second part of the HM-10 Device's Lost-Connection Response. */
    ret = HAL_UART_Receive(p_huart, &TxRx_Buffer[bytes_compared], HM10_OK_LOST_RESPONSE_SIZE_WITHOUT_THE_OK_PART, HM10_CUSTOM_HAL_TIMEOUT);
    ret = HAL_ret_handler(ret);
    if (ret != HAL_OK)
    {
        if (ret == HM10_EC_NR)
        {
            #if ETX_OTA_VERBOSE
                printf("WARNING: The second part of the Lost-Connection Response from the HM-10 BT Device was expected, but none was received (HM-10 Exception code = %d)\r\n", ret);
            #endif
            return HM10_BT_No_Connection;
        }
        #if ETX_OTA_VERBOSE
            else
            {
                printf("ERROR: The second part of the Lost-Connection Response from the HM-10 BT Device was expected, but something went wrong when our MCU/MPU was waiting to receive the data (HM-10 Exception code = %d)\r\n", ret);
            }
        #endif
        return HM10_BT_Connection_Status_Unknown;
    }

    /* Validate the second part of the HM-10 Device's Lost-Connection Response. */
    for ( ; bytes_compared<HM10_OK_LOST_RESPONSE_SIZE; bytes_compared++)
    {
        if (TxRx_Buffer[bytes_compared] != HM10_OK_LOST_resp[bytes_compared])
        {
            #if ETX_OTA_VERBOSE
                printf("ERROR: The second part of the Lost-Connection Response from the HM-10 BT Device was expected, but something else was received instead.\r\n");
            #endif
            return HM10_BT_Connection_Status_Unknown;
        }
    }
    #if ETX_OTA_VERBOSE
        printf("DONE: The HM-10 Device has been disconnected from an on-going Bluetooth Connection.\r\n");
    #endif

    return HM10_BT_Connection_Lost;
}

HM10_Status send_hm10_ota_data(uint8_t *ble_ota_data, uint16_t size, uint32_t timeout)
{
	/** <b>Local variable ret:</b> Return value of either a HAL function or a @ref HM10_Status function type. */
	int16_t  ret;

	/* Send the requested data Over the Air (OTA) via the HM-10 BT Device. */
	ret = HAL_UART_Transmit(p_huart, ble_ota_data, size, timeout);
	ret = HAL_ret_handler(ret);

	return ret;
}

HM10_Status get_hm10_ota_data(uint8_t *ble_ota_data, uint16_t size, uint32_t timeout)
{
	/** <b>Local variable ret:</b> Return value of either a HAL function or a @ref HM10_Status function type. */
	int16_t  ret;

	/* Receive the HM-10 Device's BT data that is received Over the Air (OTA), if there is any. */
	ret = HAL_UART_Receive(p_huart, ble_ota_data, size, timeout);
	ret = HAL_ret_handler(ret);

	return ret;
}

static void HAL_uart_rx_flush()
{
	/** <b>Local variable ret:</b> Return value of either a HAL function type. */
	HAL_StatusTypeDef  ret;

	/* Receive the HM-10 Device's BT data that is received Over the Air (OTA), if there is any. */
	ret = HAL_UART_Receive(p_huart, TxRx_Buffer, 1, HM10_CUSTOM_HAL_TIMEOUT);
	if (ret != HAL_TIMEOUT)
	{
		HAL_uart_rx_flush();
	}
}

static HM10_Status HAL_ret_handler(HAL_StatusTypeDef HAL_status)
{
  switch (HAL_status)
    {
  	  case HAL_BUSY:
	  case HAL_TIMEOUT:
		return HM10_EC_NR;
	  case HAL_ERROR:
		return HM10_EC_ERR;
	  default:
		return HAL_status;
    }
}

/** @} */
