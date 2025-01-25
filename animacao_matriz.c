// Anibal Maldonado: Código principal  - https://github.com/PhD-Anibal
// Anibal Maldonado: LED Vermelho ao apertar 'A' - https://github.com/PhD-Anibal

#include <stdio.h>
#include "pico/stdlib.h"
#include <hardware/pio.h>           // Biblioteca para manipulação de periféricos PIO
#include "hardware/clocks.h"        // Biblioteca para controle de relógios do hardware
#include "ws2818b.pio.h"            // Biblioteca PIO para controle de LEDs WS2818B
#include "hardware/gpio.h"
#include "hardware/pwm.h"


#define BUZZER1 28              // define o pino 28 = Buzzer

// Definições de constantes
#define LED_COUNT 25                // Número de LEDs na matriz
#define LED_PIN 7                   // Pino GPIO conectado aos LEDs

// Estrutura para representar um pixel com componentes RGB
struct pixel_t{ 
    uint32_t G, R, B;                // Componentes de cor: Verde, Vermelho e Azul
};

typedef struct pixel_t pixel_t;     // Alias para a estrutura pixel_t
typedef pixel_t npLED_t;            // Alias para facilitar o uso no contexto de LEDs

npLED_t leds[LED_COUNT];            // Array para armazenar o estado de cada LED
PIO np_pio;                         // Variável para referenciar a instância PIO usada
uint sm;                            // Variável para armazenar o número do state machine usado


// Função para inicializar o PIO para controle dos LEDs
void npInit(uint pin) 
{
    uint offset = pio_add_program(pio0, &ws2818b_program); // Carregar o programa PIO
    np_pio = pio0;                                         // Usar o primeiro bloco PIO

    sm = pio_claim_unused_sm(np_pio, false);              // Tentar usar uma state machine do pio0
    if (sm < 0)                                           // Se não houver disponível no pio0
    {
        np_pio = pio1;                                    // Mudar para o pio1
        sm = pio_claim_unused_sm(np_pio, true);           // Usar uma state machine do pio1
    }

    ws2818b_program_init(np_pio, sm, offset, pin, 800000.f); // Inicializar state machine para LEDs

    for (uint i = 0; i < LED_COUNT; ++i)                  // Inicializar todos os LEDs como apagados
    {
        leds[i].R = 0;
        leds[i].G = 0;
        leds[i].B = 0;
    }
}

// Função para definir a cor de um LED específico
void npSetLED(const uint index, const uint8_t r, const uint8_t g, const uint8_t b) 
{
    leds[index].R = r;                                    // Definir componente vermelho
    leds[index].G = g;                                    // Definir componente verde
    leds[index].B = b;                                    // Definir componente azul
}

// Função para limpar (apagar) todos os LEDs
void npClear() 
{
    for (uint i = 0; i < LED_COUNT; ++i)                  // Iterar sobre todos os LEDs
        npSetLED(i, 0, 0, 0);                             // Definir cor como preta (apagado)
}

// Função para atualizar os LEDs no hardware
void npWrite() 
{
    for (uint i = 0; i < LED_COUNT; ++i)                  // Iterar sobre todos os LEDs
    {
        pio_sm_put_blocking(np_pio, sm, leds[i].G<<24);       // Enviar componente verde
        pio_sm_put_blocking(np_pio, sm, leds[i].R<<24);       // Enviar componente vermelho
        pio_sm_put_blocking(np_pio, sm, leds[i].B<<24);       // Enviar componente azul
    }
}


// Função para acender todos os LEDs com uma cor especifica e intensidade em ponto flutuante
void acende_matrizLEDS(bool r, bool g, bool b, double intensidade)
{
    intensidade *= 255;
    for (uint i = 0; i < LED_COUNT; ++i){
        npSetLED(i, r * intensidade, g * intensidade, b * intensidade);                             
    }
    npWrite();
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

void canon(){
    set_buzzer_tone(BUZZER1, 395); 
        sleep_ms(500);
        stop_buzzer(BUZZER1);
        set_buzzer_tone(BUZZER1, 330); 
        sleep_ms(250);
        stop_buzzer(BUZZER1);
        set_buzzer_tone(BUZZER1, 352); 
        sleep_ms(250);
        stop_buzzer(BUZZER1);
        set_buzzer_tone(BUZZER1, 395); 
        sleep_ms(500);
        stop_buzzer(BUZZER1);
}

void canon2() {
    set_buzzer_tone(BUZZER1, 523); // C
    sleep_ms(300);
    stop_buzzer(BUZZER1);
    set_buzzer_tone(BUZZER1, 523); // C
    sleep_ms(300);
    stop_buzzer(BUZZER1);
    set_buzzer_tone(BUZZER1, 587); // D
    sleep_ms(300);
    stop_buzzer(BUZZER1);
}

int main() {
    stdio_init_all(); // Inicializa a comunicação serial
    init_keypad();    // Configura o teclado
    
    gpio_init(BUZZER1);
    gpio_set_dir(BUZZER1, GPIO_OUT);
    init_pwm(BUZZER1);

while (true) {
    char key = get_key(); // Lê a tecla pressionada
    if (key) {
        // por código de Apagar todos os LEDs antes de acender um novo
        switch (key) {
             case 'A': 
                set_buzzer_tone(BUZZER1, 440); // Frequência 440 Hz (Nota Lá)
                sleep_ms(500);
                stop_buzzer(BUZZER1);
                break;
            case 'B': 
                set_buzzer_tone(BUZZER1, 494); // Frequência 494 Hz (Nota Si)
                sleep_ms(500);
                stop_buzzer(BUZZER1);
                break;
            case 'C':
                set_buzzer_tone(BUZZER1, 523); // Frequência 523 Hz (Nota Dó)
                sleep_ms(500);
                stop_buzzer(BUZZER1);
                break;
            case 'D':
                set_buzzer_tone(BUZZER1, 587); // Frequência 587 Hz (Nota Ré)
                sleep_ms(500);
                stop_buzzer(BUZZER1);
                break;
            case '*':
                canon(); // Função específica
                set_buzzer_tone(BUZZER1, 659); // Frequência 659 Hz (Nota Mi)
                sleep_ms(500);
                stop_buzzer(BUZZER1);
                break;
            case '#':
                canon2(); // Função específica
                set_buzzer_tone(BUZZER1, 698); // Frequência 698 Hz (Nota Fá)
                sleep_ms(500);
                stop_buzzer(BUZZER1);
                break;
            case '0':

                break;
            case '1':

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