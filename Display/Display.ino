// Para usar a tranmissão RF 433Mhz
#include <VirtualWire.h>

#define MAX_BUFF 32                     // Seta o número máximo de bytes para serem enviados 

uint8_t buff_len = MAX_BUFF;            // Tamanho do buffer
uint8_t buff[MAX_BUFF];                 // Criação de um Buffer para receber as mensagens do tranmissor

const int receive_pin     = 11;         // Pino usado para o RX

// Variaveis de inatividade
unsigned long int time2reset = 0;
bool              flag2reset = 0;

// Para usar o Display LCD
#include <LiquidCrystal.h>

/*  LCD RS pin to digital pin 8          // Reset   
/*  LCD EN pin to digital pin 9          // Enable
/*  LCD D4 pin to digital pin 7          // Dados 1
/*  LCD D5 pin to digital pin 6          // Dados 2  Sistema 
/*  LCD D6 pin to digital pin 5          // Dados 3    SPI
/*  LCD D7 pin to digital pin 4          // Dados 4 
/*  LCD RW pin to ground                 // Read(1) ou Write(0)  
 */
const int RS=9, EN=8, D4=7, D5=6, D6=5, D7=4;

// Inicia a interface do display LCD com os pinos
LiquidCrystal lcd(RS, EN, D4, D5, D6, D7);


// Para as indicações luminosas LED RGB - Catodo comum
const int VERM    = A1;                // Controle da luz vermelha
const int CONT    = A2;                // Controle Vdd
const int VERD    = A3;                // Controle da luz amarela
const int AZUL    = A4;                // Controle da luz verde


// Constantes para controle
uint8_t *pulsos = &buff[0];
uint8_t *Seg    = &buff[1];
uint8_t *Min    = &buff[2];
uint8_t *Hor    = &buff[3];

// Botão de controle de menu 
const int BOTAO1 = 2;
const int BOTAO2 = 3;
int       menu   = 0;

// Variáveis importantes para a medição
float vazao      = 0.0;
float miliLitros = 0.0;
float Litros     = 0.0;


void setup() {

  // Para ligar o Vcc do Receptor nos pinos 13 
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);

  // Controle de luzes
  pinMode(VERM, OUTPUT);
  pinMode(VERD, OUTPUT);
  pinMode(AZUL, OUTPUT);
  pinMode(CONT, OUTPUT);

  // Evitar queimar o LED
  analogWrite(CONT, LOW);

  // Potenciometro para o LCD
  pinMode(A0, OUTPUT);
  analogWrite(A0, 120);

  // Setando as configurações do LCD * 16 colunas 2 linhas
  lcd.begin(16, 2);

  // Printando uma mensagem de boas vindas no LCD.
  lcd.setCursor(0,0);
  lcd.print("Sistema Ligado");
  lcd.setCursor(0,1);
  lcd.print("Pronto para uso");

  // Define os botões com interrupção 
  // Controle de menu
  pinMode(BOTAO1, INPUT_PULLUP);
  attachInterrupt(0, incMenu, RISING);
  // Reseta mediço
  pinMode(BOTAO2, INPUT_PULLUP);
  attachInterrupt(1, reset, RISING);

  //Iniciar as configurações de transmissão e recepção RF
  vw_set_rx_pin(receive_pin);         // Pino de Recepção RF
  vw_setup(2000);                     // Bits por segundo
  vw_rx_start();                      // Inicia a recepção de dados RX (Receive)

  // Sinalização de inicialização
  piscaCores(100);
}


void loop() {
  
  // Espera nova mensagem por 1 segundo (1000 milissegundos);
  vw_wait_rx_max(1000);

  // Recebe a mensagem no Buff e o tamanho BuffLen
  if(vw_have_message()){
    if (vw_get_message(buff, &buff_len)){

      vazao = *pulsos/7.5;
      miliLitros = vazao / 60;
      Litros += miliLitros;

      if (menu == 0){ 
        lcd.clear();            // Limpa a tela antes de nova escrita
        lcd.setCursor(0, 0);    // Setar a posição de escrita no LCD 
        lcd.print("L/min:");    // Printar a dimensão 
        lcd.setCursor(12, 0);       // Seta a posição de escrita no LCD
        lcd.print(vazao);
        printTempo();

      }
      else if (menu == 1){
        lcd.clear();            // Limpa a tela antes de nova escrita
        lcd.setCursor(0, 0);    // Setar a posição de escrita no LCD 
        lcd.print("Litros:");    // Printar a dimensão 
        lcd.setCursor(12, 0);       // Seta a posição de escrita no LCD
        lcd.print(Litros);
        printTempo();  
      }
      else{
        lcd.clear();
      }
    }

    else{
      //RECEBENDO SINAL ERRADO, FAZER AS MEDIAS E CONTORNAR
      piscadela(1,0,0,100);
      *Seg>60 ? *Seg=0 : *Seg++;
      printTempo();
    }

    flag2reset = false;
    time2reset = 0;

  }
  else{
    piscadela(1,0,1,100);
    // Não esta recebendo sinal - sinaliza 
    if(!flag2reset)
      time2reset = millis();
    flag2reset = true;

    // Se o tempo inativo for maior que 10 minutos - desliga
    if(millis()-time2reset>600000)        
      lcd.clear();
    Litros = 0;
  }

  delay(1);
}

/*  Definem os 3 estados de menu
 0 = Vazão [litros / minuto]
 1 = Litros 
 2 = Menu desligado
 */
void incMenu(){
  menu == 3 ? menu = 0 : menu++ ;
}


void reset(){
  Litros = 0 ;
}


void controleCores(bool verm, bool verd, bool azul){
  verm ? analogWrite(VERM, 130 ) : analogWrite(VERM, 0 );
  verd ? analogWrite(VERD, 130 ) : analogWrite(VERD, 0 );
  azul ? analogWrite(AZUL, 130 ) : analogWrite(AZUL, 0 );
}

void piscaCores(int tempo){
  byte control = 1;
  for (int i =0; i<8; i++){
    bool byte1 = control & 1;
    bool byte2 = control & 2;
    bool byte3 = control & 4;
    controleCores(byte1,byte2,byte3);
    delay(tempo);
    control++;
  }
}

void piscadela(bool um, bool dois, bool tres, int tempo){
  controleCores(um,dois,tres);
  delay(tempo);
  controleCores(0,0,0);
}

void lcd_print(int num){
  if (num<10){
    lcd.print("0");
    lcd.print(num);
  }
  else{
    lcd.print(num);
  }
}

void printTempo(){
  // Printar o tempo na coluna 2 
  lcd.setCursor(0,1);
  lcd.print("Tempo:");
  lcd.setCursor(8,1);
  lcd_print((int)*Hor);
  lcd.setCursor(10,1);
  lcd.print(":");
  lcd.setCursor(11,1);
  lcd_print((int)*Min);
  lcd.setCursor(13,1);
  lcd.print(":");
  lcd.setCursor(14,1);
  lcd_print((int)buff[1]); 
  piscadela(0,1,0,100);
}

