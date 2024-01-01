/** @addtogroup hm10_ble
 * @{
 */

/**@defgroup main_HM10_config HM-10 Bluetooth Device Configuration Files
 * @{
 *
 * @brief   This module contains both the default and the application HM-10 Bluetooth Driver Configuration Files.
 *
 * @author 	Cesar Miranda Meza (cmirandameza3@hotmail.com)
 * @date	December 30, 2023.
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
 * @date	December 30, 2023.
 */

#ifndef HM10_CONFIG_H_
#define HM10_CONFIG_H_
#include "hm10_ble_driver.h" // Custom Mortrack's Library to be able to initialize, send configuration commands and send and/or receive data to/from an HM-10 Bluetooth Device.

#define ENABLE_APP_HM10_CONFIG      (1)        /**< @brief Flag used to enable the use of @ref HM10_app_config with a 1 or, otherwise to disable it with a 0. */
#if ENABLE_APP_HM10_CONFIG
#include "hm10_app_config.h" // This is the user custom HM-10 Driver configuration file that is recommended to use whenever it is desired to edit default configuration settings as defined in this @ref HM10_config file.
#endif

#ifndef ETX_OTA_VERBOSE
#define ETX_OTA_VERBOSE             (0U)       /**< @brief Flag value used to enable the compiler to take into account the code of both the @ref hm10_ble library that displays detailed information about the processes made inside them via @ref printf with a \c 1 . Otherwise, a \c 0 for not displaying any messages at all with @ref printf . */
#endif

#endif /* HM10_CONFIG_H_ */

/** @} */ // HM10_config

/** @} */ // main_HM10_config

/** @} */ // hm10_ble
