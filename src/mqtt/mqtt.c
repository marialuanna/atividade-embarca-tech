
#include "mqtt.h"
#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"
#include <stdio.h>
#include "lwip/init.h"
#include "lwip/netif.h"
#include "src/display_oled/display_oled.h"

// Pino do LED para indicar conexão
#define LED_PIN 13

// Definição da variável global mqtt_client
mqtt_client_t *mqtt_client;

// Buffer global para acumular dados recebidos
#define MAX_BUFFER_SIZE 8192 // Aumente o tamanho do buffer se necessário
static char received_buffer[MAX_BUFFER_SIZE];
static size_t received_buffer_len = 0;

// Função de callback para receber mensagens
void mqtt_incoming_publish_cb(void *arg, const char *topic, u32_t tot_len)
{
    printf("Mensagem recebida no tópico: %s\n", topic);
}

// Função de callback para dados recebidos via MQTT
void mqtt_incoming_data_cb(void *arg, const u8_t *data, u16_t len, u8_t flags)
{
    // Criar um buffer temporário e garantir que seja uma string válida
    char buffer[32] = {0}; // Buffer para armazenar a string recebida
    size_t copy_len = len < sizeof(buffer) - 1 ? len : sizeof(buffer) - 1;
    memcpy(buffer, data, copy_len); // Copia os dados recebidos para o buffer

    printf("Mensagem recebida: %s\n", buffer);

    // Comparação segura
    if (strcmp(buffer, "LIGAR") == 0)
    {
        printf("Ligando o LED\n");
        gpio_put(LED_PIN, 1);
        show_text((char *[]){"Comando recebido", "", "Ligar"}, 3);
    }
    else if (strcmp(buffer, "DESLIGAR") == 0)
    {
        printf("Desligando o LED\n");
        show_text((char *[]){"Comando recebido", "", "Desligar"}, 3);
        gpio_put(LED_PIN, 0);
    }
}

// Função de callback para confirmação de inscrição (opcional)
void mqtt_subscribe_cb(void *arg, err_t err)
{
    if (err == ERR_OK)
    {
        printf("Inscrição confirmada!\n");
    }
    else
    {
        printf("Erro na confirmação da inscrição: %d\n", err);
    }
}

// Função para se inscrever em um tópico
void subscribe_to_topic(mqtt_client_t *client, const char *topic, uint8_t qos)
{
    err_t err = mqtt_subscribe(client, topic, qos, mqtt_subscribe_cb, NULL);
    if (err != ERR_OK)
    {
        printf("Erro ao se inscrever no tópico %s: %d\n", topic, err);
    }
    else
    {
        printf("Inscrito no tópico %s com QoS %d\n", topic, qos);
    }
}

// Função para publicar uma mensagem
void publish_message(mqtt_client_t *client, const char *topic, const char *message, uint8_t qos, uint8_t retain)
{
    err_t err = mqtt_publish(client, topic, message, strlen(message), qos, retain, NULL, NULL);
    if (err != ERR_OK)
    {
        printf("Erro ao publicar mensagem no tópico %s: %d\n", topic, err);
    }
    else
    {
        printf("Mensagem publicada no tópico %s: %s\n", topic, message);
    }
}

// Função de callback para conexão MQTT
void mqtt_connection_cb(mqtt_client_t *client, void *arg, mqtt_connection_status_t status)
{
    if (status == MQTT_CONNECT_ACCEPTED)
    {
        printf("Conectado ao broker MQTT!\n");

        // Inscreve-se em um tópico após a conexão
        subscribe_to_topic(client, "pico/projeto/comandos", 0);
    }
    else
    {
        printf("Erro na conexao MQTT: %d\n", status);
    }
}

// Inicialização do MQTT
void mqtt_init()
{
    ip_addr_t broker_ip;
    if (!ipaddr_aton(MQTT_BROKER_IP, &broker_ip)) // Converte o endereço IP para o formato correto
    {
        printf("Erro ao converter o endereço IP do broker MQTT\n");
        return;
    }

    mqtt_client = mqtt_client_new();
    if (mqtt_client == NULL)
    {
        printf("Erro ao criar cliente MQTT\n");
        return;
    }

    // Configura os callbacks para receber mensagens
    mqtt_set_inpub_callback(mqtt_client, mqtt_incoming_publish_cb, mqtt_incoming_data_cb, NULL);

    // Estrutura de informações do cliente MQTT
    struct mqtt_connect_client_info_t client_info = {
        .client_id = "pico_w",
        .client_user = NULL, // Se precisar de autenticação, adicione usuário e senha
        .client_pass = NULL,
        .keep_alive = 60,
        .will_topic = NULL,
        .will_msg = NULL,
        .will_qos = 0,
        .will_retain = 0};

    // Tenta conectar ao broker MQTT
    err_t err = mqtt_client_connect(mqtt_client, &broker_ip, MQTT_BROKER_PORT, mqtt_connection_cb, NULL, &client_info);
    if (err != ERR_OK)
    {
        printf("Erro ao tentar conectar ao broker MQTT: %d\n", err);
    }
}