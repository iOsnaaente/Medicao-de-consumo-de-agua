// Para usar a tranmissão RF 433Mhz
#include <VirtualWire.h>

const int transmit_pin = 11;  
const int sensorFluxo  = 2 ;

const int VCC = 10;
const int GND = 9 ;

char mensagem[4];                               // UINT8_T (Pulsos, segundos, minutos, horas, controle)
void incPulso();                                // Para sensor de fluxo - interrupçoes


// Variaveis para controle
unsigned long int Time2Reset      = 0;
unsigned long int TempoCompensado = 0;
unsigned long int Tempo_1         = 0;

bool flagStopSend = false;
bool flag2reset   = false;


// Para montar a mensagem 
uint8_t contaPulso = 0;
uint8_t Seg        = 0;
uint8_t Min        = 0;
uint8_t Hor        = 0;


void setup() {
  
  // Para usar o fs1000a nas portas digitais
  pinMode(VCC, OUTPUT);
  pinMode(GND , OUTPUT);
  digitalWrite(VCC, HIGH);
  digitalWrite(GND, LOW);
  
  //Iniciar as configurações de transmissão e recepção RF
  vw_set_tx_pin(transmit_pin);          // Pino de Tranmissão RF
  vw_setup(2000);                       // Bits por segundo
  
  // Configurações do sensor de fluxo
  pinMode(sensorFluxo, INPUT_PULLUP);          // configuração do pino 2 (sinal do sensor de efeito hall, sensor de fluxo de água)
  attachInterrupt(0, incPulso, RISING);        //Configura o pino 2(Interrupção 0) interrupção
  delay(100);
}


void loop() {

  contaPulso = 0;                       //Zera a variável
  
  //PCMSK2 |= (1 << PCINT18);           // Habilita interrupção do port D (0x6D)
  delay (1000 - TempoCompensado);       // Aguarda 1 segundo - tempo de looping
  //PCMSK2 |= (0 << PCINT18);           // Desabilita interrupção do port D
  
  // PROVISRIO PARA TESTES
  contaPulso = map(analogRead(A3), 0, 1023, 0, 75);  
  
  
  // Variavel de sincronia 
  Tempo_1 = millis();
  
  // Monta a mensagem para transmissão 
  mensagem[0] = contaPulso;
  mensagem[1] = Seg;
  mensagem[2] = Min;
  mensagem[3] = Hor; 
  
  if (!flagStopSend){
    
    // Média das leituras obtidas a cada 1 minuto
    Seg++;
    if (Seg == 60) {    
      Min++;
      if (Min >= 60){
        Min = 0;
        Hor++;
      }
      Seg = 0;
    }
    
    // Envia a mensagem via RF (Não garante recebimento)
    digitalWrite(LED_BUILTIN, HIGH);
    vw_send((uint8_t *)mensagem, sizeof(mensagem));
    vw_wait_tx();
    for(int i=0; i<sizeof(mensagem); i++)
    digitalWrite(LED_BUILTIN, LOW);
  }
  
  controle2reset();
 
  TempoCompensado = millis() - Tempo_1;
}


// Função Interrupção usada no sensor
void incPulso(){
  contaPulso++;
}


// Veriifica se esta havendo fluxo e controla funço reset
void controle2reset(){
  if(!contaPulso){
    flagStopSend = true;
    
    if(!flag2reset){
      Time2Reset   = millis();
      flag2reset   = true;
    }
    if (millis() - Time2Reset > 6000)   // 10 minutos
      Seg = Min = Hor = 0;
  
  }else{
    flagStopSend = false;
    flag2reset   = false;
    Time2Reset   = 0;  
  }
}
