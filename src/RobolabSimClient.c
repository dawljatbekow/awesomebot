 #include "../h/Configuration.h"


typedef struct Kreuzung{				 //Datentyp indem Punkt und zugehöriger Kreuzungstype
	int Gabelung;						 //gespeichert werden können
	int x;
	int y;
}Kreuzung;

typedef struct Koordinaten{ 			 //Datentyp für Kreuzungen und Kanten
	float x; 							 //Kanten bekommen die Koordinaten zwischen den Kreuzungen
	float y;							 //zB.: (0.5;0)
}Koordinaten;

struct Kreuzung pa[147];                  //Speichert alle besuchten Knoten
struct Koordinaten Strecke[147];         //Aufzeichnung von Kreuzungen für den Rückweg
struct Koordinaten snake[147];           //Aufzeichnung von Kanten
struct Koordinaten gesKnoten[49];        //Speicher alle nicht besuchte Punkte Punkte
int ks; 								 //Zähler f�r pa
int s;									 //Zähler für Strecke
int t;									 //Zähler für Snake
int n;									 //Zähler für Token(Gegenstände)
int gK;
int AWege;								 //Zähler für gefundene unbenutzte Wege
int AgesPunkte;


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
int Kontrolle(float Richtungx,float Richtungy){
	int i;								//Zählvariable für Kontrollschleife
	for(i=0;i<=t;i++){					//Durchsuchen aller Einträge von Snake nach bereits gelaufenen Kanten
		if(snake[i].y==pa[ks].y+Richtungy/2.0 && snake[i].x==pa[ks].x+Richtungx/2.0) return(0);					//Kante schon besucht: Rückgabewert=
	}
return 1;								//Kante noch nicht besucht: Rückgabewert=1
}

int controle(int Richtungx, int Richtungy){
	//Funktion zur Überprüfung, ob ein benachtbarter Punkt breits besucht wurde.
	int i;
	for(i=0;i<ks;i++){
		if(pa[i].y==pa[ks].y+Richtungy && pa[i].x==pa[ks].x+Richtungx)return 1;
	}
return 0;
}
int schongesucht(Richtungx, Richtungy){
	int i=0;
	for(i=0;i<gK;i++){
		if(gesKnoten[i].x==pa[ks].x+Richtungx && gesKnoten[i].y==pa[ks].y+Richtungy)return 1;
	}
return 0;
}

void gehen(float Richtungx, float Richtungy){
	/*Funktion für das Gehen auf neuen Wegen, wobei nur x oder y um Eins erhöht oder um Eins erniedrigt
	  wird, da es keine Diagonalen gibt.*/
	// Richtungx (Richtungy) meint dabei den Schritt, der in x-Richtung (y-Richtung) gegeangen wird.*/
	if(Robot_Move(pa[ks].x+Richtungx,pa[ks].y+Richtungy)==0x02)n++;
	/*Schritt wird ausgeführt und überprüft, ob Token gefunden wird. Wird ein Token gefunden, wird
	  die Anzahl der gefundenen Token um Eins erhöht. */							//ein Weg wurde benutzt.
	s++; 								//Zähler Strecke erhöhen
	t++; 								//Zähler Snake erhöhen
	ks++;
	snake[t].y=pa[ks-1].y+Richtungy/2.0; 		//Speicherung der gegangenen Kanten
	snake[t].x=pa[ks-1].x+Richtungx/2.0;
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


void suchen(){
	/* Die Funktion suchen beschäftigt sich mit der Suche nach Gegenständen im Labyrinth.
	   Werden dabei keine unbesuchten Kanten gefunden, geht der Roboter einen Schritt zurück. */
	n=0; 									//Gefundene Gegenstände vor dem Suchlauf auf Null setzen
	AgesPunkte=0;
	s=0;									// Startwert besuchter Kreuzungen
	t=-1;									// Startwert für die Anzahl unterschiedlicher Kanten
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
			if(hasNorth(pa[ks].Gabelung)!=0 && Kontrolle(0,1)==1) gehen(0,1); 			//Norden
			else if(hasSouth(pa[ks].Gabelung)!=0 && Kontrolle(0,-1)==1) gehen(0,-1); 	//Süden
			else if(hasEast(pa[ks].Gabelung)!=0 && Kontrolle(1,0)==1) gehen(1,0); 		//Osten
			else if(hasWest(pa[ks].Gabelung)!=0 && Kontrolle(-1,0)==1) gehen(-1,0); 	//Westen
			else{
			/*Es wird keine neue Richtung gefunden --> dann geht der Roboter einen Schritt zurück und
			  Position des Roboters wird aktualliesiert.*/
				Robot_Move(Strecke[s-1].x,Strecke[s-1].y);
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

int main(void){
	//Hauptfunktion
	ks=0;
	pa[ks].x=0;																//Startposition Roboter
	pa[ks].y=0;
	Strecke[0].x=pa[ks].x;													//Startpostion in Strecke eintragen
	Strecke[0].y=pa[ks].y;
	snake[0].x=pa[ks].x;													//Startposition in Snake eintragen
	snake[0].y=pa[ks].y;															//Am Anfang noch keine gefundenen unbenutzen Wege
	//Startkante (Startrichtung) festlegen und Startkante in Snake entsprechend ändern.
	if (hasNorth(Robot_GetIntersections())==1){ snake[0].y=0.5;}
	else if (hasSouth(Robot_GetIntersections())==1){ snake[0].y=-0.5;}
	else if (hasEast(Robot_GetIntersections())==1){ snake[0].x=0.5;}
	else if (hasWest(Robot_GetIntersections())==1){ snake[0].y=-0.5;}
	suchen();															//Token-Suche aufrufen
	rueckkehr();														//Rückkehr zum Start aufrufen

	return EXIT_SUCCESS;

/*
 * void Get_Intersection(void){
	int temp[4];
	int a =0-Richtung;
	if(a<0)a+3;
	int b =1-Richtung;
	if(a<0)b+3;
	int c =2-Richtung;
	if(c<0)c+3;
	int d =3-Richtung
	temp[b]=Kreuzung[2];
	temp[c]=Kreuzung[1];
	temp[d]=Kreuzung[3];
	temp[a]=Kreuzung[4];
	return (temp[1]<<4 | temp[3]<<3 | temp[2]<<2 | temp[0]<<1)
	}
 */
}



