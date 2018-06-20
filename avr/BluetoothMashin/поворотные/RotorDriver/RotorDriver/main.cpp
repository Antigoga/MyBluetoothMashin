/****************************************************************************
ПРОГРАММА ДЛЯ МАШИНЫ, НА BLUETOOTH УПРАВЛЕНИИ
*****************************************************************************/
//ЧАСТОТА МК
#define F_CPU 8000000UL//частота atmega8
#define BAUD 38400 //БЛЮТУЗ скорость в бодах
#define BAUD_PRESCALE ((F_CPU)/(16UL*BAUD)-1) //Предделитель частоты 


//ПОДЕЛЮЧЕНИЕ НЕОБХОДИМЫХ БИБЛИОТЕК
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

//ГЛОБАЛЬНЫЕ ПЕРЕМЕННЫЕ, ХРАНЯЩИЕ ПРЕЛЫДУЩЕЕ ЗНАЧЕНИЕ UDR И СЛЕДУЮЩЕЕ
const char bufSize = 100; // размер буфера
volatile unsigned char bufer[bufSize]; // колльцевой буфер
volatile unsigned char bufFirst = 0; // первый символ
volatile unsigned char bufLast = 0;  // последний символ + 1
volatile unsigned char lastValue=0;
volatile unsigned char blockRul = '1';//блокировка нуля
// когда bufFirst = bufLast буфер пустой

//НАЧАЛЬНАЯ ИНИЦИАЛИЗАЦИЯ ПОРТОВ УПРАВЛЕНИЯ МОТОРАМИ И СЕРВОПРИВОДОМ
void port_ini()
{
	PORTB = 0x00;
	DDRB = 0b00000011;//назначаем пин OCR1A (16bit PWM) на выход
	
	PORTC = 0b00111100;//зажигаем ходовые оги и задние габариты
	DDRC = 0b00111111;
	
	PORTD = 0b00000000;//Дубляж назад поворотников
    DDRD =  0b11000000;
	
	//с PORTB0 идёт наш ШИМ для сервы, с PORTB1 идёт включение и выключение драйвера L293D
	
	for(int i_init=0;  i_init<bufSize; i_init++)//начальная инициализация
	{
		bufer[i_init]=99;
	}
}

//ОБРАБОТЧИК ПРЕРЫВАНИЯ
ISR(USART_RXC_vect)
{
	if(((bufLast + 1)%bufSize) != bufFirst){// защита от переполнени¤
		bufer[bufLast] = UDR;
		bufLast = (bufLast + 1)%bufSize;
	}
}

//ИНИЦИАЛИЗАЦИЯ UART
void InitUART(void)
{
	UCSRB = 0b00011000;
	UCSRB |=(1<<RXCIE);
	UCSRC = 0b10000011;
	UBRRH=(BAUD_PRESCALE>>8);
	UBRRL=BAUD_PRESCALE;
}

//ИНИЦИАЛИЗАЦИЯ ТАЙМЕРА
void init_PWM_timer()
{
	TCCR1A |= (1 << COM1A1) | (1 << WGM11)|(1 << COM1B1) ;//не инвертируем ШИМ
	TCCR1B |= (1 << WGM13) | (1 << WGM12) | (1 << CS11) | (1 << CS10);//делитель частоты = 64; режим = 14(быстрый ШИМ)
	TCNT1 = 0x00;
	ICR1 = 2499;//частота ШИМ = 50Hz; Период = 20ms (период стандартный)
	OCR1A = 0x00;
}


//------------------------------------------------------------------------------
// Главная программа
//------------------------------------------------------------------------------
int main(void)
{
	//Функции начальной инициализации
	port_ini();
	InitUART();
	init_PWM_timer();
	
	//При включении устанавливаем колёса машины по направлению движения (ровно)
	PORTB |=(1<<0);//включаем серву
	_delay_ms(50);// ждём, пока пройдёт скачок
	OCR1A=188; //127 ~ 0*: 188 ~ 90* : 236 ~ 180* - карта значений при делители ШИМ = 64
	_delay_ms(1000);// докрутка и ориентирование по центру
	PORTB &=(0<<0);//выключаем серу
	_delay_ms(50);// ждём прохода импульса
		
	sei();// разршаеи прерывания
	
	// бесконечный цикл
	while(1) 
	{
		
	
	if(bufLast != bufFirst)//выполн¤ем действие только в том случае, если буфер не пуст
	{
		if(lastValue != bufer[bufFirst]){
			lastValue = bufer[bufFirst];
			
			switch(blockRul)
			{
				
				//нет блокировки
			case '0':
			switch(bufer[bufFirst])
			{
				case '4':
		     	//поворот влево
				PORTC &=(0<<0);//выключаем  правый навс¤кий
				PORTD &=(0<<6);
				PORTC |=(1<<1);//включаем левый поворотник
				PORTD |=(1<<7);
				PORTB |=(1<<0);//включаем серву
				_delay_ms(50);
				OCR1A=236;
				_delay_ms(100);// ждЄм, пока серва докрутит
				PORTB &= (0<<0);// выключаем серву
				PORTC |=(1<<1);//включаем левый поворотник
				PORTD |=(1<<7);
				_delay_ms(50);//ждЄм , пока утихнет скачок
			
				break;
				
				case '9':
				
				PORTB |= (1<<0);
				_delay_ms(50);
				OCR1A=188;
				_delay_ms(100);
				PORTB &= (0<<0);
				_delay_ms(50);
				
				
				PORTC &=(0<<0);//выключить поворотник вправо
				PORTC &=(0<<1);//выключить левый поворотник
				PORTD &=(0<<6);
				PORTD &=(0<<7);
				break;
				
				case '6':
				
				//поворот вправо
				PORTC &=(0<<1);//выключаем  левый навс¤кий
				PORTD &=(0<<7);
				PORTC |=(1<<0);//включаем правый поворотник
				PORTD |=(1<<6);
				PORTB |=(1<<0);//включаем серву
				_delay_ms(50);
				OCR1A=110;
				_delay_ms(100);// ждЄм, пока серва докрутит
				PORTB &= (0<<0);// выключаем серву
				PORTC |=(1<<0);//включаем правый поворотник
				PORTD |=(1<<6);
				_delay_ms(50);//ждЄм , пока утихнет скачок
				break;
				
				case '7':
				blockRul = char(int(!(bool(char(blockRul)-48)))+48);
				break;
				
				
			}
			
			break;
			
			//есть блокировка
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
						
						
						PORTC &=(0<<0);//выключить поворотник вправо
						PORTC &=(0<<1);//выключить левый поворотник
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
		
		bufFirst = (bufFirst + 1)%bufSize;// сдвигаем указатель буфера
	}
	} // закрывающая скобка бесконечного цикла

return 0;
} // закрывающая скобка основной программы


