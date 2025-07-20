/*
 * camera.h
 *
 *  Created on: Jul 19, 2025
 *      Author: user
 */

#ifndef INC_CAMERA_H_
#define INC_CAMERA_H_

#include "main.h"

#define OV7725_ADDRESS 0x42

#define Camera_OK 0
#define camera_ERROR 1
#define Camera_delay HAL_Delay

struct regval_t{
	uint8_t reg_addr;
	uint8_t value;
};

typedef enum {
	PIXFORMAT_INVALID = 0,
	PIXFORMAT_RGB565,    // 2BPP/RGB565
	PIXFORMAT_JPEG,      // JPEG/COMPRESSED
	PIXFORMAT_YUV422,
	PIXFORMAT_GRAYSCALE,
	PIXFORMAT_BAYER,
} pixformat_t;

typedef enum {
    FRAMESIZE_INVALID = 0,
    FRAMESIZE_QQVGA,    // 160x120 x
} framesize_t;

typedef struct {
	I2C_HandleTypeDef *hi2c;
	uint8_t addr;
	uint32_t timeout;
	uint16_t manuf_id;
	uint16_t device_id;
	framesize_t framesize;
	pixformat_t pixformat;
} Camera_HandleTypeDef;

extern Camera_HandleTypeDef hcamera;;
extern const uint16_t dvp_cam_resolution[][2];

int32_t Camera_WriteReg(Camera_HandleTypeDef *hov, uint8_t regAddr, const uint8_t *pData);
int32_t Camera_ReadReg(Camera_HandleTypeDef *hov, uint8_t regAddr, uint8_t *pData);
int32_t Camera_WriteRegb2(Camera_HandleTypeDef *hov, uint16_t reg_addr, uint8_t reg_data);
int32_t Camera_ReadRegb2(Camera_HandleTypeDef *hov, uint16_t reg_addr, uint8_t *reg_data);
int32_t Camera_WriteRegList(Camera_HandleTypeDef *hov, const struct regval_t *reg_list);
int32_t Camera_read_id(Camera_HandleTypeDef *hov);
void Camera_Reset(Camera_HandleTypeDef *hov);
void Camera_XCLK_Set(uint8_t xclktype);
void Camera_Init_Device(I2C_HandleTypeDef *hi2c, framesize_t framesize);

#endif /* INC_CAMERA_H_ */
