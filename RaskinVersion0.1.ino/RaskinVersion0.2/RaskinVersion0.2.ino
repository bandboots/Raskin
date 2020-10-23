// FIRST pick one blink as the start point

//Spin around and identify targeted blinks as 'spotted'

//IF an adjacent blink is location and this blink is pressed
//AND this blink is not spotted
//This blink becomes location and explored

//ELSE IF this blink is spotted when pressed
//Flash all blinks red and end game

//Win condition: Each blink echoes a 0 as a single pulse if they are next to a 0 state or receive a 0, then wait 250ms. If any blink does not get a 0 pulse, it begins a countdown timer of 750ms which resets if a 0 pulse arrives. If it reaches 0, win.



/*
 * CURRENT ISSUES:
 * Communication issue - when moving to a new tile, the old location may not receive the signal to turn green
 * Game Over Issue - Green tiles don't seem to register guards. I haven't been able to turn a green tile red.
 */




/* ***********
 *  REFERENCES: 
 *  Example ColorByNeighbor
 *  Berry
 *  BombBrigade
 *  Causeway
 * ***********/

//byte difficulty[] = { 225, 175, 125, 95 };
//Color dColor[] = {BLUE, GREEN, ORANGE, RED};
//byte dLevel = 0;
bool location;
bool explored;
bool spotted;
bool resetting;
bool winning;
bool isYellow;
bool dChanging;
byte faceIndex = 0;
byte faceStartIndex = 0;
Color currentColor;
int state;
/*
 * STATES
 * 10 - Unexplored
 * 1 - Explored
 * 2 - (Current)Location
 * 3 - Spotted
 * 4 - NewLocation
 * 5 - Dead
 * 6 - Reset
 * 7 - BlueRed
 * 8 - Win Condition
 * 9 - Increment Speed
 */

Timer guardSpeed;
Timer resetDelay;
Timer establishing;
Timer WinCountdown;
Timer EchoChecker;
Timer Celebration;


void setup() 
{  
  winning = true;
  resetting = false;
  location = false;
  explored = false;
  spotted = false;

  establishing.set(100);
  state = 10;
}



void loop() {  

  winning = true; //this has to be reset because it will switch to false every loop that there is a 0-state

  spotted = false; //Reset this most important variable

  lockAnimLoop(currentColor, 150);//difficulty[dLevel]);    //loop red on every blink byt calling this function, defined at the end
  
  
  if(resetDelay.isExpired() ) //resetDelay is a timer which prevents recursion when resetting the board
  {

    FOREACH_FACE(f)
    {
      if(!isValueReceivedOnFaceExpired(f))
      {
          /*if(getLastValueReceivedOnFace(f) == 9)
          {

            setValueSentOnAllFaces(9);
            dLevel +=1;
            if(dLevel > 3)
            {
              dLevel = 0;
            }
            setColor(dColor[dLevel]);
            resetDelay.set(1000);
            dChanging = true;
          } */
          if(getLastValueReceivedOnFace(f) == 10 && EchoChecker.isExpired()) //This is the echo that determines if the player has won or not
          {
            setValueSentOnAllFaces(10); //If a 10 is received, echo a 10 for a moment before returning to your own state
            winning = false; //If there is a 10 somewhere, we haven't won
            EchoChecker.set(500); //set timers
            WinCountdown.set(1750);
          }
          if(getLastValueReceivedOnFace(f) == 3 or getLastValueReceivedOnFace(f) == 7) //If a guard is looking this direction, you are at risk of being spotted
          {
            spotted = true;
          }
          if(getLastValueReceivedOnFace(f) == 4)  //If a blink signals that it is the new location 
          {
            location = false;                                         //tiles around it should check to see if location is true for them, and adjust accordingly
            //NOTE: This is having some problems, possibly just communication issues due to dev kit. On rare occasions, a blink doesn't receive the New Location signal and will stay blue
          }
          if(getLastValueReceivedOnFace(f) == 5 && !resetting) //Spread Game Over condition to all tiles
          {
             setValueSentOnAllFaces(5); //send out the Game Over signal
             state = 5;
             location = false; //reset all variables
             explored = false;
             spotted = false;
             winning = false; //can't be winning if you lost
          }
          if(getLastValueReceivedOnFace(f) == 6) //If the reset signal is received
          {
             resetting = true;
             setValueSentOnAllFaces(6); //send out the reset signal
             location = false; //reset all variables
             explored = false;
             spotted = false;
             resetDelay.set(1000); //set timer to prevent recursion
             state = 10;
          }
      }

    }
    if(winning && state != 8) //This checks if there are any 10s, and is disabled if we're already celebrating
    {

      areYaWinningSon();
      
    }
  }


  

  if (buttonPressed() && state != 8)   //Check for location in neighbors, check if spotted
  { 
    FOREACH_FACE (f) //Cycle through each face, check for position and spotted
    {
      if((getLastValueReceivedOnFace(f) == 2 or getLastValueReceivedOnFace(f) == 7)&& !spotted) //If any face is state 2 (location), this spot is available. If no guard is looking, this spot is safe.
      {
        explored = true;
        location = true; //Move the player to this location
        establishing.set(100);
        state = 4;
      }
      else if((getLastValueReceivedOnFace(f) == 2 or getLastValueReceivedOnFace(f) == 7)&& spotted) //If you move to a threatened space
      {
        state = 5; //Game Over
      }
    }
  }

  if (buttonDoubleClicked()) //This will signal the blinks to reset themselves and each other, and make this blink the new starting point
  {
    setValueSentOnAllFaces(6); //Send out the reset signal
    explored = true; //Establish this as the location of the player
    location = true;
    spotted = false;
    resetDelay.set(1000); //Set delay to prevent recursion
  }

/*    if (buttonLongPressed()) //This will signal the blinks to reset themselves and each other, and make this blink the new starting point
  {
    setColor(BLUE);
    dLevel +=1;
    if(dLevel > 3)
    {
      dLevel = 0;
    }
    setValueSentOnAllFaces(9); //Send out the reset signal
    resetDelay.set(1000); //Set delay to prevent recursion
    dChanging = true;
  }
 */ 


  //Establish the base color - Yellow if unexplored, Green if explored, Blue if the current location, Red for Game Over
  if(state == 8) //This is our celebration state. Flash yellow and white until reset
  {
    if(Celebration.isExpired())
    {
      if(isYellow)
      {
        currentColor = WHITE;
      }
      else if (!isYellow)
      {
        currentColor = YELLOW;
      }

      setColor(currentColor);

      Celebration.set(500);
    }
  }
  
  if(state==5) //YOU LOSE
  {
    currentColor = RED;
    explored == false;
    location == false;
  }
  if (explored == false && state != 5) //Unexplored. The hated 10s.
  {
    currentColor = YELLOW;
    state = 10;
    resetting = false;
  } 
  
  else if (explored == true && location == false) //Explored
  {
    currentColor = GREEN;
    state = 1;
  } 
  
  else if (location == true) //YOU ARE HERE
  {
    if(state == 4 && !establishing.isExpired())
    {
       setValueSentOnAllFaces(4); //Flash a signal to neighbors in order to show that this is the new active location.
    }
    else
    {
          state = 2;
    }
    currentColor = BLUE;

  }
  /*if(dChanging)
  {
    if(resetDelay.getRemaining() >=750)
    {
      setValueSentOnAllFaces(9);
    }
    else
    {
      dChanging = false;
    }
  }*/
}


void areYaWinningSon() //Wait until the countdown finishes. If no 10s remain after 1750ms, switch to the celebration state
{

  if(WinCountdown.isExpired() && state != 10)
  {
    isYellow = true;
    currentColor = YELLOW;
    state = 8;
  }
  
}



void lockAnimLoop(Color currentColor, int interval) //This is what actually runs the loop
{
    if (guardSpeed.isExpired()) //guardSpeed is the speed at which the guard moves from door to door
    {
      setColor(currentColor);         //Every  face needs to reset to the appropriate color, otherwise the whole thing will be red
      setValueSentOnAllFaces(state);  //Tell each face to send out the state of this blink
      faceIndex += 1;                 //move the face that will show the guard (red)
      if (faceIndex == 6) { faceIndex = 0;}
      if (state != 2)
      {
        setValueSentOnFace(3, faceIndex);//set the value on this one face to be 'spotted'
      }
      else if (state == 2)
      {
        setValueSentOnFace(7, faceIndex);//set the value on this one face to be 'spotted'
      }
      setColorOnFace(RED, faceIndex); //set the guard face to red
      guardSpeed.set(interval);        //reset the timer so the guard will move again
    }
}
