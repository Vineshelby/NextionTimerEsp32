//by: Marcus Vinícius
#include "Nextion.h"

hw_timer_t * temporizador = NULL; //Ponteiro do timer.
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED; //Técnica para evitarmos de uma memória ser escrita em cima outra
volatile int conta_seg = 0;

String strminutos;
String strsegundos;
String strbuffer;
int minutos, segundos, count, temp_seg;

NexText t0    = NexText(0, 1, "t0");
NexText t2    = NexText(0, 2, "t2");

NexButton b0  = NexButton(0, 3, "b0");
NexButton b1  = NexButton(0, 4, "b1");
NexButton b2  = NexButton(0, 5, "b2");
NexButton b3  = NexButton(0, 6, "b3");
NexButton b4  = NexButton(0, 7, "b4");
NexButton b5  = NexButton(0, 8, "b5");
NexButton b6  = NexButton(0, 9, "b6");
NexButton b7  = NexButton(0, 10, "b7");
NexButton b8  = NexButton(0, 11, "b8");
NexButton b9  = NexButton(0, 12, "b9");
NexButton b10 = NexButton(0, 13, "b10");
NexButton b11 = NexButton(0, 14, "b11");

NexTouch *nex_listen_list[] =
{
  &b0,
  &b1,
  &b2,
  &b3,
  &b4,
  &b5,
  &b6,
  &b7,
  &b8,
  &b9,
  &b10,
  &b11,
  NULL
};

void b0PopCallback(void *ptr) {
  leiturateclado('1');
}
void b1PopCallback(void *ptr) {
  leiturateclado('2');
}
void b2PopCallback(void *ptr) {
  leiturateclado('3');
}
void b3PopCallback(void *ptr) {
  leiturateclado('4');
}
void b4PopCallback(void *ptr) {
  leiturateclado('5');
}
void b5PopCallback(void *ptr) {
  leiturateclado('6');
}
void b6PopCallback(void *ptr) {
  leiturateclado('7');
}
void b7PopCallback(void *ptr) {
  leiturateclado('8');
}
void b8PopCallback(void *ptr) {
  leiturateclado('9');
}
void b9PopCallback(void *ptr) {
  pressCancela();
}
void b10PopCallback(void *ptr) {
  leiturateclado('0');
}
void b11PopCallback(void *ptr) {
  pressOk();
}


void config_Tela(){
  b0.attachPop(b0PopCallback, &b0);
  b1.attachPop(b1PopCallback, &b1);
  b2.attachPop(b2PopCallback, &b2);
  b3.attachPop(b3PopCallback, &b3);
  b4.attachPop(b4PopCallback, &b4);
  b5.attachPop(b5PopCallback, &b5);
  b6.attachPop(b6PopCallback, &b6);
  b7.attachPop(b7PopCallback, &b7);
  b8.attachPop(b8PopCallback, &b8);
  b9.attachPop(b9PopCallback, &b9);
  b10.attachPop(b10PopCallback, &b10);
  b11.attachPop(b11PopCallback, &b11);
}

void IRAM_ATTR controla_tempo() { //Função gravada na memória RAM.
  portENTER_CRITICAL_ISR(&timerMux);
  conta_seg--;
  portEXIT_CRITICAL_ISR(&timerMux);
}

void config_Timer() {
  temporizador = timerBegin(3, 80, true); //(timer utilizado, prescaler, progressivo/regressivo)
  timerAttachInterrupt(temporizador, &controla_tempo, true); // Função que será chamada pelo timer.
  timerAlarmWrite(temporizador , 1000000, true); //Frequência da interrupção -> 1 seg
  timerAlarmEnable(temporizador);// Iniciando temporizador.
  timerStop(temporizador); //Parando temporizador.
  timerWrite(temporizador, 0);//Zerando temporizador.
}

void Inicia_Contagem(int _temp) {
  conta_seg = _temp;
  timerWrite(temporizador, 0);
  timerRestart(temporizador);
}

void leiturateclado(char _number) {
  char number = _number;
  char temporario[2];
  count++;
  if (count <= 4) {
    strbuffer += number;
    switch (count) {
      case 1:
        strsegundos = "";
        strminutos = "";
        strsegundos += number;
        break;
      case 2:
        strsegundos = "";
        strsegundos += strbuffer[0];
        strsegundos += strbuffer[1];
        break;
      case 3:
        strsegundos = "";
        strminutos = "";
        strminutos += strbuffer[0];
        strsegundos += strbuffer[1];
        strsegundos += strbuffer[2];
        break;
      case 4:
        strsegundos = "";
        strminutos = "";
        strminutos += strbuffer[0];
        strminutos += strbuffer[1];
        strsegundos += strbuffer[2];
        strsegundos += strbuffer[3];
        break;
    }
    sprintf( temporario, "%02d", strminutos.toInt());
    t0.setText(temporario);
    sprintf( temporario, "%02d", strsegundos.toInt());
    t2.setText(temporario);
  }
  minutos  = strminutos.toInt();
  segundos = strsegundos.toInt();
  temp_seg = segundos + (minutos * 60);
}
void printTempo() {
  static int seg_ant = -1;
  char temporario[2];

  if (conta_seg != seg_ant) {
    seg_ant = conta_seg;
    minutos = conta_seg / 60;
    segundos = conta_seg % 60;
    sprintf(temporario, "%02d", minutos);
    t0.setText(temporario);
    sprintf(temporario, "%02d", segundos);
    t2.setText(temporario);
    if (conta_seg == 0) DesligaProcesso();
  }
}
void pressOk() { //colocar condição para começar a contagem !!!!!!!!!!!!!!!!!
  Inicia_Contagem(temp_seg);
}
void pressCancela() { //colocar condição para parar a contagem ou reseta-la!!!!!!!!!!!!!!!
  DesligaProcesso();
  t0.setText("00");
  t2.setText("00");
}

void DesligaProcesso() {
  timerStop(temporizador); 
  temp_seg = 0;
  conta_seg = 0;
  count = 0;
  minutos = 0;
  segundos = 0;
  strbuffer = "";
  strminutos = "";
  strsegundos = "";
}

//------------------------------------------------------------------------------------------------

void setup() {
  config_Timer();
  nexInit();
  config_Tela();
}

void loop() {
  nexLoop(nex_listen_list);
  printTempo();
}
