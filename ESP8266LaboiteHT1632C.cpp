#include "Arduino.h"
#include "ESP8266LaboiteHT1632C.h"

LedMatrixPanel::LedMatrixPanel(void) {
  //init();
}

void LedMatrixPanel::init(void) {
    pinMode(14, OUTPUT); // DATA
    pinMode(12, OUTPUT); // CS
    pinMode(13, OUTPUT); // WR
    pinMode(15, OUTPUT); // CLK

    begin();
    clear();
}

void LedMatrixPanel::begin(void) {
  // initialisation de la matrice de led
  // apparement les broches OSC et SYNC des quatres HT1632 ne sont pas reliées dans cette matrice
  // on ne peut donc pas définir le premier HT1632 en MASTER et les suivants en SLAVE,
  // tous doivent être déclarés en mode master et utiliser le résonnateur interne (commande RC)

  selectAll();                     // enable all HT1632s
  commandWrite(SYS_DIS);           // sends command
  selectNone();                    // disable all HT1632s

  selectAll();
  commandWrite(N_MOS_COM8);
  selectNone();

  selectAll();
  commandWrite(RC_MASTER_MODE);
  selectNone();

  selectAll();
  commandWrite(SYS_EN);
  selectNone();

  selectAll();
  setIntensity();
  selectNone();

  selectAll();
  commandWrite(BLINK_OFF);
  selectNone();

  selectAll();
  commandWrite(LED_ON);
  selectNone();
}

void LedMatrixPanel::intensity(uint8_t value) {
  // value between 0 and 15
  if (value > 15) {
    value = 15;
  }
  intentity_value = (value << 1) | 0b100101000000;

  selectAll();
  setIntensity();
  selectNone();
}

void LedMatrixPanel::setIntensity() {
  //selectAll();
  commandWrite(intentity_value);
  //selectNone();
}

void LedMatrixPanel::selectAll() {
  HT1632_CLK_0;  // clock line is 0
  HT1632_CS_0; // send "0" to cs port
  CLK_DELAY;

  HT1632_CLK_1;  // clock pulse n°1
  CLK_DELAY;
  HT1632_CLK_0;

  //HT1632_CS unchanged
  CLK_DELAY;

  HT1632_CLK_1;  // clock pulse n°2
  CLK_DELAY;

  HT1632_CLK_0;
  //HT1632_CS unchanged
  CLK_DELAY;

  HT1632_CLK_1;  // clock pulse n°3
  CLK_DELAY;

  HT1632_CLK_0;
  //HT1632_CS unchanged
  CLK_DELAY;

  HT1632_CLK_1;  // clock pulse n°4
  CLK_DELAY;

  HT1632_CLK_0;
  //HT1632_CS_1; // send "1" to cs port
  CLK_DELAY;

  HT1632_CLK_1;  // clock pulse n°5
  CLK_DELAY;
}

void LedMatrixPanel::selectNone() {
  HT1632_CLK_0;  // clock line is 0
  HT1632_CS_1; // send "1" to cs port
  CLK_DELAY;

  HT1632_CLK_1;  // clock pulse n°1
  CLK_DELAY;

  HT1632_CLK_0;
  //HT1632_CS unchanged
  CLK_DELAY;

  HT1632_CLK_1;  // clock pulse n°2
  CLK_DELAY;

  HT1632_CLK_0;
  //HT1632_CS unchanged
  CLK_DELAY;

  HT1632_CLK_1;  // clock pulse n°3
  CLK_DELAY;

  HT1632_CLK_0;
  //HT1632_CS unchanged
  CLK_DELAY;

  HT1632_CLK_1;  // clock pulse n°4
  CLK_DELAY;
}

void LedMatrixPanel::commandWrite(uint16_t command) {
  uint16_t j;
  command = command & 0x0fff;   // 12-bit command word, mask upper four bits

  // write command words in HT1632 register :
  for (uint8_t i = 0; i < 12; i++) {
    j = command & 0x0800;        // return the MSB
    command = command << 1;      // move the control character to the left one
    j = j >> 11;                 // position the value at the LSB
    HT1632_WR_0;
    HT1632_DATA(j);             // send the value to the data port
    CLK_DELAY;

    HT1632_WR_1;  // clock pulse
    CLK_DELAY;
  }
}

void LedMatrixPanel::clear() {
  selectAll();
  // pour un HT1632, soit deux matrice 8x8 bicolores :
  // on selectionne la première adresse 0x00
  // en envoyant la séquence 1010000000

  HT1632_WR_0;  // clock line is 0
  HT1632_DATA_1; // send "1" to data port
  CLK_DELAY;

  HT1632_WR_1;  // clock pulse
  CLK_DELAY;

  HT1632_WR_0;
  HT1632_DATA_0; // send "0" to data port
  CLK_DELAY;

  HT1632_WR_1;  // clock pulse
  CLK_DELAY;

  HT1632_WR_0;
  HT1632_DATA_1; // send "1" to data port
  CLK_DELAY;

  HT1632_WR_1;  // clock pulse
  CLK_DELAY;

  for (int i = 0; i < 7; i++) {
    HT1632_WR_0;
    HT1632_DATA_0; // send "0" to data port
    CLK_DELAY;

    HT1632_WR_1;  // clock pulse
    CLK_DELAY;
  }

  // en envoie ensuite les 32 octets correspondants aux 16 colonnes de 8 leds vertes et aux 16 colonnes de 8 leds rouges

  for (int i = 0; i < 32; i++) {
    for (int j = 0; j < 8 ; j++) {
      HT1632_WR_0;
      HT1632_DATA_0; // send "0" to data port
      CLK_DELAY;

      HT1632_WR_1;  // clock pulse
      CLK_DELAY;
    }
  }
  selectNone();
}

void LedMatrixPanel::dataWrite(uint8_t * buffer128) {
  // pour un HT1632, soit deux matrice 8x8 bicolores :
  // on selectionne la première adresse 0x00
  // en envoyant la séquence 1010000000
  // puis on envoie les 32 octects

  HT1632_WR_0;  // clock line is 0
  HT1632_DATA_1; // send "1" to data port
  CLK_DELAY;

  HT1632_WR_1;  // clock pulse
  CLK_DELAY;

  HT1632_WR_0;
  HT1632_DATA_0; // send "0" to data port
  CLK_DELAY;

  HT1632_WR_1;  // clock pulse
  CLK_DELAY;

  HT1632_WR_0;
  HT1632_DATA_1; // send "1" to data port
  CLK_DELAY;

  HT1632_WR_1;  // clock pulse
  CLK_DELAY;

  for (int i = 0; i < 7; i++) {
    HT1632_WR_0;
    HT1632_DATA_0; // send "0" to data port
    CLK_DELAY;

    HT1632_WR_1;  // clock pulse
    CLK_DELAY;
  }

  uint8_t k = 0b00000000;

  // matrice 1 rouge
  for (uint8_t i = 0; i < 8 ; i++) {
    for (uint8_t j = 0; j < 8 ; j++) {
      k = buffer128[j * 4 + 64] >> (7 - i);  // position the value at the LSB
      k = k & 0x01;

      HT1632_WR_0;
      HT1632_DATA(k);             // send the value to the data port
      CLK_DELAY;

      HT1632_WR_1;  // clock pulse
      CLK_DELAY;
    }
  }

  // matrice 2 rouge
  for (uint8_t i = 0; i < 8 ; i++) {
    for (uint8_t j = 0; j < 8 ; j++) {
      k = buffer128[j * 4 + 65] >> (7 - i);
      k = k & 0x01;               // position the value at the LSB

      HT1632_WR_0;
      HT1632_DATA(k);             // send the value to the data port
      CLK_DELAY;

      HT1632_WR_1;  // clock pulse
      CLK_DELAY;
    }
  }

  // matrice 1 vert
  for (uint8_t i = 0; i < 8 ; i++) {
    for (uint8_t j = 0; j < 8 ; j++) {
      k = buffer128[j * 4] >> (7 - i);
      k = k & 0x01;               // position the value at the LSB

      HT1632_WR_0;
      HT1632_DATA(k);             // send the value to the data port
      CLK_DELAY;

      HT1632_WR_1;  // clock pulse
      CLK_DELAY;
    }
  }

  // matrice 2 vert
  for (uint8_t i = 0; i < 8 ; i++) {
    for (uint8_t j = 0; j < 8 ; j++) {
      k = buffer128[j * 4 + 1] >> (7 - i);
      k = k & 0x01;               // position the value at the LSB

      HT1632_WR_0;
      HT1632_DATA(k);             // send the value to the data port
      CLK_DELAY;

      HT1632_WR_1;  // clock pulse
      CLK_DELAY;
    }
  }
}

void LedMatrixPanel::display(uint8_t * buffer128) {
  // il arrive réguliairement qu'il y ai un bug dans l'affichage
  // il semblerait qu'il y ai un problème de synchronisation entre les quatres
  // HT1632c. Pour palier à ce problème, et faute de mieux,  on réinitialise les
  // quatres HT1632c à chaque fois qu'on charge une nouvelle image.
  begin();

  // on active un a un les HT1632

  HT1632_CLK_0;  // clock line is 0
  HT1632_CS_0; // send "0" to cs port
  CLK_DELAY;

  HT1632_CLK_1;  // clock pulse n°1
  CLK_DELAY;
  HT1632_CLK_0;

  // HT1632 #1 actif
  dataWrite(buffer128);

  HT1632_CS_1; // send "1" to cs port
  CLK_DELAY;

  HT1632_CLK_1;  // clock pulse n°2
  CLK_DELAY;
  HT1632_CLK_0;

  // HT1632 #1 inactif
  // HT1632 #2 actif
  dataWrite(buffer128 + 2);

  //HT1632_CS unchanged
  CLK_DELAY;

  HT1632_CLK_1;  // clock pulse n°3
  CLK_DELAY;
  HT1632_CLK_0;

  // HT1632 #2 inactif
  // HT1632 #3 actif
  dataWrite(buffer128 + 32);

  //HT1632_CS unchanged
  CLK_DELAY;

  HT1632_CLK_1;  // clock pulse n°4
  CLK_DELAY;
  HT1632_CLK_0;

  // HT1632 #3 inactif
  // HT1632 #4 actif
  dataWrite(buffer128 + 34);

  //HT1632_CS unchanged
  CLK_DELAY;

  HT1632_CLK_1;  // clock pulse n°5
  CLK_DELAY;
  HT1632_CLK_0;
}
