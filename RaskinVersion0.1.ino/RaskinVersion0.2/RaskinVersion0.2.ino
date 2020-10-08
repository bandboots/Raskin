// FIRST pick one blink as the start point

//Spin around and identify targeted blinks as 'spotted'

//IF an adjacent blink is location and this blink is pressed
//AND this blink is not spotted
//This blink becomes location and explored

//ELSE IF this blink is spotted when pressed
//Flash all blinks red and end game


/* ***********
 *  REFERENCES: 
 *  Example ColorByNeighbor
 *  Berry
 *  BombBrigade
 *  Causeway
 * ***********/


bool location;
bool explored;
bool spotted;
byte faceIndex = 0;
byte faceStartIndex = 0;
Color currentColor;
int state;
/*
 * STATES
 * 0 - Unexplored
 * 1 - Explored
 * 2 - (Current)Location
 * 3 - Spotted
 * 4 - NewLocation
 * 5 - Dead
 * 6 - Reset
 */

Timer guardSpeed;
Timer resetDelay;


void setup() {  

  location = false;
  explored = false;
  spotted = false;
}



void loop() {  

  spotted = false; //Reset this most important variable

  lockAnimLoop(currentColor, 250);    //loop red on every blink byt calling this function, defined at the end
  
  if(resetDelay.isExpired()) //resetDelay is a timer which prevents recursion when resetting the board
  {
    FOREACH_FACE(f)
    {
      if(getLastValueReceivedOnFace(f) == 3) //If a guard is looking this direction, you are at risk of being spotted
      {
        spotted = true;
      }
      if(getLastValueReceivedOnFace(f) == 4)  //If a blink signals that it is the new location 
      {
        location = false;                                         //tiles around it should check to see if location is true for them, and adjust accordingly
        //NOTE: This is having some problems, possibly just communication issues due to dev kit. On rare occasions, a blink doesn't receive the New Location signal and will stay blue
      }
      if(getLastValueReceivedOnFace(f) == 6) //If the reset signal is received
      {
         setValueSentOnAllFaces(6); //send out the reset signal
         location = false; //reset all variables
         explored = false;
         spotted = false;
         resetDelay.set(1000); //set timer to prevent recursion
      }
    }
  }


  

  if (buttonPressed())   //Check for location in neighbors, check if spotted
  { 
    FOREACH_FACE (f) //Cycle through each face, check for position and spotted
    {
      if(getLastValueReceivedOnFace(f) == 2 && !spotted) //If any face is state 2 (location), this spot is available. If no guard is looking, this spot is safe.
      {
        explored = true;
        location = true; //Move the player to this location
        setValueSentOnAllFaces(4); //Flash a signal to neighbors in order to show that this is the new active location.
      }
      else if(getLastValueReceivedOnFace(f) == 2 && spotted) //If you move to a threatened space
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
  

  //Establish the base color - Yellow if unexplored, Green if explored, Blue if the current location
  if(state==5)
  {
    currentColor = RED;
    explored == false;
    location == false;
  }
  if (explored == false && state != 5)
  {
    currentColor = YELLOW;
    state = 0;
  } 
  
  else if (explored == true && location == false)
  {
    currentColor = GREEN;
    state = 1;
  } 
  
  else if (location == true)
  {
    currentColor = BLUE;
    state = 2;
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
      setColorOnFace(RED, faceIndex); //set the guard face to red
      setValueSentOnFace(3, faceIndex);//set the value on this one face to be 'spotted'
      guardSpeed.set(interval);        //reset the timer so the guard will move again
    }
}
