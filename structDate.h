typedef struct {
  uint8_t secondes;
  uint8_t minutes;
  uint8_t heures; // format 24h
  uint8_t jourDeLaSemaine; // 1~7 => lundi, mardi, ...
  uint8_t jour;
  uint8_t mois; 
  uint8_t annee; // format yy (ex 2012 -> 12)
} DateRTC;