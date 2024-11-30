/*
 rotenc_test.c

 Copyright 2018 Doug Szumski <d.s.szumski@gmail.com>

 Based on i2ctest.c by:

 Copyright 2008-2011 Michel Pollet <buserror@gmail.com>

 This file is part of simavr.

 simavr is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 simavr is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with simavr.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <libgen.h>
#if __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <pthread.h>
#include <curses.h>

#include "sim_avr.h"
#include "sim_gdb.h"
#include "sim_hex.h"
#include "sim_elf.h"
#include "parts/rotenc.h"
#include "avr_ioport.h"
#include "sim_vcd_file.h"

avr_t * avr = NULL;
avr_vcd_t vcd_file;
rotenc_t rotenc;

volatile int state = cpu_Running;
volatile unsigned char key_g;
uint8_t pin_state_g = 0b00010000;

float pixsize = 64;

/*
 * Called when the AVR changes any of the pins on port D. We use this to
 * hook up these pins to the virtual 'LED bar'.
 */
void pin_changed_hook(struct avr_irq_t * irq, uint32_t value, void * param) {
  pin_state_g = (pin_state_g & ~(1 << irq->irq)) | (value << irq->irq);
}

void displayCB(void) {
  glClear(GL_COLOR_BUFFER_BIT);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  float grid = pixsize;
  float size = grid * 0.8;
  glBegin(GL_QUADS);
  glColor3f(1, 0, 0);

  for (int di = 0; di < 8; di++) {
    char on = (pin_state_g & (1 << di)) != 0;
    if (on) {
      float x = (di) * grid;
      float y = 0;
      glVertex2f(x + size, y + size);
      glVertex2f(x, y + size);
      glVertex2f(x, y);
      glVertex2f(x + size, y);
    }
  }

  glEnd();
  glutSwapBuffers();
  glFlush();
}

void keyCB(unsigned char key, int x, int y) {
  if (key == 'q')
    exit(0);

  switch (key) {
    case 'q':
    case 0x1f: // escape
      exit(0);
      break;
    default:
      // Pass key to avr thread
      key_g = key;
      break;
  }
}

void timerCB(int i) {
  static uint8_t oldstate = 0b00010000;

  // Restart timer
  glutTimerFunc(1000 / 64, timerCB, 0);

  // Only update if the pin changed state
  if (oldstate != pin_state_g) {
    oldstate = pin_state_g;
    glutPostRedisplay();
  }
}

static void * avr_run_thread(void * ignore) {
  state = cpu_Running;

  while ((state != cpu_Done) && (state != cpu_Crashed)) {
    if (key_g != 0) {
      switch (key_g) {
        case 'j':
          rotenc_twist(&rotenc, ROTENC_CCW_CLICK);
          break;
        case 'k':
          rotenc_button_press(&rotenc);
          break;
        case 'l':
          rotenc_twist(&rotenc, ROTENC_CW_CLICK);
          break;
        default:
          // ignore
          break;
      }
      key_g = 0;
    }
    state = avr_run(avr);
  }
  return NULL ;
}

int avr_load_chunks(avr_t *avr, ihex_chunk_p chunk, int cnt) {
  int i;
  uint32_t loadBase = AVR_SEGMENT_OFFSET_FLASH;

  printf("Loading %d chunks into AVR\n", cnt);
  for (i = 0; i < cnt; i++) {
    printf("Section %d has baseaddr %08x\n", i, chunk[i].baseaddr);
    if (chunk[i].baseaddr < (1*1024*1024)) {
      avr_loadcode(avr, chunk[i].data, chunk[i].size, chunk[i].baseaddr);
      avr->codeend = chunk[i].size + chunk[i].baseaddr;
      printf("Load HEX flash @ %08x (%d bytes)\n", chunk[i].baseaddr, chunk[i].size);
    } else if (chunk[i].baseaddr >= AVR_SEGMENT_OFFSET_EEPROM ||
               chunk[i].baseaddr + loadBase >= AVR_SEGMENT_OFFSET_EEPROM) {
#if 0
      /* load an eeprom file */
      f->eeprom = chunk[ci].data;
      f->eesize = chunk[ci].size;
      printf("Load HEX eeprom %08x, %d\n", chunk[ci].baseaddr, f->eesize);
#endif
    }
  }

  return 0;
}

int main(int argc, char *argv[]) {
  int state;
  int gdb = 0;
  ihex_chunk_p chunk = NULL;
  int ci, cnt;
  const char *mmcu = "atmega328p";
  const char *fname, *progname;
  
  progname = *argv;

  for (--argc, ++argv; argc; --argc, ++argv) {
    if (**argv != '-')
      break;
    if (strcmp(*argv, "-g") == 0 || strcmp(*argv, "--gdb") == 0)
      gdb = 1;
  }

  if (argc == 0) {
    fprintf(stderr, "usage: %s hexfile\n", progname);
    exit(1);
  }
  fname = *argv;

  printf("*** Creating MCU for %s ***\n", mmcu);
  avr = avr_make_mcu_by_name(mmcu);
  if (!avr) {
    fprintf(stderr, "%s: AVR '%s' not known\n", progname, mmcu);
    exit(1);
  }

  printf("*** Loading HEX file (%s) ***\n", fname);
  cnt = read_ihex_chunks(fname, &chunk);
  if (cnt <= 0) {
    fprintf(stderr, "Unable to load IHEX file %s\n", fname);
    exit(2);
  }

  printf("*** Initializing AVR ***\n");
  avr_init(avr);
  avr->frequency = 16000000L;
  avr->vcc = 5.0;
  avr->avcc = 5.0;
  avr->aref = 5.0;

  printf("*** Loading flash memory into AVR ***\n");
  avr_load_chunks(avr, chunk, cnt);

  /* activate gdb if requested */
  if (gdb) {
    avr->gdb_port = 1234;
    avr->state = cpu_Stopped;
    avr_gdb_init(avr);
  }

  printf(
    "ROTARY ENCODER LED bar demo (press q to quit):\n\n"
    "Press 'j' for CCW turn \n"
    "Press 'l' for CW turn \n"
    "Press 'k' for button click \n\n");

  // Initialise rotary encoder
  rotenc_init(avr, &rotenc);
  rotenc.verbose = 1;

  // RE GPIO
  avr_connect_irq(
    rotenc.irq + IRQ_ROTENC_OUT_A_PIN,
    avr_io_getirq(avr, AVR_IOCTL_IOPORT_GETIRQ('B'), 0));

  // RE INT
  avr_connect_irq(
    rotenc.irq + IRQ_ROTENC_OUT_B_PIN,
    avr_io_getirq(avr, AVR_IOCTL_IOPORT_GETIRQ('B'), 1));

  // INT (button)
  // Pull up
  avr_raise_irq(rotenc.irq + IRQ_ROTENC_OUT_BUTTON_PIN, 1);
  avr_connect_irq(
    rotenc.irq + IRQ_ROTENC_OUT_BUTTON_PIN,
    avr_io_getirq(avr, AVR_IOCTL_IOPORT_GETIRQ('C'), 0));

  // VCD output
  avr_vcd_init(avr, "gtkwave_output.vcd", &vcd_file, 1000 /* usec */);
  avr_vcd_add_signal(
    &vcd_file,
    avr_io_getirq(avr, AVR_IOCTL_IOPORT_GETIRQ('B'), 0), 1,
    "INT0");
  avr_vcd_add_signal(&vcd_file, rotenc.irq + IRQ_ROTENC_OUT_A_PIN, 1, "A");
  avr_vcd_add_signal(&vcd_file, rotenc.irq + IRQ_ROTENC_OUT_B_PIN, 1, "B");
  avr_vcd_start(&vcd_file);

  // Connect all the pins on port D to our callback. This is the 'LED bar'.
  for (int i = 0; i < 8; i++) {
    avr_irq_register_notify(
      avr_io_getirq(avr, AVR_IOCTL_IOPORT_GETIRQ('D'), i),
      pin_changed_hook,
      NULL);
  }

  // Set the button in the 'up' state
  avr_raise_irq(rotenc.irq + IRQ_ROTENC_OUT_BUTTON_PIN, 1);

  // Start the encoder at phase 1
  avr_raise_irq(rotenc.irq + IRQ_ROTENC_OUT_A_PIN, 0);
  avr_raise_irq(rotenc.irq + IRQ_ROTENC_OUT_B_PIN, 0);

  /*
   * OpenGL init, can be ignored
   */
  glutInit(&argc, argv);

  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
  glutInitWindowSize(8 * pixsize, 1 * pixsize);
  glutCreateWindow("Glut");

  // Set up projection matrix
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, 8 * pixsize, 0, 1 * pixsize, 0, 10);
  glScalef(1, -1, 1);
  glTranslatef(0, -1 * pixsize, 0);

  glutDisplayFunc(displayCB);
  glutKeyboardFunc(keyCB);
  glutTimerFunc(1000 / 24, timerCB, 0);

  pthread_t run;
  pthread_create(&run, NULL, avr_run_thread, NULL);

  glutMainLoop();

  printf("quit\n");
}
