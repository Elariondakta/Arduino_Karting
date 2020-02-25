#define INPUTPIN 18
#define OUTPUTPIN 5

//Tout ce code en supposant que tu t'es démerdé pour arriver entre 0 et 5V

void setup()
{
    Serial.begin(9600);
    pinMode(INPUTPIN, INPUT);
    pinMode(OUTPUTPIN, OUTPUT);
}

void loop()
{
    int sensorValue = analogRead(INPUTPIN);
    Serial.println(sensorValue);


    //Variation de 0 à 1023 pour 0 à 5V
    if (sensorValue > 500) {    //Si > à ~2.5V
        Serial.println("Alerte !");
        digitalWrite(OUTPUTPIN, HIGH);  //On envoie du courant dans la led !!!! Faut foutre une résitance entre les deux sinon ta led crame
    } else {    //Si < à 2.5V
        Serial.println("Tout vas bien");
    }

}