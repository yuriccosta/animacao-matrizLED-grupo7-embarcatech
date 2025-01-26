// Anibal Maldonado: Código principal  - https://github.com/PhD-Anibal
// Anibal Maldonado: LED Vermelho ao apertar 'A' - https://github.com/PhD-Anibal

#include <stdio.h>
#include "pico/stdlib.h"
#include <hardware/pio.h>           // Biblioteca para manipulação de periféricos PIO
#include "hardware/clocks.h"        // Biblioteca para controle de relógios do hardware

#include "hardware/gpio.h"
#include "hardware/pwm.h"
// Biblioteca para RESET USB
#include "pico/bootrom.h"

#include <stdlib.h>
#include <math.h>
#include "hardware/adc.h"
#include "pico/bootrom.h"

#include "animacao_matriz.pio.h" // Biblioteca PIO para controle de LEDs WS2818B
#include "tetris.h"


#define BUZZER1 21              // define o pino 21 = Buzzer
#define FPS 3

// Definições de constantes
#define LED_COUNT 25                // Número de LEDs na matriz
#define LED_PIN 7                   // Pino GPIO conectado aos LEDs

int frame_delay = 1000 / FPS; // Intervalo em milissegundos
// Anibal Maldonado - Índices da minha matriz de desenho na ordem desejada
int ordem[] = {0, 1, 2, 3, 4, 9, 8,7, 6, 5, 10, 11, 12, 13, 14, 19, 18, 17, 16, 15 , 20, 21, 22, 23, 24};                 

//imprimir valor binário
void imprimir_binario(int num) {
 int i;
 for (i = 31; i >= 0; i--) {
  (num & (1 << i)) ? printf("1") : printf("0");
 }
}

//rotina da interrupção
static void gpio_irq_handler(uint gpio, uint32_t events){
    printf("Interrupção ocorreu no pino %d, no evento %d\n", gpio, events);
    printf("HABILITANDO O MODO GRAVAÇÃO");
	reset_usb_boot(0,0); //habilita o modo de gravação do microcontrolador
}

//rotina para definição da intensidade de cores do led
uint32_t matrix_rgb(double b, double r, double g)
{
  unsigned char R, G, B;
  R = r * 255;
  G = g * 255;
  B = b * 255;
  return (G << 24) | (R << 16) | (B << 8);
}

void init_pwm(uint gpio) {
    gpio_set_function(gpio, GPIO_FUNC_PWM); // Configura o GPIO como PWM
    uint slice_num = pwm_gpio_to_slice_num(gpio);
    pwm_set_clkdiv(slice_num, 125.0f);     // Define o divisor do clock para 1 MHz
    pwm_set_wrap(slice_num, 1000);        // Define o TOP para frequência de 1 kHz
    pwm_set_chan_level(slice_num, pwm_gpio_to_channel(gpio), 0); // Razão cíclica inicial
    pwm_set_enabled(slice_num, true);     // Habilita o PWM
}

void set_buzzer_tone(uint gpio, uint freq) {
    uint slice_num = pwm_gpio_to_slice_num(gpio);
    uint top = 1000000 / freq;            // Calcula o TOP para a frequência desejada
    pwm_set_wrap(slice_num, top);
    pwm_set_chan_level(slice_num, pwm_gpio_to_channel(gpio), top / 2); // 50% duty cycle
}

void stop_buzzer(uint gpio) {
    uint slice_num = pwm_gpio_to_slice_num(gpio);
    pwm_set_chan_level(slice_num, pwm_gpio_to_channel(gpio), 0); // Desliga o PWM
}

//rotina para acionar a matrix de leds - ws2812b
//void desenho_pio(int *desenho, uint32_t valor_led, PIO pio, uint sm, double r, double g, double b){
void desenho_pio(int desenho[][25], uint32_t valor_led, PIO pio, uint sm, double r, double g, double b) {
    for (int16_t k = 1; k < 16; k++) {
        for (int16_t i = 0; i < LED_COUNT; i++) {
                switch (desenho[k][ordem[24-i]]) {
                    case 0: 
                        valor_led = matrix_rgb(b=0.0, r=0.0, g=0.0);
                        break;
                    case 1: 
                        valor_led = matrix_rgb(b=1, r=0.0, g=0.0);
                        break;
                    case 2:
                        valor_led = matrix_rgb(b=0.0, r=1, g=0.0);
                        break;
                    case 3:
                        valor_led = matrix_rgb(b=0.0, r=0.0, g=1);
                        break;
                    case 4:
                        valor_led = matrix_rgb(b=1, r=1, g=1);
                        break;
                }

                pio_sm_put_blocking(pio, sm, valor_led);

        }
        imprimir_binario(valor_led);
        set_buzzer_tone(BUZZER1, 440); // Frequência 440 Hz (Nota Lá)        
        sleep_ms(100); 
        stop_buzzer(BUZZER1);  
        sleep_ms(frame_delay);          
    }
}

// fim função Anibal



// Função para acender todos os LEDs com uma cor específica e intensidade em ponto flutuante
void acende_matrizLEDS(bool r, bool g, bool b, double intensidade, PIO pio, uint sm)
{
    double R = r * intensidade;
    double G = g * intensidade;
    double B = b * intensidade;
    for (uint i = 0; i < LED_COUNT; ++i){
        pio_sm_put_blocking(pio, sm, matrix_rgb(B, R, G));
    }
}

// Define os pinos do teclado
uint columns[4] = {4, 3, 2, 1}; // Colunas conectadas aos GPIOs
uint rows[4] = {9, 8, 6, 5};    // Linhas conectadas aos GPIOs

// Mapa de teclas
char KEY_MAP[4][4] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

// Inicializa os pinos do teclado
void init_keypad() {
    for (int i = 0; i < 4; i++) {
        gpio_init(columns[i]);         // Inicializa os pinos das colunas
        gpio_set_dir(columns[i], GPIO_IN); // Configura as colunas como entrada com pull-up
        gpio_pull_up(columns[i]);

        gpio_init(rows[i]);            // Inicializa os pinos das linhas
        gpio_set_dir(rows[i], GPIO_OUT);   // Configura as linhas como saída
        gpio_put(rows[i], 1);          // Define as linhas como HIGH
    }
}

// Verifica qual tecla foi pressionada
char get_key() {
    for (int r = 0; r < 4; r++) {
        gpio_put(rows[r], 0); // Ativa a linha atual
        for (int c = 0; c < 4; c++) {
            if (gpio_get(columns[c]) == 0) { // Verifica se a coluna está LOW
                gpio_put(rows[r], 1); // Restaura a linha
                return KEY_MAP[r][c]; // Retorna a tecla pressionada
            }
        }
        gpio_put(rows[r], 1); // Restaura a linha
    }
    return 0; // Nenhuma tecla pressionada
}



// |====================================================|
// |                                                    |
// |                      Animações                     |
// |                                                    |
// |====================================================|
/* 
void Animacao_0(){    
    acende_matrizLEDS(false, false, true, 255);
    sleep_ms(200);
    npClear();
    acende_matrizLEDS(false, true, false, 255);
    sleep_ms(200);
    npClear();
    acende_matrizLEDS(false, true, true, 255);
    sleep_ms(200);
    npClear();
    acende_matrizLEDS(true, false, false, 255);
    sleep_ms(200);
    npClear();
    acende_matrizLEDS(true, false, true, 255);
    sleep_ms(200);
    npClear();
    acende_matrizLEDS(true, true, false, 255);
    sleep_ms(200);
    npClear();
    acende_matrizLEDS(true, true, true, 255);
    sleep_ms(200);
    npClear();
} */

int main() {
    stdio_init_all(); // Inicializa a comunicação serial
    init_keypad();    // Configura o teclado
    
    gpio_init(BUZZER1);
    gpio_set_dir(BUZZER1, GPIO_OUT);
    init_pwm(BUZZER1);

    //inicio parte da função Anibal
    PIO pio = pio0; 
    bool ok;

    uint32_t valor_led;
    double r = 0.0, b = 0.0 , g = 0.0;

    //coloca a frequência de clock para 128 MHz, facilitando a divisão pelo clock
    ok = set_sys_clock_khz(128000, false);


    printf("iniciando a transmissão PIO");
    if (ok) printf("clock set to %ld\n", clock_get_hz(clk_sys));

    //configurações da PIO
    uint offset = pio_add_program(pio, &animacao_matriz_program);
    uint sm = pio_claim_unused_sm(pio, true);
    animacao_matriz_program_init(pio, sm, offset, LED_PIN);
    // fim parte da função Anibal


    while (true) {
        char key = get_key(); // Lê a tecla pressionada
        if (key) {
            // por código de Apagar todos os LEDs antes de acender um novo
            switch (key) {
                case 'A': 
                    acende_matrizLEDS(0, 0, 0, 0, pio, sm); // Apaga todos os LEDs
                    set_buzzer_tone(BUZZER1, 440); // Frequência 440 Hz (Nota Lá)
                    sleep_ms(200);
                    stop_buzzer(BUZZER1);
                    break;
                case 'B':
                    acende_matrizLEDS(0, 0, 1, 0.8, pio, sm); // Acende todos os LEDs de azul com 80% de intensidade 
                    set_buzzer_tone(BUZZER1, 494); // Frequência 494 Hz (Nota Si)
                    sleep_ms(200);
                    stop_buzzer(BUZZER1);
                    break;
                case 'C':
                    acende_matrizLEDS(1, 0, 0, 0.8, pio, sm); // Acende todos os LEDs de vermelho com 80% de intensidade
                    set_buzzer_tone(BUZZER1, 523); // Frequência 523 Hz (Nota Dó)
                    sleep_ms(200);
                    stop_buzzer(BUZZER1);
                    break;
                case 'D':
                    acende_matrizLEDS(0, 1, 0, 0.5, pio, sm); // Acende todos os LEDs de verde com 50% de intensidade
                    set_buzzer_tone(BUZZER1, 587); // Frequência 587 Hz (Nota Ré)
                    sleep_ms(200);
                    stop_buzzer(BUZZER1);
                    break;
                case '*':
                    set_buzzer_tone(BUZZER1, 659); // Frequência 659 Hz (Nota Mi)
                    sleep_ms(200);
                    stop_buzzer(BUZZER1);
                    
                    // Escreve a sequência mágica no endereço especial
                    reset_usb_boot(0, 0);
                    break;
                case '#':
                    acende_matrizLEDS(1, 1, 1, 0.2, pio, sm); // Acende todos os LEDs de branco com 20% de intensidade
                    break;
                case '0':
                    //Animacao_0();
                    break;
                case '1':
                    //rotina para escrever na matriz de leds com o emprego de PIO - desenho 2
                    desenho_pio(desenho, valor_led, pio, sm, r, g, b);
                    break;
                case '2':

                    break;
                case '3':

                    break;
                case '4':

                    break;
                case '5':

                    break;
                case '6':

                    break;
                case '7':

                    break;
                case '8':

                    break;
                case '9':

                    break;
                default:
                    printf("Tecla não associada: %c\n", key);
                    break;
            }
            stop_buzzer(BUZZER1); 
            printf("Tecla pressionada: %c\n", key);
        }
        sleep_ms(50); // Pequeno atraso para evitar leituras duplicadas
    }
} 