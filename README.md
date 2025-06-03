# AtmoSync MQTT: Sistema Inteligente de Segurança e Ambiente

## Residência Tecnológica em Sistemas Embarcados - Tarefa Individual: Comunicação em IoT - Parte 2

## 1. Visão Geral do Projeto

O "AtmoSync MQTT" é um sistema microcontrolado interativo e conectado à internet, desenvolvido como parte da Residência Tecnológica em Sistemas Embarcados.  Este projeto utiliza a placa BitDogLab como um cliente MQTT e demonstra funcionalidades de segurança residencial e controle de modos de ambiente. [O sistema permite monitoramento e controle remotamente através do protocolo MQTT, utilizando um broker Mosquitto em um dispositivo Android e a ferramenta IoT MQTT Panel para interação. 

Este projeto é uma adaptação da tarefa anterior (HTTP Webserver) para utilizar o protocolo MQTT, visando uma comunicação mais eficiente e em tempo real para aplicações IoT. A aplicação escolhida se enquadra na categoria de Automação Residencial / Casas Inteligentes.

## 2. Funcionalidades Implementadas

O sistema opera em três modos principais, selecionáveis via botões físicos na placa BitDogLab ou por comandos enviados via MQTT através do IoT MQTT Panel:

* **Modo Segurança:**
    * Monitora um sensor de ruído (ou simulação).
    * Ao detectar um ruído acima do limiar configurado, o alarme é ativado:
        * Buzzer soa intermitentemente.
        * Matriz de LEDs em vermelho.
        * LED RGB em vermelho.
        * Display OLED exibe "INTRUSO!!!".
    * O status do alarme (intruso detectado) é publicado via MQTT.
    * O alarme pode ser desativado por um comando MQTT.
    * Quando armado e sem alarme, o LED RGB fica vermelho fixo e a matriz de LEDs apagada.

* **Modo Home:**
    * Configura uma iluminação ambiente suave.
    * LED RGB fica verde fixo.
    * Matriz de LEDs exibe um padrão calmo (ex: luz amarelada suave).
    * Display OLED exibe "Modo: Home".
    * Buzzer permanece desligado.
    * O status "Modo: Home" é publicado via MQTT.

* **Modo Festa:**
    * Ativa uma ambientação dinâmica.
    * Matriz de LEDs apresenta uma animação com cores aleatórias.
    * LED RGB alterna cores (ex: azul e magenta).
    * Display OLED exibe "Modo: Festa".
    * Buzzer pode emitir bipes curtos ou tocar uma melodia simples (opcional, se implementado).
    * O status "Modo: Festa" é publicado via MQTT.

## 3. Estrutura do Código e Desenvolvimento [cite: 11]

O código foi desenvolvido em linguagem C para o Raspberry Pi Pico W. A estrutura do projeto segue uma abordagem modular, com responsabilidades divididas em diferentes arquivos para melhor organização e manutenção:

* `main.c`: Contém a lógica principal, inicializações e o loop de execução.
* `setup.h` / `setup.c`: Responsáveis pela configuração inicial de periféricos, Wi-Fi e MQTT, além de conter as callbacks e funções de tratamento de mensagens MQTT.
* `modos.h` / `modos.c`: Gerencia a lógica dos diferentes modos de operação (Casa, Festa, Segurança) e o acionamento dos respectivos periféricos.
* `inc/`: Pasta contendo bibliotecas auxiliares para periféricos como display OLED, matriz de LEDs (ws2812.pio), buzzer.
* `CMakeLists.txt`: Configurado para compilar todos os arquivos fonte necessários, bibliotecas e definições do SDK do Pico.


## 4. Como Compilar e Executar

1.  **Pré-requisitos:**
    * Raspberry Pi Pico SDK configurado.
    * CMake e GCC para ARM.
    *  VS Code com extensões para desenvolvimento Pico.
2.  **Clonar o Repositório:**
3.  **Compilar:**
4.  **Executar:**
    * Copie o arquivo `.uf2` gerado (em `build/nome_do_seu_projeto.uf2`) para o Raspberry Pi Pico W em modo BOOTSEL.
    * Configure um broker MQTT (ex: Mosquitto no Termux em um Android) na mesma rede Wi-Fi.
    * Configure o IoT MQTT Panel (ou outro cliente MQTT) para se conectar ao mesmo broker.
    * No código, atualize as credenciais do Wi-Fi (`WIFI_SSID`, `WIFI_PASSWORD`) e o endereço IP do Broker MQTT (`MQTT_SERVER`).

## 5. Tópicos MQTT Utilizados

* **Publicados pela Placa:**
    * `/status/modo` (Payload: "home", "festa", "seguranca")
    * `/status/alarme` (Payload: "ok", "intruso")
* **Subscritos pela Placa:**
    * `/modo` (Payload: "home", "festa", "seguranca")
    * `/alarme` (Payload: "desativar")
