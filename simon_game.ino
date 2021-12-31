/* GIOCO 'Simon dice' (vedi Hasbro - Simon elettronico). 
 * Creato da: Cristiana Avati. 
 * Finito il: 02/07/2021 
 * E' possibile simulare il progetto al link: https://www.tinkercad.com/things/2WlhyxVTx2s
 *
 * Componenti necessari: N LED e pulsanti, 1 buzzer o speaker,
 * N resistori 220 ohm per i LED e N resistori 20k ohm di pull-down per i pulsanti.
 */


const int N = 4;   // numero di LED e pulsanti

int leds[] = {3, 4, 5, 6};       // pin dei LED
int pulsanti[] = {7, 8, 9, 10};   // pin dei pulsanti
boolean pulsStato[N];          
// indici dei LED e colori: 0 - rosso, 1 - verde, 2 - blue, 3 - giallo

int buzzer = 11;                 // pin del buzzer
int note[] = {262, 294, 330, 349};

int randNumber;
String simonSays = "";   // sequenza di numeri generati a random
int simonLength;
String userSays = "";    // sequenza inserita dall'utente

int playTime = 500;      // tempo di durata del beat (flash del LED + suono)
String gameState = "start";
int score = 0;
int highScore = 0;        
int beatCounter = 0;     // tiene traccia di quanti beat l'utente ha dato come input
boolean beatAggiunto = false;

// previene che la stessa cosa venga stampata a schermo più volte all'interno del loop
bool printFlag = false;   


void setup() {
  // imposta ogni LED come output e ogni pulsante come input
  for(int i = 0; i < N; i++)
  {
    pinMode(leds[i], OUTPUT);
    pinMode(pulsanti[i], INPUT);
  }
  pinMode(buzzer, OUTPUT);
  // per aver sequenze random che cambino da gioco a gioco
  randomSeed(analogRead(0));    //NOTA: perché questo funzioni il pin 0 DEVE essere inutilizzato
  Serial.begin(9600);
}


void loop() {
  if (gameState == "start") {
    if(!printFlag) 
    {
      Serial.println("Premi qualsiasi pulsante per cominciare il gioco");
      Serial.println();
      printFlag = true;
    }   
    score = 0;
    play_again();
  }
  
  if (gameState == "watch"){
    game_watch();
  }

  if (gameState == "play") {
    game_play();
  }

  if (gameState == "end") {
    if (score > highScore)
    {
      highScore = score;
    }
    
    if (!printFlag) {
      Serial.println();
      Serial.println("Peccato, hai perso. Il tuo punteggio e' stato: " + String(score));
      Serial.println("Il punteggio piu' alto è: " + String(highScore));

      if (score == highScore)
      {
        Serial.println("Congratulazioni! Hai raggiunto il punteggio piu' alto!");
      }
      
      Serial.println("Premi qualsiasi pulsante per giocare di nuovo");
      Serial.println();
      printFlag = true;
    }
    
    show_score();
    play_again();
    
    
  }
  beatAggiunto = false;
  delay(1);  // tempo tra i loop
}


// controlla se un pulsanto è stato premuto e aziona il beat corrispondente
void listen_beats() {
  // itera per tutti i LED
  for(int i = 0; i < N; i++)
  {
    // controlla lo stato del pulsante i
    pulsStato[i] = digitalRead(pulsanti[i]);  
    // se il pulsante i è stato premuto, accendi LED i per 1s
    if (pulsStato[i] == HIGH)
    {
      play_beat(i); 
      beatCounter ++;
      // aggiungi il beat scelto dall'utente a userSays
      userSays += String(i);
      // NOTA:  i: numero del pulsante, pulsSato[i]: 0 o 1 per high o low
    }
  }
}


// accendi il LED n, suona la nota n per 1s e poi fermati
void play_beat(int n) {
  digitalWrite(leds[n], HIGH);
  tone(buzzer, note[n]);
  
  delay(playTime);
  
  digitalWrite(leds[n], LOW);
  noTone(buzzer);
}


// itera per una sequenza di lunghezza l e aziona ogni beat
void play_sequence(String sequence, int l) {
  for(int i = 0; i < l; i++) {
    play_beat(int(sequence[i] - '0'));    // per andare da ASCII ad int bisogna sottrarre l'ASCII di '0'
    delay(playTime);
  }
}


// turno del computer, aziona una sequenza con un nuovo beat
void game_watch()
{
  beatCounter = 0;
  userSays = "";

  if (beatAggiunto == false)
  {
    // aggiungi un nuovo beat e aggiorna simonLength
    randNumber = random(0, N);
    simonSays = simonSays + String(randNumber);
    simonLength = simonSays.length();

    beatAggiunto = true;
    play_sequence(simonSays, simonLength);

    // il beat è stato aggiunto, la sequenza è stata azionata, è il turno dell'utente
    gameState = "play";
  }
}


// turno dell'utente, registra i pulsanti premuti e suona ogni beat
void game_play()
{
  // TUTTI i beat devono essere inseriti prima di poter procedere
  if (beatCounter == simonLength)
  {
    // turno vinto
    if (simonSays == userSays) 
    {
      Serial.println("Prossimo giro!");
      score++;
      delay(1.5*playTime);  // aspetta prima di andare a "watch"/mostrare una nuova sequenza
      gameState = "watch";
      Serial.println("E' il tuo turno!");
    }
    // turno perso
    else
    {
      gameState = "end";
    }
  }
  // l'utente deve ancora inserire altri beat, quindi continua ad ascoltare
  else 
  {
    listen_beats();
  }
}


// se l'utente preme qualsiasi pulsante, il gioco ricomcia
bool play_again() {
  // animazione dei LED nell'attesa. NOTA: per via di questa linea, il pulsante deve essere premuto leggermente 
  // più a lungo per essere registrato
  leds_flashing(); 
    
  // controlla i pulsanti
  for(int i = 0; i < N; i++)
  {
    // legge lo stato del pulsante i
    pulsStato[i] = digitalRead(pulsanti[i]); 
    // se il pulsante i è stato premuto, vai a "watch"
    if (pulsStato[i] == HIGH) {
      delay(500); // aspetta prima di mostrare nuova sequenza
      gameState = "watch";
      printFlag = false;
    }
  }

  // resetta le variabili
  score = 0;
  beatCounter = 0;
  simonSays = "";
  userSays = "";
  
}


// animazioni dei LED usata nei tempi di attesa
void leds_flashing()
{
  // leds flash forwards: 0-1-2
  // i LED si illuminano in avanti: 0-1-2
  for (int i = 0; i < 3; i++)
  {
    digitalWrite(leds[i], HIGH);
    delay(200);
    digitalWrite(leds[i], LOW);
  }

  // i LED si illuminano all'indietro: 3-2-1, nota che 0 e 3 non vengono ripetuti
  for (int i = 3; i > 0; i--)
  {
    digitalWrite(leds[i], HIGH);
    delay(200);
    digitalWrite(leds[i], LOW);
  }
}

// accendi il LED 1 (verde) tante volte quanti i sono i punti accumulati
void show_score()
{
  for(int i = 0; i < score; i++)
  {
    digitalWrite(leds[1], HIGH);
    delay(250);
    digitalWrite(leds[1], LOW);
    delay(250);
  }
}
