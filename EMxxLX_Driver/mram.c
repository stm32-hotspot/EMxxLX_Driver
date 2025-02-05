/*
 * mram.c
 *
 *  Created on: Jul 31, 2023
 *      Author: Lucas Costa
 */

#include "mram.h"
#include "main.h"
#include "octospi.h"

#define DCC MRAM_DEFAULT_DC

// Global Variables related to SPI and addressing configurations

uint32_t InstMode = 0; // Instruction mode
uint32_t Read = 0;	   // Read Command
uint32_t Write = 0;	   // Write Command
uint32_t AddMode = 0;  // Address mode
uint32_t DatMode = 0;  // Data mode
uint32_t AddSize = 0;  // Address size
uint32_t CfgDc = 0;	   // Dummy cycles for configuration phase
uint32_t DC = 0;	   // Dummy cycles for operation phase

/**
 * @brief Receive an amount of data in blocking mode.
 * @note   When UART parity is not enabled (PCE = 0), and Word Length is configured to 9 bits (M1-M0 = 01),
 *         the received data is handled as a set of u16. In this case, Size must indicate the number
 *         of u16 available through pData.
 * @note When FIFO mode is enabled, the RXFNE flag is set as long as the RXFIFO
 *       is not empty. Read operations from the RDR register are performed when
 *       RXFNE flag is set. From hardware perspective, RXFNE flag and
 *       RXNE are mapped on the same bit-field.
 * @param huart   UART handle.
 * @param pData   Pointer to data buffer (u8 or u16 data elements).
 * @param Size    Amount of data elements (u8 or u16) to be received.
 * @param Timeout Timeout duration.
 * @retval HAL status
 */

// Base for other function descriptions ^
/* Functions */

/**
 *  @brief Initialize the device with the parameters on
 * 		   EMXXLX_ConfigurationTypeDef and InterfaceMode.
 * 	@param Ctx				SPI peripheral handle.
 *  @param Config			EMXXLX_ConfigurationTypeDef Contaning the parameters.
 *  @param InterfaceMode	Can be a either 1, 2, 4 or 8 based in the number of io lines.
 *  @retval HAL status
 */
uint8_t EMXXLX_Init(OSPI_HandleTypeDef *Ctx, EMXXLX_ConfigurationTypeDef Config,
		uint8_t InterfaceMode) {
	uint8_t nvol[9], vol[9], temp[9] = { 0 }, null = 0;
	jesd_reset();

	InstMode = HAL_OSPI_INSTRUCTION_1_LINE;
	Read = MRAM_READ_FAST_CMD;
	Write = MRAM_WRITE_CMD;
	AddMode = HAL_OSPI_ADDRESS_1_LINE;
	DatMode = HAL_OSPI_DATA_1_LINE;
	AddSize = HAL_OSPI_ADDRESS_24_BITS;

	nvol[0] = Config.SpiInterfaceMode;
	nvol[1] = Config.DummyCycles;
	nvol[2] = 0xFF;
	nvol[3] = Config.DriverStrenght;
	nvol[4] = Config.AddedDsDelay;
	nvol[5] = Config.AddressMode;
	nvol[6] = Config.XIPConfiguration;
	nvol[7] = Config.WrapConfiguration;
	nvol[8] = (Config.EraseBitValue & 0x01) << 7
			| (Config.ResetPinEnable & 0x01) << 1 | (Config.WriteMode & 0x01);
	for (uint8_t i = 0; i < 9; i++) {
		vol[i] = nvol[i];
	}
	vol[8] = vol[8] | (Config.OtpLockEnable & 0x01) << 2;
	EMXXLX_Clear_flags(Ctx);
	EMXXLX_Write_Enable(Ctx);

	EMXXLX_Read_Vol(Ctx, 0, temp, 9);
	EMXXLX_Read_Nonvol(Ctx, 0, temp, 9);

	EMXXLX_Write_Nonvol(Ctx, 0, nvol, 9);

	EMXXLX_Write_Vol(Ctx, 0, vol, 9);

	DC = Config.DummyCycles;
	switch (InterfaceMode) {
	case 1:
		InstMode = HAL_OSPI_INSTRUCTION_1_LINE;
		Read = MRAM_READ_FAST_CMD;
		Write = MRAM_WRITE_CMD;
		AddMode = HAL_OSPI_ADDRESS_1_LINE;
		DatMode = HAL_OSPI_DATA_1_LINE;
		break;

	case 2:
		InstMode = HAL_OSPI_INSTRUCTION_2_LINES;
		Read = MRAM_READ_DUAL_O_CMD;
		Write = MRAM_WRITE_DUAL_CMD;
		AddMode = HAL_OSPI_ADDRESS_2_LINES;
		DatMode = HAL_OSPI_DATA_2_LINES;
		break;

	case 4:
		InstMode = HAL_OSPI_INSTRUCTION_4_LINES;
		Read = MRAM_READ_QUAD_O_CMD;
		Write = MRAM_WRITE_QUAD_CMD;
		AddMode = HAL_OSPI_ADDRESS_4_LINES;
		DatMode = HAL_OSPI_DATA_4_LINES;
		break;

	case 8:
		InstMode = HAL_OSPI_INSTRUCTION_8_LINES;
		Read = MRAM_READ_OCTO_O_CMD;
		Write = MRAM_WRITE_OCTO_E_CMD;
		AddMode = HAL_OSPI_ADDRESS_8_LINES;
		DatMode = HAL_OSPI_DATA_8_LINES;
		CfgDc = MRAM_8_DC;
		break;

	default:
		return HAL_ERROR;
	}

	if (Config.AddressMode != 0xFF) {
		AddSize = HAL_OSPI_ADDRESS_32_BITS;
	} else {
		AddSize = HAL_OSPI_ADDRESS_24_BITS;
	}

	EMXXLX_Write_Enable(Ctx);
	EMXXLX_Write_Status(Ctx, &null);

	EMXXLX_Read_Status(Ctx, &temp[8]);
	if ((temp[8] & 0x2) == 0) {
		return HAL_ERROR;
	}

	EMXXLX_Read_Nonvol(Ctx, 0, temp, 9);
	for (uint8_t i = 0; i < 9; i++) {
		if (temp[i] != nvol[i]) {
			return HAL_ERROR;
		}
	}

	EMXXLX_Read_Vol(Ctx, 0, temp, 9);
	for (uint8_t i = 0; i < 9; i++) {
		if (temp[i] != vol[i]) {
			return HAL_ERROR;
		}
	}

	Ctx->Init.ClockPrescaler = 1;

	HAL_OSPI_Init(Ctx);
	return HAL_OK;
}

uint8_t EMXXLX_Refactor(OSPI_HandleTypeDef *Ctx)
{
	//Necessary variables
	uint8_t vol[9], temp[9], null[9], set[9];
	memset(null, 0, 9);
	memset(set, 0xFF, 9);
	jesd_reset();

	//Default device mode settings
	InstMode = HAL_OSPI_INSTRUCTION_1_LINE;
	Read = MRAM_READ_FAST_CMD;
	Write = MRAM_WRITE_CMD;
	AddMode = HAL_OSPI_ADDRESS_1_LINE;
	DatMode = HAL_OSPI_DATA_1_LINE;
	AddSize = HAL_OSPI_ADDRESS_24_BITS;

	//Operational device mode settings
	vol[0] = MRAM_OSPI_W_DS;
	vol[1] = MRAM_DEFAULT_DC;
	vol[2] = 0xFF;
	vol[3] = MRAM_50_DRIVER_STR;
	vol[4] = MRAM_0_ADDED_DELAY;
	vol[5] = MRAM_ADDRESS_BYTES_4;
	vol[6] = MRAM_XIP_DISABLE;
	vol[7] = MRAM_CONTINUOUS_WRAP;
	vol[8] = (MRAM_ERASE_VALUE_1 & 0x01) << 7 | (MRAM_RESET_ENABLE & 0x01) << 1 | (MRAM_NONVOLATILE & 0x01) | (MRAM_OTPLOCK_ENABLE & 0x01) << 2;

	//Start of operation
	EMXXLX_Clear_flags(Ctx);
	EMXXLX_Write_Enable(Ctx);

	temp[0] = 0x6B;									
	EMXXLX_Write_Vol(Ctx, 0x1E, &temp[0], 1);		//Writes ID to enter DFU
	EMXXLX_Write_Vol(Ctx, 0, vol, 9);				//Configures device interface

	//Configuring IP interface
	InstMode = HAL_OSPI_INSTRUCTION_8_LINES;
	Read = MRAM_READ_DTR_OCTO_O_CMD;
	Write = MRAM_WRITE_OCTO_E_CMD;
	AddMode = HAL_OSPI_ADDRESS_8_LINES;
	AddSize = HAL_OSPI_ADDRESS_32_BITS;
	DatMode = HAL_OSPI_DATA_8_LINES;
	CfgDc = MRAM_8_DC;

	//Initialize Nonvol registers
	EMXXLX_Write_Nonvol(Ctx, 0, null, 9);
	EMXXLX_Write_Nonvol(Ctx, 0, set, 9);

	//Initialize clear block protect bits
	temp[0] = 0x7C;
	EMXXLX_Write_Status(Ctx, &temp[0]);
	EMXXLX_Read_Status(Ctx, &temp[0]);
	if ((temp[0] & 0x7C) != 0x7C)
	{
		return HAL_ERROR;
	}

	EMXXLX_Write_Status(Ctx, &null[0]);
	EMXXLX_Read_Status(Ctx, &temp[0]);
	if ((temp[0] & 0x7C) != 0)
	{
		return HAL_ERROR;
	}

	//Initialize device array sequence
	//Variables required for array sequence
	uint32_t end_addr, page_num;
	end_addr = (pow(2, OSPI_FLASH_SIZE));
	page_num = end_addr / 2048;

	uint8_t init_0[256], init_ff[256], init_temp[256];
	memset(init_ff, 0xff, 256);
	memset(init_0, 0x0, 256);

//	//Write all zeros
//	for (uint32_t i = 0; i <= page_num; i++)
//	{
//		EMXXLX_Write(Ctx, 256 * i, init_0, 256);
//		EMXXLX_Polling_MemReady(Ctx, HAL_MAX_DELAY);
//	}
//
//	//Write all ones
//	for (uint32_t i = 0; i <= page_num; i++)
//	{
//		EMXXLX_Write(Ctx, 256 * i, init_ff, 256);
//		EMXXLX_Polling_MemReady(Ctx, HAL_MAX_DELAY);
//	}
//
//	//Write all zeros
//	for (uint32_t i = 0; i <= page_num; i++)
//		{
//			EMXXLX_Write(Ctx, 256 * i, init_0, 256);
//			EMXXLX_Polling_MemReady(Ctx, HAL_MAX_DELAY);
//		}
//
//	//Verify the write process was sucessful
//	for (uint32_t i = 0; i <= page_num; i++)
//	{
//		EMXXLX_Read(Ctx, 256, init_temp, 256);
//		if (memcmp(init_0, init_temp, 256) == 0)
//		{
//			return HAL_ERROR;
//		}
//	}
//
//	//Write all ones
//	for (uint32_t i = 0; i <= page_num; i++)
//	{
//		EMXXLX_Write(Ctx, 256 * i, init_ff, 256);
//		EMXXLX_Polling_MemReady(Ctx, HAL_MAX_DELAY);
//	}
//
//	//Verify the write process was sucessful
//	for (uint32_t i = 0; i <= page_num; i++)
//	{
//		EMXXLX_Read(Ctx, 256 * i, init_temp, 256);
//		if (memcmp(init_ff, init_temp, 256) == 0)
//		{
//			return HAL_ERROR;
//		}
//	}

	//Exit DFIM mode
	EMXXLX_Write_Vol(Ctx, 0x1E, &null[0], 1);
	EMXXLX_Write_Disable(Ctx);

	return HAL_OK;
}

uint8_t EMXXLX_Write_Enable(OSPI_HandleTypeDef *Ctx)
{
	OSPI_RegularCmdTypeDef sCommand = {0};

	/* Initialize the write register command */
	sCommand.Instruction = MRAM_WRITE_ENABLE_CMD;
	sCommand.InstructionMode = InstMode;

	/* Configure the command */
	if (HAL_OSPI_Command(Ctx, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{

		return HAL_ERROR;
	}

	return HAL_OK;
}

uint8_t EMXXLX_Polling_MemReady(OSPI_HandleTypeDef *Ctx, uint32_t Timeout)
{
	uint8_t temp[1];
	uint32_t Tickstart = HAL_GetTick();

	EMXXLX_Read_Flags(Ctx, &temp[1]);

	while ((temp[1] & 0x80) == 0)
	{
		EMXXLX_Read_Flags(Ctx, &temp[1]);

		if (Timeout != HAL_MAX_DELAY)
		{
			if (((HAL_GetTick() - Tickstart) > Timeout) || (Timeout == 0U))
			{
				return HAL_ERROR;
			}
		}
	}

	return HAL_OK;
}

uint8_t EMXXLX_SRWP_Disable(OSPI_HandleTypeDef *Ctx, uint8_t Port, uint8_t Pin)
{
	HAL_GPIO_DeInit((void *)&Port, Pin);

	GPIO_InitTypeDef GPIO_InitStruct = {0};

	GPIO_InitStruct.Pin = Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init((void *)&Port, &GPIO_InitStruct);

	HAL_GPIO_WritePin((void *)&Port, Pin, GPIO_PIN_SET);

	EMXXLX_Write_Status(Ctx, 0);

	HAL_GPIO_DeInit((void *)&Port, Pin);

	GPIO_InitStruct.Pin = Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF10_OCTOSPI1;
	HAL_GPIO_Init((void *)&Port, &GPIO_InitStruct);
	return HAL_OK;
}

uint8_t EMXXLX_MemoryMapped_Config(OSPI_HandleTypeDef *Ctx)
{
	OSPI_RegularCmdTypeDef sCommand = {0};

	EMXXLX_Write_Enable(Ctx);

	sCommand.OperationType = HAL_OSPI_OPTYPE_WRITE_CFG;
	sCommand.Instruction = Write;
	sCommand.InstructionMode = InstMode;
	sCommand.AddressMode = AddMode;
	sCommand.AddressSize = HAL_OSPI_ADDRESS_32_BITS;
	sCommand.DataMode = DatMode;
	sCommand.DQSMode = HAL_OSPI_DQS_ENABLE;

	if (HAL_OSPI_Command(Ctx, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		Error_Handler();
	}

	sCommand.OperationType = HAL_OSPI_OPTYPE_READ_CFG;
	sCommand.Instruction = Read;
	sCommand.AddressMode = AddMode;
	sCommand.AddressSize = HAL_OSPI_ADDRESS_32_BITS;
	sCommand.DataMode = DatMode;
	sCommand.DummyCycles = DC;
	sCommand.DQSMode = HAL_OSPI_DQS_DISABLE;

	if (HAL_OSPI_Command(Ctx, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		Error_Handler();
	}

	OSPI_MemoryMappedTypeDef sMemMappedCfg = {0};

	sMemMappedCfg.TimeOutActivation = HAL_OSPI_TIMEOUT_COUNTER_ENABLE;
	sMemMappedCfg.TimeOutPeriod = 0xFFFF;
	if (HAL_OSPI_MemoryMapped(Ctx, &sMemMappedCfg) != HAL_OK)
	{
		Error_Handler();
	}

	return HAL_OK;
}

void jesd_reset()
{

	// Reconfigure OCTOSPI pins to manual mode
	HAL_OSPI_MspDeInit(&hospi1);

	GPIO_InitTypeDef GPIO_InitStruct = {0};

	GPIO_InitStruct.Pin = IO1_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(IO1_GPIO_Port, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = NCS_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(NCS_GPIO_Port, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = CLK_Pin | IO0_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(CLK_GPIO_Port, &GPIO_InitStruct);

	// Set IO1 to high Z
	HAL_GPIO_WritePin(IO1_GPIO_Port, IO1_Pin, SET);

	// Issue Reset sequence
	HAL_GPIO_WritePin(NCS_GPIO_Port, NCS_Pin, RESET);
	HAL_GPIO_WritePin(NCS_GPIO_Port, NCS_Pin, SET);
	HAL_GPIO_WritePin(IO0_GPIO_Port, IO0_Pin, SET);
	HAL_GPIO_WritePin(NCS_GPIO_Port, NCS_Pin, RESET);
	HAL_GPIO_WritePin(NCS_GPIO_Port, NCS_Pin, SET);
	HAL_GPIO_WritePin(IO0_GPIO_Port, IO0_Pin, RESET);
	HAL_GPIO_WritePin(NCS_GPIO_Port, NCS_Pin, RESET);
	HAL_GPIO_WritePin(NCS_GPIO_Port, NCS_Pin, SET);
	HAL_GPIO_WritePin(IO0_GPIO_Port, IO0_Pin, SET);
	HAL_GPIO_WritePin(NCS_GPIO_Port, NCS_Pin, RESET);
	HAL_GPIO_WritePin(NCS_GPIO_Port, NCS_Pin, SET);

	// Reconfigure OCTOSPI IO
	HAL_GPIO_DeInit(CLK_GPIO_Port, CLK_Pin | IO0_Pin | IO1_Pin);
	HAL_GPIO_DeInit(NCS_GPIO_Port, NCS_Pin);

	HAL_OSPI_MspInit(&hospi1);
}

uint8_t EMXXLX_Write_Disable(OSPI_HandleTypeDef *Ctx)
{
	OSPI_RegularCmdTypeDef sCommand = {0};

	/* Initialize the write register command */
	sCommand.Instruction = MRAM_WRITE_DISABLE_CMD;
	sCommand.InstructionMode = InstMode;

	/* Configure the command */
	if (HAL_OSPI_Command(Ctx, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{

		return HAL_ERROR;
	}

	return HAL_OK;
}

uint8_t EMXXLX_Reset(OSPI_HandleTypeDef *Ctx)
{
	OSPI_RegularCmdTypeDef sCommand = {0};

	/* Initialize the write register command */
	sCommand.Instruction = MRAM_RESET_CMD;
	sCommand.InstructionMode = InstMode;

	/* Configure the command */
	if (HAL_OSPI_Command(Ctx, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{

		return HAL_ERROR;
	}

	return HAL_OK;
}

uint8_t EMXXLX_Read_ID(OSPI_HandleTypeDef *Ctx, uint8_t *Value)
{
	OSPI_RegularCmdTypeDef sCommand = {0};

	/* Initialize the read register command */
	sCommand.Instruction = MRAM_READ_ID_MULTIPLE_IO_CMD;
	sCommand.InstructionMode = InstMode;
	sCommand.DataMode = DatMode;
	sCommand.NbData = 3;
	sCommand.DummyCycles = CfgDc;

	/* Configure the command */
	if (HAL_OSPI_Command(Ctx, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		return HAL_ERROR;
	}

	/* Reception of the data */
	if (HAL_OSPI_Receive(Ctx, Value, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		return HAL_ERROR;
	}

	return HAL_OK;
}

uint8_t EMXXLX_Read(OSPI_HandleTypeDef *Ctx, uint32_t address, uint8_t *pData,
					uint32_t size)
{
	OSPI_RegularCmdTypeDef sCommand = {0};

	/* Initialize the read register command */
	sCommand.Instruction = Read;
	sCommand.InstructionMode = InstMode;
	sCommand.Address = address;
	sCommand.AddressMode = AddMode;
	sCommand.AddressSize = AddSize;
	sCommand.DataMode = DatMode;
	sCommand.NbData = size;
	sCommand.DummyCycles = DC;

	/* Configure the command */
	if (HAL_OSPI_Command(Ctx, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		return HAL_ERROR;
	}

	/* Reception of the data */
	if (HAL_OSPI_Receive(Ctx, pData, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		return HAL_ERROR;
	}

	return HAL_OK;
}

uint8_t EMXXLX_Erase_Chip(OSPI_HandleTypeDef *Ctx)
{
	OSPI_RegularCmdTypeDef sCommand = {0};
	uint8_t temp[1];

	/* Initialize the read register command */
	sCommand.Instruction = MRAM_ERASE_CHIP_CMD;
	sCommand.InstructionMode = InstMode;

	/* Configure the command */
	if (HAL_OSPI_Command(Ctx, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		return HAL_ERROR;
	}

	EMXXLX_Read_Flags(Ctx, &temp[1]);

	while ((temp[1] & 0x80) == 0)
	{
		EMXXLX_Read_Flags(Ctx, &temp[1]);
	}

	return HAL_OK;
}

uint8_t EMXXLX_Write(OSPI_HandleTypeDef *Ctx, uint32_t address, uint8_t *Value,
					 uint32_t size)
{
	OSPI_RegularCmdTypeDef sCommand = {0};

	/* Initialize the read register command */
	sCommand.Instruction = Write;
	sCommand.InstructionMode = InstMode;
	sCommand.Address = address;
	sCommand.AddressMode = AddMode;
	sCommand.AddressSize = AddSize;
	sCommand.DataMode = DatMode;
	sCommand.NbData = size;

	/* Configure the command */
	if (HAL_OSPI_Command(Ctx, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		return HAL_ERROR;
	}

	/* Reception of the data */
	if (HAL_OSPI_Transmit(Ctx, Value, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		return HAL_ERROR;
	}

	return HAL_OK;
}

uint8_t EMXXLX_Read_Nonvol(OSPI_HandleTypeDef *Ctx, uint32_t address,
						   uint8_t *Value, uint8_t size)
{
	OSPI_RegularCmdTypeDef sCommand = {0};

	/* Initialize the read register command */
	sCommand.Instruction = MRAM_READ_NONVOL_CMD;
	sCommand.InstructionMode = InstMode;
	sCommand.Address = address;
	sCommand.AddressMode = AddMode;
	sCommand.AddressSize = AddSize;
	sCommand.DataMode = DatMode;
	sCommand.NbData = size;
	sCommand.DummyCycles = CfgDc;

	/* Configure the command */
	if (HAL_OSPI_Command(Ctx, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		return HAL_ERROR;
	}

	/* Reception of the data */
	if (HAL_OSPI_Receive(Ctx, (uint8_t *)Value, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		return HAL_ERROR;
	}

	return HAL_OK;
}

uint8_t EMXXLX_Write_Nonvol(OSPI_HandleTypeDef *Ctx, uint32_t address,
							uint8_t *pData, uint8_t size)
{
	OSPI_RegularCmdTypeDef sCommand = {0};

	/* Initialize the write register command */
	sCommand.Instruction = MRAM_WRITE_NONVOL_CMD;
	sCommand.InstructionMode = InstMode;
	sCommand.Address = address;
	sCommand.AddressMode = AddMode;
	sCommand.AddressSize = AddSize;
	sCommand.DataMode = DatMode;
	sCommand.NbData = size;

	/* Configure the command */
	if (HAL_OSPI_Command(Ctx, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{

		return HAL_ERROR;
	}

	if (HAL_OSPI_Transmit(Ctx, pData, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		return HAL_ERROR;
	}

	return HAL_OK;
}

uint8_t EMXXLX_Read_Vol(OSPI_HandleTypeDef *Ctx, uint32_t address,
						uint8_t *Value, uint8_t size)
{
	OSPI_RegularCmdTypeDef sCommand = {0};

	/* Initialize the read register command */
	sCommand.Instruction = MRAM_READ_VOL_CMD;
	sCommand.InstructionMode = InstMode;
	sCommand.Address = address;
	sCommand.AddressMode = AddMode;
	sCommand.AddressSize = AddSize;
	sCommand.DataMode = DatMode;
	sCommand.NbData = size;
	sCommand.DummyCycles = CfgDc;

	/* Configure the command */
	if (HAL_OSPI_Command(Ctx, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		return HAL_ERROR;
	}

	/* Reception of the data */
	if (HAL_OSPI_Receive(Ctx, (uint8_t *)Value, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		return HAL_ERROR;
	}

	return HAL_OK;
}

uint8_t EMXXLX_Write_Vol(OSPI_HandleTypeDef *Ctx, uint32_t address,
						 uint8_t *pData, uint8_t size)
{
	OSPI_RegularCmdTypeDef sCommand = {0};

	/* Initialize the write register command */
	sCommand.Instruction = MRAM_WRITE_VOL_CMD;
	sCommand.InstructionMode = InstMode;
	sCommand.Address = address;
	sCommand.AddressMode = AddMode;
	sCommand.AddressSize = AddSize;
	sCommand.DataMode = DatMode;
	sCommand.NbData = size;

	/* Configure the command */
	if (HAL_OSPI_Command(Ctx, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{

		return HAL_ERROR;
	}

	if (HAL_OSPI_Transmit(Ctx, pData, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		return HAL_ERROR;
	}

	return HAL_OK;
}

uint8_t EMXXLX_Write_Status(OSPI_HandleTypeDef *Ctx, uint8_t *pData)
{
	OSPI_RegularCmdTypeDef sCommand = {0};

	/* Initialize the write register command */
	sCommand.Instruction = MRAM_WRITE_STATUS_CMD;
	sCommand.InstructionMode = InstMode;
	sCommand.DataMode = DatMode;
	sCommand.NbData = 1;

	/* Configure the command */
	if (HAL_OSPI_Command(Ctx, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{

		return HAL_ERROR;
	}

	if (HAL_OSPI_Transmit(Ctx, pData, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		return HAL_ERROR;
	}

	return HAL_OK;
}

uint8_t EMXXLX_Read_Flags(OSPI_HandleTypeDef *Ctx, uint8_t *Value)
{
	OSPI_RegularCmdTypeDef sCommand = {0};

	/* Initialize the read register command */
	sCommand.Instruction = MRAM_READ_FLAGS_CMD;
	sCommand.InstructionMode = InstMode;
	sCommand.DataMode = DatMode;
	sCommand.NbData = 1;
	sCommand.DummyCycles = CfgDc;

	/* Configure the command */
	if (HAL_OSPI_Command(Ctx, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		return HAL_ERROR;
	}

	/* Reception of the data */
	if (HAL_OSPI_Receive(Ctx, (uint8_t *)Value, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		return HAL_ERROR;
	}

	return HAL_OK;
}

uint8_t EMXXLX_Clear_flags(OSPI_HandleTypeDef *Ctx)
{
	OSPI_RegularCmdTypeDef sCommand = {0};

	/* Initialize the write register command */
	sCommand.Instruction = MRAM_CLR_FLAGS_CMD;
	sCommand.InstructionMode = InstMode;

	/* Configure the command */
	if (HAL_OSPI_Command(Ctx, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{

		return HAL_ERROR;
	}

	return HAL_OK;
}

uint8_t EMXXLX_Read_Status(OSPI_HandleTypeDef *Ctx, uint8_t *Value)
{
	OSPI_RegularCmdTypeDef sCommand = {0};

	/* Initialize the read register command */
	sCommand.Instruction = MRAM_READ_STATUS_REG_CMD;
	sCommand.InstructionMode = InstMode;
	sCommand.DataMode = DatMode;
	sCommand.NbData = 1;
	sCommand.DummyCycles = CfgDc;

	/* Configure the command */
	if (HAL_OSPI_Command(Ctx, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		return HAL_ERROR;
	}

	/* Reception of the data */
	if (HAL_OSPI_Receive(Ctx, (uint8_t *)Value, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		return HAL_ERROR;
	}

	return HAL_OK;
}
