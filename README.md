# 🚀 Projeto IoT com Raspberry Pi Pico W

## 📌 Visão Geral

Este projeto IoT foi desenvolvido para o *Raspberry Pi Pico W* e tem como objetivo:

- 🌡️ *Leitura de Temperatura*: Monitorar a temperatura interna do microcontrolador.
- 📟 *Exibição de Dados*: Apresentar a temperatura e mensagens de status em um display OLED.
- 📡 *Comunicação via MQTT*: Publicar os dados de temperatura para um broker MQTT e receber comandos para controle de um LED.
- 🌐 *Conectividade Wi-Fi*: Utilizar a rede sem fio para permitir a comunicação via MQTT.

---

## 🛠️ Tecnologias e Componentes

- *Raspberry Pi Pico W* - Microcontrolador principal com Wi-Fi integrado.
- *Driver CYW43* - Gerencia a funcionalidade Wi-Fi.
- *Display OLED SSD1306* - Exibe informações via protocolo I2C.
- *Sensor de Temperatura Interno* - Utiliza o sensor embutido no RP2040.
- *LED* - Controlado via comandos MQTT.
- *lwIP* - Pilha TCP/IP leve para comunicação de rede.
- *MQTT* - Protocolo de mensagens leve para IoT.
- *MQTT Broker* - Servidor MQTT localizado no endereço 18.192.2.35 na porta 1883.
- *Linguagem C* - O código é escrito em C, com suporte ao SDK do Pico.

---

## 🔄 Fluxo de Execução

1. ✅ Inicializa interfaces *I2C, OLED, ADC e Wi-Fi*.
2. 📶 Conecta-se à rede *Wi-Fi*.
3. 🔗 Conecta-se ao *broker MQTT*.
4. 🌡️ Lê temperatura e exibe no *OLED*.
5. 📤 Publica temperatura no tópico pico/projeto.
6. 📥 Inscreve-se no tópico pico/projeto/comandos para comandos MQTT.
7. 💡 Liga/desliga o *LED* baseado nas mensagens recebidas.
8. 🔄 Repete o processo continuamente.
