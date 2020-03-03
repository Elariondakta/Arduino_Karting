
#define receiverPin 7
#define maxKartingTour 233 //Nombre de tour max par seconde du moteur

bool values[1000] = {false}; //représente une seconde, pas de 1 milliseconde
int oldTimestamp = 0;
int offset = 0;


int SER_Pin = 8;   //pin 14 on the 75HC595  //
int RCLK_Pin = 9;  //pin 12 on the 75HC595
int SRCLK_Pin = 10; //pin 11 on the 75HC595
 
#define number_of_74hc595s 1 //Pas touche jsp ce que c'est ^^
 
//do not touch (Ca non plus d'ailleur il le dit aussi le monsieur)
#define numOfRegisterPins number_of_74hc595s * 8
 
boolean registers[numOfRegisterPins];   
 
//set all register pins to LOW
void clearRegisters(){
  for(int i = numOfRegisterPins - 1; i >=  0; i--){
     registers[i] = LOW;
  }
} 
 
//Set and display registers
//Only call AFTER all values are set how you would like (slow otherwise)
void writeRegisters(){
 
  digitalWrite(RCLK_Pin, LOW);
 
  for(int i = numOfRegisterPins - 1; i >=  0; i--){
    digitalWrite(SRCLK_Pin, LOW);
 
    int val = registers[i];
 
    digitalWrite(SER_Pin, val);
    digitalWrite(SRCLK_Pin, HIGH);
 
  }
  digitalWrite(RCLK_Pin, HIGH);
 
}
 
//set an individual pin HIGH or LOW
void setRegisterPin(int index, int value){
  registers[index] = value;
}

void setup() {
  Serial.begin(9600);
  
  oldTimestamp = 0;

  pinMode(receiverPin, INPUT);
  pinMode(SER_Pin, OUTPUT);
  pinMode(RCLK_Pin, OUTPUT);
  pinMode(SRCLK_Pin, OUTPUT);
 
  //reset all register pins
  clearRegisters();
  writeRegisters();
}

void loop() {

  int step = (int)(millis()-oldTimestamp);   //On récupère delta t entre la dernière execution et celle-ci
  if (step == 0) step = 1;    //Si l'arduino rafraichis plus vite qu'une milliseconde alors step = 0 donc on la met à 1
  
  offset += step; //On l'ajoute à l'offset
  //Serial.println(step);
  
  if (offset < 1000) {
    if (digitalRead(receiverPin) == 0)
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
    
    float ratio = (float)count/(float)maxKartingTour;
    
    if (ratio > 1) ratio = 1;
    int led_index = (int)(ratio*10);
    
    clearRegisters();
    
    for (int i = 1; i < led_index - 2; i++) { //-2 parce que on a 2 leds en moins
      setRegisterPin(i, HIGH);
    }
    writeRegisters();
       
    offset = 0;
  }
  delay(1); // !! Ne pas enlever !! Permet au programme de pas planter !! //
}
