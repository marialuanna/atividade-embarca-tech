#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"
#include <stdio.h>
#include "lwip/init.h"
#include "lwip/netif.h"
#include "pico/binary_info.h"
#include "hardware/adc.h"
#include "src/mqtt/mqtt.h"
#include "src/ssd1306/ssd1306.h"
#include "src/display_oled/display_oled.h"

// Configurações do i2c
const uint I2C_SDA = 14;
const uint I2C_SCL = 15;

// Configurações de Wi-Fi
#define WIFI_SSID "ksksks"
#define WIFI_PASS "12345678"

// Pino do LED para indicar conexão
#define LED_PIN 13

// Definindo pin do sensor de temperatura interno
#define TEMP_PIN 4

float ler_temperatura()
{
    uint16_t raw = adc_read();
    float voltagem = raw * (3.3f / (1 << 12));                   // Conversão para tensão (3.3V e 12 bits de resolução)
    float temperatura = (voltagem - 0.706f) / 0.001721f + 27.0f; // Fórmula da documentação do RP2040
    return temperatura;                                          // Removida conversão incorreta
}

int main()
{
    stdio_init_all();

    // Inicialização do i2c
    i2c_init(i2c1, ssd1306_i2c_clock * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    ssd1306_init();

    show_text((char *[]){"INICIANDO"}, 1);

    adc_init();
    adc_set_temp_sensor_enabled(true); // Habilita o sensor de temperatura interno
    adc_select_input(4);               // O sensor interno sempre usa o canal 4

    sleep_ms(2000);
    printf("Iniciando programa\n");

    // Configura o LED como saída
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    // Inicializa o Wi-Fi
    if (cyw43_arch_init())
    {
        printf("Erro ao inicializar o Wi-Fi\n");
        show_text((char *[]){"FALHA WI-FI"}, 1);
        return 1;
    }

    cyw43_arch_enable_sta_mode();
    printf("Conectando ao Wi-Fi...\n");

    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASS, CYW43_AUTH_WPA2_AES_PSK, 10000))
    {
        printf("Falha ao conectar ao Wi-Fi\n");
        show_text((char *[]){"FALHA WI-FI", "CONEXAO FALHOU"}, 3);
        return 1;
    }
    else
    {
        printf("Connected.\n");
        // Lê o endereço IP de forma legível
        uint8_t *ip_address = (uint8_t *)&(cyw43_state.netif[0].ip_addr.addr);
        printf("Endereço IP %d.%d.%d.%d\n", ip_address[0], ip_address[1], ip_address[2], ip_address[3]);
        show_text((char *[]){"Conectado", "", "REDE " WIFI_SSID}, 3);
    }

    printf("Wi-Fi conectado!\n");
    sleep_ms(2000);

    // Inicializa o MQTT
    mqtt_init();

    while (true)
    {
        cyw43_arch_poll(); // Necessário para manter o Wi-Fi ativo

        // Publica uma mensagem a cada 5 segundos
        static absolute_time_t last_publish_time;
        if (absolute_time_diff_us(last_publish_time, get_absolute_time()) > 5000000)
        {
            // publicando temperatura a cada 1 segundo
            float temperatura = ler_temperatura();
            printf("Temperatura: %.2f\n", temperatura);

            // convertert para string
            char temperatura_str[10];
            snprintf(temperatura_str, sizeof(temperatura_str), "%.2f", temperatura);

            show_text((char *[]){"Temperatura", "", temperatura_str}, 3);

            publish_message(mqtt_client, "pico/projeto", temperatura_str, 0, 0);
            last_publish_time = get_absolute_time();
        }

        sleep_ms(100);
    }

    return 0;
}