#define receiverPin 8
#define maxKartingTour 11000

bool values[1000] = {false}; //représente une seconde, pas de 1 milliseconde
int oldTimestamp = 0;
int offset = 0;

void setup() {
  Serial.begin(9600);
  pinMode(receiverPin, INPUT);
  oldTimestamp = 0;
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
    float ratio = count/maxKartingTour;
    led_index = (int)(ratio*10)
    
    
    //delay(5000);
    offset = 0;
  }
  delay(1);
}
