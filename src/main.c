#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include "bbb_gpiolib.h"
#include "logger.h"

#define PWM_PIN "P9_14"
#define FREQUENCY_HZ 1000
#define PERIOD_NS (1000000000 / FREQUENCY_HZ)

PwmPin* pwm_led = NULL;

// Função de limpeza chamada pelo Ctrl+C para garantir que os pinos sejam liberados.
void cleanup_handler(int signum) {
    printf("\nSinal de interrupção recebido. Limpando...\n");
    if (pwm_led) {
        pwm_disable(pwm_led);
        pwm_release(pwm_led);
    }
    logger_cleanup();
    exit(0);
}

int main() {
    // Registra o handler para o sinal SIGINT (Ctrl+C).
    signal(SIGINT, cleanup_handler);

    // Inicializa o logger.
    logger_init(LOG_LEVEL_INFO, NULL);

    logger_log(LOG_LEVEL_INFO, "Iniciando setup do pino PWM '%s'...", PWM_PIN);
    
    // Tenta configurar o pino PWM.
    int rc = pwm_setup(PWM_PIN, &pwm_led);

    // A aplicação agora interpreta o código de erro retornado pela biblioteca.
    if (rc != RC_OK) {
        switch (rc) {
            case RC_ERROR_PIN_NOT_FOUND:
                logger_log(LOG_LEVEL_ERROR, "MAIN: Pino '%s' não encontrado no mapa.", PWM_PIN);
                break;
            case RC_ERROR_NO_PWM_CAP:
                logger_log(LOG_LEVEL_ERROR, "MAIN: Pino '%s' não tem capacidade de PWM.", PWM_PIN);
                break;
            case RC_ERROR_PIN_MODE:
                logger_log(LOG_LEVEL_ERROR, "MAIN: Falha ao definir modo do pino. Execute como root?");
                break;
            case RC_ERROR_EXPORT:
                logger_log(LOG_LEVEL_ERROR, "MAIN: Falha ao exportar canal PWM. Overlay ativo e correto?");
                break;
            default:
                logger_log(LOG_LEVEL_ERROR, "MAIN: Ocorreu um erro desconhecido no setup (código: %d).", rc);
                break;
        }
        logger_cleanup();
        return 1;
    }
    
    logger_log(LOG_LEVEL_INFO, "Setup do PWM concluído com sucesso.");

    // Configura os parâmetros do PWM.
    pwm_set_polarity(pwm_led, "normal");
    pwm_config(pwm_led, PERIOD_NS, 0);
    pwm_enable(pwm_led);

    // Loop principal para o efeito de fade.
    while (1) {
        for (unsigned long dc = 0; dc <= PERIOD_NS; dc += (PERIOD_NS / 100)) {
            pwm_config(pwm_led, PERIOD_NS, dc);
            usleep(10000);
        }
        for (unsigned long dc = PERIOD_NS; dc > 0; dc -= (PERIOD_NS / 100)) {
            if (dc < (PERIOD_NS / 100)) dc = 0;
            pwm_config(pwm_led, PERIOD_NS, dc);
            usleep(10000);
        }
    }
    
    return 0; // Nunca será alcançado.
}
