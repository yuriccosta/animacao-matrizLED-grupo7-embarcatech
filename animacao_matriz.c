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

#include "animacao_matriz.pio.h" // Biblioteca PIO para controle de LEDs WS2818B

#include "tetris.h"
#include "snake.h"
#include "guitarhero.h"
#include "game.h"

#include "pingpong.h"

// Definições de constantes
#define BUZZER1 21              // Define o pino 21 = Buzzer
#define FPS 3                 // Taxa de quadros por segundo
#define LED_COUNT 25            // Número de LEDs na matriz
#define LED_PIN 7               // Pino GPIO conectado aos LEDs

int frame_delay = 1000 / FPS; // Intervalo em milissegundos
// Ordem da matriz de leds
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

// |====================================================|
// |                                                    |
// |                      Animações                     |
// |                                                    |
// |====================================================|


//rotina para acionar a matrix de leds - ws2812b
void desenho_pio(int desenho[][25], PIO pio, uint sm) {
    uint32_t valor_led;
    for (int16_t k = 0; k < 15; k++) {
        for (int16_t i = 0; i < LED_COUNT; i++) {
                switch (desenho[k][ordem[24-i]]) {
                    case 0: 
                        valor_led = matrix_rgb(0.0, 0.0, 0.0); 
                        break;
                    case 1: 
                        valor_led = matrix_rgb(1, 0.0, 0.0); //azul
                        break;
                    case 2:
                        valor_led = matrix_rgb(0.0, 1, 0.0); //vermelho
                        break;
                    case 3:
                        valor_led = matrix_rgb(0.0, 0.0, 1); //verde
                        break;
                    case 4:
                        valor_led = matrix_rgb(1, 1, 1); //branco
                        break;
                    case 5:
                        valor_led = matrix_rgb(0, 1, 0.5); //laranja
                        break;
                    case 6:
                        valor_led = matrix_rgb(0, 1, 1); //amarelo
                        break;
                    case 7:
                        valor_led = matrix_rgb(1, 1, 0); //roxo
                        break;
                }
                pio_sm_put_blocking(pio, sm, valor_led);

        }
        imprimir_binario(valor_led);

        set_buzzer_tone(BUZZER1, 440); // Frequência 440 Hz (Nota Lá)        
        sleep_ms(100); 
        stop_buzzer(BUZZER1);  
        sleep_ms(frame_delay - 100); 
    }
}


void guitar_pio(int guitar[][25], PIO pio, uint sm) {
    uint32_t valor_led;
    for (int16_t k = 0; k < 15; k++) {
        for (int i = 0; i < LED_COUNT; i++) {
                switch (guitar[k][ordem[24-i]]) {
                    case 0: 
                        valor_led = matrix_rgb(0.0, 0.0, 0.0); 
                        break;
                    case 1: 
                        valor_led = matrix_rgb(0.75, 0.0, 0.75); //amarelo
                        break;
                    case 2:
                        valor_led = matrix_rgb(0.0, 0.75, 0.75); //laranja
                        break;
                    case 3:
                        valor_led = matrix_rgb(0.0, 0.0, 0.75); //verde
                        break;
                    case 4:
                        valor_led = matrix_rgb(0.5, 0.5, 0.5); //branco
                        break;
                    case 5:
                        valor_led = matrix_rgb(0, 0.75, 0.0); //vermelho
                        break;
                    case 6:
                        valor_led = matrix_rgb(0.75, 0.0, 0.0); //azul
                        break;
                    case 7:
                        valor_led = matrix_rgb(0.75, 0.0, 0.75); //ciano
                        break;
                    case 8:
                    valor_led = matrix_rgb(0.75, 0.75, 0.0); //roxo
                    break;
                    
                }

                pio_sm_put_blocking(pio, sm, valor_led);

        }
        imprimir_binario(valor_led);

        if(k == 5){
            set_buzzer_tone(BUZZER1, 330);
            sleep_ms(100);
            sleep_ms(frame_delay - 100);
        } else if(k == 6){
            set_buzzer_tone(BUZZER1, 330);
            sleep_ms(100);
            sleep_ms(frame_delay - 100);
        } else if(k == 7){
            set_buzzer_tone(BUZZER1, 260);
            sleep_ms(100);
            sleep_ms(frame_delay - 100);
        } else if(k == 8){
            set_buzzer_tone(BUZZER1, 260);
            sleep_ms(100);
            sleep_ms(frame_delay - 100);
        } else if(k == 9){
            set_buzzer_tone(BUZZER1, 290);
            sleep_ms(100);
            sleep_ms(frame_delay - 100);
        } else if(k == 10){
            set_buzzer_tone(BUZZER1, 290);
            sleep_ms(100);
            sleep_ms(frame_delay - 100);
        } else if(k == 11){
            set_buzzer_tone(BUZZER1, 330);
            sleep_ms(100);
            sleep_ms(frame_delay - 100);
        } else if(k == 12){
            set_buzzer_tone(BUZZER1, 260);
            sleep_ms(100);
            sleep_ms(frame_delay - 100);
        } else if(k == 13){
            sleep_ms(100);
            sleep_ms(frame_delay - 100);
        }
        else {
            stop_buzzer(BUZZER1);
            sleep_ms(100);             
            sleep_ms(frame_delay - 100);
        }
    }
}

void game_pio(int guitar[][25], PIO pio, uint sm) {
    uint32_t valor_led;
    for (int16_t k = 0; k < 15; k++) {
        for (int i = 0; i < LED_COUNT; i++) {
                switch (guitar[k][ordem[24-i]]) {
                    case 0: 
                        valor_led = matrix_rgb(0.0, 0.0, 0.0); 
                        break;
                    case 1: 
                        valor_led = matrix_rgb(1, 0.0, 0.0); //azul
                        break;
                    case 2:
                        valor_led = matrix_rgb(0.0, 1, 0.0); //vermelho
                        break;
                    case 3:
                        valor_led = matrix_rgb(0.0, 0.0, 1); //verde
                        break;
                    case 4:
                        valor_led = matrix_rgb(1, 1, 1); //branco
                        break;
                    case 5:
                        valor_led = matrix_rgb(0, 1, 0.5); //laranja
                        break;
                    case 6:
                        valor_led = matrix_rgb(0, 1, 1); //amarelo
                        break;
                    case 7:
                        valor_led = matrix_rgb(1, 1, 0); //roxo
                        break;
                }
                pio_sm_put_blocking(pio, sm, valor_led);

        }
        imprimir_binario(valor_led);

        if(k == 6){
            set_buzzer_tone(BUZZER1, 660);
            sleep_ms(100);
            stop_buzzer(BUZZER1);
            sleep_ms(frame_delay - 100);
        } else if(k == 11){
            set_buzzer_tone(BUZZER1, 660);
            sleep_ms(100);
            stop_buzzer(BUZZER1);
            sleep_ms(frame_delay - 100);        
        } else if(k == 13){
            set_buzzer_tone(BUZZER1, 440);
            sleep_ms(50);
            set_buzzer_tone(BUZZER1, 380);
            sleep_ms(50);
            stop_buzzer(BUZZER1);
            sleep_ms(frame_delay - 100);
        } 
        else {
            set_buzzer_tone(BUZZER1, 440);
            sleep_ms(100);
            stop_buzzer(BUZZER1);
            sleep_ms(frame_delay - 100);                                
        }
    }
    acende_matrizLEDS(0, 0, 0, 0, pio, sm); // Apaga todos os LEDs
}

void pingpong_pio(int pingpong[][25], PIO pio, uint sm) {
    uint32_t valor_led;
    bool bolinha_tocou_trave = false; // Flag para detectar colisão da bola com a trave
    const int frame_delay = 300;      // Delay entre os frames (em milissegundos)
    // const int LED_COUNT = 25;         // Quantidade de LEDs por frame

    for (int16_t k = 0; k < 25; k++) { // Percorre os frames da animação
        bolinha_tocou_trave = false;  // Reseta a flag a cada frame

        for (int i = 0; i < LED_COUNT; i++) {
            switch (pingpong[k][i]) {
                case 0: 
                    valor_led = matrix_rgb(0.0, 0.0, 0.0); // LED apagado
                    break;
                case 1: 
                    valor_led = matrix_rgb(0.0, 0.0, 1.0); // Azul (trave)
                    break;
                case 2:
                    valor_led = matrix_rgb(1.0, 0.0, 0.0); // Vermelho (bola)
                    // Verifica se a bola tocou na trave
                    if ((i > 0 && pingpong[k][i - 1] == 1) ||  // Verifica trave à esquerda
                        (i < LED_COUNT - 1 && pingpong[k][i + 1] == 1)) { // Verifica trave à direita
                        bolinha_tocou_trave = true;
                    }
                    break;
                case 3:
                    valor_led = matrix_rgb(0.0, 1.0, 0.0); // Verde (efeito de piscar)
                    break;
                case 6:
                    valor_led = matrix_rgb(1.0, 1.0, 0.0); // Amarelo (efeito especial)
                    break;
                case 7:
                    valor_led = matrix_rgb(0.5, 0.0, 1.0); // Roxo (efeito especial)
                    break;
                default:
                    valor_led = matrix_rgb(0.0, 0.0, 0.0); // Caso não reconhecido (desligado)
                    break;
            }
            pio_sm_put_blocking(pio, sm, valor_led); // Envia cor ao LED
        }

        imprimir_binario(valor_led); // Exibe o estado dos LEDs

        if (bolinha_tocou_trave) {
            set_buzzer_tone(BUZZER1, 660);
            sleep_ms(100);
            stop_buzzer(BUZZER1);
        }

        if (k >= 8 && k <= 10) {
            set_buzzer_tone(BUZZER1, 440);
            sleep_ms(50);
            stop_buzzer(BUZZER1);
            sleep_ms(50);
        }

        sleep_ms(frame_delay); // Tempo entre os frames
    }

    acende_matrizLEDS(0, 0, 0, 0, pio, sm); // Apaga todos os LEDs ao final
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
// |                   Loop principal                   |
// |                                                    |
// |====================================================|


int main() {
    init_keypad();    // Configura o teclado
    
    gpio_init(BUZZER1);
    gpio_set_dir(BUZZER1, GPIO_OUT);
    init_pwm(BUZZER1);


    PIO pio = pio0; 
    bool ok;
    
    //coloca a frequência de clock para 128 MHz, facilitando a divisão pelo clock
    ok = set_sys_clock_khz(128000, false);

    stdio_init_all(); // Inicializa a comunicação serial

    printf("iniciando a transmissão PIO");
    if (ok) printf("clock set to %ld\n", clock_get_hz(clk_sys));

    //configurações da PIO
    uint offset = pio_add_program(pio, &animacao_matriz_program);
    uint sm = pio_claim_unused_sm(pio, true);
    animacao_matriz_program_init(pio, sm, offset, LED_PIN);

    
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
                    reset_usb_boot(0, 0);
                    break;
                case '#':
                    acende_matrizLEDS(1, 1, 1, 0.2, pio, sm); // Acende todos os LEDs de branco com 20% de intensidade
                    break;
                case '0':
                    game_pio(game, pio, sm);
                    break;
                case '1':
                    //rotina para escrever na matriz de leds com o emprego de PIO - desenho 1
                    desenho_pio(desenho, pio, sm);
                    break;
                case '2':
                    desenho_pio(snake, pio, sm);
                    break;
                case '3':
                    guitar_pio(guitar, pio, sm);
                    break;
                case '4':
                    pingpong_pio(pingpong, pio, sm);
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