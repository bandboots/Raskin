
/*
 * DONE:
 * -Button Press for difficulty
 * -Basic guard loop
 * 
 * TO DO:


int gameState = 0;
//byte difficulty[] = { 225, 175, 125, 95 };
//Color dColor[] = {BLUE, GREEN, ORANGE, RED};
//byte dLevel = 0;
bool location;
bool explored;
bool spotted;
bool resetting;
bool winning;
bool isYellow;
byte faceIndex = 0;
byte faceStartIndex = 0;
Color currentColor;
enum States
{
  PreGame,
  Reset,
  Unexplored,
  Explored,
  Location,
  Spotted,
  NewLocation,
  Dead,
  BlueRed,
  WinCondition
  IncrementSpeed
}


Timer guardSpeed;
Timer resetDelay;
Timer establishing;
Timer WinCountdown;
Timer EchoChecker;


void setup() 
{  
  winning = true;
  resetting = false;
  location = false;
  explored = false;
  spotted = false;
  establishing.set(100);
  gameState = PreGame;
}



void loop() {  

  winning = true; //this has to be reset because it will switch to false every loop that there is an Unexplored

  spotted = false; //Reset this most important variable

  lockAnimLoop(currentColor, difficulty[dLevel]);    //loop red on every blink by calling this function, defined at the end
 
  if(resetDelay.isExpired())
  {
    FOREACH_FACE(f)
    {
      checkSides();
    }
  }


  if((buttonPressed())   //Check for location in neighbors, check if spotted
  { 
    FOREACH_FACE (f) //Cycle through each face, check for position and spotted
    {
      moveRaskin();
    }
  }



  if(buttonDoubleClicked())
  {
    gameReset();
  }

  if (buttonLongPressed()) 
  {
    setColor(dColor[dLevel]);
    dLevel +=1;
    if(dLevel > 3)
    {
      dLevel = 0;
    }
    setValueSentOnAllFaces(IncrementSpeed); //Send out the reset signal
    resetDelay.set(500); //Set delay to prevent recursion
    gameState = IncrementSpeed;
  }
  
  switch(gameState)
  {
    
    case PreGame:
      //PreGame: Glow yellow, wait for game start. If double-clicked or given the signal to play, enter the game.
      setValueSentOnAllFaces(PreGame);
      setColor(YELLOW);

    break;



    case Reset:
     //Reset: Send out reset signal, reset all variables, move to Unexplored
     resetting = true;
     setValueSentOnAllFaces(Reset); //send out the reset signal
     location = false; //reset all variables
     explored = false;
     spotted = false;
     if(resetDelay.isExpired())
     {
        resetDelay.set(1000); //set timer to prevent recursion
     }
     else if(resetDelay.getRemaining() < 750)
     {
        gameState = Unexplored;
     }
        

    break;



   case Unexplored:
    //Unexplored: Set color to yellow, stop resetting
    currentColor = YELLOW;
    resetting = false;

   break;


   case Explored: break;
    //Explored: Set color to green
    currentColor = GREEN;



   case Location: break;
    //Location: Set color to blue
      currentColor = BLUE;
      location = true;
      explored = true;

   
   case Spotted:
    //Spotted: If the player moves here, the guard kills them. No color change, only variables
      spotted = true;

   break;

   case NewLocation: 
     //NewLocation: Flash a signal to nearby spaces which tells them they can't be Location
       location = true;
       explored = true;
       if(resetDelay.isExpired())
       {
          resetDelay.set(500); //set timer to prevent recursion
       }
       else if(resetDelay.getRemaining() < 250)
       {
          gameState = Location;
       }
   
   break;

   
   case Dead: 
    //Dead: Set all values false, turn red
     setValueSentOnAllFaces(Death); //send out the Game Over signal
     currentColor = RED;
     location = false; //reset all variables
     explored = false;
     spotted = false;
     winning = false; //can't be winning if you lost

     break;

   
   case BlueRed: 
    //BlueRed: Sends signals for Spotted and Location at the same time.

   break;

   
   case WinCondition: 
    //WinCondition

   break;

   case IncrementSpeed: 
    //IncrementSpeed
    if(resetDelay.isExpired())
    {
      resetDelay.set(500);
    }
    else if(resetDelay.getRemaining() < 250)
    {
      gameState = 
    }

   break;
   

    
  }

  void checkSides()
  {
    if(!isValueReceivedOnFaceExpired(f))
      {
        if(getLastValueReceivedOnFace(f) == Unexplored && EchoChecker.isExpired()) //This is the echo that determines if the player has won or not
          {
            setValueSentOnAllFaces(Unexplored); //If a 3 is received, echo a 10 for a moment before returning to your own state
            winning = false; //If there is a 3 somewhere, we haven't won
            EchoChecker.set(500); //set timers
            WinCountdown.set(1750);
          }
          if(getLastValueReceivedOnFace(f) == Location or getLastValueReceivedOnFace(f) == BlueRed) //If a guard is looking this direction, you are at risk of being spotted
          {
            spotted = true;
          }
          if(getLastValueReceivedOnFace(f) == NewLocation)  //If a blink signals that it is the new location 
          {
            location = false;                                         //tiles around it should check to see if location is true for them, and adjust accordingly
            //NOTE: This is having some problems, possibly just communication issues due to dev kit. On rare occasions, a blink doesn't receive the New Location signal and will stay blue
          }
          if(getLastValueReceivedOnFace(f) == Death && !resetting) //Spread Game Over condition to all tiles
          {
             
             gameState = 5;
  
          }
        if(getLastValueReceivedOnFace(f) == Reset) //If the reset signal is received
        {
           gameState = 1;
        }
     } 
  }

  void moveRaskin()
  {
    if((getLastValueReceivedOnFace(f) == Location or getLastValueReceivedOnFace(f) == BlueRed)&& !spotted) //If any face is location, this spot is available. If no guard is looking, this spot is safe.
      {
        explored = true;
        location = true; //Move the player to this location
        establishing.set(100);
        gameState = NewLocation;
      }
      else if((getLastValueReceivedOnFace(f) == Location or getLastValueReceivedOnFace(f) == BlueRed)&& spotted) //If you move to a threatened space
      {
        gameState = Dead; //Game Over
      }
  }

  void gameReset()
  {
    setValueSentOnAllFaces(Reset); //Send out the reset signal
    explored = true; //Establish this as the location of the player
    location = true;
    spotted = false;
    resetDelay.set(1000); //Set delay to prevent recursion
    gameState = Location;
  }

  void lockAnimLoop(Color currentColor, int interval) //This is what actually runs the loop
{
    if (guardSpeed.isExpired()) //guardSpeed is the speed at which the guard moves from door to door
    {
      setColor(currentColor);         //Every  face needs to reset to the appropriate color, otherwise the whole thing will be red
      setValueSentOnAllFaces(gameState);  //Tell each face to send out the state of this blink
      faceIndex += 1;                 //move the face that will show the guard (red)
      if (faceIndex == 6) { faceIndex = 0;}
      if (state != Location)
      {
        setValueSentOnFace(Spotted, faceIndex);//set the value on this one face to be 'spotted'
      }
      else if (state == Location)
      {
        setValueSentOnFace(BlueRed, faceIndex);//set the value on this one face to be 'spotted'
      }
      setColorOnFace(RED, faceIndex); //set the guard face to red
      guardSpeed.set(interval);        //reset the timer so the guard will move again
    }
}

*/



  
