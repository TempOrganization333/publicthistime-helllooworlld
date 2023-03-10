


/*=======================================================================
  A simple parser for "self" format

  The circuit format (called "self" format) is based on outputs of
  a ISCAS 85 format translator written by Dr. Sandeep Gupta.
  The format uses only integers to represent circuit information.
  The format is as follows:

1        2        3        4           5           6 ...
------   -------  -------  ---------   --------    --------
0 GATE   outline  0 IPT    #_of_fout   #_of_fin    inlines
                  1 BRCH
                  2 XOR(currently not implemented)
                  3 OR
                  4 NOR
                  5 NOT
                  6 NAND
                  7 AND

1 PI     outline  0        #_of_fout   0

2 FB     outline  1 BRCH   inline

3 PO     outline  2 - 7    0           #_of_fin    inlines




                                    Author: Chihang Chen
                                    Date: 9/16/94

=======================================================================*/

/*=======================================================================
  - Write your program as a subroutine under main().
    The following is an example to add another command 'lev' under main()

enum e_com {READ, PC, HELP, QUIT, LEV};
#define NUMFUNCS 5
int cread(), pc(), quit(), lev();
struct cmdstruc command[NUMFUNCS] = {
   {"READ", cread, EXEC},
   {"PC", pc, CKTLD},
   {"HELP", help, EXEC},
   {"QUIT", quit, EXEC},
   {"LEV", lev, CKTLD},
};

lev()
{
   ...
}
=======================================================================*/

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#define MAXLINE 81               /* Input buffer size */
#define MAXNAME 31               /* File name size */

#define Upcase(x) ((isalpha(x) && islower(x))? toupper(x) : (x))
#define Lowcase(x) ((isalpha(x) && isupper(x))? tolower(x) : (x))

enum e_com {READ, PC, HELP, QUIT, LEV};
enum e_state {EXEC, CKTLD};         /* Gstate values */
enum e_ntype {GATE, PI, FB, PO};    /* column 1 of circuit format */
enum e_gtype {IPT, BRCH, XOR, OR, NOR, NOT, NAND, AND};  /* gate types */

struct cmdstruc {
   char name[MAXNAME];        /* command syntax */
   int (*fptr)();             /* function pointer of the commands */
   enum e_state state;        /* execution state sequence */
};

typedef struct n_struc {
   unsigned indx;             /* node index(from 0 to NumOfLine - 1 */
   unsigned num;              /* line number(May be different from indx */
   enum e_gtype type;         /* gate type */
   unsigned fin;              /* number of fanins */
   unsigned fout;             /* number of fanouts */
   struct n_struc **unodes;   /* pointer to array of up nodes */
   struct n_struc **dnodes;   /* pointer to array of down nodes */
   int level;                 /* level of the gate output */
} NSTRUC;                     

/*----------------- Command definitions ----------------------------------*/
#define NUMFUNCS 5
int cread(), pc(), help(), quit(), lev();
struct cmdstruc command[NUMFUNCS] = {
   {"READ", cread, EXEC},
   {"PC", pc, CKTLD},
   {"HELP", help, EXEC},
   {"QUIT", quit, EXEC},
   {"LEV", lev, CKTLD},
};

/*------------------------------------------------------------------------*/
enum e_state Gstate = EXEC;     /* global exectution sequence */
NSTRUC *Node;                   /* dynamic array of nodes */
NSTRUC **Pinput;                /* pointer to array of primary inputs */
NSTRUC **Poutput;               /* pointer to array of primary outputs */
int Nnodes;                     /* number of nodes */
int Npi;                        /* number of primary inputs */
int Npo;                        /* number of primary outputs */
int Done = 0;                   /* status bit to terminate program */
/*------------------------------------------------------------------------*/
char circuit_name[MAXLINE];

char *strstrip(char *s);
/*-----------------------------------------------------------------------
input: nothing
output: nothing
called by: shell
description:
  This is the main program of the simulator. It displays the prompt, reads
  and parses the user command, and calls the corresponding routines.
  Commands not reconized by the parser are passed along to the shell.
  The command is executed according to some pre-determined sequence.
  For example, we have to read in the circuit description file before any
  action commands.  The code uses "Gstate" to check the execution
  sequence.
  Pointers to functions are used to make function calls which makes the
  code short and clean.
-----------------------------------------------------------------------*/
main()
{
   enum e_com com;
   char cline[MAXLINE], wstr[MAXLINE], *cp;

   while(!Done) {
      printf("\nCommand>");
      fgets(cline, MAXLINE, stdin);
      if(sscanf(cline, "%s", wstr) != 1) continue;
      cp = wstr;
      while(*cp){
	*cp= Upcase(*cp);
	cp++;
      }
      cp = cline + strlen(wstr);
      com = READ;
      while(com < NUMFUNCS && strcmp(wstr, command[com].name)) com++;
      if(com < NUMFUNCS) {
         if(command[com].state <= Gstate) (*command[com].fptr)(cp);
         else printf("Execution out of sequence!\n");
      }
      else system(cline);
   }
}

/*-----------------------------------------------------------------------
input: circuit description file name
output: nothing
called by: main
description:
  This routine reads in the circuit description file and set up all the
  required data structure. It first checks if the file exists, then it
  sets up a mapping table, determines the number of nodes, PI's and PO's,
  allocates dynamic data arrays, and fills in the structural information
  of the circuit. In the ISCAS circuit description format, only upstream
  nodes are specified. Downstream nodes are implied. However, to facilitate
  forward implication, they are also built up in the data structure.
  To have the maximal flexibility, three passes through the circuit file
  are required: the first pass to determine the size of the mapping table
  , the second to fill in the mapping table, and the third to actually
  set up the circuit information. These procedures may be simplified in
  the future.
-----------------------------------------------------------------------*/
cread(cp)
char *cp;
{
   char buf[MAXLINE];
   int ntbl, *tbl, i, j, k, nd, tp, fo, fi, ni = 0, no = 0;
   FILE *fd;
   NSTRUC *np;

   sscanf(cp, "%s", buf);
   
   //char *strncpy(char *circuit_name,const char *cp, size_t circuit_name_length);
   //char *strncpy(char *circuit_name,const char *cp,size_t circuit_name_length );
    char *cp_copy;
    cp_copy= strstrip(cp);
    printf("copy is \'%s\'\n", cp_copy);
    
    char *folder_name="./circuits/";
    char *folder_name2="./";
    char *strcomp = strstr(cp_copy,folder_name);
    if (strcomp==NULL){
        
    }
    else{
        cp_copy+=11;
    }
    char *strcomp2 = strstr(cp_copy,folder_name2);
    if (strcomp2==NULL){
        
    }
    else{
        cp_copy+=2;
    }
   	//cp++;
	//cp[strlen(cp)-1]=0;
	int circuit_name_length=strlen(cp_copy);
	int circuit_name_length_minus_4=circuit_name_length-4;
	// circuit_name = malloc(circuit_name_length_minus_4*sizeof(char));
    memset(circuit_name, 0, MAXLINE);
	for(i=0;i<circuit_name_length_minus_4;i++){
		circuit_name[i]=cp_copy[i];
	}
    
    printf("(read)circuit_name=%s\n",circuit_name);
    printf("(read)length=%d\n",strlen(circuit_name));
	/*
   printf("(read)circuit_name=%s\n",circuit_name);
   printf("(read)cp=%s\n",cp);
   printf("(read)buf=%s\n",buf);
   printf("(read)strlen(cp)=%d\n",circuit_name_length);
   printf("(read)strlen(cp)-4=%d\n",circuit_name_length_minus_4);*/
   
   if((fd = fopen(buf,"r")) == NULL) {
      printf("File %s does not exist!\n", buf);
      return;
   }
   if(Gstate >= CKTLD) clear();
   Nnodes = Npi = Npo = ntbl = 0;
   while(fgets(buf, MAXLINE, fd) != NULL) {
      if(sscanf(buf,"%d %d", &tp, &nd) == 2) {
         if(ntbl < nd) ntbl = nd;
         Nnodes ++;
         if(tp == PI) Npi++;
         else if(tp == PO) Npo++;
      }
   }
   tbl = (int *) malloc(++ntbl * sizeof(int));

   fseek(fd, 0L, 0);
   i = 0;
   while(fgets(buf, MAXLINE, fd) != NULL) {
      if(sscanf(buf,"%d %d", &tp, &nd) == 2) tbl[nd] = i++;
   }
   allocate();

   fseek(fd, 0L, 0);
   while(fscanf(fd, "%d %d", &tp, &nd) != EOF) {
      np = &Node[tbl[nd]];
      np->num = nd;
      if(tp == PI) Pinput[ni++] = np;
      else if(tp == PO) Poutput[no++] = np;
      switch(tp) {
         case PI:
         case PO:
         case GATE:
            fscanf(fd, "%d %d %d", &np->type, &np->fout, &np->fin);
            break;
         
         case FB:
            np->fout = np->fin = 1;
            fscanf(fd, "%d", &np->type);
            break;

         default:
            printf("Unknown node type!\n");
            exit(-1);
         }
      np->unodes = (NSTRUC **) malloc(np->fin * sizeof(NSTRUC *));
      np->dnodes = (NSTRUC **) malloc(np->fout * sizeof(NSTRUC *));
      for(i = 0; i < np->fin; i++) {
         fscanf(fd, "%d", &nd);
         np->unodes[i] = &Node[tbl[nd]];
         }
      for(i = 0; i < np->fout; np->dnodes[i++] = NULL);
      }
   for(i = 0; i < Nnodes; i++) {
      for(j = 0; j < Node[i].fin; j++) {
         np = Node[i].unodes[j];
         k = 0;
         while(np->dnodes[k] != NULL) k++;
         np->dnodes[k] = &Node[i];
         }
      }
   fclose(fd);
   Gstate = CKTLD;
   printf("==> OK\n");
}

/*-----------------------------------------------------------------------
input: nothing
output: nothing
called by: main
description:
  The routine prints out the circuit description from previous READ command.
-----------------------------------------------------------------------*/
pc(cp)
char *cp;
{
   int i, j;
   NSTRUC *np;
   char *gname();
   
   printf(" Node   Type \tIn     \t\t\tOut    \n");
   printf("------ ------\t-------\t\t\t-------\n");
   for(i = 0; i<Nnodes; i++) {
      np = &Node[i];
      printf("\t\t\t\t\t");
      for(j = 0; j<np->fout; j++) printf("%d ",np->dnodes[j]->num);
      printf("\r%5d  %s\t", np->num, gname(np->type));
      for(j = 0; j<np->fin; j++) printf("%d ",np->unodes[j]->num);
      printf("\n");
   }
   printf("Primary inputs:  ");
   for(i = 0; i<Npi; i++) printf("%d ",Pinput[i]->num);
   printf("\n");
   printf("Primary outputs: ");
   for(i = 0; i<Npo; i++) printf("%d ",Poutput[i]->num);
   printf("\n\n");
   printf("Number of nodes = %d\n", Nnodes);
   printf("Number of primary inputs = %d\n", Npi);
   printf("Number of primary outputs = %d\n", Npo);
}

lev(cp)
char *cp;
{
	int i,j;
	NSTRUC *np;
	int flag_in,flag_out;
	int max_level;
	int count_minus_one=Nnodes;
	int test_number=0;
	char *gname();
	FILE *pFile;//cp is file name
	//printf("%p %s",*cp,cp);
	int gate_count;

	for(i = 0; i<Nnodes; i++) {
		np = &Node[i];
		np->level=-1;
	}
	for(i=0;i<Npi;i++){
		Pinput[i]->level=0;
	}
	/*
	for(i = 0; i<Nnodes; i++) {
		np = &Node[i];
		printf("%d ",np->level);
	}*/
	
	printf("\n");/*
	np = &Node[16];
	printf("%d %d %d %d %s\n",np->num,np->fin,np->fout,np->level,gname(np->type));
	for(j = 0; j<np->fin; j++) {
		printf("%d ",np->unodes[j]->num);
	}
	printf("\n");	
	for(j = 0; j<np->fout; j++) {
		printf("%d ",np->dnodes[j]->num);
	}
	printf("\n");	*/
	/*
	np=&Node[0];
	printf("%d %d",np->fin, np->fout);*/
/*	
	while(count_minus_one>0){
		for(i = 0; i<Nnodes; i++) {
			np = &Node[i];
			flag_in=0;
			max_level=0;
			for(j = 0; j<np->fin; j++) {
				if(np->unodes[j]->level == -1){
					flag_in=1;
				}
			}
			for(j = 0; j<np->fin; j++) {
				if(np->unodes[j]->level > max_level){
					max_level = np->unodes[j]->level;
				}
			}
			if(flag_in==0){
				for(j = 0; j<np->fout; j++) np->dnodes[j]->level=(max_level+1);
			}
		}
		count_minus_one=Nnodes;
		for(i = 0; i<Nnodes; i++) {
			np = &Node[i];
			if(np->level==-1) count_minus_one-=1;
		}
	}*/
	/*
	for(i = 0; i<Nnodes; i++) {
		np = &Node[i];
		printf("%d %d %d %d",np->num,np->fin,np->fout,np->level);
		printf("\n");
	}*/
	while(count_minus_one>0){
		for(i = 0; i<Nnodes; i++) {
			np = &Node[i];
			flag_in=0;
			flag_out=0;
			max_level=0;
			for(j = 0; j<np->fin; j++) {
				if(np->unodes[j]->level == -1){
					flag_in=1;
				}
			}
			for(j = 0; j<np->fout; j++){
				if(np->dnodes[j]->level == -1){
					flag_out=1;
				}
			}
			if(np->fout==0){
				flag_out=1;
			}
			for(j = 0; j<np->fin; j++) {
				if(np->unodes[j]->level > max_level){
					max_level = np->unodes[j]->level;
				}
			}
			if(flag_in==0 && flag_out==1){
				if(np->type==1){
					np->level=(max_level+1);
				}
				else if(np->type!=1 && np->type!=0){
					np->level=(max_level+1);
				}
			}
			/*for(j = 0; j<Nnodes; j++) {
				np = &Node[j];
				printf("%d ",np->level);
			}
			printf("\n");*/
		}
		count_minus_one=Nnodes;
		//test_number++;
		//if(test_number==15) count_minus_one=0;
		//printf("-----\n");
		for(i = 0; i<Nnodes; i++) {
			np = &Node[i];
			if(np->level!=-1) count_minus_one-=1;
		}
	}
	/*
	for(i = 0; i<Nnodes; i++) {
		np = &Node[i];
		//printf("%d %s",np->level,gname(np->type));
		printf("%d ",np->level);
	}
	printf("\n");*/
	gate_count=0;
	for(i = 0; i<Nnodes; i++) {
		np = &Node[i];
		if(np->type!=1 && np->type!=0){
			gate_count++;
		}
	}
	//char *cp_pointer=cp;
    char *cp_copy;
    cp_copy = strstrip(cp);
    
	//cp++;
	//cp[strlen(cp)-1]=0;
	
	//printf("(lev)circuit_name=%s\n",circuit_name);
	//printf("circuit_name_length=%d\n",strlen(circuit_name));
	
	
	
	/*
	int file_name_length;
	int file_name_lev;
	file_name_lev=strlen(cp_copy);
	file_name_length=file_name_lev;
	//char *out_file = malloc(file_name_length*sizeof(char));
	char *out_file = malloc(file_name_length*sizeof(char));
	for(i=0;i<file_name_lev;i++){
		out_file[i]=cp[i];
	}*/
	/*
	out_file[file_name_lev]='.';
	out_file[file_name_lev+1]='t';
	out_file[file_name_lev+2]='x';
	out_file[file_name_lev+3]='t';*/
	//pFile = fopen(out_file,"w" );
    printf("%s\n",circuit_name);
    printf("length:%d\n",strlen(circuit_name));
    pFile = fopen(cp_copy,"w" );
    if( NULL == pFile ){
        printf( "open failure" );
        return 1;
    }
	else{
		//fprintf(pFile,"%d\n",strlen(cp));
        //fprintf(pFile,"%s\n",cp);
		fprintf(pFile,"%s\n",circuit_name);
		fprintf(pFile,"#PI: %d\n",Npi);
		fprintf(pFile,"#PO: %d\n",Npo);
		fprintf(pFile,"#Nodes: %d\n",Nnodes);
		fprintf(pFile,"#Gates: %d\n",gate_count);
		for(i = 0; i<Nnodes; i++) {
			np = &Node[i];
			fprintf(pFile,"%d %d\n",np->num,np->level);
		}
    }
    fclose(pFile);
	//free(out_file);
}  
/*-----------------------------------------------------------------------
input: nothing
output: nothing
called by: main 
description:
  The routine prints ot help inormation for each command.
-----------------------------------------------------------------------*/
help()
{
   printf("READ filename - ");
   printf("read in circuit file and creat all data structures\n");
   printf("PC - ");
   printf("print circuit information\n");
   printf("HELP - ");
   printf("print this help information\n");
   printf("QUIT - ");
   printf("stop and exit\n");
}

/*-----------------------------------------------------------------------
input: nothing
output: nothing
called by: main 
description:
  Set Done to 1 which will terminates the program.
-----------------------------------------------------------------------*/
quit()
{
   Done = 1;
}

/*======================================================================*/

/*-----------------------------------------------------------------------
input: nothing
output: nothing
called by: cread
description:
  This routine clears the memory space occupied by the previous circuit
  before reading in new one. It frees up the dynamic arrays Node.unodes,
  Node.dnodes, Node.flist, Node, Pinput, Poutput, and Tap.
-----------------------------------------------------------------------*/
clear()
{
   int i;

   for(i = 0; i<Nnodes; i++) {
      free(Node[i].unodes);
      free(Node[i].dnodes);
   }
   free(Node);
   free(Pinput);
   free(Poutput);
   Gstate = EXEC;
}

/*-----------------------------------------------------------------------
input: nothing
output: nothing
called by: cread
description:
  This routine allocatess the memory space required by the circuit
  description data structure. It allocates the dynamic arrays Node,
  Node.flist, Node, Pinput, Poutput, and Tap. It also set the default
  tap selection and the fanin and fanout to 0.
-----------------------------------------------------------------------*/
allocate()
{
   int i;

   Node = (NSTRUC *) malloc(Nnodes * sizeof(NSTRUC));
   Pinput = (NSTRUC **) malloc(Npi * sizeof(NSTRUC *));
   Poutput = (NSTRUC **) malloc(Npo * sizeof(NSTRUC *));
   for(i = 0; i<Nnodes; i++) {
      Node[i].indx = i;
      Node[i].fin = Node[i].fout = 0;
   }
}

/*-----------------------------------------------------------------------
input: gate type
output: string of the gate type
called by: pc
description:
  The routine receive an integer gate type and return the gate type in
  character string.
-----------------------------------------------------------------------*/
char *gname(tp)
int tp;
{
   switch(tp) {
      case 0: return("PI");
      case 1: return("BRANCH");
      case 2: return("XOR");
      case 3: return("OR");
      case 4: return("NOR");
      case 5: return("NOT");
      case 6: return("NAND");
      case 7: return("AND");
   }
}
/*========================= End of program ============================*/

char *strstrip(char *s)
{
        size_t size;
        char *end;

        size = strlen(s);

        if (!size)
                return s;

        end = s + size - 1;
        while (end >= s && isspace(*end))
                end--;
        *(end + 1) = '\0';

        while (*s && isspace(*s))
                s++;

        return s;
}
