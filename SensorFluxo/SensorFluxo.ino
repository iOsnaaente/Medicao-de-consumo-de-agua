// Para usar a tranmissão RF 433Mhz
#include <VirtualWire.h>

const int transmit_pin    = 11;                 // Usa pino PWM    (pinos 11,10,9 e 6,5,3)

char mensagem[4];                               // UINT8_T (Pulsos, segundos, minutos, horas)
int length = sizeof(mensagem);


// Para as configurações do sensor de fluxo 
const int sensorFluxo = 2;

void incPulso();

float miliLitros = 0;
float litros     = 0;
float vazao      = 0;
float media      = 0;

uint8_t contaPulso = 0;
uint8_t Seg = 0;
uint8_t Min = 0;
uint8_t Hor = 0;


void setup() {

  // Configurações do sensor de fluxo
  pinMode(sensorFluxo, INPUT);          // configuração do pino 2 (sinal do sensor de efeito hall, sensor de fluxo de água)
  attachInterrupt(0, incPulso, RISING); //Configura o pino 2(Interrupção 0) interrupção

  //Iniciar as configurações de transmissão e recepção RF
  vw_set_tx_pin(transmit_pin);          // Pino de Tranmissão RF
  vw_setup(2000);                       // Bits por segundo

  delay(100);
}


void loop() {

  contaPulso = 0;                     //Zera a variável

  sei();                              // Habilita interrupção
  delay (1000);                       // Aguarda 1 segundo
  cli();                              // Desabilita interrupção

  Seg++;

  // Calculo pode ser feito no Receptor APENAS 
  vazao = contaPulso / 7.5;           // Converte para L/min
  media += vazao;                     // Soma a vazão para o calculo da media
  miliLitros = vazao / 60;            // Volume L/s
  litros += miliLitros;               // Volume total
  
  // Média das leituras obtidas a cada 1 minuto
  if (Seg == 60) {    
    Min++;
    if (Min >= 60){
      Min = 0;
      Hor++;
    }
    media /= 60;                      //faz a média
    media = 0;                        //Zera a variável media para uma nova contagem
    Seg = 0;                          //Zera a variável i para uma nova contagem
  }

  // Monta a mensagem para transmissão 
  mensagem[0] = contaPulso;
  mensagem[1] = Seg;
  mensagem[2] = Min;
  mensagem[3] = Hor; 
  
  // Envia a mensagem via RF (Não garante recebimento)
  vw_send((uint8_t *)mensagem, length);
  vw_wait_tx();

}

// Função Interrupção usada no sensor
void incPulso(){
  contaPulso++;
}