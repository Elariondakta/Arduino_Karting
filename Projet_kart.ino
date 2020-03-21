//À modifier si on mets de chips en série (c'est pas le cas)
#define number_of_74hc595s 1 
#define numOfRegisterPins number_of_74hc595s * 8


//Index de chaque chip dans l'array des chips
#define CHIP_COMPTE_TOUR 0
#define CHIP_CLIGNO_1 1
#define CHIP_CLIGNO_2 2
#define CHIP_FREIN_1 3
#define CHIP_FREIN_2 4

//Index de chaque pin pour chaque chip
#define SER_PIN_INDEX 0
#define RCLK_PIN_INDEX 1
#define SCLK_PIN_INDEX 2

//Groupe vitesse
#define INPUT_PIN_SPEED 5
#define RAYON 0.5 //Rayon en mètres

//Groupe compte tour
#define INPUT_PIN_TOUR 7  // Groupe compte tour
#define maxKartingTour 233 //Nombre de tour max par seconde du moteur

//Groupe Réservoir
#define INPUT_PIN_FUEL 8
#define OUTPUT_PIN_FUEL 9

#include "LiquidCrystal_I2C.h"
#include "Wire.h"

#include "Arduino.h";

/*  Pour l'écran:
 *  gnd à ground
 *  Vcc à 5V
 *  SDA à A5
 *  SLC à A4
 */

//Array de booleens (5 chip de 8 pins)
boolean registers[5][numOfRegisterPins];

// Initialise les paramètres de l'écran
LiquidCrystal_I2C lcd(0x27, 20, 4); 

//Dans l'ordre : pin 14 : SER, pin 12 : RCLK, pin 11 : SRCLK des chipset
//Array de correspondance avec les numéros de pin de l'arduino
const int chip_pin[5][3] = {
  {14, 15, 16},     //Compte tour moteur
  {17, 18, 19},     //Clignotants 1
  {20, 21, 22},     //Clignotants 2
  {23, 24, 25},     //Freinage 1
  {26, 27, 28},     //Freinage 2
};



//GROUPE COMPTE TOUR
//représente une seconde, pas de 1 milliseconde
//Sert à calculer le nombre de tours en 1 seconde : 
//Ex : si il y a un tour à la 233ms alors la case 233 sera égale à true 
bool valuesTour[1000] = {false};
int oldTimestampTour = 0;
int offsetTour = 0;

//GROUPE VITESSE
//Pareil que au dessus
bool valuesSpeed[1000] = {false}; 
int oldSpeed = 0;
int oldTimestampSpeed = 0;
int offsetSpeed = 0;

//Groupe frein
int buttonStateBrake = 0;
int oldTimestampBrake = 0;
int endLedBrake = 0;
 
//set all register pins to LOW
void clearRegisters(int index = -1){
  if (index == -1) {
    for (int j = 0; j < 5; j++) {
      for (int i = numOfRegisterPins - 1; i >=  0; i--)
        registers[j][i] = LOW;
    }
  }
  else {
    for(int i = numOfRegisterPins - 1; i >=  0; i--){
       registers[index][i] = LOW;
    }
  }
} 
 
//Set and display registers
//Only call AFTER all values are set how you would like (slow otherwise)
//Si pas de valeur spécifié ca écrit dans tous les chips
void writeRegisters(int index = -1){

  if (index == -1) {
    for (int j = 0; j < 5; j++) {
      digitalWrite(chip_pin[index][RCLK_PIN_INDEX], LOW);
 
      for(int i = numOfRegisterPins - 1; i >=  0; i--){
        digitalWrite(chip_pin[index][RCLK_PIN_INDEX], LOW);
     
        int val = registers[index][i];
     
        digitalWrite(chip_pin[index][SER_PIN_INDEX], val);
        digitalWrite(chip_pin[index][RCLK_PIN_INDEX], HIGH);
     
      }
      digitalWrite(chip_pin[index][RCLK_PIN_INDEX], HIGH);
    }
  } else {
    digitalWrite(chip_pin[index][RCLK_PIN_INDEX], LOW);
   
    for(int i = numOfRegisterPins - 1; i >=  0; i--){
      digitalWrite(chip_pin[index][RCLK_PIN_INDEX], LOW);
   
      int val = registers[index][i];
   
      digitalWrite(chip_pin[index][SER_PIN_INDEX], val);
      digitalWrite(chip_pin[index][RCLK_PIN_INDEX], HIGH);
   
    }
    digitalWrite(chip_pin[index][RCLK_PIN_INDEX], HIGH);
  }
}
 
//set an individual pin HIGH or LOW
void setRegisterPin(int chip_index, int index, int value) {
  registers[chip_index][index] = value;
}

//Fonction d'animation des leds de frein
// Argument : l'index de la dernière led (animation progressive)
void writeBrakeLed(int end) {
  //On met jusqu'à l'index de la led en allumé
  //Puis on éteint les autres
   for (int i = 0; i < end; i++) {
    setRegisterPin(CHIP_FREIN_1, i, HIGH);
    setRegisterPin(CHIP_FREIN_2, i, HIGH);
  }
  for (int i = end; i < 8; i++) {
    setRegisterPin(CHIP_FREIN_1, i, LOW);
    setRegisterPin(CHIP_FREIN_2, i, LOW);
  }
}

//Groupe Frein
void grpBrake() {
  //Si on appuis sur le bouton de freinage
  if (buttonStateBrake == HIGH) {
    //Calcul du décalage de temps par rapport à la dernière itération
    int timestamp = (int)millis();
    //Si il est supérier à 100ms on anim, et si l'anim et fini on la recommence
    if (timestamp - oldTimestampBrake > 100) {
      if (endLedBrake == 8) endLedBrake = 0;
      
      clearRegisters(CHIP_FREIN_1);
      clearRegisters(CHIP_FREIN_2);
      writeBrakeLed(endLedBrake);
      writeRegisters(CHIP_FREIN_1);
      writeRegisters(CHIP_FREIN_2);
      endLedBrake++;
      oldTimestampBrake = timestamp;
    }
  }
}

//Groupe vitesse
void grpSpeed() {
     //On récupère delta t entre la dernière execution et celle-ci 
    int step = (int)(millis()-oldTimestampSpeed);                             
    //Si l'arduino rafraichis plus vite qu'une milliseconde alors step = 0 donc on la met à 1
    if (step == 0) step = 1;    
    
    offsetTour += step; //On l'ajoute à l'offset
    
  //Si jamais on a pas fini de compter un seconde
  if (offsetSpeed < 1000) {
    //Si ca a fait un tour on l'inscrit dans le tableau
    //à la milliseconde indiqué 
    if (digitalRead(INPUT_PIN_SPEED) == 1)
      valuesSpeed[offsetSpeed] = true; 

    oldTimestampSpeed = (int)millis();
  } else {  //Si le tableau est plein on compte le nombre de tours dans le tableau
    int count = 0;
    for (int i = 0; i < 1000; i++) {
      if (valuesSpeed[i] == true) { 
        //Serial.println(i);
        count++;
        valuesSpeed[i] = false;
      }
    }
    Serial.print("Nombre de tour : ");
    Serial.println(count);

    //On divise par 1000 pour passer de m à km
    //Je suis pas sur pour le calcul
    // on divise par 60 pour avoir le nbr de tours par heures
    double speed = 2*M_PI*RAYON/1000*count/3600;

    //Code si jamais le bouton physique pour détecter le freinage ne marche pas :
    // if (speed + 1 < oldSpeed){
    //   buttonStateBrake = true;
    // }
    // else buttonStateBrake = false;
     
    lcd.setCursor(2, 7);
    lcd.print(speed);
    lcd.setCursor(2, 10);
    lcd.print("Km/h");
      
    offsetSpeed = 0;
    oldSpeed = speed;
  }
}


//Groupe compte tour moteur : Même fonctionnement que la vitesse
void grpTour() {
  
  int step = (int)(millis()-oldTimestampTour);   //On récupère delta t entre la dernière execution et celle-ci                              GROUPE COMPTE TOUR
  if (step == 0) step = 1;    //Si l'arduino rafraichis plus vite qu'une milliseconde alors step = 0 donc on la met à 1
  
  offsetTour += step; //On l'ajoute à l'offset
  
  if (offsetTour < 1000) {
    if (digitalRead(INPUT_PIN_TOUR) == 0)
      valuesTour[offsetTour] = true; 
      
    oldTimestampTour = (int)millis();
  } else {
    int count = 0;
    for (int i = 0; i < 1000; i++) {
      if (valuesTour[i] == true) { 
        //Serial.println(i);
        count++;
        valuesTour[i] = false;
      }
    }
    Serial.print("Nombre de tour moteur : ");
    Serial.println(count);
    
    float ratio = (float)count/(float)maxKartingTour;
    
    if (ratio > 1) ratio = 1;
    int led_index = (int)(ratio*8);
    
    clearRegisters(CHIP_COMPTE_TOUR);
    
    for (int i = 1; i < led_index; i++) { //-2 parce que on a 2 leds en moins
      setRegisterPin(CHIP_COMPTE_TOUR, i, HIGH);
    }
    writeRegisters(CHIP_COMPTE_TOUR);

    offsetTour = 0;
  }
}

//Groupe réservoir d'essence
void grpFuel() {
  if (analogRead(INPUT_PIN_FUEL) > 500) {
    digitalWrite(OUTPUT_PIN_FUEL, HIGH);
  }
}

void grpLights() {

}

void setup() {

  Serial.begin(9600);

  //On mets toutes les pins de chips en OUTPUT
  for (int i = 0; i < 5; i++)
    for (int j = 0; j < 3; j++)
      pinMode(chip_pin[i][j], OUTPUT);

      
  pinMode(INPUT_PIN_TOUR, INPUT);
  pinMode(INPUT_PIN_SPEED, INPUT);
  pinMode(INPUT_PIN_FUEL, INPUT);

  pinMode(OUTPUT_PIN_FUEL, OUTPUT);
  
  lcd.begin();
  lcd.setCursor(7,2);                

  //reset all register pins
  clearRegisters();
  writeRegisters();
}


void loop(){ 

  lcd.clear();
  
  //On appelle chaque fonction 
  grpBrake();
  grpSpeed();
  grpTour();
  grpFuel();
  grpLights();
  
  lcd.setCursor(0,0);
  
  //lcd.print(kmh);
  lcd.print("km/h");
  lcd.print("    ");
 
}
