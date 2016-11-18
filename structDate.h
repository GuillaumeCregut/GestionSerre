/* La strutucture suivante "DateRTC" permet de manipuler facilement les données de date / heure
 *  Date date;
 *   date.mois = 1;
 *
 * Attention ! Lorsque la structure est passé en argument via un pointeur, ou qu'on accède à celle ci via un pointeur il faut utiliser ->
 * Exemple : Date *date, date2;
 *           date = &date2; // le pointeur date (*date) pointe désormais sur l'adresse de date2 (&date2)
 *           date->mois = 1; // correspond à date2.mois = 1;
 * /!\ Quand une strucuture ou une variable est passé en référence via un pointeur à une fonction c'est la variable pointé qui est modifié ET NON UNE COPIE de la variable comme c'est le cas dans un passage d'arguments normal.  
 */
typedef struct {
  uint8_t secondes;
  uint8_t minutes;
  uint8_t heures; // format 24h
  uint8_t jour;
  uint8_t mois; 
  uint8_t annee; // format yy (ex 2012 -> 12)
} DateRTC;

typedef struct
{
  uint8_t TypeMesure;
  uint8_t HeureEEPROM;
  uint8_t MinuteEEPROM;
  uint8_t MoisEEPROM;
  uint8_t JourEEPROM;
  uint8_t AnneeEEPROM;
  int ValMesure; 
} MesureEEPROM; //Structure pour la lecture ou l'écriture d'une mesure;
