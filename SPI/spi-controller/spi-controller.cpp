#include <Arduino.h>
#include <SPI.h>

// Implement a button using a pin-change interrupt with software debouncing
class PushButton {
public:
  PushButton() = default;
  PushButton(uint8_t pin, volatile uint8_t* ddr_port,
      volatile uint8_t* pin_port, uint8_t pin_change_interrupt,
      volatile uint8_t* pci_mask_reg, uint8_t pci_control_bit) :
          pin_(pin), pin_port_(pin_port) {
    cli();

    // Set the pin to input in the DDR port (clear the bit)
    *ddr_port &= ~_BV(pin);

    // Enable pin change interrupt for the given pin
    *pci_mask_reg |= _BV(pin_change_interrupt);

    // Enable pin change interrupt for the bank
    PCICR |= _BV(pci_control_bit);

    sei();
  }

  void HandlePinChange();
  int Pressed() { return pressed_; }
  int Released() { return released_; }
  void Reset() {
    pressed_ = 0;
    released_ = 0;
  }

private:
  uint8_t pin_ = 0;
  volatile uint8_t* pin_port_ = 0;
  volatile int pressed_ = 0;
  volatile int released_ = 0;
};

void PushButton::HandlePinChange() {
  if (pin_port_ == 0) return;

  // We only want the interrupt on putton press, not release, so check that the
  // button state is LOW (pressed), and if so, set the interrupted flag.
  int b = *pin_port_ & _BV(pin_);
  if (b == 0) {
    pressed_ += 1;
  } else {
    released_ += 1;
  }
}

PushButton button;

ISR(PCINT0_vect) {
  static long last_change_time = 0;
  long now = millis();
  // ignore changes within 25ms of the last change... these are probably bounces
  if (now - last_change_time > 50) {
    button.HandlePinChange();
  }
  last_change_time = now;
}

int main() {
  init();

  // Initialize a push button.
  button = PushButton(PB1, &DDRB, &PINB, PCINT1, &PCMSK0, PCIE0);

  Serial.begin(115200);
  while (!Serial) {}

  Serial.println("Starting SPI Controller...");
  int last_output_time = millis();

  SPI.begin();
  SPI.setClockDivider(SPI_CLOCK_DIV8);
  pinMode(SS, OUTPUT);
  digitalWrite(SS, HIGH);

  unsigned long n = 0;
  unsigned long c = 0;
  for (;;) {
    int now = millis();

    cli();
    int pressed = button.Pressed();
    int released = button.Released();
    button.Reset();
    sei();

    if (pressed) {
      Serial.print("[Button Pressed = ");
      Serial.print(pressed);
      Serial.println("]");

      digitalWrite(SS, LOW);
      SPI.transfer('O');
      SPI.transfer('n');
      SPI.transfer('\n');
      digitalWrite(SS, HIGH);
    }

    if (released) {
      Serial.print("[Button Released = ");
      Serial.print(released);
      Serial.println("]");

      digitalWrite(SS, LOW);
      SPI.transfer('O');
      SPI.transfer('f');
      SPI.transfer('f');
      SPI.transfer('\n');
      digitalWrite(SS, HIGH);
    }

    if (now - last_output_time > 1000) {
      Serial.print("Output #");
      Serial.print(n++);
      Serial.print("; Loop count = ");
      Serial.println(c);
      last_output_time = now;
      c = 0;
    }
    if (serialEventRun) serialEventRun();
    c++;
  }

  return 0;
}