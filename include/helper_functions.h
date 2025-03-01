/*************************************************************
************************* DEFINITIONS ************************
**************************************************************/

#ifndef HELPER_FUNCTIONS_H
#define HELPER_FUNCTIONS_H


// Define the state machine colour states
enum class LEDColour {
  RED,
  GREEN,
  ORANGE
};

// Define the state machine mode states
enum class State {
  Colour_CHANGE_AUTO,   // automatic colour change every second
  Colour_CHANGE_MANUAL  // change colour on button press
};

// Define global variables
LEDColour currentColour = LEDColour::RED;
State currentState = State::Colour_CHANGE_AUTO; // default to Auto Mode
bool buttonPressed = false;
bool redrawRequired = true;

// Define the pins for the RGB LED
#define RED_PIN 1
#define GREEN_PIN 2
#define BUTTON_PIN 14 // built-in 'KEY' button
#define PIN_LCD_BL 15 // backlight pin for T-Display S3


/*************************************************************
********************** HELPER FUNCTIONS **********************
**************************************************************/

// Function to set the LED colour
void setLEDColour(LEDColour Colour) {
  switch (Colour) {
    case LEDColour::RED:
      digitalWrite(RED_PIN, HIGH);
      digitalWrite(GREEN_PIN, LOW);
      break;
    case LEDColour::GREEN:
      digitalWrite(RED_PIN, LOW);
      digitalWrite(GREEN_PIN, HIGH);
      break;
    case LEDColour::ORANGE:
      digitalWrite(RED_PIN, HIGH);
      digitalWrite(GREEN_PIN, HIGH);
      break;
    default: // default to red if an unknown colour is specified
      digitalWrite(RED_PIN, HIGH);
      digitalWrite(GREEN_PIN, LOW);
      break;
  }
  redrawRequired = true; // update screen
}

// Function to change the current colour based on the next colour in the sequence.
void changeColour() {
  currentColour = static_cast<LEDColour>((static_cast<int>(currentColour) + 1) % 3);
  setLEDColour(currentColour);
}

// Function to update LED colours automatically
void updateAutoColour() {
  static unsigned long lastColourChangeTime = 0;
  unsigned long currentTime = millis();

  if (currentTime - lastColourChangeTime >= 1000) { // 1000ms
    lastColourChangeTime = currentTime;
    changeColour();
    redrawRequired = true; // update screen
  }
}

// Function to display status information
void displayStatus(TFT_eSPI &tft) {
  static bool lastButtonState = !buttonPressed; // initialize to a different state to force update

  if (redrawRequired || lastButtonState != buttonPressed) {
    tft.fillScreen(TFT_BLACK);
    tft.setCursor(0, 0);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    
    tft.println("Current State:");
    tft.println((currentState == State::Colour_CHANGE_AUTO) ? "AUTO MODE" : "MANUAL MODE");

    tft.println("\nCurrent Colour:");
    const char* colourNames[] = {"RED", "GREEN", "ORANGE"};
    tft.println(colourNames[static_cast<int>(currentColour)]);

    tft.println("\nButton State:");
    tft.println(buttonPressed ? "PRESSED" : "NOT PRESSED");

    redrawRequired = false;
    lastButtonState = buttonPressed;
  }
}

// Button event handlers
void onShortPress() {
  if (currentState == State::Colour_CHANGE_MANUAL) {
    changeColour();
  }
  redrawRequired = true; // update screen
}

void onLongPressStart() { // change modes with a 1sec long press
  currentState = (currentState == State::Colour_CHANGE_AUTO) ? State::Colour_CHANGE_MANUAL : State::Colour_CHANGE_AUTO;
  redrawRequired = true; // update screen
}

#endif // HELPER_FUNCTIONS_H
