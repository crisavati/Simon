/* GAME of Simon Says. 
 * Created by: Cris Avati. 
 * Finished on: 02/07/2021 (dd/mm/yyyy)
 * Components necessary: N leds and buttons, 1 buzzer or speaker,
 * N resistors for the leds and N pull-down resistors for the buttons
 */

const int N = 4;   // how many leds and buttons

int leds[] = {3, 4, 5, 6};       // led pins
int buttons[] = {7, 8, 9, 10};   // button pins
boolean buttonState[N];          
// led index and colors: 0 - red, 1 - green, 2 - blue, 3 - yellow 

int buzzer = 11;                 // buzzer pin
int notes[] = {262, 294, 330, 349};

int randNumber;
String simonSays = "";   // sequence of random numbers
int simonLength;
String userSays = "";    // sequence inserted by the user

int playTime = 500;      // how long the beat (led flash + sound) lasts
String gameState = "start";
int score = 0;
int highScore = 0;
int beatCounter = 0;     // used to keep track of how many beats the user has inputted
boolean beatAdded = false;

// used to prevent from printing multiple times in the loop
bool printFlag = false;   

void setup() {
  // set all the leds to output and all the buttons to input
  for(int i = 0; i < N; i++)
  {
    pinMode(leds[i], OUTPUT);
    pinMode(buttons[i], INPUT);
  }
  pinMode(buzzer, OUTPUT);
  // to have random sequences that differ
  randomSeed(analogRead(0));    // for this to work, pin 0 HAS to be unconnected 
  Serial.begin(9600);
}

void loop() {
  if (gameState == "start") {
    if(!printFlag) 
    {
      Serial.println("Press any button to start the game");
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
      Serial.println("Sorry, you lost. Your score was: " + String(score));
      Serial.println("The high score is: " + String(highScore));

      if (score == highScore)
      {
        Serial.println("Congrats! You set a new high score!");
      }
      
      Serial.println("Press any button to play again");
      Serial.println();
      printFlag = true;
    }
    
    show_score();
    play_again();
    
    
  }
  beatAdded = false;
  delay(1);  // time between loops
}


// check if a button was pressed and play corresponding beat
void listen_beats() {
  // iterate trough all the leds
  for(int i = 0; i < N; i++)
  {
    // read the state of button i
    buttonState[i] = digitalRead(buttons[i]); 
    // if button i is pressed turn led i on for 1s  
    if (buttonState[i] == HIGH)
    {
      play_beat(i); 
      beatCounter ++;
      // add the beat chosen by the user to userSays
      userSays += String(i);
      // NOTE:  i: number of the button, buttonState[i]: 0 or 1 for high or low
    }
  }
}


// turn led n on, play note n for 1s and then stop
void play_beat(int n) {
  digitalWrite(leds[n], HIGH);
  tone(buzzer, notes[n]);
  
  delay(playTime);
  
  digitalWrite(leds[n], LOW);
  noTone(buzzer);
}


// iterate through a sequence of length l and plays each beat
void play_sequence(String sequence, int l) {
  for(int i = 0; i < l; i++) {
    play_beat(int(sequence[i] - '0'));  // to go from ASCII to int subtract the ASCII of '0'
    delay(playTime);
  }
}


// computer turn, play the sequence with a new beat
void game_watch()
{
  beatCounter = 0;
  userSays = "";

  if (beatAdded == false)
  {
    // add a new beat and update simonLength
    randNumber = random(0, N);
    simonSays = simonSays + String(randNumber);
    simonLength = simonSays.length();

    beatAdded = true;
    play_sequence(simonSays, simonLength);

    // beat added, sequence played, we can go to the user's turn
    gameState = "play";
  }
}


// user turn, record the buttons pressed and play each beat
void game_play()
{
  // ALL the beats need to be inputted to continue
  if (beatCounter == simonLength)
  {
    // turn won
    if (simonSays == userSays) 
    {
      Serial.println("Next round!");
      score++;
      delay(1.5*playTime); // wait before going to watch/showing new sequence
      gameState = "watch";
      Serial.println("Your turn!");
    }
    // turn lost
    else
    {
      gameState = "end";
    }
  }
  // the user has to input other beats, so keep listening
  else 
  {
    listen_beats();
  }
}


// if user presses any button, the game starts again
bool play_again() {
  // button has to be pressed for longer to be registered because of this line
  leds_flashing(); 
    
  // check buttons
  for(int i = 0; i < N; i++)
  {
    // read the state of button i
    buttonState[i] = digitalRead(buttons[i]); 
    // if button i is pressed, go to "watch" 
    if (buttonState[i] == HIGH) {
      delay(500); // wait before showing new sequence
      gameState = "watch";
      printFlag = false;
    }
  }

  // reset variables
  score = 0;
  beatCounter = 0;
  simonSays = "";
  userSays = "";
  
}


// leds animation used in waiting times
void leds_flashing()
{
  // leds flash forwards: 0-1-2
  for (int i = 0; i < 3; i++)
  {
    digitalWrite(leds[i], HIGH);
    delay(200);
    digitalWrite(leds[i], LOW);
  }

  // leds flash backwards: 3-2-1, note that 0 and 3 are not repeated
  for (int i = 3; i > 0; i--)
  {
    digitalWrite(leds[i], HIGH);
    delay(200);
    digitalWrite(leds[i], LOW);
  }
}

// flash led 1 as many times as the points scored
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



/*
 DONE:
 - connect other 4 buttons and pull-down resistors
 - set up code so that every led turns on for 1s if button is pressed
 - set up buzzer and find sounds
 
 TO DO:
X - generate random number 0 to 3   
X - play sequence of random numbers
X - record user inputted sequence
X - confront user sequence to computer sequence 
X - code "play" state
X- code a "start" state so that the game starts only after any of the 
X  buttons is pressed (instead of starting in "watch" state)
X- extend "end" state so that it asks to press a button to play again
X- code a starting sequence where leds flash while waiting for new game to start
X- store high score
X- find a way to prevent "User says" and "Simon says" to be printed a bunch 
X  of times
X - use leds to communicate score
  
 */
