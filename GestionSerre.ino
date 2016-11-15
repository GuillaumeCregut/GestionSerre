/* * * * * * * * * * * * * * * * * * * * * * * * * * */
/*            Gestion Serre intelligente             */
/*   G. Cregut                                       */
/*   DATE Création : 14/11/2016                      */
/*   Date Modification : 15/11/2016                  */
/* (c)2016 Editiel98                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * */
//déclarations includes

//Déclarations variables globales
int TempExt, TempInt, TempIntMin, TempIntMax,TempExtMin, TempExtMax ;  //Retour des mesures de températures
bool FaireMesure;   //Toggle pour déclencher les mesures. En fonction de la période des mesures
int TrigHygro, TrigTemp, TrigLumiere;  //valeur a partir du moment ou on arrose
int Mois, Annee,Jour,Heure,Minute;  //En provenance de l'horloge

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
void MesureTemp()  //Mesure la températures
{
  //Mesure la température intérieure, extérieure. Compare avec le min et le max, et si valeurdépassée, stocke en EEPROM
  //Stocke les valeurs dans les variables globales, pour les fonctions de traitement
  
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
  Mois=0;
  Jour=0;
  Heure=0;
  Minute=0;
  Annee=0;

//Lecture de l'eeprom et mise en place des min max
  TempIntMin=0;
  TempIntMax=0;
  TempExtMin=0;
  TempExtMax=0;

//Damande a faire une mesure : initialise donc le cycle
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
