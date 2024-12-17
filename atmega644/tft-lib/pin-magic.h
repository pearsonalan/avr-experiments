#ifndef _pin_magic_
#define _pin_magic_

#define DELAY7                                                                 \
  asm volatile("rjmp .+0"                                                      \
               "\n\t"                                                          \
               "rjmp .+0"                                                      \
               "\n\t"                                                          \
               "rjmp .+0"                                                      \
               "\n\t"                                                          \
               "nop"                                                           \
               "\n" ::);

#define write8inline(d)                                                        \
  {                                                                            \
    PORTA = (d);                                                               \
    WR_STROBE;                                                                 \
  }
#define read8inline(result)                                                    \
  {                                                                            \
    RD_ACTIVE;                                                                 \
    DELAY7;                                                                    \
    result = PINA;                                                             \
    RD_IDLE;                                                                   \
  }
#define setWriteDirInline() DDRA = 0xff
#define setReadDirInline() DDRA = 0

#define write8 write8inline
#define read8 read8inline
#define setWriteDir setWriteDirInline
#define setReadDir setReadDirInline

#define RD_ACTIVE (PORTD &= ~(1<<PD5))
#define RD_IDLE (PORTD |= (1<<PD5))

#define WR_ACTIVE (PORTD &= ~(1<<PD4))
#define WR_IDLE (PORTD |= (1<<PD4))

#define CD_COMMAND (PORTD &= ~(1<<PD3))
#define CD_DATA (PORTD |= (1<<PD3))

#define CS_ACTIVE (PORTD &= ~(1<<PD6))
#define CS_IDLE (PORTD |= (1<<PD6))

// Data write strobe, ~2 instructions and always inline
#define WR_STROBE                                                              \
  {                                                                            \
    WR_ACTIVE;                                                                 \
    WR_IDLE;                                                                   \
  }

// These higher-level operations are usually functionalized,
// except on Mega where's there's gobs and gobs of program space.

// Set value of TFT register: 8-bit address, 8-bit value
#define writeRegister8inline(a, d)                                             \
  {                                                                            \
    CD_COMMAND;                                                                \
    write8(a);                                                                 \
    CD_DATA;                                                                   \
    write8(d);                                                                 \
  }

// Set value of TFT register: 16-bit address, 16-bit value
// See notes at top about macro expansion, hence hi & lo temp vars
#define writeRegister16inline(a, d)                                            \
  {                                                                            \
    uint8_t hi, lo;                                                            \
    hi = (a) >> 8;                                                             \
    lo = (a);                                                                  \
    CD_COMMAND;                                                                \
    write8(hi);                                                                \
    write8(lo);                                                                \
    hi = (d) >> 8;                                                             \
    lo = (d);                                                                  \
    CD_DATA;                                                                   \
    write8(hi);                                                                \
    write8(lo);                                                                \
  }

// Set value of 2 TFT registers: Two 8-bit addresses (hi & lo), 16-bit value
#define writeRegisterPairInline(aH, aL, d)                                     \
  {                                                                            \
    uint8_t hi = (d) >> 8, lo = (d);                                           \
    CD_COMMAND;                                                                \
    write8(aH);                                                                \
    CD_DATA;                                                                   \
    write8(hi);                                                                \
    CD_COMMAND;                                                                \
    write8(aL);                                                                \
    CD_DATA;                                                                   \
    write8(lo);                                                                \
  }

#define writeRegister8 writeRegister8inline
#define writeRegister16 writeRegister16inline
#define writeRegisterPair writeRegisterPairInline

#endif // _pin_magic_
