# Memory-Game

This was a memory game submitted as a coursework to the Embedded Programming module. The goal was to create a game that could operate within the constraints of the Arduino while providing a nice experience. The game four diferent characters that can be displayed in a sequence up to 10 characters.

In the fillowing text N, M and S are mentioned. Their meaning is the following:
* N stands for th number of sequence
* S stands for the sequence of inputted characters
* M stands for the possible characters in the game
* T stands for the time delay that a character is shown

The following libraries have been used:
* <Wire.h>
* <Adafruit_RGBLCDShield.h>
* <utility/Adafruit_MCP23017.h>
* <EEPROM.h>

The display used has been the Adafruite LCD Shield with the Arduino Uno computer.

## INS: Instructions

In order to operate the game, you have to plug it into a computer that has the Arduino IDE installed.
Once the game has booted you will be greeted by the first page of the main menu.  You need to
use the up and down buttons to navigate the menu.  It is worth noting that the game utilizes arrows that
are oriented with the Arduino's buttons (UP, DOWN, RIGHT, LEFT).  To access a submenu or activate a function
press select when its page shows up in the LCD screen.  After you have inputted the settings or you want to
exit press select and you will be returned in the main menu.

The first menu is about setting custom difficulty values.  This allows to individually set the N and M set.
To do that press enter when the menu appears and you will enter the selection menu.
To set N use up and down buttons.  Up adds a character downs removes one.
To set S use right and left buttons, right adds one left removes one.
Once you have selected the values you want press select to exit to the main menu.

The next page allows you to play the practice mode.
In the practice mode, you can utilize the custom difficulty you have set as well as the difficulty presets.
When you enter the menu the game starts by presenting you the N.  When the colour changes you input
your answer, if it is correct a success message is presented where the backlight flashes green if you are wrong a red one is presented.
Game over does not exist in this mode as its sole reason is to train you.
You can exit it by pressing the select button.

Both in story and practice mode if you try to press the same or different buttons very fast without waiting a bit,
the game will kick you out.

The next page allows you to set a custom time limit.  The maximum time limit is 2000 milliseconds and
the minimum is 100 milliseconds.  To change the time period use the up and down buttons.
They change the time limit by 100 milliseconds.  To activate or deactivate the time limit use right and left buttons.
The right button activates the Time limit and the left one deactivates it. Once you have set the input press select
to exit the menu.

The next page allows you to select a pre-setted difficulty.  To navigate through them using the left and right button.
The menu also has the choice to return all values to default, each difficulty has a different score per successful input.
To select the difficulty you want just press select when it is shown on the screen. You will then exit the menu.

The next page is the story mode page.  To enter it you press select but you also have to have selected a pre-setted
difficulty.  Once you start the game it is the same with the practice mode.  The difference is that the difficulty
progressively increases and that if you make an error it is a game over.  If your high score is high enough you get to input
a high score.  You can input the three letters of your name in the high score board, to navigate through the letters use the right and left
buttons and to select the button press the select button.  Once you input your high score you return to the main menu.

Another function is to see the high scores. It is on the main menu and you only need to press select to access it.
Once you press the select button the high score table is presented in the lower line of the LCD screen.
Use up and down buttons to navigate through it.  Press the select button if you want to exit.

Finally, there is the "Create high score" function that allows you to reset the high score table to a new one.
To use it just press select when it appears in the main menu.  It is important to be utilized if the sketch has been
imported to a new Arduino as it resets the memory to a familiar format that is to be used for the high score.

## MFS: Features of the minimal feature set

Indicate by deleting YES or NO to the questions below.
If partially implemented, describe this by deleting *YES/NO* and
writing what does work.  Do not adjust any other text in this section.

* Sequences are displayed on the screen as specified: *YES*
* Backlight flashes (only when correct): *YES*
* Incorrect answer leads to game over: *YES*
* Game supports *N* up to 10: *NO*
* Game supports *M* of size 4: *YES*
* A menu is present: *YES*
* Size of *M* can be changed: *YES*
* *N* can be changed: *YES*
* Message when wrong printed: *YES*

## OAF: Optional and additional features

* The game supports N up to 14.
* The game has a suitable text when the user's input is correct as well as the use of flashing backlight.
* The game has a time limit for inputting the next character.  If typing takes longer than expected the game ends in failure.
* The game has a timer that shows the remaining time to input a character.  It is only used when a timed input is needed.
* The game has a certain time that a character is shown when the sequence is presented to the user, it changes according to the difficulty level and how much the user has progressed.
* Each difficulty preset has its own possible score from a successful input by the user, the more difficult the preset is, the higher the score is.
* The game features a practice and a story mode.
* In case that a game over occurs the game's difficulty reverts to the preset selected.
* A suitable display is used when the user fails to input the correct sequence or runs of time.
* A suitable display is used when the user successfully inputs the correct sequence.
* In story mode every time the user's input is correct the difficulty increases (N and M increase while T and D decrease) as well as its score.
* The game features difficulty presets that are used for the story mode.
* After the end of story mode, the difficulty of the came returns to the registered preset.
* The practice mode does not change the difficulty of the game at all.
* The game also features a main menu to switch between settings and different modes.
* The game allows customization of N, S and T.
* The game allows by pushing the select button during the time the user inputs the sequence he saw to exit the game.
* Exiting the story mode using the select button is the same as quitting the game.  The player's score will not be registered.
* The game has custom characters that are easy for the user to associate with the direction buttons.
* The characters utilize both lines of the screen in order to be easier from the user to see them.
* The screen changes colour from WHITE to TEAL in order to showcase to the user when his turn is.
* The game has suitable displays for winning and losing.
* In the story mode, if your score is high enough it will be entered in the high score table that is stored in the EEPROM.
* The high score allows for the user to input their 3 initials to represent their alias.
* The user is able to see all the high scores through the Arduino's LCD screen.
* There is a function that resets the high score stored in EEPROM to a custom value that is lower from the lowest score the user can score.

PLEASE do NOT change the lines that start with hashes.
