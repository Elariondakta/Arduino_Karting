#include <Arduino.h>
#include<LiquidCrystal.h>


#define INPUT_PIN 5
#define RAYON 0.5 //Rayon en mètres


LiquidCrystal lcd(12,11,6,5,4,3);

void setup()
{
  Serial.begin(9600);
  pinMode(INPUT_PIN, INPUT);

  bool values[1000] = {false}; //représente une seconde, pas de 1 milliseconde
  int oldTimestamp = 0;
  int offset = 0;

  oldTimestamp = 0;

  lcd.begin(16,2);

  attachInterrupt(0,isr,RISING);  
}

void loop()
{
  if (offset < 1000) {
    if (digitalRead(INPUT_PIN) == 1)
      values[offset] = true; 

    oldTimestamp = (int)millis();
  } else {
    int count = 0;
    for (int i = 0; i < 1000; i++) {
      if (values[i] == true) { 
        //Serial.println(i);
        count++;
        values[i] = false;
      }
    }
    Serial.print("Nombre de tour : ");
    Serial.println(count);

    //On divise par 1000 pour passer de m à km
    double speed = 2*M_PI*RAYON/1000*count/3600  // on divise par 60 pour avoir le nbr de tours par heures

    lcd.setCursor(4, 0);
    lcd.print(speed);
      
    offset = 0;
  }
  delay(1); // !! Ne pas enlever !! Permet au programme de pas planter !! //
}
