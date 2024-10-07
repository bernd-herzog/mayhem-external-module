#include <iostream>
#include <memory>
#include <cstring>

#include "driver/i2c.h"

// NANO:A1 is NORA:H8 is ESP32:7 is GPIO:2
#define I2C_SLAVE_SDA_IO GPIO_NUM_2

// NANO:A2 is NORA:J9 is ESP32:8 is GPIO:3
#define I2C_SLAVE_SCL_IO GPIO_NUM_3

#define ESP_SLAVE_ADDR 0x51

#define LED_RED GPIO_NUM_46
#define LED_GREEN GPIO_NUM_0
#define LED_BLUE GPIO_NUM_45

typedef struct
{
    uint32_t api_version;
    uint32_t module_version;
    char module_name[20];
    // uint32_t _unused;
} device_info;

void initialize_gpio()
{
    gpio_install_isr_service(0);

    gpio_set_direction(LED_RED, GPIO_MODE_OUTPUT);
    gpio_set_direction(LED_GREEN, GPIO_MODE_OUTPUT);
    gpio_set_direction(LED_BLUE, GPIO_MODE_OUTPUT);

    gpio_set_level(LED_RED, 1);
    gpio_set_level(LED_GREEN, 1);
    gpio_set_level(LED_BLUE, 1);
}

/*
// old api



void initialize_old_i2c()
{

    i2c_config_t conf_slave;
    std::memset(&conf_slave, 0, sizeof(i2c_config_t));

    conf_slave.sda_io_num = I2C_SLAVE_SDA_IO;
    conf_slave.sda_pullup_en = GPIO_PULLUP_DISABLE;
    conf_slave.scl_io_num = I2C_SLAVE_SCL_IO;
    conf_slave.scl_pullup_en = GPIO_PULLUP_DISABLE,
    conf_slave.mode = I2C_MODE_SLAVE;
    conf_slave.slave.addr_10bit_en = 0;
    conf_slave.slave.slave_addr = ESP_SLAVE_ADDR;

    if (i2c_param_config(I2C_NUM_0, &conf_slave) != ESP_OK)
    {
        std::cout << "Error configuring I2C" << std::endl;
        return;
    }

    if (i2c_driver_install(I2C_NUM_0, conf_slave.mode, I2C_SLAVE_RX_BUF_LEN, I2C_SLAVE_TX_BUF_LEN, 0) != ESP_OK)
    {
        std::cout << "Error installing I2C driver" << std::endl;
        return;
    }
}
*/

// new api
/*
#include "driver/i2c_slave.h"

i2c_slave_dev_handle_t slave_handle_command;
// // i2c_slave_dev_handle_t slave_handle_data;
QueueHandle_t slave_queue;

#define DATA_LENGTH 512
#define I2C_SLAVE_TX_BUF_LEN (2 * DATA_LENGTH)
#define I2C_SLAVE_RX_BUF_LEN (2 * DATA_LENGTH)

bool i2c_slave_received_callback(i2c_slave_dev_handle_t i2c_slave, const i2c_slave_rx_done_event_data_t *edata, void *user_data)
{
    BaseType_t high_task_wakeup = pdFALSE;
    QueueHandle_t receive_queue = (QueueHandle_t)user_data;

    xQueueSendFromISR(receive_queue, edata, &high_task_wakeup);

    return high_task_wakeup == pdTRUE;
}

void initialize_new_i2c()
{
    i2c_slave_config_t slave_config;
    std::memset(&slave_config, 0, sizeof(i2c_slave_config_t));
    slave_config.addr_bit_len = I2C_ADDR_BIT_LEN_7;
    slave_config.clk_source = I2C_CLK_SRC_DEFAULT;
    slave_config.i2c_port = I2C_NUM_0;
    slave_config.send_buf_depth = I2C_SLAVE_TX_BUF_LEN;
    slave_config.scl_io_num = I2C_SLAVE_SCL_IO;
    slave_config.sda_io_num = I2C_SLAVE_SDA_IO;
    slave_config.slave_addr = ESP_SLAVE_ADDR;
    slave_config.flags.access_ram_en = 0;

    ESP_ERROR_CHECK(i2c_new_slave_device(&slave_config, &slave_handle_command));

    i2c_slave_event_callbacks_t cbs = {
        .on_recv_done = i2c_slave_received_callback,
    };

    slave_queue = xQueueCreate(1, sizeof(i2c_slave_rx_done_event_data_t));

    ESP_ERROR_CHECK(i2c_slave_register_event_callbacks(slave_handle_command, &cbs, slave_queue));

    // i2c_slave_config_t slave_config_data;
    // std::memset(&slave_config_data, 0, sizeof(i2c_slave_config_t));
    // slave_config_data.addr_bit_len = I2C_ADDR_BIT_LEN_7;
    // slave_config_data.clk_source = I2C_CLK_SRC_DEFAULT;
    // slave_config_data.i2c_port = I2C_NUM_1;
    // slave_config_data.send_buf_depth = I2C_SLAVE_TX_BUF_LEN;
    // slave_config_data.scl_io_num = I2C_SLAVE_SCL_IO;
    // slave_config_data.sda_io_num = I2C_SLAVE_SDA_IO;
    // slave_config_data.slave_addr = 0x50;
    // slave_config_data.flags.access_ram_en = 1;
    // ESP_ERROR_CHECK(i2c_new_slave_device(&slave_config_data, &slave_handle_data));
}
//*/

// fixed api
#include "i2c_slave_driver.h"

QueueHandle_t slave_queue;

bool i2c_slave_callback(struct i2c_slave_device_t *dev, I2CSlaveCallbackReason reason)
{
    switch (reason)
    {
    case I2C_CALLBACK_REPEAT_START:
        gpio_set_level(LED_RED, 0);
        gpio_set_level(LED_GREEN, 1);
        gpio_set_level(LED_BLUE, 1);

        // std::cout << "I2C_CALLBACK_REPEAT_START" << std::endl;
        break;

    case I2C_CALLBACK_SEND_DATA:
        gpio_set_level(LED_RED, 1);
        gpio_set_level(LED_GREEN, 0);
        gpio_set_level(LED_BLUE, 1);

        // std::cout << "I2C_CALLBACK_SEND_DATA" << std::endl;
        if (dev->state == I2C_STATE_SEND)
        {
            device_info info = {1, 1, "ESP32-S3-PPDEVKIT"};

            // std::cout << "I2C_STATE_SEND" << std::endl;
            i2c_slave_send_data(dev, (uint8_t *)&info, sizeof(device_info));
        }

        break;

    case I2C_CALLBACK_DONE:

        if (dev->state == I2C_STATE_RECV)
        {
            gpio_set_level(LED_RED, 1);
            gpio_set_level(LED_GREEN, 1);
            gpio_set_level(LED_BLUE, 0);
            BaseType_t high_task_wakeup = pdFALSE;
            uint16_t data = *(uint16_t *)&dev->buffer[dev->bufstart];

            xQueueSendFromISR(slave_queue, &data, &high_task_wakeup);
        }
        break;

    default:
        // std::cout << "Unknown reason" << std::endl;
        break;
    }

    return true;
}

i2c_slave_config_t i2c_slave_config = {
    i2c_slave_callback,
    ESP_SLAVE_ADDR,
    I2C_SLAVE_SCL_IO,
    I2C_SLAVE_SDA_IO,
    I2C_NUM_0};

i2c_slave_device_t *slave_device;

void initialize_fixed_i2c()
{
    slave_queue = xQueueCreate(1, sizeof(uint16_t));

    ESP_ERROR_CHECK(i2c_slave_new(&i2c_slave_config, &slave_device));
}

#include "hal/i2c_hal.h"

// i2c_dev_t I2C0;

static void dump_i2c_registers()
{
    // uint32_t *registerPointer = (uint32_t *)&I2C0;
    // for (int i = 0x0; i < 0x80; i++)
    // {
    //     std::cout << "Register " << std::hex << i << ": " << std::hex << registerPointer[i] << std::endl;
    // }
}

static void i2c_task(void *arg)
{
    while (true)
    {
        try
        {
            /*
            // new api
            // auto data = std::make_unique<device_info>();
            uint16_t rbuf;

            // i2c_bus_t *base = slave_handle_command.base;

            i2c_slave_rx_done_event_data_t rx_data;
            ESP_ERROR_CHECK(i2c_slave_receive(slave_handle_command, (uint8_t *)&rbuf, sizeof(rbuf)));
            auto success = xQueueReceive(slave_queue, &rx_data, pdMS_TO_TICKS(10000));

            if (success != pdTRUE)
            {
                std::cout << "No data received" << std::endl;
                continue;
            }

            dump_i2c_registers();

            std::cout << "rbuf: " << std::to_string(rbuf) << std::endl;

            // continue;
            // ESP_ERROR_CHECK(i2c_slave_read_ram(slave_handle_command, 0x0, (uint8_t *)&rbuf, sizeof(rbuf)));

            // size_t size = i2c_slave_read_buffer(I2C_NUM_0, (uint8_t *)&rbuf, 2, 1000 / portTICK_PERIOD_MS);
            // std::cout << "i2c_slave_read_buffer returned success: " << std::to_string(success) << std::endl;
            // std::cout << "rx_data.buffer: " << std::to_string(*(uint16_t *)rx_data.buffer) << std::endl;

            // if (success != pdTRUE)
            //     continue;

            if (rbuf != 0x1234)
                continue;

            std::cout << "Received: " << rbuf << std::endl;
            device_info info = {1, 1, "ESP32-S3-PPDEVKIT"};

            //
            // ESP_ERROR_CHECK(i2c_slave_write_ram(slave_handle_command, 0x0, (uint8_t *)&info, sizeof(device_info)));

            ESP_ERROR_CHECK(i2c_slave_transmit(slave_handle_command, (uint8_t *)&info, sizeof(device_info), 1000));
            //   i2c_slave_write_buffer(I2C_NUM_0, (uint8_t *)&info, sizeof(device_info), 1000 / portTICK_PERIOD_MS);

            // sleep 1ms
*/

            // fixed api

            uint16_t rx_data;
            auto success = xQueueReceive(slave_queue, &rx_data, pdMS_TO_TICKS(10000));

            gpio_set_level(LED_RED, 1);
            gpio_set_level(LED_GREEN, 1);
            gpio_set_level(LED_BLUE, 1);

            if (success != pdTRUE)
            {
                std::cout << "No data received" << std::endl;

                std::cout << "slave_device->bufstart: " << std::to_string(slave_device->bufstart) << std::endl;
                std::cout << "slave_device->bufend: " << std::to_string(slave_device->bufend) << std::endl;
                // std::cout << "Received: " << data << std::endl;

                // dump all 128 bytes of slave_device->buffer in hex
                // for (int i = 0; i < 128; i++)
                // {
                //     std::cout << std::hex << (int)slave_device->buffer[i] << " ";
                // }
                // std::cout << std::endl;

                dump_i2c_registers();

                continue;
            }

            // uint16_t rbuf = *(uint16_t *)rx_data;

            std::cout << "-- Received: 0x" << std::hex << rx_data << std::endl;
            // std::cout << "slave_device->bufstart: " << std::to_string(slave_device->bufstart) << std::endl;
            // std::cout << "slave_device->bufend: " << std::to_string(slave_device->bufend) << std::endl;
            // std::cout << "Received: " << data << std::endl;

            // dump all 128 bytes of slave_device->buffer in hex
            // for (int i = 0; i < 128; i++)
            // {
            //     std::cout << std::hex << (int)slave_device->buffer[i] << " ";
            // }
            // std::cout << std::endl;

            // vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
        catch (const std::exception &ex)
        {
            std::cout << "Exception: " << std::endl;
            std::cout << ex.what() << std::endl;
        }
    }
}

extern "C" void app_main(void)
{
    initialize_gpio();
    // initialize_old_i2c();
    //  initialize_new_i2c();
    initialize_fixed_i2c();

    std::cout << "Hello, world!" << std::endl;

    xTaskCreate(i2c_task, "i2c_test_task_0", 1024 * 2, (void *)0, 10, NULL);
}
