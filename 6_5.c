#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct nlist nlist;
struct nlist { /* table entry: */
	struct nlist *next; /* next entry in chain */
	char *name; /* defined name */
	char *defn; /* replacement text */
};

#define HASHSIZE 101
static struct nlist *hashtab[HASHSIZE]; /* pointer table */

struct nlist *lookup(char *);
char *strdup1(char *);

char *strdup1(char *s) /* make a duplicate of s */
{
	char *p;
	p = (char *)malloc(strlen(s) + 1); /* +1 for '\0' */
	if (p != NULL)
		strcpy(p, s);
	return p;
}

/* hash: form hash value for string s */
unsigned hash(char *s)
{
	unsigned hashval;
	for (hashval = 0; *s != '\0'; s++)
		hashval = *s + 31 * hashval;

	return hashval % HASHSIZE;
}

/* lookup: look for s in hashtab */
struct nlist *lookup(char *s)
{
	struct nlist *np;
	for (np = hashtab[hash(s)]; np != NULL; np = np->next)
		if (strcmp(s, np->name) == 0)
			return np; /* found */
	return NULL; /* not found */
}

/* install: put (name, defn) in hashtab */
struct nlist *install(char *name, char *defn)
{
	struct nlist *np;
	unsigned hashval;
	if ((np = lookup(name)) == NULL) { /* not found */
		np = (struct nlist *) malloc(sizeof(*np));
		if (np == NULL || (np->name = strdup1(name)) == NULL)
			return NULL;
		hashval = hash(name);
		np->next = hashtab[hashval];
		hashtab[hashval] = np;
	}
	else /* already there */
		free((void *)np->defn); /*free previous defn */
	if ((np->defn = strdup1(defn)) == NULL)
		return NULL;
	return np;
}

void print_defn(const char* s) {
	nlist* p = lookup(s);
	printf("'%s': ", s);
	printf("%s\n", p ? p->defn : "not found");
}

void clear() {
	for (unsigned int i = 0; i < HASHSIZE; ++i)
	{
		while (hashtab[i] != NULL)
		{
			nlist* temp = hashtab[i];
			hashtab[i] = temp->next;
			free(temp);
		}
	}
}

void test_hash_table() {
	printf("%s\n", "// ---------------------- TESTING HASH_TABLE ---------------------");
	install("cat", "animal that likes fish");   
	install("dog", "animal that hates cats");   
	install("mouse", "animal eaten by cats");   
	install("virus", "pain in the neck");    
	print_defn("cat");   
	print_defn("pig");   
	print_defn("dog");   
	print_defn("pig");   
	print_defn("mouse");   
	print_defn("bacteria");   
	print_defn("amoeba");   
	print_defn("paramecium");   
	print_defn("virus");         
	printf("\nredefining cat...\n");   
	printf("redefining virus...\n");   
	install("cat", "animal that likes fish and mice and birds");   
	install("virus", "VERY EXPENSIVE pain in the neck");   
	print_defn("cat");   
	print_defn("virus");
	printf("\n%s\n", "// ----------------END OF TESTING HASH_TABLE ---------------------");
	clear();
}

void undef(char *s) {
	int h;
	struct nlist *prev, *np;

	prev = NULL;
	h = hash(s);
	for (np = hashtab[h]; np != NULL; np = np->next) {
		if (strcmp(s, np->name) == 0)
			break;
		prev = np;
	}
	if (np != NULL) {
		if (prev == NULL)
			hashtab[h] = np->next;
		else
			prev->next = np->next;
		free((void *)np->name);
		free((void *)np->defn);
		free((void *)np);
	}
}

int main(int argc, const char* argv[]) {
	test_hash_table();
	return 0;
}
