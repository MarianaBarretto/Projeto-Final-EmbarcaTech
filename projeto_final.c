#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "inc/ssd1306.h" // Biblioteca para controle do display OLED
#include "hardware/adc.h" // Biblioteca para controle do ADC (Conversor Analógico-Digital)
#include "hardware/i2c.h" // Biblioteca para controle do I2C
#include "hardware/gpio.h" // Biblioteca para controle de GPIO
#include "led.pio.h"

#define MICROPHONE_PIN 28 // Pino onde o microfone está conectado
#define LED_PIN 12        // Pino onde o LED azul está conectado
#define BUTTON_PIN 5      // Pino do botão A (GPIO5)
#define MATRIX_LED_PIN 7  // Pino base da matriz de LED 5x5
#define NUM_PIXELS 25     // Número total de LEDs da matriz

// Definições dos pinos I2C
#define I2C_SDA 14
#define I2C_SCL 15
#define I2C_PORT i2c1

// Definindo o endereço do display I2C
#define ENDERECO_OLED 0x3C

// Parâmetros do filtro de média móvel
#define SAMPLE_SIZE   5 // Tamanho da amostra para a média móvel
#define THRESHOLD 2100   // Limiar de detecção do pico de som (ajustado para 1000)

// Variáveis globais para controlar o estado do LED e do display
volatile int led_on = 0;  // Estado do LED (0 - apagado, 1 - aceso)
ssd1306_t display;        // Display OLED (declarado globalmente)
PIO pio;
uint sm;
double r = 0.2, g = 0.2, b = 0.2;  // Cor inicial dos LEDs RGB (no formato RGB)

// Definição do padrão da carinha feliz (5x5)
double carinha_feliz[25] = {
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 1.0, 0.0, 1.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,
    1.0, 0.0, 0.0, 0.0, 1.0,
    0.0, 1.0, 1.0, 1.0, 0.0
};

// Sequência correta de LEDs para formar imagens
int sequencia[25] = {
    0, 1, 2, 3, 4,
    9, 8, 7, 6, 5,
    10, 11, 12, 13, 14,
    19, 18, 17, 16, 15,
    20, 21, 22, 23, 24
};

// Converte valores RGB para o formato do WS2812
uint32_t matrix_rgb(double r, double g, double b) {
    unsigned char R = r * 255;
    unsigned char G = g * 255;
    unsigned char B = b * 255;
    return (G << 24) | (R << 16) | (B << 8);
}

// Exibir a carinha feliz na matriz de LEDs
void desenhar_carinha_led() {
    uint32_t valor_led;
    for (int i = 0; i < NUM_PIXELS; i++) {
        valor_led = matrix_rgb(r * carinha_feliz[24 - i], g * carinha_feliz[24 - i], b * carinha_feliz[24 - i]);
        pio_sm_put_blocking(pio, sm, valor_led);
    }
}

// Apagar os LEDs da matriz
void apagar_matriz_led() {
    uint32_t valor_led = matrix_rgb(0.0, 0.0, 0.0);  // LEDs apagados
    for (int i = 0; i < NUM_PIXELS; i++) {
        pio_sm_put_blocking(pio, sm, valor_led);
    }
}

// Função para inicializar o ADC no pino do microfone
void init_adc() {
    adc_init();
    adc_gpio_init(MICROPHONE_PIN);
    adc_select_input(2); // Seleciona o pino 28 para leitura
}

// Função para ler o valor do microfone (sinal analógico)
uint16_t read_microphone() {
    return adc_read(); // Lê o valor do ADC
}

// Função para acender o LED no GPIO12
void light_up_led() {
    gpio_put(LED_PIN, 1); // Acende o LED (envia 1 para o pino 12)
}

// Função para apagar o LED no GPIO12
void turn_off_led() {
    gpio_put(LED_PIN, 0); // Apaga o LED (envia 0 para o pino 12)
}

// Função para calcular a média móvel dos últimos N valores
uint16_t moving_average(uint16_t new_sample, uint16_t *history, uint8_t *index) {
    history[*index] = new_sample;  // Atualiza o valor mais recente na amostra
    *index = (*index + 1) % SAMPLE_SIZE; // Atualiza o índice circularmente

    // Calcula a média dos valores armazenados
    uint32_t sum = 0;
    for (int i = 0; i < SAMPLE_SIZE; i++) {
        sum += history[i];
    }

    return (uint16_t)(sum / SAMPLE_SIZE); // Retorna a média
}

// Função para detectar um pico de som baseado na média móvel
int detect_bell_sound(uint16_t audio_signal, uint16_t *history, uint8_t *index) {
    uint16_t avg_signal = moving_average(audio_signal, history, index);
    
    // Exibe o valor lido do microfone para fins de depuração
    printf("Valor lido: %d\n", audio_signal); 

    if (avg_signal > THRESHOLD) {
        return 1; // Pico detectado, provavelmente a campainha
    }
    return 0; // Nenhum pico detectado
}

// Função de callback para a interrupção do botão A (GPIO5)
void button_irq_handler(uint gpio, uint32_t events) {
    if (gpio == BUTTON_PIN && events & GPIO_IRQ_EDGE_FALL) {
        if (led_on) {
            turn_off_led();  // Apaga o LED quando o botão for pressionado
            led_on = 0;      // Atualiza o estado do LED
            ssd1306_fill(&display, false);  // Limpa o display
            ssd1306_send_data(&display);    // Envia o comando para apagar o display
            apagar_matriz_led();
        }
    }
}

// Função para inicializar o display I2C
void init_display() {
    i2c_init(I2C_PORT, 400000); // Inicializa a I2C com velocidade de 400kHz
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C); // Configura pino SDA
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C); // Configura pino SCL
    gpio_pull_up(I2C_SDA); // Habilita pull-up
    gpio_pull_up(I2C_SCL); // Habilita pull-up
    ssd1306_init(&display, 128, 64, false, ENDERECO_OLED, I2C_PORT); // Inicializa o display OLED
    ssd1306_config(&display);
    ssd1306_send_data(&display);
}

// Função para atualizar o display OLED com o texto fornecido
void atualizar_display(ssd1306_t *ssd, const char *linha1, const char *linha2) {
    ssd1306_fill(ssd, false);  // Limpa o display

    // Adicionando prints de depuração
    printf("Exibindo texto no display: %s, %s\n", linha1, linha2);

    ssd1306_draw_string(ssd, linha1, 30, 20);  // Primeira linha
    ssd1306_draw_string(ssd, linha2, 40, 40);  // Segunda linha
    ssd1306_send_data(ssd); // Envia os dados para o display

    sleep_ms(1000); // Pausa para permitir que o texto seja visível por 1 segundo
}

int main() {
    // Inicializa os pinos da Raspberry Pi Pico
    stdio_init_all();
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT); // Configura o pino do LED como saída
    
    // Inicializa o pino do botão A (GPIO5) como entrada com pull-up
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN); // Configura o pino do botão como entrada
    gpio_pull_up(BUTTON_PIN);          // Ativa o resistor de pull-up para o botão

    // Inicializa o display I2C
    init_display();    

    // Inicializa PIO para LEDs WS2812
    pio = pio0;
    uint offset = pio_add_program(pio, &led_program); 
    sm = pio_claim_unused_sm(pio, true);
    led_program_init(pio, sm, offset, MATRIX_LED_PIN);

    // Configura a interrupção no botão A (GPIO5) para detecção da borda de queda
    gpio_set_irq_enabled_with_callback(BUTTON_PIN, GPIO_IRQ_EDGE_FALL, true, button_irq_handler);

    // Inicializa o ADC para leitura do microfone
    init_adc();

    uint16_t history[SAMPLE_SIZE] = {0};  // Histórico dos últimos N sinais
    uint8_t index = 0;  // Índice do histórico para média móvel
    
    while (true) {
        // Lê o valor do microfone
        uint16_t audio_signal = read_microphone();
        
        // Verifica se a campainha foi detectada
        if (detect_bell_sound(audio_signal, history, &index)) {
            if (!led_on) {
                light_up_led();  // Liga o LED se o som da campainha for detectado
                led_on = 1;      // Atualiza o estado do LED
                atualizar_display(&display, "Campainha", "Tocando"); // Exibe mensagem no display
                desenhar_carinha_led(); // Exibe a carinha na matriz de LEDs
            }
        }
        
        // Aguarda um pouco para evitar leituras excessivas
        sleep_ms(100); 
    }
    return 0;
}
