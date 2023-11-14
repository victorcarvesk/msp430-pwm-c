#include <msp430.h> 


#define DIR_BTN     BIT3
#define ESP_BTN     BIT4
#define VEL_POT     BIT5
#define SERVO       BIT6

#define DEG_MIN 1068 // 1ms
#define DEG_MID 1600 // 1.5ms
#define DEG_MAX 2135 // 2ms

int increment = 1; // verificar se está correto, precisa ser global


/**
 * main.c
 */
void main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	
	// Texas recommendations
	P1DIR = 0xFF;               // Define todos os pinos do PORT1 como sa�da
	P1OUT = 0x00;               // Desliga todos os pinos do PORT1
	P2DIR = 0xFF;               // Define todos os pinos do PORT2 como sa�da
	P2OUT = 0x00;               // Desliga todos os pinos do PORT2
	P3DIR = 0xFF;               // Define all PORT3 pins as OUTPUT
	P3OUT = 0x00;               // Turn  PORT3

	P1DIR |= SERVO; // Pino de controle do servomotor
	P1SEL |= SERVO; // define pino como modo especial (conecta ao Timer A)

	P1DIR &= ~DIR_BTN;           // Define pino 3 do PORT1 como entrada (P1.3 = 0)
	P1REN |= DIR_BTN;            // Habilita resistor de tra��o (pull) do P1.3
	P1OUT |= DIR_BTN;            // Define resistor de tra��o do P1.3 como UP (pull-up)
	P1IES |= DIR_BTN;            // Define borda de sensibilidade do P1.3 como DESCIDA (1)
    P1IE |= DIR_BTN;             // Habilida P1.3 a subir flag de interrup��o caso mude* de estado

    /*
	P1DIR &= ~ESP_BTN;           // Define pino 3 do PORT1 como entrada (P1.3 = 0)
	P1REN |= ESP_BTN;            // Habilita resistor de tra��o (pull) do P1.3
	P1OUT |= ESP_BTN;            // Define resistor de tra��o do P1.3 como UP (pull-up)
	P1IES |= ESP_BTN;            // Define borda de sensibilidade do P1.3 como DESCIDA (1)
    P1IE |= ESP_BTN;             // Habilida P1.3 a subir flag de interrup��o caso mude* de estado
    */

	P1IFG = 0x00;               // Desliga todas as flags do sinalizador do PORT1

	TACCR0 = 21352;                             // Set Timer0 PWM Period to 50 Hz

	TACCR1 = DEG_MIN;                               // Set TA0.1 PWM duty cycle

	TACCTL1 = OUTMOD_7;                   // Set TA0.1 Waveform Mode - Clear on Compare, Set on Overflow
	TACCTL0 = CCIE;

	TACTL = TASSEL_2 + MC_1 + ID_0;                // Timer Clock -> SMCLK, Mode -> Up Count

	__bis_SR_register(GIE);

	while(1)
	{
	    TACCR1 += increment;

	    if(TACCR1==DEG_MAX)
	    {
	        increment = -1;
	    }
	    if(TACCR1==DEG_MIN)
        {
            increment = 1;
        }
	    __delay_cycles(5000);
	}

}

#pragma vector=TIMER0_A0_VECTOR
__interrupt void something(void)
{
}

#pragma vector=PORT1_VECTOR
__interrupt void change_dir(void)
{
    if(P1IFG & DIR_BTN)                  // Verifica se o botão no pino P1.3 está pressionado
    {
        P1OUT ^= BIT0;
        if(increment == 1)
        {
            increment = -1;
        }
        else if(increment == -1)
        {
            increment = 1;
        }
        P1IFG &= ~DIR_BTN;               // Limpa flag do Button
    }
}
