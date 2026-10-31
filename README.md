# Smart Vote Arduino demo

This project has a browser results dashboard and an Arduino sketch. The dashboard reads newline-delimited JSON vote totals over USB serial. The Arduino sketch counts button presses and sends the updated totals.

## Hardware

Use an Arduino Uno, Nano, or similar board and four normally-open push buttons:

| Candidate | Arduino pin | Other button terminal |
| --- | --- | --- |
| Ananya Rao | D2 | GND |
| Kabir Mehta | D3 | GND |
| Mira Das | D4 | GND |
| Arjun Sen | D5 | GND |

The sketch enables each pin's internal pull-up resistor, so no external resistors are needed for this demo.

## Run it

1. Open `arduino_vote_counter.ino` in Arduino IDE, choose your board and port, then upload it.
2. Serve this folder from the computer connected to the Arduino. For example, run `python -m http.server 8000` in this folder and open `http://localhost:8000` in Chrome or Edge.
3. Click **Connect Arduino** and choose the board's USB serial port.
4. Press a candidate's button. The matching total and bar on the dashboard update.
5. When counting is complete, use **Declare final winner**. After announcing the result, choose **Start new count** to clear both the dashboard and the connected Arduino for another round.

The dashboard initially shows sample totals. Use the **+1** buttons to preview the interface without hardware, or **Reset sample** to restore the starting numbers. Connecting an Arduino replaces sample totals with the board's current counts.

## Serial message

At 9600 baud, the sketch sends one JSON object per line when it starts, after each button press, and once per second so a newly connected dashboard receives the current totals:

```json
{"type":"vote_count","votes":[1,0,0,0],"total":1}
```

The values in `votes` follow the candidate order above. The browser validates the four counts and displays them. The dashboard sends `RESET` over serial when a new count starts; the Arduino responds with all-zero totals. Counts also clear when the board is reset or powered off.

This is a classroom/demo prototype, not a secure or certified election system. It does not identify voters, persist results, or protect the hardware from tampering.
