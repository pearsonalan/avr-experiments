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

avr_t *avr = NULL;
avr_vcd_t vcd_file;

void dump_mem()
{
	static int dump_number = 0;
	char filename[256];
	FILE *f = NULL;
	size_t n;

	sprintf(filename, "dump.%d.out", dump_number++);
	f = fopen(filename, "w");
	if (f != NULL)
	{
		printf("Dumping %d bytes of memory to %s\n", avr->ramend, filename);
		n = fwrite(avr->data, 1, avr->ramend, f);
		if (n != avr->ramend)
		{
			printf("Error dumping memory\n");
		}
		fclose(f);
	}
	else
	{
		printf("Cannot open %s\n", filename);
	}
}

void pin_changed_hook(struct avr_irq_t *irq, uint32_t value, void *param)
{
	printf("Pin change @%ld: %s => %d\n", (long) avr->cycle, irq->name, value);
	if (value == 1)
	{
		/* if the pin was set high, use that as a signal to dump the memory */
		dump_mem();
	}
}

static void uart_output_cb(struct avr_irq_t *irq, uint32_t value, void *param)
{
	fputc(value, stdout);
}

void sig_int(int sign)
{
	printf("signal caught, simavr terminating\n");
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

avr_cycle_count_t termination_timer(struct avr_t *avr, avr_cycle_count_t when, void *param)
{
	printf("*** Termination timer called at %ld (avr cycle %ld) ***\n", (long) when, (long) avr->cycle);
	avr->state = cpu_Done;
	return 0;
}

int main(int argc, char *argv[])
{
	int state;
	int gdb = 0;
	ihex_chunk_p chunk = NULL;
	int ci, cnt;
	const char *mmcu = "atmega328p";
	const char *fname, *progname;
	
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

	/* IRQ callback hooks */
	avr_irq_register_notify(avr_io_getirq(avr, AVR_IOCTL_IOPORT_GETIRQ('B'), 0), pin_changed_hook, NULL); 

	/* hook up the UART to stdout */
	avr_irq_register_notify(avr_io_getirq(avr, AVR_IOCTL_UART_GETIRQ('0'), UART_IRQ_OUTPUT),
		uart_output_cb, NULL);

	/*	VCD file initialization */
	printf("*** Initializing VCD Output ***\n");
	avr_vcd_init(avr, "wave.vcd", &vcd_file, 10000 /* usec */);
	avr_vcd_add_signal(&vcd_file, avr_io_getirq(avr, AVR_IOCTL_IOPORT_GETIRQ('B'), 0), 1, "B0" );
	avr_vcd_add_signal(&vcd_file, avr_io_getirq(avr, AVR_IOCTL_IOPORT_GETIRQ('D'), 4), 1, "D4" );
	avr_vcd_start(&vcd_file);

	/* set up interrupt handlers */
	signal(SIGINT, sig_int);
	signal(SIGTERM, sig_int);

	/* register a timer to terminate the loop */
	avr_cycle_timer_register_usec(avr, 1000000, termination_timer, NULL);

	/* show port info */
	show_ports(avr);

	printf("*** Entering main loop ***\n");
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

	printf("*** Terminating ***\n");
	avr_vcd_stop(&vcd_file);
	avr_terminate(avr);

	return 0;
}

