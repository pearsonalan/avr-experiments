#include <Arduino.h>
#include <SPI.h>

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

LED led;

char buf[100];
volatile byte pos;
volatile boolean process_it;

// SPI interrupt routine
ISR(SPI_STC_vect) {
  byte c = SPDR;  // grab byte from SPI Data Register
  
  // add to buffer if room
  if (pos < (sizeof(buf) - 1))
    buf[pos++] = c;
    
  // newline means time to process buffer
  if (c == '\n')
    process_it = true;
}

int main() {
  init();

  // LED is on PB0. Initialize pin PB0 as an output (set the bit )
  led = LED(PB0, &DDRB, &PORTB);

  Serial.begin(115200);
  while (!Serial) {}

  Serial.println("Starting SPI Perhipheral...");
  int last_output_time = millis();

  pinMode(SCK, INPUT);
  pinMode(MOSI, INPUT);
  // have to send on master in, *slave out*
  pinMode(MISO, OUTPUT);
  
  // SPI.begin();

  // turn on SPI in slave mode
  SPCR |= bit(SPE);

  // turn on interrupts
  SPCR |= _BV(SPIE);

  // get ready for an interrupt 
  pos = 0;   // buffer empty
  process_it = false;

  // // now turn on interrupts
  // SPI.attachInterrupt();

  int n = 0;
  for (;;) {
    int now = millis();
    if (now - last_output_time > 1000) {
      Serial.print("Running ");
      Serial.println(n++);
      last_output_time = now;
    }

    if (process_it) {
      buf[pos] = 0;  
      Serial.println(buf);
      if (strcmp(buf, "On\n") == 0) {
        led.On();
      }
      if (strcmp(buf, "Off\n") == 0) {
        led.Off();
      }
      pos = 0;
      process_it = false;
    }

    if (serialEventRun) serialEventRun();
  }

  return 0;
}