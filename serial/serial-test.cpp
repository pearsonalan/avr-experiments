#include <Arduino.h>

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

class LED {
 public:
  LED() = default;
  LED(uint8_t pin, volatile uint8_t* ddr_port, volatile uint8_t* port) :
      pin_(pin), port_(port) {
    *ddr_port |= _BV(pin_);
  }
  void On() {
    state_ = 1;
    *port_ |= _BV(pin_);
  }
  void Off() {
    state_ = 0;
    *port_ &= ~_BV(pin_);
  }
  void Toggle() {
    if (state_) Off();
    else On();
  }

 private:
  uint8_t state_ = 0;
  uint8_t pin_ = 0;
  volatile uint8_t* port_ = 0;
};

PushButton button;
LED led;

ISR(PCINT0_vect) {
  static long last_change_time = 0;
  long now = millis();
  // ignore changes within 25ms of the last change... these are probably bounces
  if (now - last_change_time > 50) {
    button.HandlePinChange();
  }
  last_change_time = now;
}

// the setup function runs once when you press reset or power the board
void setup() {
  // LED is on PB0. Initialize pin PB0 as an output (set the bit )
  led = LED(PB0, &DDRB, &PORTB);

  // Initialize a push button.
  button = PushButton(PB1, &DDRB, &PINB, PCINT1, &PCMSK0, PCIE0);

  Serial.begin(115200);
  while (!Serial) {}

  Serial.println("Ready");
}

// the loop function runs over and over again forever
void loop() {
  static unsigned int n = 0;
  static unsigned long c = 0;
  static int last_output = 0;
  static int last_toggle = 0;
  static int toggle_time = 50;

  // Perform these checks and the release very close together. There is a 
  // race condition here where the button press could happen after the check
  // and before the reset in which case the state change is missed. To prevent
  // that, we should disable interrupts here...
  cli();
  int pressed = button.Pressed();
  int released = button.Released();
  button.Reset();
  sei();

  if (pressed) {
    Serial.print("[Button Pressed = ");
    Serial.print(pressed);
    Serial.println("]");
    toggle_time *= 2;
    if (toggle_time > 800) toggle_time = 50;
  }

  if (released) {
    Serial.print("[Button Released = ");
    Serial.print(released);
    Serial.println("]");
  }

  int now = millis();
  if (now - last_output > 500) {
    Serial.print("Output #");
    Serial.print(n++);
    Serial.print(" Loop Count = ");
    Serial.println(c);
    last_output = now;
    c = 0;
  }

  if (now - last_toggle > toggle_time) {
    led.Toggle();
    last_toggle = now;
  }

  c++;
}
