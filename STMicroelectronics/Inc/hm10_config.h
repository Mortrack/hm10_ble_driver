/** @addtogroup hm10_ble
 * @{
 */

/**@defgroup main_HM10_config HM-10 Bluetooth Device Configuration Files
 * @{
 *
 * @brief   This module contains both the default and the application HM-10 Bluetooth Driver Configuration Files.
 *
 * @author 	Cesar Miranda Meza (cmirandameza3@hotmail.com)
 * @date	January 01, 2024.
 */

/**@file
 * @brief	HM-10 Bluetooth Module Driver default configuration file.
 *
 * @defgroup HM10_config HM-10 Default ETX OTA Protocol Configuration File
 * @{
 *
 * @brief   This file contains all the HM-10 Default ETX OTA Protocol Configurations.
 *
 * @note    It is highly suggested not to directly edit the Configurations Settings defined in this file. Instead of
 *          doing that whenever requiring different Configuration Settings, it is suggested to do that instead in an
 *          additional header file named as "hm10_app_config.h" whose File Path should be located as a sibbling of
 *          this @ref HM10_config header file. However, to enable the use of that additional header file, you will
 *          also need to change the @ref ENABLE_APP_HM10_CONFIG define, that is located at @ref HM10_config , to a
 *          value of 1.
 *
 * @author 	Cesar Miranda Meza (cmirandameza3@hotmail.com)
 * @date	January 01, 2024.
 */

#ifndef HM10_CONFIG_H_
#define HM10_CONFIG_H_
#include "hm10_ble_driver.h" // Custom Mortrack's Library to be able to initialize, send configuration commands and send and/or receive data to/from an HM-10 Bluetooth Device.

#define ENABLE_APP_HM10_CONFIG              (1)            /**< @brief Flag used to enable the use of @ref HM10_app_config with a 1 or, otherwise to disable it with a 0. */
#if ENABLE_APP_HM10_CONFIG
#include "hm10_app_config.h" // This is the user custom HM-10 Driver configuration file that is recommended to use whenever it is desired to edit default configuration settings as defined in this @ref HM10_config file.
#endif

#ifndef ETX_OTA_VERBOSE
#define ETX_OTA_VERBOSE 			        (0U)   	       /**< @brief Flag value used to enable the compiler to take into account the code of both the @ref app_side_etx_ota and the @ref firmware_update_config libraries that displays detailed information about the processes made inside them via @ref printf with a \c 1 . Otherwise, a \c 0 for not displaying any messages at all with @ref printf . */
#endif

#ifndef HM10_CUSTOM_HAL_TIMEOUT
#define HM10_CUSTOM_HAL_TIMEOUT	            (320U)         /**< @brief Designated time in milliseconds for the HAL Timeout to be requested during each UART transaction/request made between the HM-10 BT Device and our MCU/MPU. @note As a reference, the lowest value at which the author the @ref hm10_ble had unsuccessful responses was around 150 milliseconds and below that time. On the other hand, 160 milliseconds worked with one HM-10 BT Device used as a sample. However, repeating that test with more units in the future would help to learn the right value for this Definition. Therefore, a higher value than the one mentioned is suggested in order to guarantee that the HM-10 BT Device will work properly. */
#endif

#ifndef HM10_RESET_AND_RENEW_CMDS_DELAY
#define HM10_RESET_AND_RENEW_CMDS_DELAY     (1000U)        /**< @brief Designated time in milliseconds for the Delay to be requested each time after either the Reset or the Renew Command is solicited to the HM-10 BT Device. @details In order to guarantee that any other AT Command will work as expected after Resetting the HM-10 BT Device, a Delay is needed in order to wait for the Device to complete the Reset Process. This is particularly necessary if a Bluetooth Connection is requested to the HM-10 BT Device after applying a Reset to itself. @note On a validation test made with only one HM-10 BT Device, a Delay of 500 milliseconds worked fine, but repeating that test with more units in the future would help to learn the right value for this Definition. Therefore, a higher value than the one mentioned is suggested in order to guarantee that the HM-10 BT Device will work properly. */
#endif

#endif /* HM10_CONFIG_H_ */

/** @} */ // HM10_config

/** @} */ // main_HM10_config

/** @} */ // hm10_ble
