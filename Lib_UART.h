 
/*----------------------------------------------------------------------------
 * Define de code pour DFPlayer
 *---------------------------------------------------------------------------*/
	
#define PLAY 											(0x0D)		// Play
#define PAUSE 										(0x0E)		// Commande pour mettre en pause la piste Audio
#define SPECIFY_VOLUME 						(0x06)		// Commande pour séléctioner un volume spécifique
#define REPEAT_PLAY 							(0x11)		// Jouer en boucle ou ne plus jouer en boucle une piste audio

/*----------------------------------------------------------------------------
 * Fonction Init UART & DFPlayer
 *---------------------------------------------------------------------------*/

void Init_UART ( void );

void LinkDFPlayer ( void );
void Send_DFPlayer_Command ( unsigned char command, unsigned const char param1, unsigned const char param2 );
 
/*----------------------------------------------------------------------------
 * Fonction Tempos
 *---------------------------------------------------------------------------*/

void tempo ( int ms );

/*----------------------------------------------------------------------------
 * Lecture RFID
 *---------------------------------------------------------------------------*/

char *Read_RFID ( void );
