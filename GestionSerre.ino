/* * * * * * * * * * * * * * * * * * * * * * * * * * */
/*            Gestion Serre intelligente             */
/*   G. Cregut                                       */
/*   DATE Création : 14/11/2016                      */
/*   Date Modification : 16/11/2016                  */
/* (c)2016 Editiel98                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * */

//déclarations includes
#include <Wire.h>  //pour l'EEPROM, la RTC et l'écran.
#include <EEPROM.h> // Pour le stockage des paramètres dans la PROM de l'arduino
#include "structDate.h" //stucture de la date
//Déclarations variables globales
int TempExt, TempInt, TempMin, TempMax ;  //Retour des mesures de températures
bool FaireMesure;   //Toggle pour déclencher les mesures. En fonction de la période des mesures
bool DebutScript; // indique qu'on démarre la carte
int TrigHygro, TrigTemp, TrigLumiere,TrigOuverture;  //valeur a partir du moment ou on arrose, aere ou allume la lumière
int PointeurEEPROM;  //Pointeur de position dans la prom. Pas forcement INT, a voir !
byte TempoTrig,HeureMesure; //Delai en heure entre 2 mesures, Heure de la dernière mesure
//Structure pour lire et écrire dans la RTC
DateRTC DateMesure;
MesureEEPROM MesureFaite;  //Génère une structure type mesure

//Déclarations des constantes globales
  //Adresses Bus
#define  Adress_RTC 0
//Entrées analogiques
#define PinTempInt A0  //Broche connectée au capteur intérieur.
#define PinTempExt A1
#define PinHygro A2
#define PinLuminosite A3

//Entrées numériques  //Changer les valeurs
#define BoutonMenu 1
#define BoutonHaut 2
#define BoutonGauche 3
#define BoutonDroit 4
#define BoutonBas 5

//sorties  //Changer les valeurs
#define PinVanne 6
#define PinChauffage 7
#define PinServo 8
#define PinLumiere 9
//Bus

//Declaration des types de mesures
#define TempExtM 1
#define TempIntM 2
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
int EcrireEEPROM(int debut,MesureEEPROM *MesureAEnregistrer)
{
//Verifions si on déborde pas.... On est à l'adresse d'écriture
  //Si adresse+taille enregistrement >taille EEPROM, adresse=0
//Se positionne au bon endroit dans l'eeprom

//Ecrit dans l'eeprom

//augmente le pointeur de la taille d'une structure
  debut=debut+sizeof(MesureEEPROM);  
  return debut;
}

int MesureTemp(int TypeTemp)  //Mesure la températures
{
  int AdresseMin,  AdresseMax, ValeurLue;
  //Mesure la température intérieure, extérieure. Compare avec le min et le max, et si valeurdépassée, stocke en EEPROM
  //Stocke les valeurs dans les variables globales, pour les fonctions de traitement
   MesureFaite.TypeMesure=TypeTemp;
//Lecture de la mesure
  switch(TypeTemp)  //En fonction du type de capteur (interieur ou exterieur)
  {
    case TempExtM :
      MesureFaite.ValMesure=analogRead(PinTempExt);
      //Adresse Min et Max dans l'EEPROM arduino
      AdresseMin=sizeof(int)*2;  //L'adresse Temp Ext Min est la 3ème position après 2 autres int
      AdresseMax=sizeof(int)*3;  //L'adresse Temp Ext Max est la 4ème position après 3 autres int
      break;
    case TempIntM : 
      MesureFaite.ValMesure=analogRead(PinTempInt);
      //Adresse Min et Max dans l'EEPROM arduino
      AdresseMin=0;  //La veleur de temp Int min est à l'adresse 0
      AdresseMax=sizeof(int); //La valeur temp Int Max est la suivante, soit 0+taille de int
      break; 
  }  
//Vérifie sur mesureTemp.ValMesure<min ou >max
   EEPROM.get(AdresseMin,ValeurLue);  //On récupère la temp Min
   if (MesureFaite.ValMesure<ValeurLue)
   {
      //On stocke la valeur et on passe
      EEPROM.put(AdresseMin,MesureFaite.ValMesure); 
   }
   else  //Si la mesure est au dessus de la min elle peut etre max.
   {
       EEPROM.get(AdresseMax,ValeurLue);  //On récupère la temp Max
       if (MesureFaite.ValMesure>ValeurLue)
       {
          //On stocke la valeur max
           EEPROM.put(AdresseMax,MesureFaite.ValMesure);
       }
   }

//Si mesure correspond, on stocke min ou max, et on memorise
  
//Enregistrement de la mesure dans la PROM
  PointeurEEPROM=EcrireEEPROM(PointeurEEPROM, &MesureFaite);
}
void GestionAreoChauffage(int Interieur, int Exterieur)
{
  //En fonction de la température, algorithme qui défini l'ouverture de la serre, la mise en route du ventilo ou le chauffage
  //TrigTemp,TrigOuverture
  
  
}
int MesureHygro()
{
  //Mesure l'hygrométrie et la stocke en mémoire
  //Retourne la valeur mesurée
   MesureFaite.TypeMesure=HygroM;
//Lecture de la mesure
   MesureFaite.ValMesure=analogRead(PinHygro);  
//Enregistrement de la mesure dans la PROM
   PointeurEEPROM=EcrireEEPROM(PointeurEEPROM, &MesureFaite);   
   return MesureFaite.ValMesure; 
}
void GestionVanne(int Hygro)
{
  //Agit sur l'electrovanne en fonction de l'hygrométrie
  int HygroBascule;
  //On créé un trigger de schmitt pour ne pas faire que arroser arret arrosage
  HygroBascule=(TrigHygro/100);
  HygroBascule=(HygroBascule*10)+TrigHygro;   //10% inférieur au seuil
  if (Hygro<HygroBascule)
  {
    //On ouvre la vanne
    digitalWrite(PinVanne,HIGH);
  }
  else  //si on est pas en bas, on peut etre en haut...
  {
    HygroBascule=(TrigHygro/100);
    HygroBascule=(HygroBascule*20)+TrigHygro;  //20 % supérieur au seuil
    if (Hygro>HygroBascule)
    {
    //On ferme la vanne
      digitalWrite(PinVanne,LOW);
    }
  }  
}
int MesureLumiere()
{
  //Mesure la luminosité. Idem, stocke et retourne la valeur mesurée
   MesureFaite.TypeMesure=LuminositeM;
//Lecture de la mesure
   MesureFaite.ValMesure=analogRead(PinLuminosite);  
//Enregistrement de la mesure dans la PROM
   PointeurEEPROM=EcrireEEPROM(PointeurEEPROM, &MesureFaite);   
   return MesureFaite.ValMesure;  
}
void GestionLumiere(int LumiereMesuree)
{
  //En fonction de la lumière, allume ou non la lumière
  int LumiereBascule;
  //On créé un trigger de schmitt pour ne pas faire que arroser arret arrosage
  LumiereBascule=(TrigLumiere/100);
  LumiereBascule=(LumiereBascule*10)+TrigLumiere;   //10% inférieur au seuil
  if (LumiereMesuree<LumiereBascule)
  {
    //On allume la lumière
    digitalWrite(PinVanne,HIGH);
  }
  else  //si on est pas en bas, on peut etre en haut...
  {
    LumiereBascule=(TrigLumiere/100);
    LumiereBascule=(LumiereBascule*20)+TrigLumiere;  //20 % supérieur au seuil
    if (LumiereMesuree>LumiereBascule)
    {
    //On éteint la lumière
      digitalWrite(PinLumiere,LOW);
    }
  }    
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
//Mise à 0 de toutes les sorties
  digitalWrite(PinVanne,LOW);  //On arrete la vanne
  digitalWrite(PinLumiere,LOW);  //On arrete la lumière
  digitalWrite(PinChauffage,LOW);  //On arrete le chauffage
//initialise les affichages
//initialise l'horloge (en provenance de l'horloge)
  RecupereDateHeure(&DateMesure);
  FaireMesure=true;
//Lecture de l'eeprom et mise en place des min max
  EEPROM.get(sizeof(int)*4,TrigOuverture);
  EEPROM.get(sizeof(int)*6,TrigHygro);
  EEPROM.get(sizeof(int)*8,TrigTemp);
  EEPROM.get(sizeof(int)*9,TrigLumiere);
//Lecture   
  EEPROM.get(sizeof(int)*10,TempoTrig);
 
//Demande a faire une mesure : initialise donc le cycle
  DebutScript=true;
//Initialise le pointeur d'eeprom
  PointeurEEPROM=0;  
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
     //Stockage de la date et de l'heure
     MesureFaite.HeureEEPROM=DateMesure.heures;
     MesureFaite.MinuteEEPROM=DateMesure.minutes;
     MesureFaite.JourEEPROM=DateMesure.jour;
     MesureFaite.MoisEEPROM=DateMesure.mois;
     MesureFaite.AnneeEEPROM=DateMesure.annee;
     DebutScript=false;  //On a démarrer la carte, on fait la mesure
     TempInt=MesureTemp(TempIntM);
     TempExt=MesureTemp(TempExtM);
     GestionAreoChauffage(TempInt, TempExt);
     GestionVanne(MesureHygro());
     GestionLumiere(MesureLumiere());
     ReinitialiseMesure(); 
  }
  
}
