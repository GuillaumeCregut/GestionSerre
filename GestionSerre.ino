/* * * * * * * * * * * * * * * * * * * * * * * * * * */
/*            Gestion Serre intelligente             */
/*   G. Cregut                                       */
/*   DATE Création : 14/11/2016                      */
/*   Date Modification : 20/11/2016                  */
/* (c)2016 Editiel98                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * */

//déclarations includes
#include <Wire.h>  //pour l'EEPROM, la RTC et l'écran.
#include <EEPROM.h> // Pour le stockage des paramètres dans la PROM de l'arduino
#include "structDate.h" //stucture de la date

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/*                        Déclarations variables globales                  */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int TempExt, TempInt, MesHygro, MesLum ;  //Retour des mesures de températures
bool FaireMesure;   //Toggle pour déclencher les mesures. En fonction de la période des mesures
bool DebutScript; // indique qu'on démarre la carte
bool EtatVolet, EtatLumiere,EtatVanne, EtatChauffage;
bool InMenu; //Défini si on est entrer dans le menu ou non
int TrigHygro, TrigTemp, TrigLumiere,TrigOuverture;  //valeur a partir du moment ou on arrose, aere ou allume la lumière
int PointeurEEPROM;  //Pointeur de position dans la prom. Pas forcement INT, a voir !
byte TempoTrig,HeureMesure; //Delai en heure entre 2 mesures, Heure de la dernière mesure
//Structure pour lire et écrire dans la RTC
DateRTC DateMesure;
MesureEEPROM MesureFaite;  //Génère une structure type mesure

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/*                       Déclarations constantes globales                  */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

  //Adresses Bus
#define Adress_RTC 0     //A changer
#define AdresseLCD 1     //A changer
#define AdresseEEPROM 2     //A changer
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

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/*                          Déclarations des fonctions                     */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

                     /********Fonctions nécessaires pour la RTC********/ 
  //Fonction conversion BCD vers Décimale
byte BCD2Dec(byte bcd)
{
  return((bcd /16*10)+(bcd%16));
}
  //Fonction conversion Décimale vers BCD
byte Dec2BCD(byte dec)
{
  return((dec/10*16)+(dec%10));
}
                     /********Fonction Lecture de la RTC********/ 
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

                     /********Fonction Ecriture de la RTC********/ 
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
                     /********Fonction Conversion 2 Bytes en Int ********/ 
int ConvertByteToInt(byte ValMsb, byte ValLsb)
{
  return (ValMsb<<8)|ValLsb;
}

                     /********Fonction Ecriture d'un octet en mémoire ********/ 
void I2CEEPROMWrite(int Adresse,byte Valeur)
{
  Wire.beginTransmission(AdresseEEPROM);  //adresse de la prom 
  Wire.write((Adresse>>8)&0xFF);
  Wire.write((Adresse>>0)&0xFF);
  Wire.write(Valeur);
  Wire.endTransmission();
  delay(5);                    
}

                     /********Fonction Lecture d'un octet en mémoire ********/ 
byte I2CEEPROMRead(int Adresse)
{
  byte tempo=0xFF;  //Au cas ou on ai rien lu
  Wire.beginTransmission(AdresseEEPROM);  //adresse de la prom
  Wire.write((Adresse>>8)&0xFF);
  Wire.write((Adresse>>0)&0xFF);
  Wire.endTransmission();
  delay(5);
  Wire.requestFrom(AdresseEEPROM,1);
  tempo=Wire.read();
  delay(5);
  return tempo;
}
                     /********Fonction Gestion de l'EEPROM I2C********/ 
//Fonction écriture dans l'EEPROM I2C
int EcrireEEPROM(int debut,MesureEEPROM *MesureAEnregistrer)
{
//Variables
  byte MSBVal,LSBVal;  //On écrit octet par octet.
  byte MesuresAEcrire[8];
  MSBVal=highByte(MesureAEnregistrer->ValMesure);
  LSBVal=lowByte(MesureAEnregistrer->ValMesure);
 /* MSBVal=(MesureAEnregistrer->ValMesure &0xFF00)>>8;
  LSBVal=MesureAEnregistrer->ValMesure&0x00FF;*/
  /*      Schéma adressage
   *  -Adresse 0, soit début : Minutes
   *  -Adresse+1 : Heures
   *  -Adresse+2 : Jour
   *  -Adresse+3 : Mois
   *  -Adresse+4 : Annee
   *  -Adresse+5 : Type Mesure
   *  -Adresse+6 : MSB
   *  -Adresse+7 : LSB
   */
   //On affecte les mesures à un tableau
   MesuresAEcrire[0]=MesureAEnregistrer->HeureEEPROM;
   MesuresAEcrire[1]=MesureAEnregistrer->MinuteEEPROM;
   MesuresAEcrire[2]=MesureAEnregistrer->JourEEPROM;
   MesuresAEcrire[3]=MesureAEnregistrer->MoisEEPROM;
   MesuresAEcrire[4]=MesureAEnregistrer->AnneeEEPROM;
   MesuresAEcrire[5]=MesureAEnregistrer->TypeMesure;
   MesuresAEcrire[6]=MSBVal;
   MesuresAEcrire[7]=LSBVal;
  //Verifions si on déborde pas.... On est à l'adresse d'écriture
  //Si adresse+taille enregistrement >taille EEPROM, adresse=0
  if (debut>65527)  //65535-8 car on a 8 octets dans l'enregistrement
  {
    debut=0;
  }
  //Ecrit dans l'eeprom
   for (int i=0;i<8;i++)
   {
      I2CEEPROMWrite(debut+i,MesuresAEcrire[i]);
   }
  //augmente le pointeur de la taille d'une structure
  debut=debut+sizeof(MesureEEPROM);  
  return debut;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/*                         Fonctions mesures et actions                    */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */  
int MesureTemp(byte TypeTemp)  //Mesure la température
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
      TempExt=MesureFaite.ValMesure;
      break;
    case TempIntM : 
      MesureFaite.ValMesure=analogRead(PinTempInt);
      TempInt=MesureFaite.ValMesure;
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
  //Enregistrement de la mesure dans la PROM
  PointeurEEPROM=EcrireEEPROM(PointeurEEPROM, &MesureFaite);
}

                     /********Fonction Gestion de Chauffage / Aération********/  
void GestionAreoChauffage(int Interieur, int Exterieur)
{
  //En fonction de la température, algorithme qui défini l'ouverture de la serre, la mise en route du ventilo ou le chauffage
  //TrigTemp,TrigOuverture
 // Gere_Chauffage();
 // Gere_Volet(); 
}

                     /********Fonction Mesure Hyrgro********/
int MesureHygro()
{
  //Mesure l'hygrométrie et la stocke en mémoire
   MesureFaite.TypeMesure=HygroM;
   //Lecture de la mesure
   MesureFaite.ValMesure=analogRead(PinHygro); 
   MesHygro=MesureFaite.ValMesure;
   //Enregistrement de la mesure dans la PROM
   PointeurEEPROM=EcrireEEPROM(PointeurEEPROM, &MesureFaite);   
   return MesureFaite.ValMesure; 
}

                     /********Fonction Gestion de Vanne********/
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
    Gere_Vanne(true);
  }
  else  //si on est pas en bas, on peut etre en haut...
  {
    HygroBascule=(TrigHygro/100);
    HygroBascule=(HygroBascule*20)+TrigHygro;  //20 % supérieur au seuil
    if (Hygro>HygroBascule)
    {
    //On ferme la vanne
      Gere_Vanne(false);
    }
  }  
}

                     /********Fonction Mesure Luminosité********/
int MesureLumiere()
{
  //Mesure la luminosité. Idem, stocke et retourne la valeur mesurée
  MesureFaite.TypeMesure=LuminositeM;
  //Lecture de la mesure
  MesureFaite.ValMesure=analogRead(PinLuminosite);  
  MesLum=MesureFaite.ValMesure;
  //Enregistrement de la mesure dans la PROM
  PointeurEEPROM=EcrireEEPROM(PointeurEEPROM, &MesureFaite);   
  return MesureFaite.ValMesure;  
}

                     /********Fonction Gestion Lumiere********/
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
    Gere_Lumiere(true);
  }
  else  //si on est pas en bas, on peut etre en haut...
  {
    LumiereBascule=(TrigLumiere/100);
    LumiereBascule=(LumiereBascule*20)+TrigLumiere;  //20 % supérieur au seuil
    if (LumiereMesuree>LumiereBascule)
    {
    //On éteint la lumière
     Gere_Lumiere(false);
    }
  }    
}

                     /********Fonction Action chauffage ********/
void Gere_Chauffage(bool EtatNouveau)
{
  if(EtatNouveau)
  {
    
  }
  else
  {
    
  }
}
                     /********Fonction Action Volet ********/      
void Gere_Volet(bool EtatNouveau)
{
  if(EtatNouveau)
  {
    
  }
  else
  {
    
  }  
}                     
                     /********Fonction Action Lumière ********/
void Gere_Lumiere(bool EtatNouveau)
{
  if(EtatNouveau)
  {
    //On allume la lumière
    digitalWrite(PinVanne,HIGH);
    EtatLumiere=true;
  }
  else
  {
    //On allume la lumière
    digitalWrite(PinVanne,LOW);
    EtatLumiere=false;
  }  
}
                     /********Fonction Action vanne ********/
void Gere_Vanne(bool EtatNouveau)
{
  if(EtatNouveau)
  {
    //On ouvre la vanne
    digitalWrite(PinVanne,HIGH);
    EtatVanne=true;
  }
  else
  {
    //On ouvre la vanne
    digitalWrite(PinVanne,LOW);
    EtatVanne=false;
  }  
}

                     /********Fonction Menu  A (re)définir ********/
void GestionMenu()
{
  //Gestion du menu
  InMenu=true; //On informe que l'on est dans le menu

  //en fin de menu
  InMenu=false;
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

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                Initialisation                                       */
/*  On Va cherches les infos diverses dans l'eeprom                                    */
/*  On initialise les différentes broches des ports                                    */
/*  On initialise la mémoire EEPROM extérieure                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void setup() {
  //Initialisation système
  InMenu=false;
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
  EEPROM.get(sizeof(int)*7,TrigLumiere);
  //Lecture de l'intervalle de mesure  
  EEPROM.get(sizeof(int)*9,TempoTrig);
 
  //Demande a faire une mesure : initialise donc le cycle
  DebutScript=true;
  //Initialise le pointeur d'eeprom
  PointeurEEPROM=0;  
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                 Boucle principale                                               */
/*              On releve la date/heure et on regarde si on doit faire une mesure, puis on         */
/*              on regarde si on a appuyer sur une touche haut ou bas.                             */
/*              Ensuite, on regarde si on doit entrer dans le menu. Enfin on repart au début       */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void loop() {
  //Boucle principale.
  byte HeureActu;
  //Faire mesure est calculée en fonction de la l'heure de la dernière mesure
  //On va lire l'heure sur la RTC Puis comparer avec le trig      
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
  //Scan des boutons
}
