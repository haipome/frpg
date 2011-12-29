#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define LEN 1024

char * help = "Formatting random password generator\n"
			  "-l n: lowercase letters (a-z) of n times\n"
			  "-u n: uppercase letters (A-Z)of n times\n"
			  "-n n: digital number (1-9) of n times\n"
			  "-s n: signs such !#$%&... of n times\n"
			  "-h: print help\n"
			  "-v: print version";
			  
char * version = "Formatting random password generator\n"
				 "version: 0.1\n"
				 "author: yang@haipo.me";

struct memory {
	char * p;
	int i;
	size_t len;
};

void process(struct memory * m, char * s, char fun())
{
	int n, i;
	if ((n = atoi(s)) > 0) {
		for (i = 0; i < n; ++i) {
			if (m->i >= m->len) {
				m->len *= 2;
				m->p = realloc(m->p, m->len);
			}
			(m->p)[m->i] = fun();
			m->i += 1;
		}
	}
}

int getrand(int max)
{
	return rand() % max;
}

char getlower()
{
	return (char)(getrand(26) + 97);
}

char getupper()
{
	return (char)(getrand(26) + 65);
}

char getdigit()
{
	return (char)(getrand(10) + 48);
}

char getsign()
{
	int i = getrand(3);
	switch(i) {
		case 0:
			return (char)(getrand(15) + 33);
		case 1:
			return (char)(getrand(6) + 91);
		case 2:
			return (char)(getrand(4) + 123);
	}
}


int main(int argc, char *argv[])
{
	FILE * f;
	unsigned int seed;
	if ((f = fopen("randseed.txt", "r")) != NULL) {
		fscanf(f, "%u", &seed);
		fclose(f);
	} else {
		seed = (unsigned int)time(NULL);
	}
	srand(seed);
	
	struct memory m;
	m.len = LEN;
	m.i = 0;
	m.p = malloc(m.len);
	
	int c;
	while ((c = getopt(argc, argv, "l:u:n:s:hv")) != -1) {
		switch(c) {
			case 'l':
				process(&m, optarg, getlower);
				break;
			case 'u':
				process(&m, optarg, getupper);
				break;
			case 'n':
				process(&m, optarg, getdigit);
				break;
			case 's':
				process(&m, optarg, getsign);
				break;
			case 'h':
				printf("%s", help);
				break;
			case 'v':
				printf("%s", version);
				break;
			case '?':
				printf("There Must got some thing wrong\n");
				printf("%s", help);
				return opterr;
			default:
				abort();
		}
	}
	
	(m.p)[m.i] = '\0';
	printf("%s\n", m.p);
	
	if ((f = fopen("randseed.txt", "w+")) != NULL) {
		fprintf(f, "%d", rand());
		fclose(f);
	}
	
	return 0;
}
