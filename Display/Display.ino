// Para usar a tranmissão RF 433Mhz
#include <VirtualWire.h>

#define MAX_BUFF 32                     // Seta o número máximo de bytes para serem enviados 

uint8_t buff_len = MAX_BUFF;            // Tamanho do buffer
uint8_t buff[MAX_BUFF];                 // Criação de um Buffer para receber as mensagens do tranmissor

const int receive_pin     = 11;         // Pino usado para o RX


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
const int RS=8, EN=9, D4=7, D5=6, D6=5, D7=4;

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


void setup() {
  
  // Para ligar o Gnd e Vcc do Receptor nos pinos 10 e 13 
  pinMode(10, OUTPUT);
  pinMode(13, OUTPUT);
  digitalWrite(10, LOW );
  digitalWrite(13, HIGH);
  
  // Controle de luzes
  pinMode(VERM, OUTPUT);
  pinMode(VERD, OUTPUT);
  pinMode(AZUL, OUTPUT);
  pinMode(CONT, OUTPUT);
  
  // Evitar queimar o LED
  analogWrite(CONT, LOW);
  
  piscaCores();

  // Potenciometro para o LCD
  pinMode(A0, OUTPUT);
  analogWrite(A0,120);

  // Setando as configurações do LCD * 16 colunas 2 linhas
  lcd.begin(16, 2);

  // Printando uma mensagem de boas vindas no LCD.
  lcd.setCursor(0,0);
  lcd.print("Sistema Ligado");
  lcd.setCursor(0,1);
  lcd.print("Pronto para uso");

  //Iniciar as configurações de transmissão e recepção RF
  vw_set_rx_pin(receive_pin);         // Pino de Recepção RF
  vw_setup(2000);                     // Bits por segundo

  delay(1000);

  vw_rx_start();                      // Inicia a recepção de dados RX (Receive)
}


void loop() {

  // Espera nova mensagem por 1 segundo (1000 milissegundos);
  vw_wait_rx_max(1000);
  // Recebe a mensagem no Buff e o tamanho BuffLen
  if (vw_get_message(buff, &buff_len)){

    lcd.clear();            // Limpa a tela antes de nova escrita
    lcd.setCursor(0, 0);    // Setar a posição de escrita no LCD 
    lcd.print("L/min:");    // Printar a dimensão 

    lcd.setCursor(0, 9);                     // Seta a posição de escrita no LCD
    lcd.print((int)pulsos*100/75);

    // Printar o tempo na coluna 2 
    lcd.setCursor(0,1);
    lcd.print((int)Hor);
    lcd.setCursor(3,1);
    lcd.print(":");
    lcd.setCursor(4,1);
    lcd.print((int)Min);
    lcd.setCursor(7,1);
    lcd.print(":");
    lcd.setCursor(8,1);
    lcd.print((int)Seg);    

  }
  else{
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Sistema nao esta");
    lcd.setCursor(1,1);
    lcd.print("Rcebendo sinal");
  }

  delay(990);
}


void controleCores(bool verm, bool verd, bool azul){
  
  analogWrite(CONT, 0);  
  verm ? analogWrite(VERM, 130 ) : analogWrite(VERM, 0 );
  verd ? analogWrite(VERD, 150 ) : analogWrite(VERD, 0 );
  azul ? analogWrite(AZUL, 150 ) : analogWrite(AZUL, 0 );

}


void piscaCores(){
  controleCores(1,0,0);
  delay(300);
  controleCores(0,1,0);
  delay(300);
  controleCores(0,0,1);
  delay(300);
  controleCores(1,1,0);
  delay(300);
  controleCores(0,1,1);
  delay(300);
  controleCores(1,0,1);
  delay(300);
  controleCores(1,1,1);
  delay(300);
  controleCores(0,0,0);
}









