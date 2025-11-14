# dual_to_mono_morse_keyer_translator
Arduino code that translated a dual morse keyer into a mono input needed by my Yaesu FT-950

I found that my Yaseu considers the paddle to transmit when the MONO paddle jack is shorted; so im shorting it with a relay.
I decided to use a relay because I like the sound but a transistor will 100% work.

Im just hooking up the arduino through a buck straight into my transcievers external PSU.


[](https://github.com/Michael-Likh/dual_to_mono_morse_keyer_translator/blob/main/arduino.png)


heres the arduino code so that you can easely copy paste:

```cpp
// Pin setup
const int DOT_PIN  = 2;   // Left paddle
const int DASH_PIN = 3;   // Right paddle
const int KEY_OUT  = 8;   // Relay control

// Settings
int wpm = 20;
unsigned long ditTime;  // ms

// State machine variables
enum State { IDLE, KEY_DOWN, KEY_UP };
State state = IDLE;

unsigned long stateStartTime = 0;
bool sendDot = false;
bool sendDash = false;
bool elementIsDot = true;

void setup() {
  pinMode(DOT_PIN, INPUT_PULLUP);
  pinMode(DASH_PIN, INPUT_PULLUP);
  pinMode(KEY_OUT, OUTPUT);
  digitalWrite(KEY_OUT, HIGH);

  ditTime = (1200UL / wpm) * 4; // doesnt work as expected; basically magic number rn
}


void loop() {
  unsigned long now = millis();


  switch (state) {
    case IDLE:
      // Check paddles
      if (!digitalRead(DOT_PIN)) {
        elementIsDot = true;
        state = KEY_DOWN;
        stateStartTime = now;
        keyDown();
      } else if (!digitalRead(DASH_PIN)) {
        elementIsDot = false;
        state = KEY_DOWN;
        stateStartTime = now;
        keyDown();
      }
      break;

    case KEY_DOWN:
      if (now - stateStartTime >= (elementIsDot ? ditTime : ditTime * 3)) {
        keyUp();
        state = KEY_UP;
        stateStartTime = now;
        // Paddle memory: if opposite paddle pressed, remember it
        if (elementIsDot && !digitalRead(DASH_PIN)) sendDash = true;
        if (!elementIsDot && !digitalRead(DOT_PIN)) sendDot = true;
      }
      break;

    case KEY_UP:
      if (now - stateStartTime >= ditTime) {
        if (sendDot) {
          sendDot = false;
          elementIsDot = true;
          state = KEY_DOWN;
          stateStartTime = now;
          keyDown();
        } else if (sendDash) {
          sendDash = false;
          elementIsDot = false;
          state = KEY_DOWN;
          stateStartTime = now;
          keyDown();
        } else if (!digitalRead(DOT_PIN)) {
          elementIsDot = true;
          state = KEY_DOWN;
          stateStartTime = now;
          keyDown();
        } else if (!digitalRead(DASH_PIN)) {
          elementIsDot = false;
          state = KEY_DOWN;
          stateStartTime = now;
          keyDown();
        } else {
          state = IDLE;
        }
      }
      break;
  }
}

void keyDown() {
  digitalWrite(KEY_OUT, LOW); // Relay ON
}

void keyUp() {
  digitalWrite(KEY_OUT, HIGH);  // Relay OFF
}
```
