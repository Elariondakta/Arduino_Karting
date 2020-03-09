#include "Arduino.h";

#define INPUTPIN 18 //Numéro de pin d'entrée (ton détecteur)
#define OUTPUTPIN 5 //Numéro de pin de sortie (ta led)

//Tout ce code en supposant que tu t'es démerdé pour arriver entre 0 et 5V

//Fonction qui est lancée uniquement lors du démarrage de l'arduino
void setup()
{
    //On initialise le système de communication (pour écrire dans la console pour faire des tests)
    Serial.begin(9600);
    pinMode(INPUTPIN, INPUT);   //On spécifie le mode des pins qu'on utilise (input, output)
    pinMode(OUTPUTPIN, OUTPUT);
}

//Fonction qui est lancée en boucle à l'infini
void loop()
{
    //On lit la tension sur la pin d'entrée 
    //Variation de 0 à 1023 pour 0 à 5V

    int sensorValue = analogRead(INPUTPIN);

    //On l'affiche dans la console
    Serial.println(sensorValue);


    if (sensorValue > 500) {    //Si > à ~2.5V
        Serial.println("Alerte !");
        digitalWrite(OUTPUTPIN, HIGH);  //On envoie du courant dans la led !!!! Faut foutre une résitance entre les deux sinon ta led crame
    } else {    //Si < à 2.5V
        Serial.println("Tout vas bien");
    }

}