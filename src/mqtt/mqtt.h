#ifndef MQTT_H
#define MQTT_H

#include "lwip/apps/mqtt.h"

// Configurações do Broker MQTT
#define MQTT_BROKER_IP "18.192.2.35"
#define MQTT_BROKER_PORT 1883

// Declaração da variável global mqtt_client como extern
extern mqtt_client_t *mqtt_client;

// Declaração das funções
void mqtt_incoming_publish_cb(void *arg, const char *topic, u32_t tot_len);
void mqtt_incoming_data_cb(void *arg, const u8_t *data, u16_t len, u8_t flags);
void mqtt_subscribe_cb(void *arg, err_t err);
void subscribe_to_topic(mqtt_client_t *client, const char *topic, uint8_t qos);
void publish_message(mqtt_client_t *client, const char *topic, const char *message, uint8_t qos, uint8_t retain);
void mqtt_connection_cb(mqtt_client_t *client, void *arg, mqtt_connection_status_t status);
void mqtt_init();

#endif // MQTT_H