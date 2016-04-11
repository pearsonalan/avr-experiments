#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <libgen.h>
#include <signal.h>

#include "sim_avr.h"
#include "avr_ioport.h"
#include "avr_spi.h"
#include "avr_uart.h"
#include "avr_timer.h"
#include "sim_gdb.h"
#include "sim_hex.h"
#include "sim_elf.h"
#include "sim_vcd_file.h"

#if __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <pthread.h>

#include "hd44780_glut.h"

avr_t *avr = NULL;
avr_vcd_t vcd_file;
hd44780_t hd44780;

int color = 0;
uint32_t colors[][4] = {
		{ 0x00aa00ff, 0x00cc00ff, 0x000000ff, 0x00000055 },	/* fluo green */
		{ 0xaa0000ff, 0xcc0000ff, 0x000000ff, 0x00000055 },	/* red        */
};


/* keyCB is called on key press */
void keyCB(unsigned char key, int x, int y)
{
	switch (key) {
	case 'q':
		printf("Quit notification in keyCB\n");
		avr_vcd_stop(&vcd_file);
		exit(0);
		break;

	case 'r':
		printf("Starting VCD trace; press 's' to stop\n");
		avr_vcd_start(&vcd_file);
		break;

	case 's':
		printf("Stopping VCD trace\n");
		avr_vcd_stop(&vcd_file);
		break;
	}
}


/* function called whenever redisplay needed */
void displayCB(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW); /* Select modelview matrix */
	glPushMatrix();
	glLoadIdentity();			/* Start with an identity matrix */
	glScalef(3, 3, 1);

	hd44780_gl_draw(&hd44780,
		colors[color][0],		/* background */
		colors[color][1],		/* character background */
		colors[color][2],		/* text */
		colors[color][3] );		/* shadow */

	glPopMatrix();
    glutSwapBuffers();
}

/* gl timer. if the lcd is dirty, refresh display */
void timerCB(int i)
{
	/* restart timer */
	glutTimerFunc(1000/64, timerCB, 0);
	glutPostRedisplay();
}

int initGL(int w, int h)
{
	/* Set up projection matrix */
	glMatrixMode(GL_PROJECTION);	/* Select projection matrix */
	glLoadIdentity();				/* Start with an identity matrix */
	glOrtho(0, w, 0, h, 0, 10);
	glScalef(1,-1,1);
	glTranslatef(0, -1 * h, 0);

	glutDisplayFunc(displayCB);		/* set window's display callback */
	glutKeyboardFunc(keyCB);		/* set window's key callback */
	glutTimerFunc(1000 / 24, timerCB, 0);

	glEnable(GL_TEXTURE_2D);
	glShadeModel(GL_SMOOTH);

	glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	hd44780_gl_init();

	return 1;
}

static void uart_output_cb(struct avr_irq_t *irq, uint32_t value, void *param)
{
	fputc(value, stdout);
}

void sig_int(int sign)
{
	printf("INT signal caught, simavr terminating\n");
	if (avr)
		avr_terminate(avr);
	exit(0);
}

int avr_load_chunks(avr_t *avr, ihex_chunk_p chunk, int cnt)
{
	int i;
	uint32_t loadBase = AVR_SEGMENT_OFFSET_FLASH;

	printf("Loading %d chunks into AVR\n", cnt);
	for (i = 0; i < cnt; i++)
	{
		printf("Section %d has baseaddr %08x\n", i, chunk[i].baseaddr);
		if (chunk[i].baseaddr < (1*1024*1024))
		{
			avr_loadcode(avr, chunk[i].data, chunk[i].size, chunk[i].baseaddr);
			avr->codeend = chunk[i].size + chunk[i].baseaddr;
			printf("Load HEX flash @ %08x (%d bytes)\n", chunk[i].baseaddr, chunk[i].size);
		}
		else if (chunk[i].baseaddr >= AVR_SEGMENT_OFFSET_EEPROM || chunk[i].baseaddr + loadBase >= AVR_SEGMENT_OFFSET_EEPROM)
		{
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

void show_ports(avr_t *avr)
{
	avr_io_t * port = avr->io_port;
	while (port)
	{
		printf("%s %s %08x %d\n", port->kind, port->irq_names ? *port->irq_names : NULL, port->irq_ioctl_get, port->irq_count);
		port = port->next;
	}
}

static void * avr_run_thread(void * ignore)
{
	int state;

	while (1) 
	{
		state = avr_run(avr);
		if (state == cpu_Done)
		{
			printf("*** Exiting Main Loop: CPU done (state %d) ***\n", state);
			break;
		}
		else if (state == cpu_Crashed)
		{
			printf("!!! Exiting Main Loop CPU crashed (state %d) !!!\n", state);
			break;
		}
	}

	printf("*** Terminating avr thread ***\n");
	avr_vcd_stop(&vcd_file);
	avr_terminate(avr);
	return NULL;
}

int main(int argc, char *argv[])
{
	int gdb = 0;
	ihex_chunk_p chunk = NULL;
	int i, ci, cnt;
	const char *mmcu = "atmega328p";
	const char *fname, *progname;
	int window;
	
	progname = *argv;

	for (--argc, ++argv; argc; --argc, ++argv)
	{
		if (**argv != '-')
			break;
		if (strcmp(*argv, "-g") == 0 || strcmp(*argv, "--gdb") == 0)
			gdb = 1;
	}

	if (argc == 0) 
	{
		fprintf(stderr, "usage: %s hexfile\n", progname);
		exit(1);
	}
	fname = *argv;

	printf("*** Creating MCU for %s ***\n", mmcu);
	avr = avr_make_mcu_by_name(mmcu);
	if (!avr)
	{
		fprintf(stderr, "%s: AVR '%s' not known\n", progname, mmcu);
		exit(1);
	}

	printf("*** Loading HEX file (%s) ***\n", fname);
	cnt = read_ihex_chunks(fname, &chunk);
	if (cnt <= 0)
	{
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
	if (gdb)
	{
		avr->gdb_port = 1234;
		avr->state = cpu_Stopped;
		avr_gdb_init(avr);
	}

	hd44780_init(avr, &hd44780, 20, 2);

	/* Connect Data Lines to Port B, 0-3 */
	for (i = 0; i < 4; i++) {
		avr_irq_t * iavr = avr_io_getirq(avr, AVR_IOCTL_IOPORT_GETIRQ('B'), i);
		avr_irq_t * ilcd = hd44780.irq + IRQ_HD44780_D4 + i;

		/* These are bidirectional too:
		 *  connect AVR -> LCD
		 *  connect LCD -> AVR
		 */
		avr_connect_irq(iavr, ilcd);
		avr_connect_irq(ilcd, iavr);
	}
	avr_connect_irq(
			avr_io_getirq(avr, AVR_IOCTL_IOPORT_GETIRQ('B'), 4),
			hd44780.irq + IRQ_HD44780_RS);
	avr_connect_irq(
			avr_io_getirq(avr, AVR_IOCTL_IOPORT_GETIRQ('B'), 5),
			hd44780.irq + IRQ_HD44780_E);
	avr_connect_irq(
			avr_io_getirq(avr, AVR_IOCTL_IOPORT_GETIRQ('B'), 6),
			hd44780.irq + IRQ_HD44780_RW);

	/*	VCD file initialization */
	printf("*** Initializing VCD Output ***\n");
	avr_vcd_init(avr, "wave.vcd", &vcd_file, 10 /* usec */);

	avr_vcd_add_signal(&vcd_file, avr_io_getirq(avr, AVR_IOCTL_IOPORT_GETIRQ('D'), 4), 1, "D4" );
	avr_vcd_add_signal(&vcd_file,
			avr_io_getirq(avr, AVR_IOCTL_IOPORT_GETIRQ('B'), IOPORT_IRQ_PIN_ALL),
			4 /* bits */, "D4-D7");
	avr_vcd_add_signal(&vcd_file,
			avr_io_getirq(avr, AVR_IOCTL_IOPORT_GETIRQ('B'), 4),
			1 /* bits */, "RS");
	avr_vcd_add_signal(&vcd_file,
			avr_io_getirq(avr, AVR_IOCTL_IOPORT_GETIRQ('B'), 5),
			1 /* bits */, "E");
	avr_vcd_add_signal(&vcd_file,
			avr_io_getirq(avr, AVR_IOCTL_IOPORT_GETIRQ('B'), 6),
			1 /* bits */, "RW");
	avr_vcd_add_signal(&vcd_file,
			hd44780.irq + IRQ_HD44780_BUSY,
			1 /* bits */, "LCD_BUSY");
	avr_vcd_add_signal(&vcd_file,
			hd44780.irq + IRQ_HD44780_ADDR,
			7 /* bits */, "LCD_ADDR");
	avr_vcd_add_signal(&vcd_file,
			hd44780.irq + IRQ_HD44780_DATA_IN,
			8 /* bits */, "LCD_DATA_IN");
	avr_vcd_add_signal(&vcd_file,
			hd44780.irq + IRQ_HD44780_DATA_OUT,
			8 /* bits */, "LCD_DATA_OUT");

	avr_vcd_start(&vcd_file);

	/* set up interrupt handlers */
	signal(SIGINT, sig_int);
	signal(SIGTERM, sig_int);

	/* show port info */
	show_ports(avr);

	glutInit(&argc, argv);		/* initialize GLUT system */

	int w = 5 + hd44780.w * 6;
	int h = 5 + hd44780.h * 8;
	int pixsize = 3;

	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(w * pixsize, h * pixsize);
	window = glutCreateWindow("Press 'q' to quit");	/* create window */

	initGL(w * pixsize, h * pixsize);

	pthread_t run;
	pthread_create(&run, NULL, avr_run_thread, NULL);

	printf("*** Entering main GL loop ***\n");
	glutMainLoop();
	printf("*** Exiting main GL loop ***\n");

	return 0;
}

