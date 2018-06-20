/****************************************************************************
��������� ��� ������, �� BLUETOOTH ����������
*****************************************************************************/
//������� ��
#define F_CPU 8000000UL//������� atmega8
#define BAUD 38400 //������ �������� � �����
#define BAUD_PRESCALE ((F_CPU)/(16UL*BAUD)-1) //������������ ������� 


//����������� ����������� ���������
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

//���������� ����������, �������� ���������� �������� UDR � ���������
const char bufSize = 100; // ������ ������
volatile unsigned char bufer[bufSize]; //����������� �����
volatile unsigned char bufFirst = 0; // ������ ������
volatile unsigned char bufLast = 0;  // ��������� ������ + 1
volatile unsigned char lastValue=0;
volatile unsigned char blockRul = '1';//���������� ����
// ����� bufFirst = bufLast ����� ������

//��������� ������������� ������ ���������� �������� � �������������
void port_ini()
{
	PORTB = 0x00;
	DDRB = 0b00000011;//��������� ��� OCR1A (16bit PWM) �� �����
	
	PORTC = 0b00111100;//�������� ������� ��� � ������ ��������
	DDRC = 0b00111111;
	
	PORTD = 0b00000000;//������ ����� ������������
    DDRD =  0b11000000;
	
	//� PORTB0 ��� ��� ��� ��� �����, � PORTB1 ��� ��������� � ���������� �������� L293D
	
	for(int i_init=0;  i_init<bufSize; i_init++)//��������� �������������
	{
		bufer[i_init]=99;
	}
}

//���������� ����������
ISR(USART_RXC_vect)
{
	if(((bufLast + 1)%bufSize) != bufFirst){// ������ �� �����������
		bufer[bufLast] = UDR;
		bufLast = (bufLast + 1)%bufSize;
	}
}

//������������� UART
void InitUART(void)
{
	UCSRB = 0b00011000;
	UCSRB |=(1<<RXCIE);
	UCSRC = 0b10000011;
	UBRRH=(BAUD_PRESCALE>>8);
	UBRRL=BAUD_PRESCALE;
}

//������������� �������
void init_PWM_timer()
{
	TCCR1A |= (1 << COM1A1) | (1 << WGM11)|(1 << COM1B1) ;//�� ����������� ���
	TCCR1B |= (1 << WGM13) | (1 << WGM12) | (1 << CS11) | (1 << CS10);//�������� ������� = 64; ����� = 14(������� ���)
	TCNT1 = 0x00;
	ICR1 = 2499;//������� ��� = 50Hz; ������ = 20ms (������ �����������)
	OCR1A = 0x00;
}


//------------------------------------------------------------------------------
// ������� ���������
//------------------------------------------------------------------------------
int main(void)
{
	//������� ��������� �������������
	port_ini();
	InitUART();
	init_PWM_timer();
	
	//��� ��������� ������������� ����� ������ �� ����������� �������� (�����)
	PORTB |=(1<<0);//�������� �����
	_delay_ms(50);// ���, ���� ������ ������
	OCR1A=188; //127 ~ 0*: 188 ~ 90* : 236 ~ 180* - ����� �������� ��� �������� ��� = 64
	_delay_ms(1000);// �������� � �������������� �� ������
	PORTB &=(0<<0);//��������� ����
	_delay_ms(50);// ��� ������� ��������
		
	sei();// �������� ����������
	
	// ����������� ����
	while(1) 
	{
		
	
	if(bufLast != bufFirst)//�������� �������� ������ � ��� ������, ���� ����� �� ����
	{
		if(lastValue != bufer[bufFirst]){
			lastValue = bufer[bufFirst];
			
			switch(blockRul)
			{
				
				//��� ����������
			case '0':
			switch(bufer[bufFirst])
			{
				case '4':
		     	//������� �����
				PORTC &=(0<<0);//���������  ������ �������
				PORTD &=(0<<6);
				PORTC |=(1<<1);//�������� ����� ����������
				PORTD |=(1<<7);
				PORTB |=(1<<0);//�������� �����
				_delay_ms(50);
				OCR1A=236;
				_delay_ms(100);// ���, ���� ����� ��������
				PORTB &= (0<<0);// ��������� �����
				PORTC |=(1<<1);//�������� ����� ����������
				PORTD |=(1<<7);
				_delay_ms(50);//��� , ���� ������� ������
			
				break;
				
				case '9':
				
				PORTB |= (1<<0);
				_delay_ms(50);
				OCR1A=188;
				_delay_ms(100);
				PORTB &= (0<<0);
				_delay_ms(50);
				
				
				PORTC &=(0<<0);//��������� ���������� ������
				PORTC &=(0<<1);//��������� ����� ����������
				PORTD &=(0<<6);
				PORTD &=(0<<7);
				break;
				
				case '6':
				
				//������� ������
				PORTC &=(0<<1);//���������  ����� �������
				PORTD &=(0<<7);
				PORTC |=(1<<0);//�������� ������ ����������
				PORTD |=(1<<6);
				PORTB |=(1<<0);//�������� �����
				_delay_ms(50);
				OCR1A=110;
				_delay_ms(100);// ���, ���� ����� ��������
				PORTB &= (0<<0);// ��������� �����
				PORTC |=(1<<0);//�������� ������ ����������
				PORTD |=(1<<6);
				_delay_ms(50);//��� , ���� ������� ������
				break;
				
				case '7':
				blockRul = char(int(!(bool(char(blockRul)-48)))+48);
				break;
				
				
			}
			
			break;
			
			//���� ����������
			case '1':
			    switch(bufer[bufFirst])
			    {
						case '9':
						
						PORTB |= (1<<0);
						_delay_ms(50);
						OCR1A=188;
						_delay_ms(100);
						PORTB &= (0<<0);
						_delay_ms(50);
						
						
						PORTC &=(0<<0);//��������� ���������� ������
						PORTC &=(0<<1);//��������� ����� ����������
						PORTD &=(0<<6);
						PORTD &=(0<<7);
						break;
						
					case '7':
					blockRul =char(int(!(bool(char(blockRul)-48)))+48);// (blockRul++)%2;
					break;
				}
			break;
		}
		
		}
		
		bufFirst = (bufFirst + 1)%bufSize;// �������� ��������� ������
	}
	} // ����������� ������ ������������ �����

return 0;
} // ����������� ������ �������� ���������


