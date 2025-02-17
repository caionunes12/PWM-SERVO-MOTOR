#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/pio.h"


int main() {
    const uint gpio_pin = 22; // Definição do pino GPIO 22
    const float freq = 50;    // Frequência de operação do PWM: 50 Hz

    // Configuração do pino como saída PWM
    gpio_set_function(gpio_pin, GPIO_FUNC_PWM);
    
    // Obtém o número do slice e do canal associados ao pino
    uint slice_num = pwm_gpio_to_slice_num(gpio_pin);
    uint channel = pwm_gpio_to_channel(gpio_pin);

    // Define a resolução do PWM para 16 bits
    uint32_t wrap = 65535;
    float divider = 125000000.0f / (freq * (wrap + 1));

    // Configuração do módulo PWM
    pwm_config config = pwm_get_default_config();
    pwm_config_set_wrap(&config, wrap);
    pwm_config_set_clkdiv(&config, divider);
    pwm_init(slice_num, &config, true);

    // Definição dos valores de duty cycle correspondentes às posições do servo
    uint32_t duty_min = (uint32_t)(0.025f * (wrap + 1)); // Posição inicial (~0 graus)
    uint32_t duty_max = (uint32_t)(0.12f * (wrap + 1));   // Posição máxima (~180 graus)
    uint32_t duty_current = duty_min; // Inicia no menor duty cycle
    int direction = 1; // Variável de controle da direção de movimento

    // Conversão de 5 µs para o equivalente em duty cycle
    const uint32_t step_us = 5; // Incremento de 5 µs por ciclo
    const uint32_t period_us = 20000; // Período do sinal PWM (20ms)
    uint32_t step_duty = (uint32_t)((step_us / (float)period_us) * (wrap + 1)); // Conversão para duty cycle

    pwm_set_chan_level(slice_num, channel, wrap * 0.12); // Ajusta para 180 graus
    sleep_ms(5000);

    pwm_set_chan_level(slice_num, channel, wrap * 0.0735); // Ajusta para 90 graus
    sleep_ms(5000);

    pwm_set_chan_level(slice_num, channel, wrap * 0.025); // Ajusta para 0 graus
    sleep_ms(5000);

    while (true) {
        // Atualiza o PWM para movimentação gradual
        pwm_set_chan_level(slice_num, channel, duty_current);

        // Modifica o duty cycle para criar o movimento contínuo
        if (direction == 1) {
            duty_current += step_duty; // Incrementa gradualmente
            if (duty_current >= duty_max) {
                direction = -1; // Inverte o sentido
            }
        } else {
            duty_current -= step_duty; // Decrementa gradualmente
            if (duty_current <= duty_min) {
                direction = 1; // Retorna ao movimento crescente
            }
        }

        // Pequeno atraso para suavizar a movimentação
        sleep_ms(10);
    }
}
