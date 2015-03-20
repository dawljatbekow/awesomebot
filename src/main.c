#include "../h/main.h"


int knoten;									//Zaehlen der Knoten
int kreuzung[3];							//Array mit {Links, Rechts, Geradeaus} kodierung
int lichtgrenze;


int Richtung;

typedef struct Kreuzung{				 //Datentyp indem Punkt und zugehöriger Kreuzungstype
	int Gabelung;						 //gespeichert werden können
	int x;
	int y;
}Kreuzung;

typedef struct Koordinaten{ 			 //Datentyp für Kreuzungen und Kanten
	int x; 							 //Kanten bekommen die Koordinaten zwischen den Kreuzungen
	int y;
}Koordinaten;

struct Kreuzung pa[147];                  //Speichert alle besuchten Knoten
struct Koordinaten Strecke[147];         //Aufzeichnung von Kreuzungen für den Rückweg
struct Koordinaten gesKnoten[49];        //Speicher alle nicht besuchte Punkte Punkte
int ks; 								 //Zähler f�r pa
int s;									 //Zähler für Strecke
int n;									 //Zähler für Token(Gegenstände)
int gK;
int AWege;								 //Zähler für gefundene unbenutzte Wege
int AgesPunkte;


void stop(){
	/*
	 * Funktion zum stoppen der Motoren
	 */
	nxt_motor_set_speed(NXT_PORT_C, 0, 1);
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
				lenken(NXT_PORT_C, 2, v);		//Beschleunigung beider Motoren
				lenken(NXT_PORT_B, -3, v+5);	//Korrektur am rechten Motor noetig
				i=i+2;
		}
	}
	else{
		while (i>=s){
				lenken(NXT_PORT_C, -2, v);		//wie oben, nur andere Richtung
				lenken(NXT_PORT_B, 3, v+5);
				i=i-2;
		}
	}
	stop();										//Ende des Lenkvorgangs
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



void kreuzungerkennen(int lenken_v){
	/*
	 * Funktion zum Drehen am Knoten, dabei werden die moeglichen Richtungen in Array (kreuzung) gespeichert
	 * uebergeben wird die Lenkgeschwindigkeit (lenken_v)
	 */
	kreuzung[0]=kreuzung[1]=kreuzung[2]=0;		//Nullsetzen aller Array-Werte

	ecrobot_set_light_sensor_active(NXT_PORT_S4);	//Lichtsensor initiallisieren

	int x=190, s=0, a=ecrobot_get_light_sensor(NXT_PORT_S4);//x ist die Strecke der abgetasteten Drehung
															//250 = eine Umdrehung
	while(s<x){
		richtiglenken(2, lenken_v);				//gedreht wird in 2-er Schritten
		s=s+2;									//zurueckgelegte Schritte werden in s gespeichert

		a=ecrobot_get_light_sensor(NXT_PORT_S4);
		if(a>lichtgrenze){
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
	anzeige(kreuzung);
}


/* nxtOSEK hook to be invoked from an ISR in category 2 */
void user_1ms_isr_type2(void) {
}

void ecrobot_device_initialize(void) {
}

void ecrobot_device_terminate(void) {
}


void kalibrierung(){
	ecrobot_set_light_sensor_active(NXT_PORT_S4);		//Lichtsensor initiallisieren
	int a=ecrobot_get_light_sensor(NXT_PORT_S4);
	richtiglenken(20,30);
	int b=ecrobot_get_light_sensor(NXT_PORT_S4);
	richtiglenken(-20,30);
	lichtgrenze=((a+2*b)/3);
}

void orientierung(int lenken_v){
	/*
	 * Roboter wird an der letzten Linie ausgerichtet
	 * bzw. ruzueckgedreht nach der letzten Pfadsuche
	 * uebergeben wird die Lenkgeschwindigkeit (leneken_v)
	 */
	int b=0;
	while(b<18){							//Gesucht wird um 20LE nach recht und 40LE nach links
		richtiglenken(2, lenken_v);			//Um zur Ursprungsfunktion zu gelangen, wird um 20LE
		b=b+2;								//wieder nach rechts gedreht
	}
}

void fahren(int v){
	/*
	 *Funktion zum Vorfahren zur ungefairen Mitte des Knotens
	 */
	int a=0;
	nxt_motor_set_count(NXT_PORT_B, 0);
	while(a<200){
		nxt_motor_set_speed(NXT_PORT_C, v, 1);		//Motor am Port_B faehrt langsamer als der am Port_A
		nxt_motor_set_speed(NXT_PORT_B, v, 1);		//dies wird hier mit v+3 korrigiert
		a=nxt_motor_get_count(NXT_PORT_B);
	}
	//systick_wait_ms(1000);						//nach 1 sekunde fahren ist man ungefair in der Mitte des Knoten
	stop();
}


int folgen(int v, int lenken_v){
	/*
	 * Funktion zum Folgen der schwarzen Linie, aendern der Knoten-variable und
	 * erkennen eines Gegenstandes
	 * uebergeben werden die Fahrgeschwindigkeit (v) und die Lenkgeschwindigkeit (lenk_v)
	 */
	int gegenstand=0;
	int a= ecrobot_get_light_sensor(NXT_PORT_S4);		//Lichtwert auslesen
	U8 touch1=0, touch2=0;

	while(knoten==0){									//Solange man nicht am Knoten ist, wiederholen der Wegfolgenroutine

		while(a>lichtgrenze && (touch1==0 && touch2==0)){						//fahren mit gesetzter Geschwindigkeit
			touch1=ecrobot_get_touch_sensor(NXT_PORT_S1);//faehrt solange kein Gegenstand im Weg ist
			touch2=ecrobot_get_touch_sensor(NXT_PORT_S2);
			nxt_motor_set_speed(NXT_PORT_C, v, 1);		//bei gleichzeitigem Abtasten der Linie
			nxt_motor_set_speed(NXT_PORT_B, v, 1);
			a = ecrobot_get_light_sensor(NXT_PORT_S4);

		}
		stop();											//bei Verlust der Linie oder Gegenstand stoppen

		if(touch1>0 || touch2>0){									//Routine nach Erkennung eines Gegenstandes
			ecrobot_sound_tone(220, 1000, 20);			//Ausgabe eines vorgeschriebenen Tons
			systick_wait_ms(10000);						//warte 10s auf entnahme des Gegenstandes
			gegenstand=1;								//Speicherung des Gegenstandes
			touch1 = touch2 = 0;
		}

		int b=0;										//Linie wird gesucht durch Auslenkung um 20 LE
		while(b<18 && a<=lichtgrenze){							//nach rechts
			richtiglenken(2, lenken_v);
			a = ecrobot_get_light_sensor(NXT_PORT_S4);
			b=b+2;
			}
		b=0;
		while(b<42 && a<=lichtgrenze){							//dann um 40LE nach links
			richtiglenken(-2, lenken_v);
			a = ecrobot_get_light_sensor(NXT_PORT_S4);
			b=b+2;
		}
		if (a<lichtgrenze){										//Wenn Linie nicht gefunden wird, dann muss es ein Knoten sein
			knoten=1;

		}
	}
	orientierung(lenken_v);				//bei erreichtem Knoten wird der Roboter gerade gestellt und
	fahren(35);							//ungefair zur Mitte des Knotens bewegt
	return gegenstand;
}

void fahre_richtung(int r){
	/*
	 * Funktion zum Drehen in eine Richtung vom Mittelpunkt eines Knotens
	 * uebergeben wird die Richtung (r)
	 */

	switch(r){									//Entscheidung zwischen den moeglichen Richtungen
	case 0:
		richtiglenken(-210/2, 30);
		Richtung=Richtung+3;
		break;  //0=Drehung nach links
	case 1:
		richtiglenken( 210/2, 30);
		Richtung=Richtung+1;
		break;	//1=Drehung nach rechts
	case 2:
		break;								//2=keine Drehung, da Ausgangslage nach vorne zeigt
	case 3:
		richtiglenken( 200, 30);
		Richtung=Richtung+2;
		break;	//3=Drehung nach hinten
	}
	if(Richtung>3){
		Richtung=Richtung-4;
	}
}



//Funktion zur Verarbeitung des Kreuzungstyps
int hasNorth(int value) {
	if(value==16 || value==144 || value==80 || value== 48 || value==176 || value==112 || value==208 || value==240) return(1);
	else return(0);
}

int hasSouth(int value){
	if(value==32 || value==48 || value==160 || value==96 || value==176 || value==112 || value==224 || value==240)return(1);
	else return(0);
}

int hasWest(int value){
	if(value==64 || value==80 || value==192 || value==96 || value==112 || value==208 || value==240 || value==224) return(1);
	else return(0);
}
int hasEast(int value){
	if(value==128|| value==144 || value==192 || value==160 || value==176 || value==224 || value==208 ||value==240)return(1);
	else return(0);
}

// Funktion zur Kontrolle, ob der Roboter schon mal auf einer der gefundenen Kanten gelaufen ist

int controle(int Richtungx, int Richtungy){
	//Funktion zur Überprüfung, ob ein benachtbarter Punkt breits besucht wurde.
	int i;
	for(i=0;i<ks;i++){
		if(pa[i].y==pa[ks].y+Richtungy && pa[i].x==pa[ks].x+Richtungx)return 1;
	}
return 0;
}
int schongesucht(int Richtungx, int Richtungy){
	int i=0;
	for(i=0;i<gK;i++){
		if(gesKnoten[i].x==pa[ks].x+Richtungx && gesKnoten[i].y==pa[ks].y+Richtungy)return 1;
	}
return 0;
}

int Robot_Move(int x, int y){
	knoten=0;
	if(y==1){
		switch(Richtung){
		case 0: break;
		case 1: fahre_richtung(1); break;
		case 2: fahre_richtung(3); break;
		case 3: fahre_richtung(0); break;
		}
	}
	if(y==-1){
		switch(Richtung){
		case 2: break;
		case 3: fahre_richtung(1); break;
		case 0: fahre_richtung(3); break;
		case 1: fahre_richtung(0); break;
		}
	}
	if(x==1){
		ecrobot_sound_tone(220, 1000, 20);
		switch(Richtung){
		case 1: break;
		case 0: fahre_richtung(1); break;
		case 3: fahre_richtung(3); break;
		case 2: fahre_richtung(0); break;
			}
	}
	if(x==-1){
		ecrobot_sound_tone(220, 1000, 20);
		switch(Richtung){
		case 3: break;
		case 2: fahre_richtung(1); break;
		case 1: fahre_richtung(3); break;
		case 0: fahre_richtung(0); break;
		}
	}
	int a=folgen(50,20);
	return a;
}

void gehen(int Richtungx, int Richtungy){
	/*Funktion für das Gehen auf neuen Wegen, wobei nur x oder y um Eins erhöht oder um Eins erniedrigt
	  wird, da es keine Diagonalen gibt.*/
	// Richtungx (Richtungy) meint dabei den Schritt, der in x-Richtung (y-Richtung) gegeangen wird.*/
	int a=Robot_Move(Richtungx,Richtungy);
	if(a==1)n++;
	/*Schritt wird ausgeführt und überprüft, ob Token gefunden wird. Wird ein Token gefunden, wird
	  die Anzahl der gefundenen Token um Eins erhöht. */							//ein Weg wurde benutzt.
	s++; 								//Zähler Strecke erhöhen
	ks++;
	pa[ks].y=pa[ks-1].y+Richtungy; 				//neuer Standort
	pa[ks].x=pa[ks-1].x+Richtungx;
	Strecke[s].x=pa[ks].x; 					//Speicherung des aktuellen Standorts für den Rückweg
	Strecke[s].y=pa[ks].y;
	if (schongesucht(0,0)==1 && controle(0,0)==0){
		gesKnoten[gK].x=0;
		gesKnoten[gK].y=0;
		AgesPunkte--;
	}
}

int Robot_GetIntersections(void){
	int temp[4];
	int a =2-Richtung;
	if(a<0)
		a=a+4;

	int b =1-Richtung;
	if(b<0)
		b=b+4;
	int c =3-Richtung;
	if(c<0)
		c=c+4;
	int d =0-Richtung;

	kreuzungerkennen(30);

	temp[c]=kreuzung[0];
	temp[d]=kreuzung[2];
	temp[b]=kreuzung[1];
	temp[a]=1;

	return temp[0]*16+temp[2]*32+temp[3]*64+temp[1]*128;
	//return 224;
	}

void suchen(){
	/* Die Funktion suchen beschäftigt sich mit der Suche nach Gegenständen im Labyrinth.
	   Werden dabei keine unbesuchten Kanten gefunden, geht der Roboter einen Schritt zurück. */
	n=0; 									//Gefundene Gegenstände vor dem Suchlauf auf Null setzen
	AgesPunkte=0;
	s=0;									// Startwert besuchter Kreuzungen
	gK=0;
	while(n!=3){
		/*Sobald drei Gegenstände gefunden wurden, soll der Roboter mit dem Suchen im Labyrinth aufhören.*/
		pa[ks].Gabelung= Robot_GetIntersections();     //Bestimmung Kreuzungstype (Jeder Krezungstype hat eine Nummer)
		//Überprüfung, die Kante bereits als möglicher Weg hinzugezählt wurde.
		if(controle(0,0)!=1){
			if(hasNorth(pa[ks].Gabelung)!=0 && controle(0,1)==0 && schongesucht(0,1)==0){//Norden
				gesKnoten[gK].y=pa[ks].y+1;
				gesKnoten[gK].x=pa[ks].x;
				gK++;
				AgesPunkte++;
			}
			if(hasSouth(pa[ks].Gabelung)!=0 && controle(0,-1)==0 && schongesucht(0,-1)==0){
				gesKnoten[gK].y=pa[ks].y-1;
				gesKnoten[gK].x=pa[ks].x;//Süden
				gK++;
				AgesPunkte++;
			}
			if(hasEast(pa[ks].Gabelung)!=0 && controle(1,0)==0 && schongesucht(1,0)==0){
				gesKnoten[gK].y=pa[ks].y;
				gesKnoten[gK].x=pa[ks].x+1;//Osten
				gK++;
				AgesPunkte++;
			}
			if(hasWest(pa[ks].Gabelung)!=0 && controle(-1,0)==0 && schongesucht(-1,0)==0){
				gesKnoten[gK].y=pa[ks].y;
				gesKnoten[gK].x=pa[ks].x-1;//Westen
				gK++;
				AgesPunkte++;
			}
		}
		if(AgesPunkte==0){
			/*Wenn keine neuen/unbefahrenen Wege vorhanden sind (gesamtes Labyrinth durchsucht und weniger als
			  3 Gegenstände gefunden wurden, soll die Suche abgebrochen werden. Ist er bereits am Startpunkt
			  ist er fertig. */
			s=0;
			break;}
		else{
			//Kontrolle, ob Richtung vorhanden ist und ob die gefundene Richtung bereits besucht wurde:
			if(hasNorth(pa[ks].Gabelung)!=0 && controle(0,1)==0) gehen(0,1); 			//Norden
			else if(hasSouth(pa[ks].Gabelung)!=0 && controle(0,-1)==0) gehen(0,-1); 	//Süden
			else if(hasEast(pa[ks].Gabelung)!=0 && controle(1,0)==0) gehen(1,0); 		//Osten
			else if(hasWest(pa[ks].Gabelung)!=0 && controle(-1,0)==0) gehen(-1,0); 	//Westen
			else{
			/*Es wird keine neue Richtung gefunden --> dann geht der Roboter einen Schritt zurück und
			  Position des Roboters wird aktualliesiert.*/
				Robot_Move(Strecke[s-1].x-Strecke[s].x,Strecke[s-1].y-Strecke[s].y);
				ks++;
				pa[ks].x=Strecke[s-1].x;
				pa[ks].y=Strecke[s-1].y;
				Strecke[s].x=Strecke[s-1].x; 				//Löschen der doppelten Kreuzung
				Strecke[s].y=Strecke[s-1].y;
				s--;
				if(pa[ks].x==0 && pa[ks].y==0){
					s=0;
					break;}
			}
		}
	}
}


void rueckkehr(void){
	/* Die Funktion rueckkehr gibt dem Roboter die Möglichkeit von seiner momentanen Position aus
	   zu seinem Startpunkt zurück zu finden. Dabei nimmt er den in Strecke gespeicherten Weg und zieht
	   benachbarte Punkte mit in seine Betrachtung ein, um eventuelle Abkürzungen zu finden. */
	int i;
	while(s!=0){
		/*Solange er nicht wieder am Start ist (Strecke[s=0] ist dabei der Startpunkt), läuft der Roboter
		rückwärts und schaut nach Abkürzungen durch bekannte Punkte in der direkten Umgebung*/
		for(i=0;i<s;i++){
			if (Strecke[s].x+1==Strecke[i].x && Strecke[s].y==Strecke[i].y && (hasEast(Robot_GetIntersections())==1)) {
				//Kontrolle ob ein Punkt in östlicher Richtung schon befahren wurde und ob eine Verbindung zu ihm existiert.
				Robot_Move(Strecke[i].x,Strecke[i].y);
				s=i;
				break;}
			else if (Strecke[s].x-1==Strecke[i].x && Strecke[s].y==Strecke[i].y && (hasWest(Robot_GetIntersections())==1)) {
				//Analog für Westen
				Robot_Move(Strecke[i].x,Strecke[i].y);
				s=i;
				break;}
			else if (Strecke[s].y+1==Strecke[i].y && Strecke[s].x==Strecke[i].x && (hasNorth(Robot_GetIntersections())==1)) {
				//Analog für Norden
				Robot_Move(Strecke[i].x,Strecke[i].y);
				s=i;
				break;}
			else if (Strecke[s].y-1==Strecke[i].y && Strecke[s].x==Strecke[i].x  && (hasSouth(Robot_GetIntersections())==1)) {
				//Analog für Süden
				Robot_Move(Strecke[i].x,Strecke[i].y);
				s=i;
				break;}
		}
	}
}




TASK(OSEK_Main_Task) {
	/*
	 * Hauptfunktion des Roboters
	 */
	lichtgrenze=Richtung=0;
	systick_wait_ms(2000);
	kalibrierung();
	/*int i=0;
	while (1) {
		knoten=0;
		folgen(50, 20);
		kreuzungerkennen(30);
		anzeige(kreuzung);
		fahre_richtung(3);
		//i++;
		if (i==4){
			i=0;
		}
	}*/
	Richtung=0;
	ks=0;
	pa[ks].x=0;																//Startposition Roboter
	pa[ks].y=0;
	Strecke[0].x=pa[ks].x;													//Startpostion in Strecke eintragen
	Strecke[0].y=pa[ks].y;
	Robot_Move(0,1);
	suchen();															//Token-Suche aufrufen
	rueckkehr();														//Rückkehr zum Start aufrufen

}
