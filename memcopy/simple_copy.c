#define BUFSZ 32

char a[BUFSZ];
char b[BUFSZ];

int main()
{
	int i;

	for (i = 0; i < BUFSZ; i++)
		a[i] = (char)i;

	for (i = 0; i < BUFSZ; i++)
		b[i] = a[i];

	return 0;
}
