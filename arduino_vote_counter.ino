// Smart Vote vote counter demo
// One push button per candidate: button between the listed pin and GND.
// Counts live in RAM and restart at zero when the board is reset.
#include <string.h>

const byte CANDIDATE_COUNT = 4;
const byte BUTTON_PINS[CANDIDATE_COUNT] = {2, 3, 4, 5};
const unsigned long DEBOUNCE_MS = 40;
const unsigned long REPORT_INTERVAL_MS = 1000;
const byte COMMAND_BUFFER_SIZE = 16;

unsigned long votes[CANDIDATE_COUNT] = {0, 0, 0, 0};
int lastReading[CANDIDATE_COUNT] = {HIGH, HIGH, HIGH, HIGH};
int stableState[CANDIDATE_COUNT] = {HIGH, HIGH, HIGH, HIGH};
unsigned long lastChangeAt[CANDIDATE_COUNT] = {0, 0, 0, 0};
unsigned long lastReportAt = 0;
char commandBuffer[COMMAND_BUFFER_SIZE];
byte commandLength = 0;

void sendVoteCounts() {
  unsigned long total = 0;

  Serial.print("{\"type\":\"vote_count\",\"votes\":[");
  for (byte i = 0; i < CANDIDATE_COUNT; i++) {
    if (i > 0) Serial.print(',');
    Serial.print(votes[i]);
    total += votes[i];
  }
  Serial.print("],\"total\":");
  Serial.print(total);
  Serial.println('}');
}

void readSerialCommands() {
  while (Serial.available() > 0) {
    const char incoming = (char)Serial.read();
    if (incoming == '\r') continue;

    if (incoming == '\n') {
      commandBuffer[commandLength] = '\0';
      if (strcmp(commandBuffer, "RESET") == 0) {
        for (byte i = 0; i < CANDIDATE_COUNT; i++) votes[i] = 0;
        sendVoteCounts();
        lastReportAt = millis();
      }
      commandLength = 0;
    } else if (commandLength < COMMAND_BUFFER_SIZE - 1) {
      commandBuffer[commandLength++] = incoming;
    } else {
      // Ignore an overlong command until its line ending arrives.
      commandLength = 0;
    }
  }
}

void setup() {
  Serial.begin(9600);

  for (byte i = 0; i < CANDIDATE_COUNT; i++) {
    pinMode(BUTTON_PINS[i], INPUT_PULLUP);
    lastReading[i] = digitalRead(BUTTON_PINS[i]);
    stableState[i] = lastReading[i];
  }

  // Send an initial all-zero count so the dashboard receives the current state.
  delay(500);
  sendVoteCounts();
  lastReportAt = millis();
}

void loop() {
  readSerialCommands();
  const unsigned long now = millis();

  for (byte i = 0; i < CANDIDATE_COUNT; i++) {
    const int reading = digitalRead(BUTTON_PINS[i]);

    if (reading != lastReading[i]) {
      lastReading[i] = reading;
      lastChangeAt[i] = now;
    }

    if ((now - lastChangeAt[i]) >= DEBOUNCE_MS && reading != stableState[i]) {
      stableState[i] = reading;

      // INPUT_PULLUP means a pressed button reads LOW. Count once per press.
      if (stableState[i] == LOW) {
        votes[i]++;
        sendVoteCounts();
        lastReportAt = now;
      }
    }
  }

  // Repeat the current state so a dashboard connected after startup gets a snapshot.
  if (now - lastReportAt >= REPORT_INTERVAL_MS) {
    sendVoteCounts();
    lastReportAt = now;
  }
}
