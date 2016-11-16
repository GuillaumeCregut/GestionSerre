/* * * * * * * * * * * * * * * * * * * * * * * * * * */
/*            Gestion Serre intelligente             */
/*   G. Cregut                                       */
/*   DATE Création : 14/11/2016                      */
/*   Date Modification : 16/11/2016                  */
/* (c)2016 Editiel98                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * */

//déclarations includes
#include <Wire.h>  //pour l'EEPROM, la RTC et l'écran.
#include "structDate.h" //stucture de la date
//Déclarations variables globales
int TempExt, TempInt, TempIntMin, TempIntMax,TempExtMin, TempExtMax ;  //Retour des mesures de températures
bool FaireMesure;   //Toggle pour déclencher les mesures. En fonction de la période des mesures
bool DebutScript; // indique qu'on démarre la carte
int TrigHygro, TrigTemp, TrigLumiere;  //valeur a partir du moment ou on arrose, aere ou allume la lumière
byte TempoTrig,HeureMesure; //Delai en heure entre 2 mesures, Heure de la dernière mesure
//Structure pour lire et écrire dans la RTC
DateRTC DateMesure;
struct MesureEEPROM //Structure pour la lecture ou l'écriture d'une mesure
{
  int TypeMesure;
  byte HeureEEPROM;
  byte MinuteEEPROM;
  byte MoisEEPROM;
  byte JourEEPROM;
  byte AnneeEEPROM;
  int ValMesure; 
};

//Déclarations des constantes globales
  //Adresses Bus
#define  Adress_RTC 0
//Entrées analogiques
#define PinTempInt A0  //Broche connectée au capteur intérieur.
#define PinTempExt A1
#define PinHygro A2
#define Luminosite A3

//Entrées numériques
#define BoutonMenu 1
#define BoutonHaut 2
#define BoutonGauche 3
#define BoutonDroit 4
#define BoutonBas 5

//sorties

//Bus

//Declaration des types de mesures
#define TempExt 1
#define TempInt 2
#define HygroM 3
#define LuminositeM 4
//déclarations fonctions

//Fonctions nécessaires pour la RTC
byte BCD2Dec(byte bcd)
{
  return((bcd /16*10)+(bcd%16));
}
byte Dec2BCD(byte dec)
{
  return((dec/10*16)+(dec%10));
}
void RecupereDateHeure(DateRTC *date)
{
  //Récupère la date de la RTC
  byte drop; //supprime le jour de la semaine
  Wire.beginTransmission(Adress_RTC);  //Initialise la com vers la RTC
  Wire.write(0);  //Demande les infos a partir de l'adresse 0
  Wire.endTransmission(); //Fin d'écriture de la demande RTC
  Wire.requestFrom(Adress_RTC,7);  //Demande 7 octets
  date->secondes=BCD2Dec(Wire.read());
  date->minutes=BCD2Dec(Wire.read());
  date->heures=BCD2Dec(Wire.read()& 0b111111);
  drop=BCD2Dec(Wire.read());
  date->jour=BCD2Dec(Wire.read());
  date->mois=BCD2Dec(Wire.read());
  date->annee=BCD2Dec(Wire.read());
  //Stocke l'heure de mesure
  HeureMesure=date->heures;
}
void EcrireRTC(DateRTC *date)
{
  byte Drop;   //on s'en fout
  Drop=1;
  Wire.beginTransmission(Adress_RTC);  //Initialise la com vers la RTC
  Wire.write(0);
  Wire.write(Dec2BCD(date->secondes));
  Wire.write(Dec2BCD(date->minutes));
  Wire.write(Dec2BCD(date->heures));
  Wire.write(Dec2BCD(Drop));
  Wire.write(Dec2BCD(date->jour));
  Wire.write(Dec2BCD(date->mois));
  Wire.write(Dec2BCD(date->annee));
  Wire.write(0);
  Wire.endTransmission(); //Fin d'écriture de la demande RTC
}
void MesureTemp()  //Mesure la températures
{
/* * * * * * * * * * * * * * * * * *  
 *  Voir pour améliorer la boucle  *
 * * * * * * * * * * * * * * * * * */
  //Mesure la température intérieure, extérieure. Compare avec le min et le max, et si valeurdépassée, stocke en EEPROM
  //Stocke les valeurs dans les variables globales, pour les fonctions de traitement
   MesureEEPROM MesureTemp;  //Génère une structure type mesure
  //Récupère la date et l'heure du RTC
   MesureTemp.HeureEEPROM=DateMesure.heures;
   MesureTemp.MinuteEEPROM=DateMesure.minutes;
   MesureTemp.JourEEPROM=DateMesure.jour;
   MesureTemp.MoisEEPROM=DateMesure.mois;
   MesureTemp.AnneeEEPROM=DateMesure.annee;
//Mesure interieure
   MesureTemp.TypeMesure=TempInt;
//Lecture de la mesure
   MesureTemp.ValMesure=0;  ///////////////////////////////A modifier
//Vérifie sur mesureTemp.ValMesure<min ou >max
//Si mesure correspond, on stocke min ou max, et on memorise
  
//Enregistrement de la mesure dans la PROM

//Mesure temp Ext
   MesureTemp.TypeMesure=TempExt;
      MesureTemp.ValMesure=0;  ///////////////////////////////A modifier
//Vérifie sur mesureTemp.ValMesure<min ou >max
//Si mesure correspond, on stocke min ou max, et on memorise
  
//Enregistrement de la mesure dans la PROM

}
void GestionAreoChauffage()
{
  //En fonction de la température, algorithme qui défini l'ouverture de la serre, la mise en route du ventilo ou le chauffage
  
}
int MesureHygro()
{
  //Mesure l'hygrométrie et la stocke en mémoire
  //Retourne la valeur mesurée
   MesureEEPROM MesureTemp;  //Génère une structure type mesure
  //Récupère la date et l'heure du RTC
   MesureTemp.HeureEEPROM=DateMesure.heures;
   MesureTemp.MinuteEEPROM=DateMesure.minutes;
   MesureTemp.JourEEPROM=DateMesure.jour;
   MesureTemp.MoisEEPROM=DateMesure.mois;
   MesureTemp.AnneeEEPROM=DateMesure.annee;
//Mesure interieure
   MesureTemp.TypeMesure=HygroM;
//Lecture de la mesure
   MesureTemp.ValMesure=0;  ///////////////////////////////A modifier
   return MesureTemp.ValMesure;
}
void GestioVanne(int Hygro)
{
  //Agit sur l'electrovanne en fonction de l'hygrométrie
  
}
int MesureLumiere()
{
  //Mesure la luminosité. Idem, stocke et retourne la valeur mesurée
   MesureEEPROM MesureTemp;  //Génère une structure type mesure
  //Récupère la date et l'heure du RTC
   MesureTemp.HeureEEPROM=DateMesure.heures;
   MesureTemp.MinuteEEPROM=DateMesure.minutes;
   MesureTemp.JourEEPROM=DateMesure.jour;
   MesureTemp.MoisEEPROM=DateMesure.mois;
   MesureTemp.AnneeEEPROM=DateMesure.annee;
//Mesure interieure
   MesureTemp.TypeMesure=LuminositeM;
//Lecture de la mesure
   MesureTemp.ValMesure=0;  ///////////////////////////////A modifier
   return MesureTemp.ValMesure;  
}
void GestionLumiere(int LumiereMesuree)
{
  //En fonction de la lumière, allume ou non la lumière
  
}

void GestionMenu()
{
  //Gestion du menu
}
void Touche_Appuyee()
{
  //Gestion de l'affichage hors menu
  
}
void ReinitialiseMesure()
{
  FaireMesure=false;
  //on créé le prochain moment de faire une mesure
  HeureMesure=DateMesure.heures+TempoTrig;
}

//Initialisation
void setup() {
  //Initialisation système
//Initialise les bus
//Initialise les ports
  pinMode(BoutonMenu, INPUT_PULLUP);
  pinMode(BoutonGauche, INPUT_PULLUP);
  pinMode(BoutonDroit, INPUT_PULLUP);
  pinMode(BoutonHaut, INPUT_PULLUP);
  pinMode(BoutonBas, INPUT_PULLUP);
//initialise les affichages
//initialise l'horloge (en provenance de l'horloge)
  RecupereDateHeure(&DateMesure);
  FaireMesure=true;
//Lecture de l'eeprom et mise en place des min max
  TempIntMin=0;
  TempIntMax=0;
  TempExtMin=0;
  TempExtMax=0;
  
//Demande a faire une mesure : initialise donc le cycle
DebutScript=true;
}

void loop() {
//Boucle principale.
 byte HeureActu;
  //Faire mesure est calculée en fonction de la l'heure de la dernière mesure
    //Calcul de FaireMesure 
/* * * * * * * * * * * * * * * * * * *    
 *  On va lire l'heure sur la RTC    *
 *  Puis comparer avec le trig       *
 * * * * * * * * * * * * * * * * * * */
  RecupereDateHeure(&DateMesure); 
  if  (DateMesure.heures>=HeureMesure)
  {
    FaireMesure=true; 
  }
  if (FaireMesure or DebutScript)
  {
     DebutScript=false;  //On a démarrer la carte, on fait la mesure
     MesureTemp();
     GestionAreoChauffage();
     GestioVanne(MesureHygro());
     GestionLumiere(MesureLumiere());
     ReinitialiseMesure(); 
  }
  
}
