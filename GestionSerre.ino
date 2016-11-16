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
int TrigHygro, TrigTemp, TrigLumiere;  //valeur a partir du moment ou on arrose
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

//déclarations fonctions
void RecupereDateHeure(DateRTC *date)
{
  //Récupère la date de la RTC
}
//Fonctions nécessaires pour la RTC
byte BCDToDec(byte bcd)
{
  return((bcd /16*10)+(bcd%16));
}
byte DecToBCD(byte dec)
{
  return((dec/10*16)+(dec%10));
}
void MesureTemp()  //Mesure la températures
{
  //Mesure la température intérieure, extérieure. Compare avec le min et le max, et si valeurdépassée, stocke en EEPROM
  //Stocke les valeurs dans les variables globales, pour les fonctions de traitement
 MesureEEPROM MesureTemp;  //Génère une structure type mesure
  //Récupère la date et l'heure du RTC
}
void GestionAreoChauffage()
{
  //En fonction de la température, algorithme qui défini l'ouverture de la serre, la mise en route du ventilo ou le chauffage
  
}
int MesureHygro()
{
  //Mesure l'hygrométrie et la stocke en mémoire
  //Retourne la valeur mesurée
  
}
void GestioVanne(int Hygro)
{
  //Agit sur l'electrovanne en fonction de l'hygrométrie
  
}
int MesureLumiere()
{
  //Mesure la luminosité. Idem, stocke et retourne la valeur mesurée
  
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
 /* Mois=0;
  Jour=0;
  Heure=0;
  Minute=0;
  Annee=0;*/

//Lecture de l'eeprom et mise en place des min max
  TempIntMin=0;
  TempIntMax=0;
  TempExtMin=0;
  TempExtMax=0;

//Demande a faire une mesure : initialise donc le cycle
}

void loop() {
  //Boucle principale.
  if (FaireMesure)
  {
     MesureTemp();
     GestionAreoChauffage();
     GestioVanne(MesureHygro());
     GestionLumiere(MesureLumiere());
     ReinitialiseMesure(); 
  }
  
}
