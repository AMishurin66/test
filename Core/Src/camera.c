#include "camera.h"
#include "ov7725.h"

Camera_HandleTypeDef hcamera;

// Resolution table
const uint16_t dvp_cam_resolution[][2] = {
	{0, 0},
	{160, 120}, /* QQVGA     */
};

int32_t Camera_WriteReg(Camera_HandleTypeDef *hov, uint8_t regAddr, const uint8_t *pData)
{
	uint8_t tt[2];
	tt[0] = regAddr;
	tt[1] = pData[0];
	if (HAL_I2C_Master_Transmit(hov->hi2c, hov->addr, tt, 2, hov->timeout) == HAL_OK)
	{return Camera_OK;}
	else {return camera_ERROR;}
}

int32_t Camera_ReadReg(Camera_HandleTypeDef *hov, uint8_t regAddr, uint8_t *pData)
{
	HAL_I2C_Master_Transmit(hov->hi2c, hov->addr + 1, &regAddr, 1, hov->timeout);
	if (HAL_I2C_Master_Receive(hov->hi2c, hov->addr + 1, pData, 1, hov->timeout) == HAL_OK)
	{return Camera_OK;}
	else {return camera_ERROR;}
}

int32_t Camera_WriteRegb2(Camera_HandleTypeDef *hov, uint16_t reg_addr, uint8_t reg_data)
{
	if (HAL_I2C_Mem_Write(hov->hi2c, hov->addr + 1, reg_addr,
						  I2C_MEMADD_SIZE_16BIT, &reg_data, 1, hov->timeout) == HAL_OK)
	{return Camera_OK;}
	else {return camera_ERROR;}
}

int32_t Camera_ReadRegb2(Camera_HandleTypeDef *hov, uint16_t reg_addr, uint8_t *reg_data)
{
	if (HAL_I2C_Mem_Read(hov->hi2c, hov->addr + 1, reg_addr,
						 I2C_MEMADD_SIZE_16BIT, reg_data, 1, hov->timeout) == HAL_OK)
	{return Camera_OK;}
	else {return camera_ERROR;}
}

int32_t Camera_WriteRegList(Camera_HandleTypeDef *hov, const struct regval_t *reg_list)
{
	const struct regval_t *pReg = reg_list;
	while (pReg->reg_addr != 0xFF && pReg->value != 0xFF)
	{
		int write_result = Camera_WriteReg(hov, pReg->reg_addr, &(pReg->value));
		if (write_result != Camera_OK)
		{
			return write_result;
		}
		pReg++;
	}
	return Camera_OK;
}

int32_t Camera_read_id(Camera_HandleTypeDef *hov)
{
	uint8_t temp[2];
	temp[0] = 0x01;

	Camera_WriteReg(hov, 0xFF, temp);
	Camera_ReadReg(hov, 0x1C, &temp[0]);
	Camera_ReadReg(hov, 0x1D, &temp[1]);
	hov->manuf_id = ((uint16_t)temp[0] << 8) | temp[1];
	Camera_ReadReg(hov, 0x0A, &temp[0]);
	Camera_ReadReg(hov, 0x0B, &temp[1]);

	hov->device_id = ((uint16_t)temp[0] << 8) | temp[1];
	return 0;
}

void Camera_Reset(Camera_HandleTypeDef *hov)
{
	uint8_t temp;
	temp = 0x01;
	Camera_WriteReg(hov, 0xFF, &temp);
	temp = 0x80;
	Camera_WriteReg(hov, 0x12, &temp);
	HAL_Delay(100);
}

void Camera_Init_Device(I2C_HandleTypeDef *hi2c, framesize_t framesize)
{
	hcamera.hi2c = hi2c;
	hcamera.addr = OV7725_ADDRESS;
	hcamera.timeout = 100;
	Camera_read_id(&hcamera);
	if (hcamera.manuf_id == 0x7fa2 && ((hcamera.device_id - 0x7721) <= 2))
	{
		ov7725_init(framesize);
	}
	else
	{
		hcamera.addr = 0;
		hcamera.device_id = 0;
	}
}

