#include <stdio.h>
#include "pico/stdlib.h"

// Definição dos pinos
#define LED_VERMELHO 13
#define LED_VERDE 11
#define BOTAO_PEDESTRE 5
#define BUZZER 21

bool pedestre_acionado = false;

// Função para simular uma sirene com o buzzer
void tocar_sirene_buzzer(int duracao_ms) {
    int tempo_passado = 0;
    while (tempo_passado < duracao_ms) {
        // Tom alto
        for (int i = 0; i < 100; i++) {
            gpio_put(BUZZER, 1);
            sleep_us(500); // 1kHz aprox
            gpio_put(BUZZER, 0);
            sleep_us(50);
        }
        sleep_ms(100);
        tempo_passado += 100;

        // Tom baixo
        for (int i = 0; i < 100; i++) {
            gpio_put(BUZZER, 1);
            sleep_us(1000); // 500Hz aprox
            gpio_put(BUZZER, 0);
            sleep_us(1000);
        }
        sleep_ms(100);
        tempo_passado += 100;
    }
}

// Função chamada quando o botão for pressionado
void tratar_pedestre() {
    // Ativa sirene por 3 segundos enquanto amarelo acende
    gpio_put(LED_VERMELHO, 1);
    gpio_put(LED_VERDE, 1);
    printf("Sinal: Amarelo (pedestre acionado)\n");
    tocar_sirene_buzzer(3000);

    // Vermelho por 5 segundos (sem som)
    gpio_put(LED_VERMELHO, 1);
    gpio_put(LED_VERDE, 0);
    printf("Sinal: Vermelho (pedestre acionado)\n");
    sleep_ms(5000);

    pedestre_acionado = false;
}

// Verifica botão pressionado (com debounce)
bool verificar_botao() {
    if (!gpio_get(BOTAO_PEDESTRE)) {
        sleep_ms(20);
        if (!gpio_get(BOTAO_PEDESTRE)) {
            return true;
        }
    }
    return false;
}

// Ciclo normal do semáforo
void semaforo_func() {
    gpio_put(LED_VERMELHO, 1);
    gpio_put(LED_VERDE, 0);
    printf("Sinal: Vermelho\n");
    for (int i = 0; i < 100; i++) {
        if (verificar_botao()) {
            pedestre_acionado = true;
            return;
        }
        sleep_ms(100);
    }

    gpio_put(LED_VERMELHO, 0);
    gpio_put(LED_VERDE, 1);
    printf("Sinal: Verde\n");
    for (int i = 0; i < 100; i++) {
        if (verificar_botao()) {
            pedestre_acionado = true;
            return;
        }
        sleep_ms(100);
    }

    gpio_put(LED_VERMELHO, 1);
    gpio_put(LED_VERDE, 1);
    printf("Sinal: Amarelo\n");
    for (int i = 0; i < 30; i++) {
        if (verificar_botao()) {
            pedestre_acionado = true;
            return;
        }
        sleep_ms(100);
    }
}

int main() {
    stdio_init_all();

    gpio_init(LED_VERMELHO);
    gpio_init(LED_VERDE);
    gpio_init(BOTAO_PEDESTRE);
    gpio_init(BUZZER);

    gpio_set_dir(LED_VERMELHO, GPIO_OUT);
    gpio_set_dir(LED_VERDE, GPIO_OUT);
    gpio_set_dir(BUZZER, GPIO_OUT);
    gpio_set_dir(BOTAO_PEDESTRE, GPIO_IN);
    gpio_pull_up(BOTAO_PEDESTRE);

    while (true) {
        if (pedestre_acionado || verificar_botao()) {
            pedestre_acionado = true;
            tratar_pedestre();
        } else {
            semaforo_func();
        }
    }

    return 0;
}
