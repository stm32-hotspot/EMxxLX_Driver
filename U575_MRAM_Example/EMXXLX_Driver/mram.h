/*
 * mram.h
 *
 *  Created on: Jul 11, 2023
 *      Author: Lucas Costa
 */

#include "main.h"

#ifndef INC_MRAM_H_
#define INC_MRAM_H_

typedef struct
{
  uint8_t SpiInterfaceMode;     				/*!< It configures the OCTOSPI interface mode.
                                  	  	  	  	  This parameter can be a value of @ref OSPI_Interface_Mode*/

  uint8_t DummyCycles;          				/*!< It configures the Dummy Cycles setting.
                                  	  	  	  	  This parameter can be a value of @ref EMXXLX_Dummy_Cycles*/

  uint8_t DriverStrenght;						/*!< It configures the driver strength setting.
                                  	  	  	  	  This parameter can be a value of @ref EMXXLX_Driver_Strength */

  uint8_t AddedDsDelay;							/*!< It configures Ds Delay setting.
                                  	  	  	  	  This parameter can be a value between of @ref EMXXLX_Ds_Delay*/

  uint8_t AddressMode;          				/*!< It configures the addressing mode.
                                  	  	  	  	  This parameter can be a value of @ref EMXXLX_Address_Mode */

  uint8_t XIPConfiguration;						/*!< It configures the Execute in place mode.
                                  	  	  	  	  This parameter can be a value of @ref EMXXLX_Execute_in_Place */

  uint8_t WrapConfiguration;					/*!< It configures the wrapping configuration.
                                  	  	  	  	  This parameter can be a value of @ref EMXXLX_Wrap_Configuration */

  uint8_t WriteMode;							/*!< It configures the write mode setting.
                                    	  	  	  	  This parameter can be either MRAM_NONVOLATILE or MRAM_VOLATILE */

  uint8_t ResetPinEnable;						/*!< It configures the Reset Pin.
                                    	  	  	  	  This parameter can be either MRAM_RESET_ENABLE or MRAM_RESET_DISABLE */

  uint8_t EraseBitValue;						/*!< It configures the erase bit value setting.
                                    	  	  	  	  This parameter can be either MRAM_ERASE_VALUE_1 or MRAM_ERASE_VALUE_0 */

  uint8_t OtpLockEnable;						/*!< It configures the OTP lock.
                                    	  	  	  	  This parameter can be either MRAM_OTPLOCK_ENABLE or MRAM_OTPLOCK_DISABLE */
} EMXXLX_ConfigurationTypeDef;


uint8_t EMXXLX_Init(OSPI_HandleTypeDef *Ctx, EMXXLX_ConfigurationTypeDef Config,
		uint8_t InterfaceMode);
uint8_t EMXXLX_Refactor(OSPI_HandleTypeDef *Ctx);
uint8_t EMXXLX_MemoryMapped_Config (OSPI_HandleTypeDef *Ctx);
uint8_t EMXXLX_Erase_Chip(OSPI_HandleTypeDef *Ctx);
uint8_t EMXXLX_Reset(OSPI_HandleTypeDef *Ctx);
uint8_t EMXXLX_Read_ID(OSPI_HandleTypeDef *Ctx, uint8_t *Value);
uint8_t EMXXLX_Read_Nonvol(OSPI_HandleTypeDef *Ctx,uint32_t address, uint8_t *Value, uint8_t size);
uint8_t EMXXLX_Read_Vol(OSPI_HandleTypeDef *Ctx,uint32_t address, uint8_t *Value, uint8_t size);
uint8_t EMXXLX_Write_Nonvol(OSPI_HandleTypeDef *Ctx, uint32_t address, uint8_t *pData, uint8_t size);
uint8_t EMXXLX_Write_Vol(OSPI_HandleTypeDef *Ctx, uint32_t address, uint8_t *pData, uint8_t size);
uint8_t EMXXLX_Read_Flags(OSPI_HandleTypeDef *Ctx, uint8_t *Value);
uint8_t EMXXLX_Clear_flags(OSPI_HandleTypeDef *Ctx);
uint8_t EMXXLX_Write_Status(OSPI_HandleTypeDef *Ctx, uint8_t *pData);
uint8_t EMXXLX_Read_Status(OSPI_HandleTypeDef *Ctx, uint8_t *Value);
uint8_t EMXXLX_Write_Enable(OSPI_HandleTypeDef *Ctx);
uint8_t EMXXLX_Write_Disable(OSPI_HandleTypeDef *Ctx);
uint8_t EMXXLX_Read(OSPI_HandleTypeDef *Ctx, uint32_t address, uint8_t *pData, uint32_t size);
uint8_t EMXXLX_Write(OSPI_HandleTypeDef *Ctx, uint32_t address, uint8_t *Value, uint32_t size);
uint8_t EMXXLX_4BADD_Enable(OSPI_HandleTypeDef *Ctx);
uint8_t EMXXLX_Polling_MemReady(OSPI_HandleTypeDef *Ctx, uint32_t Timeout);
void jesd_reset();


/* Exported constants --------------------------------------------------------*/
/** @defgroup EMXXLX_Exported_Constants EMXXLX Exported Constants
  * @{
  */

#define OSPI_FLASH_SIZE 			27
#define OSPI_PAGE_SIZE 				256
#define OSPI_END_ADDR               (1 << OSPI_FLASH_SIZE)

/* Configuration Registers Values */

/** @defgroup OSPI_Interface_Mode OSPI Interface Mode
  * @{
  */
#define MRAM_SPI_W_DS							0xFFU // SPI with DS
#define MRAM_SPI_WO_DS							0xDFU // SPI without DS
#define MRAM_DSPI_W_DS							0xFDU // Dual SPI with DS
#define MRAM_DSPI_WO_DS							0xDDU // Dual SPI without DS
#define MRAM_QSPI_W_DS							0xFBU // Quad SPI with DS
#define MRAM_QSPI_WO_DS							0xDBU // Quad SPI without DS
#define MRAM_QDTR_W_DS							0xEBU // Quad DTR with DS
#define MRAM_QDTR_WO_DS							0xCBU // Quad DTR without DS
#define MRAM_ODTR_W_DS							0xE7U // Octo DTR with DS
#define MRAM_ODTR_WO_DS							0xC7U // Octo DTR without DS
#define MRAM_OSPI_W_DS							0xB7U // Octo SPI with DS
#define MRAM_OSPI_WO_DS							0x97U // Octo SPI without DS
/**
  * @}
  */

/** @defgroup EMXXLX_Dummy_Cycles EMXXLX Dummy Cycles
  * @{
  */
#define MRAM_DEFAULT_DC							0x10U // Default dummy cycle value 16
#define MRAM_1_DC								0x01U // 1 dummy cycle
#define MRAM_2_DC								0x02U // 2 dummy cycles
#define MRAM_3_DC								0x03U // 3 dummy cycles
#define MRAM_4_DC								0x04U // 4 dummy cycles
#define MRAM_5_DC								0x05U // 5 dummy cycles
#define MRAM_6_DC								0x06U // 6 dummy cycles
#define MRAM_7_DC								0x07U // 7 dummy cycles
#define MRAM_8_DC								0x08U // 8 dummy cycles
#define MRAM_9_DC								0x09U // 9 dummy cycles
#define MRAM_10_DC								0x0AU // 10 dummy cycles
#define MRAM_11_DC								0x0BU // 11 dummy cycles
#define MRAM_12_DC								0x0CU // 12 dummy cycles
#define MRAM_13_DC								0x0DU // 13 dummy cycles
#define MRAM_14_DC								0x0EU // 14 dummy cycles
#define MRAM_15_DC								0x0FU // 15 dummy cycles
#define MRAM_16_DC								0x10U // 16 dummy cycles
#define MRAM_17_DC								0x11U // 17 dummy cycles
#define MRAM_18_DC								0x12U // 18 dummy cycles
#define MRAM_19_DC								0x13U // 19 dummy cycles
#define MRAM_20_DC								0x14U // 20 dummy cycles
#define MRAM_21_DC								0x15U // 21 dummy cycles
#define MRAM_22_DC								0x16U // 22 dummy cycles
#define MRAM_23_DC								0x17U // 23 dummy cycles
#define MRAM_24_DC								0x18U // 24 dummy cycles
#define MRAM_25_DC								0x19U // 25 dummy cycles
#define MRAM_26_DC								0x1AU // 26 dummy cycles
#define MRAM_27_DC								0x1BU // 27 dummy cycles
#define MRAM_28_DC								0x1CU // 28 dummy cycles
#define MRAM_29_DC								0x1DU // 29 dummy cycles
#define MRAM_30_DC								0x1EU // 30 dummy cycles
#define MRAM_31_DC								0x1FU // 31 dummy cycles
/**
  * @}
  */

/* Driver Strength Configuration Values */
#define MRAM_50_DRIVER_STR						0xFFU // Driver strength 50 Ohm
#define MRAM_35_DRIVER_STR						0xFEU // Driver strength 35 Ohm
#define MRAM_25_DRIVER_STR						0xFDU // Driver strength 25 Ohm
#define MRAM_18_DRIVER_STR						0xFCU // Driver strength 18 Ohm

/* Added DS Delay Values */
#define MRAM_0_ADDED_DELAY						0x0FU // Zero added DS delay
#define MRAM_100_ADDED_DELAY					0x0EU // 100pS added DS delay
#define MRAM_200_ADDED_DELAY					0x0DU // 200pS added DS delay
#define MRAM_300_ADDED_DELAY					0x0CU // 300pS added DS delay
#define MRAM_400_ADDED_DELAY					0x0BU // 400pS added DS delay
#define MRAM_500_ADDED_DELAY					0x0AU // 500pS added DS delay
#define MRAM_600_ADDED_DELAY					0x09U // 600pS added DS delay
#define MRAM_700_ADDED_DELAY					0x08U // 700pS added DS delay
#define MRAM_800_ADDED_DELAY					0x07U // 800pS added DS delay
#define MRAM_900_ADDED_DELAY					0x06U // 900pS added DS delay
#define MRAM_1000_ADDED_DELAY					0x05U // 1000pS added DS delay
#define MRAM_1100_ADDED_DELAY					0x04U // 1100pS added DS delay
#define MRAM_1200_ADDED_DELAY					0x03U // 1200pS added DS delay
#define MRAM_1300_ADDED_DELAY					0x02U // 1300pS added DS delay
#define MRAM_1400_ADDED_DELAY					0x01U // 1400pS added DS delay
#define MRAM_1500_ADDED_DELAY					0x00U // 1500pS added DS delay

/* Address Bytes Configuration Values */
#define MRAM_ADDRESS_BYTES_3					0xFFU // 3 Byte address mode
#define MRAM_ADDRESS_BYTES_4					0xFEU // 4 Byte address mode

/* XIP Execute-In-Place Configuration Values */
#define MRAM_XIP_DISABLE						0xFFU // XIP disabled default
#define MRAM_XIP_ENABLE							0xFEU // XIP enabled
#define MRAM_XIP_BOOT							0xFCU // XIP activated at boot time

/* Wrap Configuration Values */
#define MRAM_CONTINUOUS_WRAP					0xFFU // Continuous Wrap default
#define MRAM_64BYTE_WRAP						0xFEU // 64 byte wrapping
#define MRAM_32BYTE_WRAP						0xFDU // 32 byte wrapping
#define MRAM_16BYTE_WRAP						0xFCU // 16 byte wrapping

/* Miscellaneous Configuration Values */
#define MRAM_ERASE_VALUE_1						0x01U // Erased value set to 1
#define MRAM_ERASE_VALUE_0						0x00U // Erased value set to 0
#define MRAM_RESET_ENABLE						0x01U // Reset pin enabled
#define MRAM_RESET_DISABLE						0x00U // Reset pin disabled
#define MRAM_NONVOLATILE						0x01U // Non volatile operation mode
#define MRAM_VOLATILE							0x00U // Volatile operation mode
#define MRAM_OTPLOCK_ENABLE						0x01U // OTP locking enabled
#define MRAM_OTPLOCK_DISABLE					0x00U // OTP locking disabled

/**
  * @}
  */

/* Operation Commands */

/* Reset Operations */
#define MRAM_RESET_ENABLE_CMD                   0x66U // Enable reset
#define MRAM_RESET_CMD							0x99U // Reset memory

/* Read Operations */
#define MRAM_READ_NONVOL_CMD					0xB5U // Read non volatile register
#define MRAM_READ_VOL_CMD						0x85U // Read volatile register
#define MRAM_READ_GPR_CMD						0x96U // Read general purpose register

#define MRAM_READ_ID_CMD						0x9FU // Read manufacturer ID
#define MRAM_READ_ID_MULTIPLE_IO_CMD			0xAFU // Read manufacturer ID in multiple IO
#define MRAM_READ_CMD                           0x03U // Read given address data
#define MRAM_READ_FAST_CMD                   	0x0BU // Read given address data at a higher frequency
#define MRAM_READ_DUAL_O_CMD	 		    	0x3BU // Read given address data with dual SPI output
#define MRAM_READ_DUAL_IO_CMD					0xBBU // Read given address data with dual SPI input/output
#define MRAM_READ_QUAD_O_CMD					0x6BU // Read given address data with quad SPI output
#define MRAM_READ_QUAD_IO_CMD					0xEBU // Read given address data with quad SPI input/output
#define MRAM_READ_OCTO_O_CMD					0x8BU // Read given address data with octo SPI output
#define MRAM_READ_OCTO_IO_CMD					0xCBU // Read given address data with octo SPI input/output
#define MRAM_READ_DTR_CMD						0x0DU // Read given address data at a higher frequency with DTR protocol
#define MRAM_READ_DTR_DUAL_O_CMD				0x3DU // Read given address data with dual DTR output
#define MRAM_READ_DTR_DUAL_IO_CMD				0xBDU // Read given address data with dual DTR input/output
#define MRAM_READ_DTR_QUAD_O_CMD				0x6DU // Read given address data with quad DTR output
#define MRAM_READ_DTR_QUAD_IO_CMD				0xEDU // Read given address data with quad DTR input/output
#define MRAM_READ_WORD_QUAD_IO_CMD				0xE7U // Read given address data with quad SPI, lowest address bit must be 0
#define MRAM_READ_DTR_OCTO_O_CMD				0x9DU // Read given address data with octo DTR output
#define MRAM_READ_DTR_OCTO_IO_CMD				0xFDU // Read given address data with octo DTR input/output

/* Write Operations */
#define MRAM_WRITE_ENABLE_CMD                   0x06U // Sets the write enable latch bit, WEL
#define MRAM_WRITE_DISABLE_CMD                  0x04U // Clears the write enable latch bit, WEL
#define MRAM_WRITE_STATUS_CMD					0x01U // Write new values to status register
#define MRAM_WRITE_NONVOL_CMD					0xB1U // Write non volatile register data
#define MRAM_WRITE_VOL_CMD						0x81U // Write volatile register data

#define MRAM_WRITE_CMD							0x02U // Write data to given address
#define MRAM_WRITE_DUAL_CMD						0xA2U // Write data to given address in dual SPI mode
#define MRAM_WRITE_DUAL_E_CMD					0xD2U // Write data to given address in dual Extended SPI mode
#define MRAM_WRITE_QUAD_CMD						0x32U // Write data to given address in quad SPI mode
#define MRAM_WRITE_QUAD_E_CMD					0x38U // Write data to given address in quad Extended SPI mode
#define MRAM_WRITE_OCTO_CMD						0x82U // Write data to given address in octo SPI  mode
#define MRAM_WRITE_OCTO_E_CMD					0xC2U // Write data to given address in octo Extended SPI mode

/* Erase Operations */
#define MRAM_ERASE_4kB_SECTOR_CMD				0x20U // Erases 4kB of the addressed subsector
#define MRAM_ERASE_32kB_SECTOR_CMD				0x52U // Erases 32kB of the addressed subsector
#define MRAM_ERASE_SECTOR_CMD					0xD8U // Erases addressed 64kB sector
#define MRAM_ERASE_BULK_CMD						0xC7U // Erases selected address range ??
#define MRAM_ERASE_CHIP_CMD						0x60U // Erases the entire chip

/* OTP Operations */
#define MRAM_OTP_READ_CMD						0x4BU // Reads one time programmable data
#define MRAM_OTP_WRITE_CMD						0x42U // Writes one time programmable data

/* Extended Address Operation */
#define MRAM_4BADD_ENTER_CMD					0xB7U // Enables 4 Byte addressing mode
#define MRAM_4BADD_EXIT_CMD						0xE9U // Disables 4 Byte addressing mode

// Reading
#define MRAM_4BADD_READ_CMD						0x13U // Read given 4 Byte address data
#define MRAM_4BADD_READ_FAST_CMD				0x0CU // Read given 4 Byte address data at a higher frequency
#define MRAM_4BADD_READ_DUAL_O_CMD				0x3CU // Read given 4 Byte address data with dual SPI output
#define MRAM_4BADD_READ_DUAL_IO_CMD				0xBCU // Read given 4 Byte address data with dual SPI input/output
#define MRAM_4BADD_READ_QUAD_O_CMD				0x6CU // Read given 4 Byte address data with quad SPI output
#define MRAM_4BADD_READ_QUAD_IO_CMD				0xECU // Read given 4 Byte address data with quad SPI input/output
#define MRAM_4BADD_READ_OCTO_O_CMD				0x7CU // Read given 4 Byte address data with octo SPI output
#define MRAM_4BADD_READ_OCTO_IO_CMD				0xCCU // Read given 4 Byte address data with octo SPI input/output
#define MRAM_4BADD_READ_DTR_CMD					0x0EU // Read given 4 Byte address data with DTR protocol
#define MRAM_4BADD_READ_DTR_DUAL_IO_CMD			0xBEU // Read given 4 Byte address data with dual DTR input/output
#define MRAM_4BADD_READ_DTR_QUAD_IO_CMD			0xEEU // Read given 4 Byte address data with quad DTR input/output


// Writing
#define MRAM_4BADD_WRITE_CMD					0x12U // Write data to given 4 byte address
#define MRAM_4BADD_WRITE_QUAD_CMD				0x34U // Write data to given 4 byte address in quad SPI mode
#define MRAM_4BADD_WRITE_QUAD_E_CMD				0x3EU // Write data to given 4 byte address in quad Extended SPI mode
#define MRAM_4BADD_WRITE_OCTO_CMD				0x84U // Write data to given 4 byte address in octo SPI mode
#define MRAM_4BADD_WRITE_OCTO_E_CMD				0x8EU // Write data to given 4 byte address in octo Extended SPI mode

// Erasing
#define MRAM_4BADD_ERASE_SECTOR_4kB_CMD			0x21U // Erases 4kB of the 4 Byte addressed subsector
#define MRAM_4BADD_ERASE_SECTOR_32kB_CMD		0x5CU // Erases 32kB of the 4 Byte addressed subsector
#define MRAM_4BADD_ERASE_SECTOR_CMD				0xDCU // Erases 4 Byte addressed 64kB sector

/* Deep Power Down Operation */
#define MRAM_DPD_ENTER_CMD						0xB9U // Deep power down enter
#define MRAM_DPD_EXIT_CMD						0xABU //	Deep power down exit

/* Error handling */
#define MRAM_CLR_FLAGS_CMD						0x50U // Clear flag status register
#define MRAM_READ_STATUS_REG_CMD				0x05U // Read status register data
#define MRAM_READ_FLAGS_CMD						0x70U // Read flag status register data

#endif /* INC_MRAM_H_ */
