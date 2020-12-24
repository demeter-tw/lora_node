#include "bsp_i2c.h"
#include "errno.h"
#include <stdio.h>

#if CONFIG_MODULE_I2C

int sensor_i2c_byte_write(unsigned char slave_addr, unsigned char reg_addr, unsigned char length, unsigned char const *data)
{
        unsigned char i;
        uint32_t i2c_timeout = I2C_FLAG_TIMEOUT_MS;

        /* Send START condition */
        I2C_GenerateSTART(CONFIG_I2C, ENABLE);

        i2c_timeout = I2C_FLAG_TIMEOUT_MS;

        /* Test on EV5 and clear it */
        while (!I2C_CheckEvent(CONFIG_I2C, I2C_EVENT_MASTER_MODE_SELECT)) {
                if ((i2c_timeout--) == 0) I2C_ERR_CB(NCORE_ERR_I2C_SEND_START);
        }

        /* Send slave address for write */
        I2C_Send7bitAddress(CONFIG_I2C, (slave_addr << 1), I2C_Direction_Transmitter);

        i2c_timeout = I2C_FLAG_TIMEOUT;
        /* Test on EV6 and clear it */
        while (!I2C_CheckEvent(CONFIG_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) {
                if ((i2c_timeout--) == 0) I2C_ERR_CB(NCORE_ERR_I2C_SEND_SLAVE_ADDR);
        }

        /* Send the slave's internal address to write to */
        I2C_SendData(CONFIG_I2C, reg_addr);


        i2c_timeout = I2C_FLAG_TIMEOUT_MS;
        /* Test on EV8 and clear it */
        while (!I2C_CheckEvent(CONFIG_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) {
                if ((i2c_timeout--) == 0) I2C_ERR_CB(NCORE_ERR_I2C_SEND_DATA);
        }

        for (i = 0; i < length; i++) {
                /* Prepare the register value to be sent */
                I2C_SendData(CONFIG_I2C, data[i]);

                i2c_timeout = I2C_FLAG_TIMEOUT_MS;
                /* Test on EV8 and clear it */
                while (!I2C_CheckEvent(CONFIG_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) {
                        if ((i2c_timeout--) == 0) I2C_ERR_CB(NCORE_ERR_I2C_SEND_DATA);
                }
        }

        /* Send STOP condition */
        I2C_GenerateSTOP(CONFIG_I2C, ENABLE);

        return NCORE_ERR_OK;
}


int sensor_i2c_buffer_read(unsigned char slave_addr, unsigned char reg_addr, unsigned char length, unsigned char *data)
{
        uint32_t i2c_timeout = I2C_LONG_TIMEOUT_MS;

        while (I2C_GetFlagStatus(CONFIG_I2C, I2C_FLAG_BUSY)) {
                if ((i2c_timeout--) == 0) I2C_ERR_CB(NCORE_ERR_I2C_BUSY);
        }


        I2C_GenerateSTART(CONFIG_I2C, ENABLE);

        i2c_timeout = I2C_FLAG_TIMEOUT_MS;
        /* Test on EV5 and clear it */
        while (!I2C_CheckEvent(CONFIG_I2C, I2C_EVENT_MASTER_MODE_SELECT)) {
                if ((i2c_timeout--) == 0) I2C_ERR_CB(NCORE_ERR_I2C_SEND_START);
        }

        /* Send slave address for write */
        I2C_Send7bitAddress(CONFIG_I2C, (slave_addr << 1), I2C_Direction_Transmitter);

        i2c_timeout = I2C_FLAG_TIMEOUT_MS;

        /* Test on EV6 and clear it */
        while (!I2C_CheckEvent(CONFIG_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) {
                if ((i2c_timeout--) == 0) I2C_ERR_CB(NCORE_ERR_I2C_SEND_SLAVE_ADDR);
        }

        /* Clear EV6 by setting again the PE bit */
        I2C_Cmd(CONFIG_I2C, ENABLE);

        /* Send the slave's internal address to write to */
        I2C_SendData(CONFIG_I2C, reg_addr);

        i2c_timeout = I2C_FLAG_TIMEOUT_MS;
        /* Test on EV8 and clear it */
        while (!I2C_CheckEvent(CONFIG_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) {
                if ((i2c_timeout--) == 0) I2C_ERR_CB(NCORE_ERR_I2C_SEND_DATA);
        }

        /* Send START condition a second time */
        I2C_GenerateSTART(CONFIG_I2C, ENABLE);

        i2c_timeout = I2C_FLAG_TIMEOUT_MS;
        /* Test on EV5 and clear it */
        while (!I2C_CheckEvent(CONFIG_I2C, I2C_EVENT_MASTER_MODE_SELECT)) {
                if ((i2c_timeout--) == 0) I2C_ERR_CB(NCORE_ERR_I2C_SEND_START);
        }

        /* Send slave address for read */
        I2C_Send7bitAddress(CONFIG_I2C, (slave_addr << 1), I2C_Direction_Receiver);

        i2c_timeout = I2C_FLAG_TIMEOUT_MS;
        /* Test on EV6 and clear it */
        while (!I2C_CheckEvent(CONFIG_I2C, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)) {
                if ((i2c_timeout--) == 0) I2C_ERR_CB(NCORE_ERR_I2C_SEND_SLAVE_ADDR);
        }

        /* While there is data to be read */
        while (length) {
                if (length == 1) {
                        /* Disable Acknowledgement */
                        I2C_AcknowledgeConfig(CONFIG_I2C, DISABLE);
                        /* Send STOP condition */
                        I2C_GenerateSTOP(CONFIG_I2C, ENABLE);
                }

                /* Test on EV7 and clear it */
                if (I2C_CheckEvent(CONFIG_I2C, I2C_EVENT_MASTER_BYTE_RECEIVED)) {
                        /* Read a byte from the slave */
                        *data = I2C_ReceiveData(CONFIG_I2C);

                        /* Point to the next location where the byte read will be saved */
                        data++;

                        /* Decrement the read bytes counter */
                        length--;
                }
        }

        /* Enable Acknowledgement to be ready for another reception */
        I2C_AcknowledgeConfig(CONFIG_I2C, ENABLE);

        return NCORE_ERR_OK;
}


int sensor_i2c_byte_write_16bit(unsigned char slave_addr, unsigned short reg_addr, unsigned char length, unsigned char const *data)
{
        unsigned char i;
        uint32_t i2c_timeout = I2C_FLAG_TIMEOUT_MS;

        /* Send START condition */
        I2C_GenerateSTART(CONFIG_I2C, ENABLE);

        i2c_timeout = I2C_FLAG_TIMEOUT_MS;

        /* Test on EV5 and clear it */
        while (!I2C_CheckEvent(CONFIG_I2C, I2C_EVENT_MASTER_MODE_SELECT)) {
                if ((i2c_timeout--) == 0) I2C_ERR_CB(NCORE_ERR_I2C_SEND_START);
        }

        /* Send slave address for write */
        I2C_Send7bitAddress(CONFIG_I2C, (slave_addr << 1), I2C_Direction_Transmitter);

        i2c_timeout = I2C_FLAG_TIMEOUT;
        /* Test on EV6 and clear it */
        while (!I2C_CheckEvent(CONFIG_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) {
                if ((i2c_timeout--) == 0) I2C_ERR_CB(NCORE_ERR_I2C_SEND_SLAVE_ADDR);
        }

        /* Send the slave's internal address to write to */
        I2C_SendData(CONFIG_I2C, (reg_addr >> 8) & 0x00FF);


        i2c_timeout = I2C_FLAG_TIMEOUT_MS;
        /* Test on EV8 and clear it */
        while (!I2C_CheckEvent(CONFIG_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) {
                if ((i2c_timeout--) == 0) I2C_ERR_CB(NCORE_ERR_I2C_SEND_DATA);
        }


        /* Send the slave's internal address to write to */
        I2C_SendData(CONFIG_I2C, reg_addr & 0x00FF);


        i2c_timeout = I2C_FLAG_TIMEOUT_MS;
        /* Test on EV8 and clear it */
        while (!I2C_CheckEvent(CONFIG_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) {
                if ((i2c_timeout--) == 0) I2C_ERR_CB(NCORE_ERR_I2C_SEND_DATA);
        }

        for (i = 0; i < length; i++) {
                /* Prepare the register value to be sent */
                I2C_SendData(CONFIG_I2C, data[i]);

                i2c_timeout = I2C_FLAG_TIMEOUT_MS;
                /* Test on EV8 and clear it */
                while (!I2C_CheckEvent(CONFIG_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) {
                        if ((i2c_timeout--) == 0) I2C_ERR_CB(NCORE_ERR_I2C_SEND_DATA);
                }
        }

        /* Send STOP condition */
        I2C_GenerateSTOP(CONFIG_I2C, ENABLE);

        return NCORE_ERR_OK;
}


int sensor_i2c_buffer_read_16bit(unsigned char slave_addr, unsigned short reg_addr, unsigned char length, unsigned char *data)
{
        uint32_t i2c_timeout = I2C_LONG_TIMEOUT_MS;

        while (I2C_GetFlagStatus(CONFIG_I2C, I2C_FLAG_BUSY)) {
                if ((i2c_timeout--) == 0) I2C_ERR_CB(NCORE_ERR_I2C_BUSY);
        }

        I2C_GenerateSTART(CONFIG_I2C, ENABLE);

        i2c_timeout = I2C_FLAG_TIMEOUT_MS;
        /* Test on EV5 and clear it */
        while (!I2C_CheckEvent(CONFIG_I2C, I2C_EVENT_MASTER_MODE_SELECT)) {
                if ((i2c_timeout--) == 0) I2C_ERR_CB(NCORE_ERR_I2C_SEND_START);
        }

        /* Send slave address for write */
        I2C_Send7bitAddress(CONFIG_I2C, (slave_addr << 1), I2C_Direction_Transmitter);

        i2c_timeout = I2C_FLAG_TIMEOUT_MS;

        /* Test on EV6 and clear it */
        while (!I2C_CheckEvent(CONFIG_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) {
                if ((i2c_timeout--) == 0) I2C_ERR_CB(NCORE_ERR_I2C_SEND_SLAVE_ADDR);
        }

        /* Clear EV6 by setting again the PE bit */
        I2C_Cmd(CONFIG_I2C, ENABLE);

        /* Send the slave's internal address to write to */
        I2C_SendData(CONFIG_I2C, (reg_addr >> 8) & 0x00FF);


        i2c_timeout = I2C_FLAG_TIMEOUT_MS;
        /* Test on EV8 and clear it */
        while (!I2C_CheckEvent(CONFIG_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) {
                if ((i2c_timeout--) == 0) I2C_ERR_CB(NCORE_ERR_I2C_SEND_DATA);
        }


        /* Send the slave's internal address to write to */
        I2C_SendData(CONFIG_I2C, reg_addr & 0x00FF);


        i2c_timeout = I2C_FLAG_TIMEOUT_MS;
        /* Test on EV8 and clear it */
        while (!I2C_CheckEvent(CONFIG_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) {
                if ((i2c_timeout--) == 0) I2C_ERR_CB(NCORE_ERR_I2C_SEND_DATA);
        }


        /* Send START condition a second time */
        I2C_GenerateSTART(CONFIG_I2C, ENABLE);

        i2c_timeout = I2C_FLAG_TIMEOUT_MS;
        /* Test on EV5 and clear it */
        while (!I2C_CheckEvent(CONFIG_I2C, I2C_EVENT_MASTER_MODE_SELECT)) {
                if ((i2c_timeout--) == 0) I2C_ERR_CB(NCORE_ERR_I2C_SEND_START);
        }

        /* Send slave address for read */
        I2C_Send7bitAddress(CONFIG_I2C, (slave_addr << 1), I2C_Direction_Receiver);

        i2c_timeout = I2C_FLAG_TIMEOUT_MS;
        /* Test on EV6 and clear it */
        while (!I2C_CheckEvent(CONFIG_I2C, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)) {
                if ((i2c_timeout--) == 0) I2C_ERR_CB(NCORE_ERR_I2C_SEND_SLAVE_ADDR);
        }

        /* While there is data to be read */
        while (length) {
                if (length == 1) {
                        /* Disable Acknowledgement */
                        I2C_AcknowledgeConfig(CONFIG_I2C, DISABLE);
                        /* Send STOP condition */
                        I2C_GenerateSTOP(CONFIG_I2C, ENABLE);
                }

                /* Test on EV7 and clear it */
                if (I2C_CheckEvent(CONFIG_I2C, I2C_EVENT_MASTER_BYTE_RECEIVED)) {
                        /* Read a byte from the slave */
                        *data = I2C_ReceiveData(CONFIG_I2C);

                        /* Point to the next location where the byte read will be saved */
                        data++;

                        /* Decrement the read bytes counter */
                        length--;
                }
        }

        /* Enable Acknowledgement to be ready for another reception */
        I2C_AcknowledgeConfig(CONFIG_I2C, ENABLE);

        return NCORE_ERR_OK;
}


int sensor_i2c_double_reg8_read(unsigned char slave_addr,
        unsigned char reg1_addr, unsigned char length1, unsigned char *data1,
        unsigned char reg2_addr, unsigned char length2, unsigned char *data2)
{
        uint32_t i2c_timeout = I2C_LONG_TIMEOUT_MS;

        while (I2C_GetFlagStatus(CONFIG_I2C, I2C_FLAG_BUSY)) {
                if ((i2c_timeout--) == 0) I2C_ERR_CB(NCORE_ERR_I2C_BUSY);
        }

        //=====  1. Send START + Slave addr (#W) + Reg1 addr =====
        I2C_GenerateSTART(CONFIG_I2C, ENABLE);

        i2c_timeout = I2C_FLAG_TIMEOUT_MS;
        /* Test on EV5 and clear it */
        while (!I2C_CheckEvent(CONFIG_I2C, I2C_EVENT_MASTER_MODE_SELECT)) {
                if ((i2c_timeout--) == 0) I2C_ERR_CB(NCORE_ERR_I2C_SEND_START);
        }

        /* Send slave address for write */
        I2C_Send7bitAddress(CONFIG_I2C, (slave_addr << 1), I2C_Direction_Transmitter);

        i2c_timeout = I2C_FLAG_TIMEOUT_MS;

        /* Test on EV6 and clear it */
        while (!I2C_CheckEvent(CONFIG_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) {
                if ((i2c_timeout--) == 0) I2C_ERR_CB(NCORE_ERR_I2C_SEND_SLAVE_ADDR);
        }

        /* Clear EV6 by setting again the PE bit */
        I2C_Cmd(CONFIG_I2C, ENABLE);

        /* Send the slave's internal address to write to */
        I2C_SendData(CONFIG_I2C, reg1_addr);

        i2c_timeout = I2C_FLAG_TIMEOUT_MS;
        /* Test on EV8 and clear it */
        while (!I2C_CheckEvent(CONFIG_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) {
                if ((i2c_timeout--) == 0) I2C_ERR_CB(NCORE_ERR_I2C_SEND_DATA);
        }

        //=====  2. Send START + Slave addr (R) =====
        /* Send START condition a second time */
        I2C_GenerateSTART(CONFIG_I2C, ENABLE);

        i2c_timeout = I2C_FLAG_TIMEOUT_MS;
        /* Test on EV5 and clear it */
        while (!I2C_CheckEvent(CONFIG_I2C, I2C_EVENT_MASTER_MODE_SELECT)) {
                if ((i2c_timeout--) == 0) I2C_ERR_CB(NCORE_ERR_I2C_SEND_START);
        }

        /* Send slave address for read */
        I2C_Send7bitAddress(CONFIG_I2C, (slave_addr << 1), I2C_Direction_Receiver);

        i2c_timeout = I2C_FLAG_TIMEOUT_MS;
        /* Test on EV6 and clear it */
        while (!I2C_CheckEvent(CONFIG_I2C, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)) {
                if ((i2c_timeout--) == 0) I2C_ERR_CB(NCORE_ERR_I2C_SEND_SLAVE_ADDR);
        }

        /* While there is data to be read */
        while (length1) {
                if (length1 == 1) {
                        /* Disable Acknowledgement */
                        I2C_AcknowledgeConfig(CONFIG_I2C, DISABLE);
                }

                /* Test on EV7 and clear it */
                if (I2C_CheckEvent(CONFIG_I2C, I2C_EVENT_MASTER_BYTE_RECEIVED)) {
                        /* Read a byte from the slave */
                        *data1 = I2C_ReceiveData(CONFIG_I2C);

                        /* Decrement the read bytes counter */
                        length1--;

                        /* Point to the next location where the byte read will be saved */
                        if (length1 > 0) {
                                data1++;
                        }
                }
        }

        /* Enable Acknowledgement to be ready for another reception */
        I2C_AcknowledgeConfig(CONFIG_I2C, ENABLE);

        //=====  3. Send START + Slave addr (#W) + Reg2 addr =====
        I2C_GenerateSTART(CONFIG_I2C, ENABLE);

        i2c_timeout = I2C_FLAG_TIMEOUT_MS;
        /* Test on EV5 and clear it */
        while (!I2C_CheckEvent(CONFIG_I2C, I2C_EVENT_MASTER_MODE_SELECT)) {
                if ((i2c_timeout--) == 0) I2C_ERR_CB(NCORE_ERR_I2C_SEND_START);
        }

        /* Send slave address for write */
        I2C_Send7bitAddress(CONFIG_I2C, (slave_addr << 1), I2C_Direction_Transmitter);

        i2c_timeout = I2C_FLAG_TIMEOUT_MS;

        /* Test on EV6 and clear it */
        while (!I2C_CheckEvent(CONFIG_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) {
                if ((i2c_timeout--) == 0) I2C_ERR_CB(NCORE_ERR_I2C_SEND_SLAVE_ADDR);
        }

        /* Clear EV6 by setting again the PE bit */
        I2C_Cmd(CONFIG_I2C, ENABLE);

        /* Send the slave's internal address to write to */
        I2C_SendData(CONFIG_I2C, reg2_addr);

        i2c_timeout = I2C_FLAG_TIMEOUT_MS;
        /* Test on EV8 and clear it */
        while (!I2C_CheckEvent(CONFIG_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) {
                if ((i2c_timeout--) == 0) I2C_ERR_CB(NCORE_ERR_I2C_SEND_DATA);
        }

        //===== 4. Send START + Slave addr (R) =====
        /* Send START condition a second time */
        I2C_GenerateSTART(CONFIG_I2C, ENABLE);

        i2c_timeout = I2C_FLAG_TIMEOUT_MS;
        /* Test on EV5 and clear it */
        while (!I2C_CheckEvent(CONFIG_I2C, I2C_EVENT_MASTER_MODE_SELECT)) {
                if ((i2c_timeout--) == 0) I2C_ERR_CB(NCORE_ERR_I2C_SEND_START);
        }

        /* Send slave address for read */
        I2C_Send7bitAddress(CONFIG_I2C, (slave_addr << 1), I2C_Direction_Receiver);

        i2c_timeout = I2C_FLAG_TIMEOUT_MS;
        /* Test on EV6 and clear it */
        while (!I2C_CheckEvent(CONFIG_I2C, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)) {
                if ((i2c_timeout--) == 0) I2C_ERR_CB(NCORE_ERR_I2C_SEND_SLAVE_ADDR);
        }

        /* While there is data to be read */
        while (length2) {
                if (length2 == 1) {
                        /* Disable Acknowledgement */
                        I2C_AcknowledgeConfig(CONFIG_I2C, DISABLE);
                        /* Send STOP condition */
                        I2C_GenerateSTOP(CONFIG_I2C, ENABLE);
                }

                /* Test on EV7 and clear it */
                if (I2C_CheckEvent(CONFIG_I2C, I2C_EVENT_MASTER_BYTE_RECEIVED)) {
                        /* Read a byte from the slave */
                        *data2 = I2C_ReceiveData(CONFIG_I2C);

                        /* Decrement the read bytes counter */
                        length2--;

                        /* Point to the next location where the byte read will be saved */
                        if (length2 > 0) {
                                data2++;
                        }
                }
        }

        /* Enable Acknowledgement to be ready for another reception */
        I2C_AcknowledgeConfig(CONFIG_I2C, ENABLE);

        return NCORE_ERR_OK;
}


/**
  * @brief I2C master init function
  * @param None
  * @retval None
  */
void sensor_i2c_init(void)
{
        GPIO_InitTypeDef gpio_init;
        I2C_InitTypeDef i2c_init;

        printf("Sensor I2C init\r\n");

        CONFIG_I2C_SCL_GPIO_CLK_FUNC(CONFIG_I2C_SCL_GPIO_CLK, ENABLE);
        CONFIG_I2C_SDA_GPIO_CLK_FUNC(CONFIG_I2C_SDA_GPIO_CLK, ENABLE);
        CONFIG_I2C_CLK_FUNC(CONFIG_I2C_CLK, ENABLE);

        gpio_init.GPIO_Pin = CONFIG_I2C_SCL_GPIO_PIN;
        gpio_init.GPIO_Mode = GPIO_Mode_AF_OD;
        gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(CONFIG_I2C_SCL_GPIO_PORT, &gpio_init);

        gpio_init.GPIO_Pin = CONFIG_I2C_SDA_GPIO_PIN;
        GPIO_Init(CONFIG_I2C_SDA_GPIO_PORT, &gpio_init);

        i2c_init.I2C_Mode = I2C_Mode_I2C;
        i2c_init.I2C_DutyCycle = I2C_DutyCycle_2;
        i2c_init.I2C_OwnAddress1 = I2C_OWN_ADDRESS;
        i2c_init.I2C_Ack = I2C_Ack_Enable;
        i2c_init.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
        i2c_init.I2C_ClockSpeed = I2C_SPEED;
        I2C_Init(CONFIG_I2C, &i2c_init);

        I2C_Cmd(CONFIG_I2C, ENABLE);
}

#endif /* CONFIG_MODULE_HW_I2C */

