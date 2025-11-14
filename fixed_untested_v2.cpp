//I noticed that the old code was whack so I quickly fixed it; ill test it; if it works ill shove this into the main code, yes I am too lazy to make a new branch.

// Pin setup
const int DOT_PIN  = 2;   // Left paddle (active LOW)
const int DASH_PIN = 3;   // Right paddle (active LOW)
const int KEY_OUT  = 8;   // Relay output (active HIGH)

// Settings
int wpm = 20;
unsigned long ditTime;

// State machine
enum State { IDLE, KEY_DOWN, KEY_UP };
State state = IDLE;

unsigned long stateStartTime = 0;
bool sendDot  = false;
bool sendDash = false;
bool elementIsDot = true;

void setup() {
  pinMode(DOT_PIN, INPUT_PULLUP);
  pinMode(DASH_PIN, INPUT_PULLUP);
  pinMode(KEY_OUT, OUTPUT);

  digitalWrite(KEY_OUT, LOW);  // relay idle (KEY UP)

  // Correct Morse timing
  ditTime = 1200UL / wpm;
}

void loop() {
  unsigned long now = millis();

  bool dotPressed  = (digitalRead(DOT_PIN)  == LOW);
  bool dashPressed = (digitalRead(DASH_PIN) == LOW);

  switch (state) {

    case IDLE:
      if (dotPressed) {
        elementIsDot = true;
        startElement(now);
      } else if (dashPressed) {
        elementIsDot = false;
        startElement(now);
      }
      break;

    case KEY_DOWN:
      if (now - stateStartTime >= (elementIsDot ? ditTime : ditTime * 3)) {
        keyUp();
        state = KEY_UP;
        stateStartTime = now;

        // Paddle memory
        if (elementIsDot && dashPressed) sendDash = true;
        if (!elementIsDot && dotPressed) sendDot = true;
      }
      break;

    case KEY_UP:
      if (now - stateStartTime >= ditTime) {

        if (sendDot) {
          sendDot = false;
          elementIsDot = true;
          startElement(now);
        } else if (sendDash) {
          sendDash = false;
          elementIsDot = false;
          startElement(now);
        } else if (dotPressed) {
          elementIsDot = true;
          startElement(now);
        } else if (dashPressed) {
          elementIsDot = false;
          startElement(now);
        } else {
          state = IDLE;
        }
      }
      break;
  }
}

void startElement(unsigned long now) {
  state = KEY_DOWN;
  stateStartTime = now;
  keyDown();
}

void keyDown() {
  digitalWrite(KEY_OUT, HIGH); // Relay ON (ACTIVE HIGH)
}

void keyUp() {
  digitalWrite(KEY_OUT, LOW);  // Relay OFF
}
