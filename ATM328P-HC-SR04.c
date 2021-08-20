//Codigo sensor hc04 + led + atm328p

#define F-CPU 1000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

//================================================================================
int pulso = 0;		//Se le da el valor del timer
int i = 0;		
int distance = 0;	//Se le da el valor del pulso ya transformado a cm
//================================================================================

int main(void)
{
//==============================================================================================================
	//Activacion de interrupcion
	cli();			//Se desactivan todas las interrupciones globales
	DDRD &=~(1<<DDD2);	//Se configura ECHO pin como entrada.
	EICRA |=(1<<ISC00);	//Bit de orden 0 en 1 logico
	EICRA &=~(1<<ISC01);	//Bit de orden 0 en 0 logico (External Interrupt Control Register A)
	//Esta configuracion genera una interrupcion en cualquier cambio logico para INT0
	EIMSK |=(1<<INT0);	//Hace posible que se detecte la interrupcion en INT0 (External Interrupt Mask Register)
	sei();			//Se activan todas las interrupciones globales
//==============================================================================================================

	while (1)
	{
		//Se genera un pequeño pulso de 10 microsegundos para inicializar el sensor ultrasonico
		PORTD =(1<<PIND1);
		_delay_us(10);
		PORTD &=~(1<<PIND1);

		distance = pulso/58;	//Se lee el ancho de pulso en el pin ECHO y se manda al microcontrolador para transformarlo a cm
	}
	return 0;
}

//Este es el codigo que se va a ejecutar cuando la interrupcion sea activada (Interrupt Service Routine)
//Se elige el TIMER 1 para implementaciï¿½n

ISR (INT0_vect)
{
	//Se configuran estos pines para el modo de operacion NORMAL
	TCCR1A &=~(1<<WGM10);
	TCCR1A &=~(1<<WGM11);
	TCCR1B &=~(1<<WGM12);
	TCCR1B &=~(1<<WGM13);
	if (i==1)
	{
		pulso=TCNT1;		//TCNT1 es un registro que almacena el tiempo transcurrido por el timer
		TCNT1=0x0000;		//Se hace 0 para no acumular valores
		i=0;
	}
	if (i==0)
	{
		TCNT1=0x0000;		//Este es el contador que comienza a incrementar y almacena los pulsos del timer
		
		//Determinar a que frecuencia se va a generar la interrupcion
		TCCR1B |=(1<<CS10);
		TCCR1B &=~(1<<CS11);
		TCCR1B &=~(1<<CS12);
		i=1;
	}
}