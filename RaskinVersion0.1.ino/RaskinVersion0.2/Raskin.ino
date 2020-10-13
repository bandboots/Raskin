
int gameState = 0;
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
  gameState = 0;
}



void loop() {  

  winning = true; //this has to be reset because it will switch to false every loop that there is a 0-state

  spotted = false; //Reset this most important variable

  lockAnimLoop(currentColor, 125);    //loop red on every blink byt calling this function, defined at the end
 
  
  
  
  if(resetDelay.isExpired())
  {
    FOREACH_FACE(f)
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
  }


  if((buttonPressed())   //Check for location in neighbors, check if spotted
  { 
    FOREACH_FACE (f) //Cycle through each face, check for position and spotted
    {
      if((getLastValueReceivedOnFace(f) == Location or getLastValueReceivedOnFace(f) == BlueRed)&& !spotted) //If any face is location, this spot is available. If no guard is looking, this spot is safe.
      {
        explored = true;
        location = true; //Move the player to this location
        establishing.set(100);
        gameState = 5;
      }
      else if((getLastValueReceivedOnFace(f) == Location or getLastValueReceivedOnFace(f) == BlueRed)&& spotted) //If you move to a threatened space
      {
        gameState = 7; //Game Over
      }
    }
  }



  if(buttonDoubleClicked())
  {
    gameReset();
  }


  
  switch(gameState)
  {
    
    case 0:
      //Glow yellow, wait for game start. If double-clicked or given the signal to play, enter the game.
      setValueSentOnAllFaces(PreGame);
      setColor(YELLOW);

    break;



    case 1:
     resetting = true;
     setValueSentOnAllFaces(Reset); //send out the reset signal
     location = false; //reset all variables
     explored = false;
     spotted = false;
     resetDelay.set(1000); //set timer to prevent recursion
     gameState = 2;

   break;



   case 2:
    currentColor = YELLOW;
    resetting = false;

   break;


   case 3: break;



   case 4: break;


   
   case 5:
     setValueSentOnAllFaces(Death); //send out the Game Over signal
     location = false; //reset all variables
     explored = false;
     spotted = false;
     winning = false; //can't be winning if you lost

   break;

   case 6: break;


   
   case 7: break;


   
   case 8: break;


   
   case 9: break;
   

    
  }

  void gameReset()
  {
    setValueSentOnAllFaces(Reset); //Send out the reset signal
    explored = true; //Establish this as the location of the player
    location = true;
    spotted = false;
    resetDelay.set(1000); //Set delay to prevent recursion
    gameState = 5;
  }
  
