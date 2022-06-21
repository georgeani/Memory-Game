#include <Wire.h>
#include <Adafruit_RGBLCDShield.h>
#include <utility/Adafruit_MCP23017.h>
#include <EEPROM.h>

//used to initialize the display and the buttons
Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();

//the different states the machine goes through
unsigned int state = 0;
#define CentralMenu 0
#define CustomDifficulty 1
#define Practice 2
#define TimeLimit 3
#define PresetDifficulty 4
#define Story 5
#define HighScore 6
#define PresetHighScore 7
#define SetHighScore 8

/*
  This software does not utilize the delay method at all, instead it utilizes the use of millis()
  as a way to measure the delay.
  The code for this implementation of the delay was taken from here:https://www.norwegiancreations.com/2017/09/arduino-tutorial-using-millis-instead-of-delay/
*/

/*
   For use in the input polling, timePassed indicates the minimum amount of time passed between
   each press in order for the press to be registered
   trigger is a boolean that indicates that the input has been registered
   firstPress holds the time since the previous press
*/
unsigned volatile long timeNow = 0;
unsigned long firstPress = 0;
unsigned long timePassed = 50;
boolean trigger = false;
/*
   for use in the input limitation,
  timeLimit is the limit, used to get to the time setting stage,
  timeLimitOn activating the time limit,
  reference used to enter into time limited input, it keeps the millis before the mode starts
  it is used in the practice mode, or the standard game, and in the story mode
  inp is used to detect if the user has cancelled his game
*/
long volatile timeLimit = 1000;
bool volatile timeLimitOn = false;
unsigned volatile long reference = 0;
volatile bool inp;

/*
  For use in the storyMode, score is for the score that each user scores
  when completing a round
  the gameS is used to indicate that the storyMode is activated
*/
int score = 0;
volatile bool gameS = false;

//integer variable used to navigate the start menu
unsigned int menu1 = 0;

/*
  the delayT is used as a standard time period for the output
*/
long delayT = 1000;

/*
   user input and system input arrays that are used to check whether the inputs are correct
  systemInput is used to get the inputs from the system and the user and put them in the arrays
  s is used to increment through the arrays
  exitPar is used to indicate that the game is cancelled by the user
*/
long userIn[14];
long systemIn[14];
int systemInput = 0;
int s = 0;
bool exitPar = false;

/*
   used to create custom level difficulty,
  n is for the amount of symbols that will be used and len is the length of the sequence created
  dif is to enter the setting custom difficulty state and acN is used to change from the length of the sequence to the amount of symbols used
*/
int n = 2;
int len = 4;
bool acN = true;
unsigned int period2;

/*
  Selectable level difficulty, used to check if a preset difficulty level is used
  setDifficultyUsed to enter in the difficulty presets
  selector is used to navigate through each difficulty
*/
bool selectableDifficultyOn = false;
unsigned int selector = 0;

/*
   the custom data type to keep and present
  the highscore as well as with the three leters
  of the player for the highscore
  it was taken fromthe lecture slides.
*/
struct HS {
  char username1;
  char username2;
  char username3;
  int hs;
};

/*
  hold the initial array for the score
  as taken from the EEPROM
  then it is the updated one
  it also allows for the input of the initials
  It also utilises a custom struct to make it work
*/

HS h[10];
HS f[10];
unsigned int nav;
bool hsOn = false;
int letter = 0;
char nam[3];
int namIndex = 0;

//used for the input of the initials
char alphabet[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G',
                   'H', 'I', 'J', 'K', 'L', 'M', 'N',
                   'O', 'P', 'Q', 'R', 'S', 'T', 'U',
                   'V', 'W', 'X', 'Y', 'Z'
                  };

//custom chars to be used in the practice and story mode
//up triangle right
byte char1[] = {B00000, B00000, B00000, B00001, B00011, B00111, B01111, B11111};
//up triangle left
byte char2[] = {B00000, B00000, B00000, B10000, B11000, B11100, B11110, B11111};
//down triangle left
byte char3[] = {B11111, B01111, B00111, B00011, B00001, B00000, B00000, B00000};
//down triangle right
byte char4[] = {B11111, B11110, B11100, B11000, B10000, B00000, B00000, B00000};

//the colors used for the interfaces in the menus and in the game
#define RED 0x1
#define YELLOW 0x3
#define GREEN 0x2
#define TEAL 0x6
#define BLUE 0x4
#define VIOLET 0x5
#define WHITE 0x7


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  lcd.begin(16, 2);

  //the up completely custom char will be from 11 to 14
  lcd.createChar(1, char1);
  lcd.createChar(2, char2);
  lcd.createChar(3, char3);
  lcd.createChar(4, char4);

  randomSeed(analogRead(0));

}

//used as a setter for the input
uint8_t i = 0;

/*
  in the main loop the program switches between the main menu, the story mode and
  the practice mode and the different menus regarding the settings, the menus have their one methods where they are housed in
  order for the system to be more efficient
  it is also where the highscore is inputted
*/
void loop() {
  switch (state) {
    case CentralMenu:
      menu();
      decide();
      break;
    case CustomDifficulty:
      setDifficulty();
      decide();
      selectableDifficultyOn = false;
      break;
    case Practice:
      practice();
      break;
    case TimeLimit:
      setTimeLimit();
      decide();
      selectableDifficultyOn = false;
      break;
    case PresetDifficulty:
      selectableDifficulty();
      decide();
      break;
    case Story:
      if (selectableDifficultyOn) {
        storyMode();
      }
      break;
    case HighScore:
      printAll();
      decide();
      break;
    case PresetHighScore:
      createHighScore();
      state = 0;
      break;
    case SetHighScore:
      decide();
      enterHighScore();
      break;
  }

}


/*
  the is the starting menu method
  it contains the format of the screen for each setting and mode the game does
  it uses an int to go around the menu that is used to enter into different menus and in the story mode as well as in the practice mode
*/
void menu() {
  if (menu1 == 0) {
    lcd.setCursor(0, 0);
    lcd.setBacklight(BLUE);
    lcd.print("Use up and down");
    lcd.setCursor(3, 1);
    lcd.print("to navigate");
    state = 0;
  }
  else if (menu1 == 1) {
    lcd.setCursor(0, 0);
    lcd.setBacklight(RED);
    lcd.print("Set Custom Value");
    lcd.setCursor(2, 1);
    lcd.print("Press Select");
  }
  else if (menu1 == 2) {
    lcd.setCursor(2, 0);
    lcd.setBacklight(VIOLET);
    lcd.print("Practice Mode");
    lcd.setCursor(2, 1);
    lcd.print("Press Select");
  }
  else if (menu1 == 3) {
    lcd.setCursor(1, 0);
    lcd.setBacklight(TEAL);
    lcd.print("Set time limit");
    lcd.setCursor(2, 1);
    lcd.print("Press Select");
  }
  else if (menu1 == 4) {
    lcd.setCursor(0, 0);
    lcd.setBacklight(RED);
    lcd.print("Difficulty Level");
    lcd.setCursor(2, 1);
    lcd.print("Press Select");
  }
  else if (menu1 == 5) {
    lcd.setCursor(3, 0);
    lcd.setBacklight(GREEN);
    lcd.print("Story Mode");
    lcd.setCursor(2, 1);
    lcd.print("Press Select");
  }
  else if (menu1 == 6) {
    lcd.setCursor(3, 0);
    lcd.setBacklight(WHITE);
    lcd.print("High Score");
    lcd.setCursor(2, 1);
    lcd.print("Press Select");
  }
  else if (menu1 == 7) {
    lcd.setCursor(0, 0);
    lcd.setBacklight(GREEN);
    lcd.print("Create HighScore");
    lcd.setCursor(2, 1);
    lcd.print("Press Select");
  }
}

/*
  used to combine the 4 existing symbols into one for each direction
  of the arrows
*/
int displayer(int pattern) {
  //up symbol
  if (pattern == 0) {
    lcd.setCursor(7, 0);
    lcd.write(1);
    lcd.setCursor(8, 0);
    lcd.write(2);
  }//down symbol
  else if (pattern == 1) {
    lcd.setCursor(7, 1);
    lcd.write(3);
    lcd.setCursor(8, 1);
    lcd.write(4);
  }
  //right symbol
  else if (pattern == 2) {
    lcd.setCursor(7, 0);
    lcd.write(2);
    lcd.setCursor(7, 1);
    lcd.write(4);
  }
  //left symbol
  else if (pattern == 3) {
    lcd.setCursor(7, 0);
    lcd.write(1);
    lcd.setCursor(7, 1);
    lcd.write(3);
  } else {
    pattern = -1;
  }
  return pattern;
}


/*
  this method is used to get the button input for all different states
  as such there is no need to reimplament the lines used to listen the buttons
  this is done by using multiple if statements delay for the input is implemented using while loops
*/
void decide() {

  uint8_t buttons = lcd.readButtons();
  trigger = false;

  if (buttons & BUTTON_UP) {
    if (millis() - firstPress > timePassed) {

      trigger = true;
      firstPress = millis();
    }

    if (menu1 >= 0 && menu1 < 7 && (state == 0) && trigger) {
      //used to navigate the central menu
      menu1++;
      lcd.clear();
      trigger = false;
    } else if (n < 4  && state == 1 && trigger) {
      //used to set up the amount of characters used
      acN = true;
      n++;
      lcd.clear();
      trigger = false;
    } else if (timeLimit < 2000 && state == 3 && trigger) {
      //used to set up a custom time limit
      timeLimit = timeLimit + 100;
      lcd.clear();
      trigger = false;
    } else if (state == 4 && selector >= 0 && selector < 4 && trigger) {
      //used in the menu to select a custom difficulty
      lcd.clear();
      selector++;
      trigger = false;
    } else if (state == 6 && nav > 0 && nav <= 9 && trigger) {
      lcd.clear();
      nav--;
      trigger = false;
    }
  }
  else if (buttons & BUTTON_DOWN) {
    if (millis() - firstPress > timePassed) {

      trigger = true;
      firstPress = millis();
    }

    if (menu1 > 0 && menu1 <= 7 && state == 0 && trigger) {
      //used to navigate the central menu
      menu1--;
      lcd.clear();
      trigger = false;
    } else if (state == 1 && n > 1 && trigger) {
      //used to set up the amount of characters used
      acN = true;
      n--;
      lcd.clear();
      trigger = false;
    } else if (timeLimit > 100 && state == 3 && trigger) {
      //used to set up a custom time limit
      timeLimit = timeLimit - 100;
      lcd.clear();
      trigger = false;
    } else if (state == 4 && selector > 0 && selector <= 4 && trigger) {
      lcd.clear();
      selector--;
      trigger = false;
    } else if (state == 6 && nav >= 0 && nav < 9 && trigger) {
      lcd.clear();
      nav++;
      trigger = false;
    }
  }
  else if (buttons & BUTTON_LEFT) {
    if (millis() - firstPress > timePassed) {
      trigger = true;
      firstPress = millis();
    }

    if (state == 1 && len > 1 && trigger) {
      //used to set up the length of the sequence of characters used
      acN = false;
      len--;
      lcd.clear();
      trigger = false;
    } else if (state == 3 && timeLimitOn && trigger) {
      //used to activate the custom time limit
      timeLimitOn = !timeLimitOn;
      lcd.clear();
      trigger = false;
    } else if (state == 8 && letter > 0 && letter <= 25 && trigger) {
      lcd.clear();
      letter--;
      trigger = false;
    }
  }
  else if (buttons & BUTTON_RIGHT) {
    if (millis() - firstPress > timePassed) {
      trigger = true;
      firstPress = millis();
    }

    if (state == 1 && len < 14 && trigger) {
      //used to set up the length of the sequence of characters used
      acN = false;
      len++;
      lcd.clear();
      trigger = false;
    } else if (state == 3 && !timeLimitOn && trigger) {
      //used to activate the custom time limit
      timeLimitOn = !timeLimitOn;
      lcd.clear();
      trigger = false;
    } else if (state == 8 && letter >= 0 && letter < 25 && trigger) {
      lcd.clear();
      letter++;
      trigger = false;
    }
  }
  else if (buttons & BUTTON_SELECT) {
    if (millis() - firstPress > timePassed) {
      trigger = true;
      firstPress = millis();
    }

    if (state == 0 && menu1 == 2 && trigger) {
      //used to activate the practice mode
      state = 2;
      trigger = false;
    } else if (state == 0 && menu1 == 1 && trigger) {
      //used to activate the custom difficulty sub-menu
      lcd.clear();
      state = 1;
      trigger = false;
    } else if (state == 1 && trigger) {
      //used to deactivate the custom difficulty sub-menu
      state = 0;
      lcd.clear();
      trigger = false;
    } else if (state == 0 && menu1 == 3 && trigger) {
      //used to activate the custom time limit sub-menu
      selectableDifficultyOn = false;
      lcd.clear();
      state = 3;
      trigger = false;
    } else if (state == 0 && menu1 == 4 && trigger) {
      //used to activate the preset difficulty sub-menu
      state = 4;
      lcd.clear();
      trigger = false;
    } else if (state == 0 && menu1 == 5 && selectableDifficultyOn && trigger) {
      //used to activate the story mode
      state = 5;
      lcd.clear();
      trigger = false;
    } else if (state == 0 && menu1 == 6 && trigger) {
      //used to enter in the High Score's submenu
      lcd.clear();
      state = 6;
      trigger = false;
    } else if (state == 0 && menu1 == 7 && trigger) {
      //used to create artificial high scores
      state = 7;
      trigger = false;
    } else if (state == 3 && trigger) {
      //used to exit the custom time limit sub-menu
      state = 0;
      lcd.clear();
      trigger = false;
    } else if (state == 4 && trigger) {
      //used to exit the pre-set difficulty submenu
      state = 0;
      lcd.clear();
      trigger = false;
    } else if (state == 8 && trigger) {
      nam[namIndex] = alphabet[letter];
      namIndex++;
      letter = 0;
      lcd.clear();
      trigger = false;
    } else if (state == 6 && trigger) {
      //used to exit the high score's submenu
      state = 0;
      nav = 0;
      trigger = false;
      lcd.clear();
    }
  }
}

/*
  for use when the time limit is on, it restricts the input for a certain time limit
  as such if the user fails to enter an input in time he misses to input that character,
  even if he successfuly input all the characters he fails
*/
void timeLimitedInput() {

  /*
    The code for the while loop and the time checking parameter
    was taken from here: https://stackoverflow.com/questions/34308806/how-can-i-set-a-ceratin-time-interval-for-reading-input-in-my-arduino-game
  */

  inp = false;
  reference = millis();
  uint8_t buttons = lcd.readButtons();
  lcd.clear();

  while (!buttons) {
    buttons = lcd.readButtons();
    if (buttons & BUTTON_UP) {
      systemInput = 0;
      userIn[s] = systemInput;
      s++;
      lcd.clear();
    }
    else if (buttons & BUTTON_DOWN) {
      systemInput = 1;
      userIn[s] = systemInput;
      s++;
      lcd.clear();
    }
    else if (buttons & BUTTON_RIGHT) {
      systemInput = 2;
      userIn[s] = systemInput;
      s++;
      lcd.clear();
    }
    else if (buttons & BUTTON_LEFT) {
      systemInput = 3;
      userIn[s] = systemInput;
      s++;
      lcd.clear();
    }
    else if (buttons & BUTTON_SELECT) {
      exitPar = true;
      lcd.clear();
    }

    if ((timeLimit - (millis() - reference)) > 0) {
      lcd.setCursor(12, 1);
      lcd.print((timeLimit - (millis() - reference)) / 1000.0);
    }
    if (millis() - reference >= timeLimit) {
      inp = true;
      break;
    }
  }

  if (inp) {
    while (s < len) {
      systemInput = -3;
      s++;
    }
  }
}

/*
  the practiceInput method is used to to get the player's input and translate them into the characters of the game
  it is only meant to be used in practice mode when the timer is off
*/
void practiceInput() {
  uint8_t buttons = lcd.readButtons();
  lcd.clear();

  while (!buttons) {
    buttons = lcd.readButtons();
    if (buttons & BUTTON_UP) {
      systemInput = 0;
      userIn[s] = systemInput;
      s++;
      lcd.clear();
    }
    else if (buttons & BUTTON_DOWN) {
      //while (millis() < tim + 300) ;
      systemInput = 1;
      userIn[s] = systemInput;
      s++;
      lcd.clear();
    }
    else if (buttons & BUTTON_RIGHT) {
      //while (millis() < tim + 300) ;
      systemInput = 2;
      userIn[s] = systemInput;
      s++;
      lcd.clear();
    }
    else if (buttons & BUTTON_LEFT) {
      //while (millis() < tim + 300) ;
      systemInput = 3;
      userIn[s] = systemInput;
      s++;
      lcd.clear();
    }
    else if (buttons & BUTTON_SELECT) {
      //while (millis() < tim + 300) ;
      exitPar = true;
      lcd.clear();
    }

  }

}

/*
  this is the practice mode, it allows for the user to practice in static conditions without any score
  it also allows for custom difficulty settings to be used as well as the presets,
  to exit the mode you need to press select after the sequence is completed
*/
void practice() {
  lcd.setBacklight(WHITE);
  bool ch;
  timeNow = millis();
  period2 = 1000;
  for (int i = 0; i < len; i++) {

    lcd.clear();
    timeNow = millis();
    while (millis() <  timeNow + 800);
    lcd.clear();
    long pattern = random(0, n);
    systemIn[i] = displayer(pattern);
    timeNow = millis();
    while (millis() <  timeNow + period2);

  }
  systemInput = -1;
  lcd.clear();
  lcd.setBacklight(TEAL);
  //the reference point is used to indicate how many times the loop has actually run
  //it is used to avoid the user holding down the game and abusing the system
  unsigned int referencePoint = 0;
  while (s < len) {

    if (referencePoint > len) {
      exitPar = true;
    }

    //depending if the time limit is implemented different methods are used
    if (timeLimitOn) {
      timeLimitedInput();
      displayer(systemInput);
      referencePoint++;
    } else {
      practiceInput();
      displayer(systemInput);
      referencePoint++;
    }
    timeNow = millis();
    while (millis() < timeNow + 200);

    if (exitPar) {
      s = 100;
    }
  }

  //used to display the last element of the inputted sequence
  displayer(systemInput);
  timeNow = millis();
  while (millis() <  timeNow + 400);
  lcd.clear();

  /*
    used to check whether the user wants to exit
    if not it checks if the user's input work
  */
  if (!exitPar) {
    for (int i = 0; i < len; i++) {
      if (systemIn[i] == userIn[i]) {
        ch = true;
      } else {
        ch = false;
        i = 20;
      }
    }
    if (ch == true) {
      // if the input is correct this page is produced
      lcd.clear();
      lcd.setCursor(5, 0);
      lcd.print("Success");
      lcd.setBacklight(GREEN);
      timeNow = millis();
      while (millis() <  timeNow + 300);
      timeNow = millis();
      lcd.setBacklight(WHITE);
      while (millis() <  timeNow + 300);
      lcd.setBacklight(GREEN);
      timeNow = millis();
      while (millis() <  timeNow + 300);
      timeNow = millis();
      lcd.setBacklight(WHITE);
      while (millis() <  timeNow + 300);
    } else {
      // if the input is incorrect this page is produced
      lcd.clear();
      lcd.setCursor(6, 0);
      lcd.print("Fail");
      lcd.setBacklight(RED);
      timeNow = millis();
      while (millis() <  timeNow + delayT) ;
    }
  } else {
    //used to exit the practice mode
    state = 0;
    exitPar = false;
  }

  //reseting the arrays and s in order for the array to be clean
  lcd.clear();
  for (int i = 0; i < 14 ; i++) {
    systemIn[i] = -4;
    userIn[i] = -2;
    s = 0;
    systemInput = 0;
  }
  state = 0;
  exitPar = false;
}

/*
  used to set the a custom amount of characters used
  as well as the length of the sequence
*/
void setDifficulty() {
  if (acN == true) {
    lcd.setCursor(0, 0);
    lcd.print("The set of N is:");
    lcd.setCursor(0, 1);
    lcd.print(n);
  } else {
    lcd.setCursor(0, 0);
    lcd.print("The set of M is:");
    lcd.setCursor(0, 1);
    lcd.print(len);
  }
}

/*
  it is used to set a custom time limit
  as well as to activate it or not
*/
void setTimeLimit() {
  lcd.setCursor(0, 0);
  lcd.print("Time limit:");
  lcd.setCursor(11, 0);
  lcd.print(timeLimit);
  lcd.setCursor(0, 1);
  if (timeLimitOn) {
    lcd.print("On");
  } else {
    lcd.print("Off");
  }

}

/*
  it is used to select one of the presets or deactivate them all together,
  it is the menu for the presets
  it has all the different presets
*/
void selectableDifficulty() {
  if (selector == 0) {
    lcd.setCursor(4, 0);
    lcd.setBacklight(WHITE);
    lcd.print("Very Easy");
    lcd.setCursor(0, 1);
    lcd.print("M:");
    len = 4;
    lcd.setCursor(2, 1);
    lcd.print(len);
    lcd.setCursor(4, 1);
    lcd.print("N:");
    n = 2;
    lcd.setCursor(6, 1);
    lcd.print(n);
    lcd.setCursor(8, 1);
    lcd.print("T:");
    timeLimit = 2000;
    lcd.setCursor(10, 1);
    lcd.print(timeLimit / 1000);
    timeLimitOn = true;
    lcd.setCursor(12, 1);
    lcd.print("D:");
    period2 = 1000;
    lcd.print(period2 / 1000);
    //used to indicate that a preset is used
    selectableDifficultyOn = true;
  }
  if (selector == 1) {
    lcd.setCursor(6, 0);
    lcd.setBacklight(GREEN);
    lcd.print("Easy");
    lcd.setCursor(0, 1);
    lcd.print("M:");
    len = 4;
    lcd.setCursor(2, 1);
    lcd.print(len);
    lcd.setCursor(3, 1);
    lcd.print("N:");
    n = 3;
    lcd.print(n);

    lcd.setCursor(6, 1);
    lcd.print("T:");
    timeLimit = 1600;
    lcd.print("1.6");
    lcd.setCursor(11, 1);
    lcd.print("D:");
    period2 = 800;
    lcd.print("0.8");
    timeLimitOn = true;
    //used to indicate that a preset is used
    selectableDifficultyOn = true;
  }
  if (selector == 2) {
    lcd.setCursor(5, 0);
    lcd.setBacklight(YELLOW);
    lcd.print("Medium");
    lcd.setCursor(0, 1);
    lcd.print("M:");
    len = 4;
    lcd.setCursor(2, 1);
    lcd.print(len);
    lcd.setCursor(3, 1);
    lcd.print("N:");
    n = 4;
    lcd.print(n);
    lcd.setCursor(6, 1);
    lcd.print("T:");
    timeLimit = 1400;
    lcd.print("1.4");
    lcd.setCursor(11, 1);
    lcd.print("D:");
    period2 = 800;
    lcd.print("0.8");
    timeLimitOn = true;
    //used to indicate that a preset is used
    selectableDifficultyOn = true;
  }
  if (selector == 3) {
    lcd.setCursor(6, 0);
    lcd.setBacklight(RED);
    lcd.print("Hard");
    lcd.setCursor(0, 1);
    lcd.print("M:");
    len = 6;
    lcd.setCursor(2, 1);
    lcd.print(len);
    lcd.setCursor(3, 1);
    lcd.print("N:");
    n = 4;
    lcd.print(n);
    lcd.setCursor(6, 1);
    lcd.print("T:");
    timeLimit = 1200;
    lcd.print("1.3");
    lcd.print("D:");
    period2 = 700;
    lcd.print("0.7");
    timeLimitOn = true;
    //used to indicate that a preset is used
    selectableDifficultyOn = true;
  }
  if (selector == 4) {
    lcd.setCursor(1, 0);
    lcd.setBacklight(WHITE);
    lcd.print("No Difficulty");
    lcd.setCursor(0, 1);
    lcd.print("M:");
    len = 4;
    lcd.setCursor(2, 1);
    lcd.print(len);
    lcd.setCursor(4, 1);
    lcd.print("N:");
    n = 2;
    lcd.print(n);
    lcd.setCursor(8, 1);
    lcd.print("T:");
    lcd.print("NO");
    lcd.setCursor(13, 1);
    lcd.print("D:");
    period2 = 1000;
    lcd.print("1");
    timeLimitOn = true;
    timeLimitOn = false;
    //used to indicate that a preset is not used
    selectableDifficultyOn = false;
  }
}


/*
  this is the story mode
  it is quite similar to the practice mode
  the difference is that it has different
  screens for success or failure.
  it also is created in order to handle longer
  and chenging sequences
*/
void storyMode() {
  lcd.setBacklight(WHITE);
  bool ch;
  timeNow = millis();
  for (int i = 0; i < len; i++) {

    lcd.clear();
    timeNow = millis();
    while (millis() <  timeNow + 800);
    lcd.clear();
    long pattern = random(0, n);
    systemIn[i] = displayer(pattern);
    timeNow = millis();
    while (millis() <  timeNow + period2);
  }

  systemInput = -1;
  lcd.clear();
  lcd.setBacklight(TEAL);

  //the reference point is used to indicate how many times the loop has actually run
  //it is used to avoid the user holding down the game and abusing the system
  unsigned int referencePoint = 0;

  while (s < len) {

    if (referencePoint > len) {
      exitPar = true;
    }

    timeLimitedInput();
    displayer(systemInput);
    referencePoint++;

    timeNow = millis();
    while (millis() < timeNow + 200);

    if (exitPar) {
      s = 100;
    }
  }

  //used to display the last element of the inputted sequence
  displayer(systemInput);
  timeNow = millis();
  lcd.clear();
  while (millis() <  timeNow + 400) ;
  lcd.clear();

  if (!exitPar) {
    for (int i = 0; i < len; i++) {
      if (systemIn[i] == userIn[i]) {
        ch = true;
      } else {
        ch = false;
        break;
      }
    }
    if (ch == true) {
      lcd.clear();
      lcd.setCursor(5, 0);
      lcd.print("Success");
      lcd.setBacklight(GREEN);
      timeNow = millis();
      while (millis() <  timeNow + 300);
      timeNow = millis();
      lcd.setBacklight(WHITE);
      while (millis() <  timeNow + 300);
      lcd.setBacklight(GREEN);
      timeNow = millis();
      while (millis() <  timeNow + 300);
      timeNow = millis();
      lcd.setBacklight(WHITE);
      while (millis() <  timeNow + 300);
      increaseDifficulty();
    } else {
      lcd.clear();
      lcd.setCursor(6, 0);
      lcd.print("Fail");
      lcd.setCursor(0, 1);
      lcd.print("The score is:");
      lcd.setCursor(13, 1);
      lcd.print(score);
      lcd.setBacklight(RED);
      timeNow = millis();
      while (millis() <  timeNow + 3000) ;
      if (score > 0 && score < 255 && positionFound({'A', 'B', 'C', score}) > -1) {
        lcd.clear();
        lcd.setCursor(6, 0);
        lcd.print("New");
        lcd.setCursor(3, 1);
        lcd.print("High Score");
        lcd.setBacklight(GREEN);
        timeNow = millis();
        while (millis() <  timeNow + 300) ;
        lcd.setBacklight(WHITE);
        timeNow = millis();
        while (millis() <  timeNow + 300) ;
        lcd.setBacklight(GREEN);
        timeNow = millis();
        while (millis() <  timeNow + 300) ;
        lcd.setBacklight(WHITE);
        timeNow = millis();
        while (millis() <  timeNow + 300) ;
        state = 8;
        selectableDifficulty();
      } else {
        score = 0;
        //used to reset the system to the staring values of that pre-set
        selectableDifficulty();
        state = 0;
      }

    }
  } else {
    score = 0;
    //used to reset the system to the staring values of that pre-set
    selectableDifficulty();
    state = 0;
  }
  lcd.clear();
  /*
    re-setting all arrays and s to the original values
    as such there is no memory leakage and the results
    are correct
  */
  for (int i = 0; i < 14 ; i++) {
    systemIn[i] = -4;
    userIn[i] = -2;
    s = 0;
    systemInput = 0;
    exitPar = false;
  }
}

/*
  this method is used to increase the difficulty for the players
  the rate at which the difficulty increases varies from
  pre-set to pre-set
*/
void increaseDifficulty() {

  //the "very easy" pre-set
  if (selector == 0 && len < 14 && n <= 4) {
    score = score + 5;
    len = len + 2;
  } else if (selector == 0 && len == 14 && n < 4) {
    score = score + 5;
    len = 4;
    n = n + 1;
    period2 = period2 - 50;
  } else if (selector == 0 && len == 14 && n == 4 && timeLimit > 1000 && period2 > 800) {
    score = score + 5;
    len = 4;
    timeLimit = timeLimit - 100;
    period2 = period2 - 10;
  } else if (selector == 0 && len == 14 && n == 4 && timeLimit == 1000) {
    score = score + 5;
    len = 4;
  }

  //the "easy" pre-set
  if (selector == 1 && len < 14 && n <= 4) {
    score = score + 10;
    len = len + 2;
  } else if (selector == 1 && len == 14 && n < 4) {
    score = score + 10;
    len = 4;
    n = n + 2;
    period2 = period2 - 50;
  } else if (selector == 1 && len == 14 && n == 4 && timeLimit > 1000) {
    score = score + 10;
    len = 4;
    timeLimit = timeLimit - 100;
    period2 = period2 - 25;
  } else if (selector == 1 && len == 14 && n == 4 && timeLimit == 1000) {
    score = score + 10;
    len = 4;
  }

  //the "medium" pre-set
  if (selector == 2 && len < 14) {
    score = score + 15;
    len = len + 2;
  } else if (selector == 2 && len == 14 && timeLimit == 950) {
    score = score + 15;
    len = 4;
  } else if (selector == 2 && len == 14 && n == 4 && timeLimit > 950) {
    score = score + 15;
    len = 4;
    timeLimit = timeLimit - 50;
    period2 = period2 - 22;
  }

  //the "hard" pre-set
  if (selector == 3 && len < 14) {
    score = score + 20;
    len = len + 2;
  } else if (selector == 3 && len == 14 && timeLimit == 900) {
    score = score + 20;
    len = 6;
  } else if (selector == 3 && len == 14 && n == 4 && timeLimit > 900) {
    score = score + 20;
    len = 6;
    timeLimit = timeLimit - 50;
    period2 = period2 - 25;
  }
}


/*
  saves the final array of the highscores in the EEPROM
*/
void save() {
  for (int i = 0; i < 10; i++)
    EEPROM.put(i * 5, f[i]);
}

//retrieves the original array from the EEPROM
void retrieve() {
  for (int i = 0; i < 50; i = i + 5) {
    HS m;
    h[i / 5] = EEPROM.get(i, m);
  }
}

//prints all the values in the highscore in a special menu page in the screen
void printAll() {
  retrieve();
  lcd.setCursor(3, 0);
  lcd.print("High Scores");
  lcd.setCursor(6, 1);
  lcd.print(h[nav].username1);
  lcd.print(h[nav].username2);
  lcd.print(h[nav].username3);
  lcd.print(" ");
  lcd.print(h[nav].hs);
}

//updates the highscores with the new one
void updateHighScore(HS m) {
  retrieve();
  int pos = positionFound(m);

  for (int i = 0; i < 10; i++) {
    if (i == pos) {
      HS temp = h[i];
      f[i] = m;
      i++;
      f[i] = temp;
    }  else if (i > pos) {
      f[i] = h[i - 1];
    } else {
      f[i] = h[i];
    }
  }
  save();
}

//finds the new position of the HS object in the array
int positionFound(HS score) {
  int m = -1;
  for (int i = 0; i < 10; i++) {
    if (h[i].hs < score.hs) {
      return m = i;
      break;
    }
  }
}

/*
  this method is used to enter the highscore as well as
  save it in the internal memory
*/
void enterHighScore() {

  lcd.setCursor(0, 0);
  lcd.println("Enter your initial");
  lcd.setCursor(6, 1);
  lcd.print(alphabet[letter]);

  if (namIndex > 2) {
    HS in = {nam[0], nam[1], nam[2], score};
    updateHighScore(in);
    state = 0;
    score = 0;
    lcd.clear();
    nam[0] = '\0';
    nam[1] = '\0';
    nam[2] = '\0';
    namIndex = 0;
  }
}

/*
  this method is used to insert an artificialy created
  highscore board to showcase that the highscore methods work
*/
void createHighScore() {
  for (int i = 0 ; i < EEPROM.length() ; i++) {
    EEPROM.update(i, 0);
  }

  for (int i = 0; i < 10; i++) {
    updateHighScore({'A', 'B', 'C', 1});
  }
}
