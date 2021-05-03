#include <Adafruit_CircuitPlayground.h>
#include <Wire.h>
#include <SPI.h>
#include <Keyboard.h>

#define tapThreshold 120
#define fistThreshold 5
#define jumpThreshold 90

int lastMotionX = 0;
int lastMotionY = 0;
int lastMotionZ = 0;
bool isChargingFist = false;
bool isOnCheatMode = false;
char masterMode[] = {KEY_TAB, 'p', 'r', 'o', 'g', 'r', 'a', 'm'};
char tenMoreTings[] = {KEY_TAB, 'j', 'o', 'j', 'o'};
char freeMovement[] = {KEY_TAB, ';', 'o', 'v', 'e', 'r', 'a', 'y', KEY_BACKSPACE};

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
  getHorizontalInputs();
  getFistInputs();
  getJumpInputs();
  updateCheatMode();
  delay(100);
}

void getHorizontalInputs()
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
      // allow actions while moving
      getFistInputs();
      getJumpInputs();
    }
    Keyboard.release(keyPressed);
  }
}

void getFistInputs()
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

  if (overallDelta > fistThreshold && !isChargingFist)
  {
    isChargingFist = true;
    Keyboard.press(KEY_LEFT_ALT);
  }

  else if (overallDelta < fistThreshold)
  {
    isChargingFist = false;
    Keyboard.release(KEY_LEFT_ALT);
  }
}

void getJumpInputs()
{
  if (CircuitPlayground.mic.soundPressureLevel(10) > jumpThreshold)
  {
    Keyboard.press(KEY_LEFT_CTRL);
  }
  else
  {
    Keyboard.release(KEY_LEFT_CTRL);
  }
}

void updateCheatMode()
{
  if (CircuitPlayground.slideSwitch() && !isOnCheatMode)
  {
    Serial.print("ON!");
    isOnCheatMode = true;
    freeMovementCheat();
  }

  else if (!CircuitPlayground.slideSwitch() && isOnCheatMode)
  {
    isOnCheatMode = false;
    freeMovementCheat();
  }
}

void masterModeCheat()
{
  for (int i = 0; i < strlen(masterMode); i++)
    {
      char c = masterMode[i];
      Keyboard.write(c);
    }
}

void tenMoreTingsCheat()
{
  for (int i = 0; i < strlen(tenMoreTings); i++)
    {
      char c = tenMoreTings[i];
      Keyboard.write(c);
    }
}

void freeMovementCheat()
{
    for (int i = 0; i < strlen(freeMovement); i++)
    {
      char c = freeMovement[i];
      Keyboard.write(c);
    }
}