#include <ArduinoUnitTests.h>
#include <Arduino.h>

unittest(millis_micros_and_delay)
{
  GodmodeState* state = GODMODE();
  state->reset();
  assertEqual(0, millis());
  assertEqual(0, micros());
  delay(3);
  assertEqual(3, millis());
  assertEqual(3000, micros());
  delayMicroseconds(11000);
  assertEqual(14, millis());
  assertEqual(14000, micros());
}

unittest(random)
{
  randomSeed(1);
  unsigned long x;
  x = random(4294967293);
  assertEqual(4294967292, x);
  x = random(50, 100);
  assertEqual(83, x);
  x = random(100);
  assertEqual(74, x);
}

void myInterruptHandler() {
}

unittest(interrupts)
{
  // these are meaningless for testing; just call the routine directly.
  // make sure our mocks work though
  attachInterrupt(2, myInterruptHandler, CHANGE);
  detachInterrupt(2);
}

unittest(pins)
{
  GodmodeState* state = GODMODE();
  state->reset();
  pinMode(1, OUTPUT);  // this is a no-op in unit tests.  it's just here to prove compilation
  digitalWrite(1, HIGH);
  assertEqual(HIGH, state->digitalPin[1]);
  digitalWrite(1, LOW);
  assertEqual(LOW, state->digitalPin[1]);

  pinMode(1, INPUT);
  state->digitalPin[1] = HIGH;
  assertEqual(HIGH, digitalRead(1));
  state->digitalPin[1] = LOW;
  assertEqual(LOW, digitalRead(1));

  analogWrite(1, 37);
  assertEqual(37, state->analogPin[1]);
  analogWrite(1, 22);
  assertEqual(22, state->analogPin[1]);

  state->analogPin[1] = 99;
  assertEqual(99, analogRead(1));
  state->analogPin[1] = 56;
  assertEqual(56, analogRead(1));
}

#ifdef HAVE_HWSERIAL0

  void smartLightswitchSerialHandler(int pin) {
    if (Serial.available() > 0) {
      int incomingByte = Serial.read();
      int val = incomingByte == '0' ? LOW : HIGH;
      Serial.print("Ack ");
      digitalWrite(pin, val);
      Serial.print(String(pin));
      Serial.print(" ");
      Serial.print((char)incomingByte);
    }
  }

  unittest(does_nothing_if_no_data)
  {
      GodmodeState* state = GODMODE();
      int myPin = 3;
      state->serialPort[0].dataIn = "";
      state->serialPort[0].dataOut = "";
      state->digitalPin[myPin] = LOW;
      smartLightswitchSerialHandler(myPin);
      assertEqual(LOW, state->digitalPin[myPin]);
      assertEqual("", state->serialPort[0].dataOut);
  }

  unittest(keeps_pin_low_and_acks)
  {
      GodmodeState* state = GODMODE();
      int myPin = 3;
      state->serialPort[0].dataIn = "0";
      state->serialPort[0].dataOut = "";
      state->digitalPin[myPin] = LOW;
      smartLightswitchSerialHandler(myPin);
      assertEqual(LOW, state->digitalPin[myPin]);
      assertEqual("", state->serialPort[0].dataIn);
      assertEqual("Ack 3 0", state->serialPort[0].dataOut);
  }

  unittest(flips_pin_high_and_acks)
  {
      GodmodeState* state = GODMODE();
      int myPin = 3;
      state->serialPort[0].dataIn = "1";
      state->serialPort[0].dataOut = "";
      state->digitalPin[myPin] = LOW;
      smartLightswitchSerialHandler(myPin);
      assertEqual(HIGH, state->digitalPin[myPin]);
      assertEqual("", state->serialPort[0].dataIn);
      assertEqual("Ack 3 1", state->serialPort[0].dataOut);
  }

  unittest(two_flips)
  {
      GodmodeState* state = GODMODE();
      int myPin = 3;
      state->serialPort[0].dataIn = "10junk";
      state->serialPort[0].dataOut = "";
      state->digitalPin[myPin] = LOW;
      smartLightswitchSerialHandler(myPin);
      assertEqual(HIGH, state->digitalPin[myPin]);
      assertEqual("0junk", state->serialPort[0].dataIn);
      assertEqual("Ack 3 1", state->serialPort[0].dataOut);
      state->serialPort[0].dataOut = "";
      smartLightswitchSerialHandler(myPin);
      assertEqual(LOW, state->digitalPin[myPin]);
      assertEqual("junk", state->serialPort[0].dataIn);
      assertEqual("Ack 3 0", state->serialPort[0].dataOut);
  }
#endif



unittest_main()
