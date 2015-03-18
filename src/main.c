#include "../h/main.h"


int knoten;									//Zaehlen der Knoten
int kreuzung[3];							//Array mit {Links, Rechts, Geradeaus} kodierung
int gegenstand;								//Gegenstandszaehlung

/* nxtOSEK hook to be invoked from an ISR in category 2 */
void user_1ms_isr_type2(void) {
}

void ecrobot_device_initialize(void) {
}

void ecrobot_device_terminate(void) {
}

void stop(){
	/*
	 * Funktion zum stoppen der Motoren
	 */
	nxt_motor_set_speed(NXT_PORT_A, 0, 1);
	nxt_motor_set_speed(NXT_PORT_B, 0, 1);
}

void lenken(U32 n, int s, int v){
	/*
	 * Funktion beschreibt die Ansteuerung der einzelnen Motoren
	 * uebergeben wird Port des Motors (n), die gewollte Umdrechung des Rades (s)
	 * und die Lenkgeschwindigkeit
	 */
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
	/*
	 * Geschmeidigere Lenkfunktion
	 * uebergeben wird die gewollte Umdrehung des Rades (s)
	 * und die Lenkgeschwindigkeit (v)
	 * Benutzt wird die einfache Lenkfunktion
	 */
	int i=0;
	if (s>0){									//Fallunterscheidung bez. der Lenkrichtung
		while (i<=s){							//Auslenkung beider Raeder durch gleichmaessige
				lenken(NXT_PORT_A, 2, v);		//Beschleunigung beider Motoren
				lenken(NXT_PORT_B, -2, v+4);	//Korrektur am rechten Motor noetig
				i=i+2;
		}
	}
	else{
		while (i>=s){
				lenken(NXT_PORT_A, -2, v);		//wie oben, nur andere Richtung
				lenken(NXT_PORT_B, 2, v+4);
				i=i-2;
		}
	}
	stop();										//Ende des Lenkvorgangs
}


void orientierung(int lenken_v){
	/*
	 * Roboter wird an der letzten Linie ausgerichtet
	 * bzw. ruzueckgedreht nach der letzten Pfadsuche
	 * uebergeben wird die Lenkgeschwindigkeit (leneken_v)
	 */
	int b=0;
	while(b<20){							//Gesucht wird um 20LE nach recht und 40LE nach links
		richtiglenken(2, lenken_v);			//Um zur Ursprungsfunktion zu gelangen, wird um 20LE
		b=b+2;								//wieder nach rechts gedreht
	}
}

void fahren(int v){
	/*
	 *Funktion zum Vorfahren zur ungefairen Mitte des Knotens
	 */
	nxt_motor_set_speed(NXT_PORT_A, v, 1);		//Motor am Port_B faehrt langsamer als der am Port_A
	nxt_motor_set_speed(NXT_PORT_B, v+2, 1);	//dies wird hier mit v+3 korrigiert
	systick_wait_ms(1000);						//nach 1 sekunde fahren ist man ungefair in der Mitte des Knoten
	stop();
}

void suchen(int lenken_v){
	/*
	 * Funktion zum Drehen am Knoten, dabei werden die moeglichen Richtungen in Array (kreuzung) gespeichert
	 * uebergeben wird die Lenkgeschwindigkeit (lenken_v)
	 */
	kreuzung[0]=kreuzung[1]=kreuzung[2]=0;		//Nullsetzen aller Array-Werte

	ecrobot_set_light_sensor_active(NXT_PORT_S4);	//Lichtsensor initiallisieren

	int x=250, s=0, a=ecrobot_get_light_sensor(NXT_PORT_S4);//x ist die Strecke der abgetasteten Drehung
															//250 = eine Umdrehung
	while(s<x){
		richtiglenken(2, lenken_v);				//gedreht wird in 2-er Schritten
		s=s+2;									//zurueckgelegte Schritte werden in s gespeichert

		a=ecrobot_get_light_sensor(NXT_PORT_S4);
		if(a>500){
			if((s >= 0 && s<= x/16) || (s >= x-x/16)){	//Schwarte Linie am Anfang der Drehug oder am Ende
				kreuzung[2]=1;							//entspicht Weg geradeaus vorhanden
														//wird an der letzten Position im Array gespeichert
			}
			if(s >= x/4-x/16 && s<=x/4+x/16 ){			//Schwarz bei einem Viertel der Umdrehung = rechts
				kreuzung[1]=1;							//gespeichert an zweiter Stelle im Array
			}
			if(s >=3*x/4-x/16 && s<=3*x/4+x/16 ){		//nach 3/4 der Umdrehung = links
				kreuzung[0]=1;							//an erster Stelle
			}
		}
	}
	stop();
}

void folgen(int v, int lenken_v){
	/*
	 * Funktion zum Folgen der schwarzen Linie, aendern der Knoten-variable und
	 * erkennen eines Gegenstandes
	 * uebergeben werden die Fahrgeschwindigkeit (v) und die Lenkgeschwindigkeit (lenk_v)
	 */
	ecrobot_set_light_sensor_active(NXT_PORT_S4);		//Lichtsensor initiallisieren
	int a= ecrobot_get_light_sensor(NXT_PORT_S4);		//Lichtwert auslesen

	U8 touch=ecrobot_get_touch_sensor(NXT_PORT_S1);

	while(knoten==0){									//Solange man nicht am Knoten ist, wiederholen der Wegfolgenroutine

		while(a>500 && touch==0){						//fahren mit gesetzter Geschwindigkeit
			nxt_motor_set_speed(NXT_PORT_A, v, 1);		//bei gleichzeitigem Abtasten der Linie
			nxt_motor_set_speed(NXT_PORT_B, v+2, 1);
			a = ecrobot_get_light_sensor(NXT_PORT_S4);
			touch=ecrobot_get_touch_sensor(NXT_PORT_S1);//faehrt solange kein Gegenstand im Weg ist
		}
		stop();											//bei Verlust der Linie oder Gegenstand stoppen

		if(touch==1){									//Routine nach Erkennung eines Gegenstandes
			ecrobot_sound_tone(220, 1000, 20);			//Ausgabe eines vorgeschriebenen Tons
			systick_wait_ms(10000);						//warte 10s auf entnahme des Gegenstandes
			gegenstand++;								//Speicherung des Gegenstandes
			touch =0;
		}

		int b=0;										//Linie wird gesucht durch Auslenkung um 20 LE
		while(b<18 && a<500){							//nach rechts
			richtiglenken(2, lenken_v);
			a = ecrobot_get_light_sensor(NXT_PORT_S4);
			b=b+2;
			}
		b=0;
		while(b<46 && a<500){							//dann um 40LE nach links
			richtiglenken(-2, lenken_v+5);
			a = ecrobot_get_light_sensor(NXT_PORT_S4);
			b=b+2;
		}
		if (a<500){										//Wenn Linie nicht gefunden wird, dann muss es ein Knoten sein
			knoten=1;
		}
	}
	orientierung(lenken_v);				//bei erreichtem Knoten wird der Roboter gerade gestellt,
	fahren(30);							//ungefair zur Mitte des Knotens bewegt
	suchen(30);							//und die Suche nach Wegen am Knoten wird eingeleitet
}

void fahre_richtung(int r){
	/*
	 * Funktion zum Drehen in eine Richtung vom Mittelpunkt eines Knotens
	 * uebergeben wird die Richtung (r)
	 */
	switch(r){									//Entscheidung zwischen den moeglichen Richtungen
	case 0: richtiglenken(-250/2, 30);	break;  //0=Drehung nach links
	case 1: richtiglenken( 250/2, 30);	break;	//1=Drehung nach rechts
	case 2: break;								//2=keine Drehung, da Ausgangslage nach vorne zeigt
	case 3: richtiglenken( 250, 30);	break;	//3=Drehung nach hinten
	}

}

void anzeige(int x[]){
	/*
	 * Funktion übernimmt den globalen Array (kreuzung) und
	 * zeigt die Eigenschaft des Knotens am Display an
	 */

	int a=x[0]*100+x[1]*10+x[2];		//Array wird zu einem int umgeformt, damit man es im Switch benutzen kann

	switch(a) {
		case 0: ecrobot_status_monitor("Sackgasse"); break;					//case Szenarien zur Displayanzeige
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
	/*
	 * Hauptfunktion des Roboters
	 */
	int i=0;
	gegenstand=0;
	while (1) {

		knoten=0;
		folgen(50, 20);
		anzeige(kreuzung);
		fahre_richtung(i);
		i++;
		if (i==4){
			i=0;
		}
	}
}
