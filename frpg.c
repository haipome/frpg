#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define LEN 1024

char * help = 
	"Formatting random password generator\n"
	"\n"
	"-l n: lowercase letters [a-z] of n times\n"
	"-u n: uppercase letters [A-Z]of n times\n"
	"-d n: digital number [1-9] of n times\n"
	"-p n: punctuations such !#$%&... of n times\n"
	"-s n: [A-Za-z] of n times\n"
	"-e n: [a-z0-9] of n times\n"
	"-E n: [A-Z0-9] of n times\n"
	"-w n: [A-Za-z0-9] of n times\n"
	"-a n: [A-Za-z0-9] and punctuations of n times\n"
	"-n n: the number of passwords you need, default one\n"
	"-h: print help\n"
	"-v: print version\n"
	"\n"
	"default -w 8\n"
	;
	
char * version = 
	"Formatting random password generator\n"
	"\n"
	"version: 0.2\n"
	"author: yang@haipo.me\n"
	;

struct memory {
	char * p;
	int i;
	size_t len;
};

struct format {
	char type;
	int n;
	struct format * next;
};

int get_rand(int m)
{
	return rand() % m;
}

char get_lower()
{
	return (char)(get_rand(26) + 97);
}

char get_upper()
{
	return (char)(get_rand(26) + 65);
}

char get_digit()
{
	return (char)(get_rand(10) + 48);
}

char get_punct()
{
	int i = get_rand(25);
	if (i < 15)
		return (char)(get_rand(15) + 33);
	else if (i >= 15 && i < 21)
		return (char)(get_rand(6) + 91);
	else
		return (char)(get_rand(4) + 123);
}

char get_letter()
{
	int i = get_rand(2);
	if (i < 1)
		return get_lower();
	else
		return get_upper();
}

char get_l_d()
{
	int i = get_rand(36);
	if (i < 26)
		return get_lower();
	else
		return get_digit();
}

char get_u_d()
{
	int i = get_rand(36);
	if (i < 26)
		return get_upper();
	else
		return get_digit();
}

char get_letter_digit()
{
	int i = get_rand(62);
	if (i < 26)
		return get_upper();
	else if (i >= 26 && i < 52)
		return get_lower();
	else
		return get_digit();
}

char get_all()
{
	int i = get_rand(87);
	if (i < 62)
		return get_letter_digit();
	else
		return get_punct();
}

void process(struct memory * m, int n, char fun())
{
	int i;
	for (i = 0; i < n; ++i) {
		if (m->i >= m->len) {
			m->len *= 2;
			m->p = realloc(m->p, m->len);
		}
		(m->p)[m->i] = fun();
		m->i += 1;
	}
}

char * get_pwd(struct memory * m, struct format * f)
{
	struct format * tmp = f->next;
	while (tmp != NULL) {
		switch(tmp->type) {
			case 'l':
				process(m, tmp->n, get_lower);
				break;
			case 'u':
				process(m, tmp->n, get_upper);
				break;
			case 'd':
				process(m, tmp->n, get_digit);
				break;
			case 'p':
				process(m, tmp->n, get_punct);
				break;
			case 's':
				process(m, tmp->n, get_letter);
				break;
			case 'e':
				process(m, tmp->n, get_l_d);
				break;
			case 'E':
				process(m, tmp->n, get_u_d);
				break;
			case 'w':
				process(m, tmp->n, get_letter_digit);
				break;
			case 'a':
				process(m, tmp->n, get_all);
				break;
			default:
				break;
		}
		tmp = tmp->next;
	}
	(m->p)[m->i] = '\0';
	return m->p;
}

int get_format(struct format * p, char c, char * s)
{
	struct format * tmp, * end = p;
	while (end->next != NULL) {
		end = end->next;
	}
	tmp = (struct format *)malloc(sizeof(struct format));
	tmp->type = c;
	tmp->n = atoi(s);
	tmp->next = NULL;
	end->next = tmp;
	if (tmp->n)
		return 1;
	else
		return -1;
}

void print_error()
{
	printf("There Must got some thing wrong\n");
	printf("%s", help);
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
	
	struct format * f_start = malloc(sizeof(struct format));
	f_start->next = NULL;
	int num = 1, c, i;
	while ((c = getopt(argc, argv, "l:u:d:p:s:e:E:w:a:n:hv")) != -1) {
		switch(c) {
			case 'l':
			case 'u':
			case 'd':
			case 'p':
			case 's':
			case 'e':
			case 'E':
			case 'w':
			case 'a':
				if (get_format(f_start, c, optarg) < 0) {
					print_error();
					return opterr;
				}
				break;
			case 'n':
				if ((num = atoi(optarg)) > 0)
					break;
				else:
					print_error();
					return -1;
			case 'h':
				printf("%s", help);
				return 0;
			case 'v':
				printf("%s", version);
				return 0;
			case '?':
				print_error();
				return opterr;
			default:
				abort();
		}
	}
	if (f_start->next == NULL) {
		get_format(f_start, 'w', "8");
	}
	
	struct memory * m = malloc(sizeof(struct memory));
	m->len = LEN;
	m->i = 0;
	m->p = malloc(m->len);
	
	for (i = 0; i < num; ++i) {
		m->i = 0;
		printf("%s\n", get_pwd(m, f_start));
	}
	
	if ((f = fopen("randseed.txt", "w+")) != NULL) {
		fprintf(f, "%d", rand());
		fclose(f);
	}
	
	return 0;
}
