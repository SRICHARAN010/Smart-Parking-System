#include "stm32f4xx_hal.h"

/* ================= GLOBAL HANDLES ================= */
ADC_HandleTypeDef hadc1;
SPI_HandleTypeDef hspi1;
OLED_Init();
OLED_Clear();

/* ================= DEFINES ================= */
#define NIGHT_THRESHOLD 2000

/* ================= FUNCTION DECLARATIONS ================= */
void SystemClock_Config(void);
void GPIO_Init_All(void);
void ADC1_Init(void);
void SPI1_Init(void);

void MAX7219_Send(uint8_t addr, uint8_t data);
void MAX7219_Init(void);
void Display_Status(uint8_t code);

/* ================= MAIN ================= */
int main(void)
{
    HAL_Init();
    SystemClock_Config();

    GPIO_Init_All();
    ADC1_Init();
    SPI1_Init();

    HAL_ADC_Start(&hadc1);
    MAX7219_Init();

    while (1)
    {
        GPIO_PinState pir = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0);
        GPIO_PinState moisture = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_2);

        HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
        uint32_t ldr = HAL_ADC_GetValue(&hadc1);

        if (ldr < NIGHT_THRESHOLD)
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
        else
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

        if (moisture == GPIO_PIN_SET)
            Display_Status(3);        // WET
        else if (pir == GPIO_PIN_SET)
            Display_Status(2);        // PEDESTRIAN
        else if (ldr < NIGHT_THRESHOLD)
            Display_Status(1);        // NIGHT
        else
            Display_Status(0);        // NORMAL

        HAL_Delay(300);
    }
}

/* ================= CLOCK ================= */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef osc = {0};
    RCC_ClkInitTypeDef clk = {0};

    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

    osc.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    osc.HSIState = RCC_HSI_ON;
    osc.PLL.PLLState = RCC_PLL_ON;
    osc.PLL.PLLSource = RCC_PLLSOURCE_HSI;
    osc.PLL.PLLM = 16;
    osc.PLL.PLLN = 336;
    osc.PLL.PLLP = RCC_PLLP_DIV4;
    osc.PLL.PLLQ = 7;
    HAL_RCC_OscConfig(&osc);

    clk.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                    RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    clk.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    clk.AHBCLKDivider = RCC_SYSCLK_DIV1;
    clk.APB1CLKDivider = RCC_HCLK_DIV2;
    clk.APB2CLKDivider = RCC_HCLK_DIV1;
    HAL_RCC_ClockConfig(&clk, FLASH_LATENCY_2);
}

/* ================= GPIO ================= */
void GPIO_Init_All(void)
{
    GPIO_InitTypeDef gpio = {0};

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();

    /* PIR + Moisture */
    gpio.Pin = GPIO_PIN_0 | GPIO_PIN_2;
    gpio.Mode = GPIO_MODE_INPUT;
    gpio.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &gpio);

    /* LED + MAX7219 CS */
    gpio.Pin = GPIO_PIN_13 | GPIO_PIN_4;
    gpio.Mode = GPIO_MODE_OUTPUT_PP;
    gpio.Pull = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOC, &gpio);
    HAL_GPIO_Init(GPIOA, &gpio);
}

/* ================= ADC ================= */
void ADC1_Init(void)
{
    ADC_ChannelConfTypeDef ch = {0};

    __HAL_RCC_ADC1_CLK_ENABLE();

    hadc1.Instance = ADC1;
    hadc1.Init.Resolution = ADC_RESOLUTION_12B;
    hadc1.Init.ContinuousConvMode = ENABLE;
    hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc1.Init.NbrOfConversion = 1;
    HAL_ADC_Init(&hadc1);

    ch.Channel = ADC_CHANNEL_0;
    ch.Rank = 1;
    ch.SamplingTime = ADC_SAMPLETIME_144CYCLES;
    HAL_ADC_ConfigChannel(&hadc1, &ch);
}

/* ================= SPI ================= */
void SPI1_Init(void)
{
    __HAL_RCC_SPI1_CLK_ENABLE();

    hspi1.Instance = SPI1;
    hspi1.Init.Mode = SPI_MODE_MASTER;
    hspi1.Init.Direction = SPI_DIRECTION_2LINES;
    hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
    hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
    hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
    hspi1.Init.NSS = SPI_NSS_SOFT;
    hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
    hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
    HAL_SPI_Init(&hspi1);
}

/* ================= MAX7219 ================= */
void MAX7219_Send(uint8_t addr, uint8_t data)
{
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
    uint8_t tx[2] = {addr, data};
    HAL_SPI_Transmit(&hspi1, tx, 2, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
}

void MAX7219_Init(void)
{
    MAX7219_Send(0x09, 0x00);
    MAX7219_Send(0x0A, 0x08);
    MAX7219_Send(0x0B, 0x07);
    MAX7219_Send(0x0C, 0x01);
    MAX7219_Send(0x0F, 0x00);
}

void Display_Status(uint8_t code)
{
    for (uint8_t i = 1; i <= 8; i++)
        MAX7219_Send(i, 0x00);

    if (code == 1) MAX7219_Send(1, 0x18);     // NIGHT
    else if (code == 2) MAX7219_Send(2, 0x3C);// PEDESTRIAN
    else if (code == 3) MAX7219_Send(3, 0x7E);// WET
    else MAX7219_Send(4, 0xFF);               // NORMAL
}


#define SSD1306_ADDR  (0x3C << 1)

void OLED_Command(uint8_t cmd)
{
    uint8_t data[2] = {0x00, cmd};
    HAL_I2C_Master_Transmit(&hi2c1, SSD1306_ADDR, data, 2, HAL_MAX_DELAY);
}

void OLED_Data(uint8_t data)
{
    uint8_t buf[2] = {0x40, data};
    HAL_I2C_Master_Transmit(&hi2c1, SSD1306_ADDR, buf, 2, HAL_MAX_DELAY);
}

void OLED_Init(void)
{
    HAL_Delay(100);
    OLED_Command(0xAE); // Display OFF
    OLED_Command(0x20); OLED_Command(0x00); // Horizontal addressing
    OLED_Command(0xB0);
    OLED_Command(0xC8);
    OLED_Command(0x00);
    OLED_Command(0x10);
    OLED_Command(0x40);
    OLED_Command(0x81); OLED_Command(0x7F);
    OLED_Command(0xA1);
    OLED_Command(0xA6);
    OLED_Command(0xA8); OLED_Command(0x3F);
    OLED_Command(0xA4);
    OLED_Command(0xD3); OLED_Command(0x00);
    OLED_Command(0xD5); OLED_Command(0x80);
    OLED_Command(0xD9); OLED_Command(0xF1);
    OLED_Command(0xDA); OLED_Command(0x12);
    OLED_Command(0xDB); OLED_Command(0x40);
    OLED_Command(0x8D); OLED_Command(0x14);
    OLED_Command(0xAF); // Display ON
}

void OLED_Clear(void)
{
    for (uint8_t page = 0; page < 8; page++)
    {
        OLED_Command(0xB0 + page);
        OLED_Command(0x00);
        OLED_Command(0x10);
        for (uint8_t col = 0; col < 128; col++)
            OLED_Data(0x00);
    }
}
