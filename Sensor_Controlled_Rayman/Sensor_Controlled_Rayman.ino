/*
 * Sensor-Controlled Gameplay: Rayman!
 * 
 * We created a sensor-controlled controller to play one of the most beloved video games of the 90's - Rayman!
 * 
 * Press the left button A (D4) - to move left
 * Press the right Button B (D5) - to move right
 * Blow on the sound sensor (microphone) - to jump
 * Blow on the sound sensor again while Rayman is in mid-air - to do a helicopter
 * Shake and wave your hand - to charge a punch (hand motion is detected by the motion sensor / accelerometer)
 * Release your hand - to launch a punch
 * Switch the slide switch - to turn on the cheat mode (cheat: Free movement)
 * Switch the slide switch again - to turn off the cheat mode
 *
 * Video links (turn on the CAPTIONS of the videos!) :
 * - IoT: Sensor-Controlled Gameplay - Rayman!
 *   https://www.youtube.com/watch?v=p_g85eZUXA0
 *
 * You can download Rayman for Windows from here: https://gamesnostalgia.com/download/rayman/2626?t=52500836
 * and for Mac from here: https://gamesnostalgia.com/download/rayman/1581?t=45038610
 * 
 * Created by :
 * Tomer Ben-Gigi, 206198772
 * Lior Reytan, 204326607
 */
 
#include <Adafruit_CircuitPlayground.h>
#include <Wire.h>
#include <SPI.h>
#include <Keyboard.h>

#define tapThreshold 120
#define punchThreshold 5
#define msTimeBeforeJump 10
#define jumpThreshold 90

int lastMotionX = 0;
int lastMotionY = 0;
int lastMotionZ = 0;
bool isChargingPunch = false;
bool isD7SwitchInitiallyOn = false;
bool isOnCheatMode = false;;
char freeMovement[] = {KEY_TAB, ';', 'o', 'v', 'e', 'r', 'a', 'y', KEY_BACKSPACE}; // [TAB];overay[BACKSPACE]

void setup()
{
  Serial.begin(9600);
  Keyboard.begin();
  CircuitPlayground.begin();
  CircuitPlayground.setAccelRange(LIS3DH_RANGE_2_G); // 2, 4, 8 or 16 G!
  CircuitPlayground.setAccelTap(1, tapThreshold);
  attachInterrupt(digitalPinToInterrupt(CPLAY_LIS3DH_INTERRUPT), onTapHandler, FALLING);
}

// Handles the event of the instrument being tapped
void onTapHandler(void)
{
  Serial.println("TAP!");
}

void loop()
{
  handleMovement();
  handlePunch();
  handleJump();
  updateCheatMode();
  delay(100);
}

// Handles Rayman's horizontal movement
void handleMovement()
{
  int keyPressed = -1;

  if (CircuitPlayground.leftButton())
  {
    keyPressed = KEY_LEFT_ARROW;
  }

  else if (CircuitPlayground.rightButton())
  {
    keyPressed = KEY_RIGHT_ARROW;
  }

  if (keyPressed != -1)
  {
    Keyboard.press(keyPressed);
    while (CircuitPlayground.leftButton() || CircuitPlayground.rightButton())
    {
      // Allows actions while moving
      handlePunch();
      handleJump();
    }

    Keyboard.release(keyPressed);
  }
}

// Samples the accelerometer in every 3D-axis and checks if a punch should be charged and launched according to the samplings
void handlePunch()
{
  int motionX = CircuitPlayground.motionX();
  int motionY = CircuitPlayground.motionY();
  int motionZ = CircuitPlayground.motionZ();
  int deltaX = abs(motionX - lastMotionX);
  int deltaY = abs(motionY - lastMotionY);
  int deltaZ = abs(motionZ - lastMotionZ);
  int overallDelta = deltaX + deltaY + deltaZ;

  lastMotionX = motionX;
  lastMotionY = motionY;
  lastMotionZ = motionZ;
  if (overallDelta > punchThreshold && !isChargingPunch)
  {
    isChargingPunch = true;
    Keyboard.press(KEY_LEFT_ALT);
  }
  else if (overallDelta < punchThreshold)
  {
    isChargingPunch = false;
    Keyboard.release(KEY_LEFT_ALT);
  }
}

// Checks if the sound pressure level of the microphone is greater than the given jump threshold, if so - perform a jump
void handleJump()
{
  if (CircuitPlayground.mic.soundPressureLevel(msTimeBeforeJump) > jumpThreshold)
  {
    Keyboard.press(KEY_LEFT_CTRL);
  }
  else
  {
    Keyboard.release(KEY_LEFT_CTRL);
  }
}

// Checks if the slide switch has been toggled relative to its initial state, to enable/disable the cheat mode accordingly
void updateCheatMode()
{
  if (CircuitPlayground.slideSwitch() && !isOnCheatMode)
  {
    Serial.print("ON!");
    isOnCheatMode = true;
    switchFreeMovementCheat();
  }
  else if (!CircuitPlayground.slideSwitch() && isOnCheatMode)
  {
    isOnCheatMode = false;
    switchFreeMovementCheat();
  }
}

// Toggles on/off the cheat of free movement: "[TAB];overay[BACKSPACE]"
void switchFreeMovementCheat()
{
  for (int i = 0; i < strlen(freeMovement); i++)
  {
    char c = freeMovement[i];
    Keyboard.write(c);
  }
}