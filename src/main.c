#include "../h/main.h"


int knoten;
int kreuzung[3];


/* nxtOSEK hook to be invoked from an ISR in category 2 */
void user_1ms_isr_type2(void) {
}

void ecrobot_device_initialize(void) {
}

void ecrobot_device_terminate(void) {
}

void hello_world() {
	ecrobot_status_monitor("Hello, World!");

}

void stop(){
/*Funktion zum stoppen der Motoren*/
	nxt_motor_set_speed(NXT_PORT_A, 0, 1);
	nxt_motor_set_speed(NXT_PORT_B, 0, 1);
}

void lenken(U32 n, int s, int v){
/*Funktion beschreibt die Ansteuerung der einzelnen Motoren
 * uebergeben wird Port des Motors (n), die gewollte Umdrechung des Rades (s)
 * und die Lenkgeschwindigkeit*/
	int b=0;
	nxt_motor_set_count(n, 0);					//Count des Rades wird Null gesetzt
	if(s<0){									//Fallunterscheidung fuer Richtung (rechts/links)
		while (b>s){
			nxt_motor_set_speed(n, -v, 1);		//Initiallisieren des Motors mit Geschw. (v)
			b=nxt_motor_get_count(n);			//Abtasten des Counts
		}
	}
	else{
		while (b<s){
			nxt_motor_set_speed(n, v, 1);		//wie oben
			b=nxt_motor_get_count(n);
		}
	}
}

void richtiglenken(int s, int v){
/* Geschmeidigere Lenkfunktion
 * uebergeben wird die gewollte Umdrehung des Rades (s)
 * und die Lenkgeschwindigkeit (v)
 * Benutzt wird die einfache Lenkfunktion
 */
	int i=0;
	if (s>0){									//Fallunterscheidung bez. der Lenkrichtung
		while (i<=s){							//Auslenkung beider Raeder durch gleichmaessige
				lenken(NXT_PORT_A, 2, v);		//Beschleunigung beider Motoren
				lenken(NXT_PORT_B, -2, v+5);
				i=i+2;
		}
	}
	else{
		while (i>=s){
				lenken(NXT_PORT_A, -2, v);		//wie oben, nur andere Richtung
				lenken(NXT_PORT_B, 2, v+5);
				i=i-2;
		}
	}
	stop();										//Ende des Lenkvorgangs
}


void orientierung(int lenken_v){
/*Roboter wird an der letzten Linie ausgerichtet*/

	int b=0;										//Linie wird gesucht durch groesser werdende
	while(b<20){							//Auslenkungen (b++)
		richtiglenken(2, lenken_v);				//zuerst rechts
		b=b+2;
	}

}

void folgen(int v, int lenken_v){
/*Funktion zum Folgen der schwarzen Linie und aendern der Knoten-variable
 * uebergeben werden die Fahrgeschwindigkeit (v) und die Lenkgeschwindigkeit (lenk_v)
 */
	int a;										//Variable mit dem Lichtwert
	while(knoten==0){							//Solange man nicht am Knoten ist, faehrt der Roboter

		ecrobot_set_light_sensor_active(NXT_PORT_S4);	//Lichtsensor initiallisieren
		a = ecrobot_get_light_sensor(NXT_PORT_S4);		//Lichtwert auslesen

		/*int i=1;
		while(a>500 && i<v){							//Lichtwert > 500 = schwarz
			nxt_motor_set_speed(NXT_PORT_A, i, 1);		//Langsame Beschleunigung beider Motoren
			nxt_motor_set_speed(NXT_PORT_B, i, 1);		//bis Ausgangsgeschw. erreicht ist
			a = ecrobot_get_light_sensor(NXT_PORT_S4);
			systick_wait_ms(10);						//Regelung der Beschleunigung durch Pausen
			i++;
		}
		*/

		while(a>500){									//Weiterfahren mit Ausgangsgeschw.
			nxt_motor_set_speed(NXT_PORT_A, v, 1);		//bei gleichzeitigem Abtasten der Linie
			nxt_motor_set_speed(NXT_PORT_B, v+5, 1);
			a = ecrobot_get_light_sensor(NXT_PORT_S4);
		}

		stop();											//bei verlust der Linie Stop

		int b=0;										//Linie wird gesucht durch groesser werdende
		while(b<20 && a<500){							//Auslenkungen (b++)
			richtiglenken(2, lenken_v);			//zuerst rechts
			a = ecrobot_get_light_sensor(NXT_PORT_S4);
			b=b+2;
			}
		b=0;
		while(b<40 && a<500){
			richtiglenken(-2, lenken_v);
			a = ecrobot_get_light_sensor(NXT_PORT_S4);
			b=b+2;
		}
		if (a<500){										//Linie zuende, also konten
			knoten=1;
		}
	}
	orientierung(lenken_v);

}

void fahren(int v){

	nxt_motor_set_count(NXT_PORT_A, 0);
	nxt_motor_set_count(NXT_PORT_B, 0);


	nxt_motor_set_speed(NXT_PORT_A, v, 1);		//Langsame Beschleunigung beider Motoren
	nxt_motor_set_speed(NXT_PORT_B, v+3, 1);
	systick_wait_ms(1000);
	stop();

}

void suchen(int lenken_v){
	kreuzung[0]=kreuzung[1]=kreuzung[2]=0;
	ecrobot_set_light_sensor_active(NXT_PORT_S4);	//Lichtsensor initiallisieren
	int x=250, s=0, a=ecrobot_get_light_sensor(NXT_PORT_S4);  //Linie wird gesucht durch groesser werdende

	while(s<x){

		richtiglenken(2, lenken_v);
		s=s+2;
		a=ecrobot_get_light_sensor(NXT_PORT_S4);
		if(a>500){
			if((s >= 0 && s<= x/16) || (s >= x-x/16)){
				kreuzung[2]=1;
				ecrobot_sound_tone(200, 500, 20);

			}
			if(s >= x/4-x/16 && s<=x/4+x/16 ){
				kreuzung[1]=1;
				ecrobot_sound_tone(400, 500, 20);
			}
			if(s >=3*x/4-x/16 && s<=3*x/4+x/16 ){
				kreuzung[0]=1;
				ecrobot_sound_tone(600, 500, 20);
			}
		}
		stop();
	}

	stop();
}

void anzeige(int x[]){

	int a=x[0]*100+x[1]*10+x[2];
	switch(a) {
		case 0: ecrobot_status_monitor("Sackgasse"); break;
		case 1: ecrobot_status_monitor("             GERADEAUS"); break;
		case 10: ecrobot_status_monitor("      RECHTS          "); break;
		case 11: ecrobot_status_monitor("      RECHTS GERADEAUS"); break;
		case 100: ecrobot_status_monitor("LINKS                 "); break;
		case 101: ecrobot_status_monitor("LINKS        GERADEAUS"); break;
		case 110: ecrobot_status_monitor("LINKS RECHTS          "); break;
		case 111: ecrobot_status_monitor("LINKS RECHTS GERADEAUS"); break;
	}

}


TASK(OSEK_Main_Task) {

	while (1) {
		knoten=0;
		folgen(50, 20);
		fahren(30);
		suchen(30);
		anzeige(kreuzung);
	  	/*500msec wait */
		systick_wait_ms(2000);
		ecrobot_sound_tone(220, 1000, 20);				//Sound beschreibt einen Durchgang
	}
}
