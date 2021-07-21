/*Declaração de bibliotecas*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "mono-os.h"


#define MEM_SIZE 512
/*Variaveis para o controle de 
argumentos passados ao programa*/
unsigned NEXT_PROGRAM;
unsigned LAST_PROGRAM;
const char** PROGRAMS;

/*Estrutura de uma lista
 duplamente encadeada, usada no 
gerenciamento de memoria do programa */
typedef struct lists{
	uint32_t  	  base;
	uint32_t 	 limit;
	struct lists *prox;
	struct lists  *ant;

} list;
/*variaveis globais utilizadas*/
list *ocupado;
list *livre;
int ocupado_base=0;


struct s_bcp BCP; /* BCP of program being executed */
/*variaveis referentes da
 fila de acesso a CPU*/
struct s_bcp * process_row;
unsigned id_process_row=0;


/*Função que procura um espaço
livre na memoria para alocar
um processo */
int     search_free(list *lista, int size){
    list *aux;
    aux = lista;

    while(aux!=NULL){

        if((aux->limit - aux->base) >= (unsigned)size)
            return aux->base;
        aux = aux->prox;
    }
    return -1;


}

/*Função responsavel por tornar a lista de
gerenciamento de memoria com alocações contiguas*/
list    *contigous_busy_list_make(list *lista){
    list *aux;
    list *ahd;
    aux = lista;
    while(aux->prox != NULL){

       if(aux->limit == aux->prox->base-1){
            aux->limit = aux->prox->limit;
            ahd = aux;
            ahd = ahd->prox;
            aux->prox = ahd->prox;
            if(ahd->prox != NULL)
                ahd->prox->ant = aux;
       }
       else
          aux = aux->prox;
    }

	while(aux->ant!=NULL){
		aux = aux->ant;
	}
    return aux;
}
/*Função com papel de liberar espaço na lista de 
gerenciamento de memoria*/
list *free_space(list *lista, unsigned base, unsigned size){
    list *aux;
    list *pro;
    list *ant;
    list *nno;
    aux = lista;

    while((aux->base <= base)&&(aux->limit <= base)){
        aux=aux->prox;
    }

    if((aux->base == base)&&(aux->limit == base+size)){
            if((aux->ant == NULL)&&(aux->prox != NULL)){
                aux = aux->prox;
                aux->ant = NULL;
            }
            else if((aux->prox == NULL)&&(aux->ant != NULL)){
                aux = aux->ant;
                aux -> prox = NULL;
            }
            else if((aux->prox == NULL)&&(aux->ant == NULL)){
                aux = NULL;
            }
            else{
                ant = aux->ant;
                pro = aux->prox;
                ant -> prox = pro;
                pro -> ant = ant;
            }
    }
    else if ((aux->base == base)&&(aux->limit != base+size)){
        aux->base = base+size+1;
    }
    else if ((aux->base != base)&&(aux->limit == base+size)){
        aux->limit = base - 1;
    }
    else if ((aux->base != base)&&(aux->limit != base+size)){
        nno = malloc(sizeof(list));
        nno->base = base+size+1;
        nno->limit = aux->limit;
        nno->prox = NULL;
        nno->ant = NULL;

        aux->limit = base - 1;
        nno->prox = aux->prox;
        if(aux->prox != NULL){
            pro = aux->prox;
            pro -> ant =nno;
        }
        aux->prox = nno;
    }
    if(aux!=NULL)
    while(aux->ant!=NULL){
		aux = aux->ant;
	}
	return aux;
}

/*Fução que a partir da lista de 
memoria alocada monta lista de espaços vazios*/
list  	*make_free(list *lista){
	list *aux=NULL;
	list *liv=NULL;
	list *nno=NULL;
	aux = lista;
    nno = malloc(sizeof(list));

    if(aux == NULL){
        liv = malloc(sizeof(list));
        liv->base = 0;
        liv->limit = MEM_SIZE;
        liv->prox = NULL;
        liv->ant = NULL;
        return liv;

    }
    if((aux->base==0)&&(aux->limit==MEM_SIZE-1))
    	return liv;

    if(aux->base > 0){
        liv = malloc(sizeof(list));
        liv->base = 0;
        liv->limit = aux->base - 1;
        liv->prox = NULL;
        liv->ant = NULL;
    }

    while(aux->prox != NULL){

            if (liv==NULL){
                  liv = malloc(sizeof(list));
                  liv->base = aux->limit+1;
                  liv->limit = aux->prox->base - 1;
                  liv->prox = NULL;
                  liv->ant = NULL;
                  aux = aux->prox;
            }
            else{
                nno->base = aux->limit+1;
                nno->limit = aux->prox->base - 1;
                nno->ant = liv;
                nno->prox = NULL;
                liv->prox = nno;
                aux = aux->prox;
                liv = liv->prox;
                nno = NULL;
                nno = malloc(sizeof(list));
            }
    }
    if(aux->limit<MEM_SIZE){
        nno = NULL;
        nno = malloc(sizeof(list));
        nno->base = aux->limit+1;
        nno->limit = MEM_SIZE;
        nno->prox = NULL;
        nno->ant = liv;
        if(liv!=NULL)
            liv->prox =nno;
        else
            liv = nno;
    }

	while(liv->ant!=NULL){
		liv = liv->ant;
	}
	return liv;


}
/*Função de inserção na lista de gerenciamento da memoria*/
list *insertion(list *lista, uint32_t nbase, uint32_t nsize){
	list *aux;
	list *nno;
	aux = lista;
	nno = malloc(sizeof(list));
	nno->base = nbase;
	nno->limit = (nbase + nsize)-1;
	nno->ant = NULL;
	nno->prox = NULL;

	if((nbase>MEM_SIZE-1)||((nbase+nsize)>MEM_SIZE-1))
		return aux;

	if(aux==NULL)
		aux = nno;

	else{
		while(aux!=NULL){
			if((aux->ant == NULL)&&(aux->base > nno->limit)){
                aux->ant = nno;
                nno->prox = aux;
                break;
			}
			if((aux->prox == NULL)&&(aux->limit < nno->base)){
                aux->prox = nno;
                nno->ant = aux;
                break;
			}
			if((aux->limit < nno->base)&&(aux->prox->base > nno->limit)){
                nno->prox = aux->prox;
                nno->ant = aux;
                aux->prox = nno;
                nno->prox->ant = nno;
                break;
			}

			aux = aux->prox;
		}
        while(aux->ant != NULL){
            aux = aux->ant;
        }
	}

	return aux;
}


/* Maps virtual to real address */
void translate_addr(uint32_t* addr)
{
	assert( addr != NULL );

	if( *addr >= REGS.limit )
		printf("\nMONO-OS: erro acessando endereco 0X%X", *addr);

	*addr=*addr + REGS.base;
}
void restore_context();

/* Clock interruption handler */
void clock_int()
{

	/*Momento de preempção*/
    /*Restaura o contexto perante a fila de prontos*/
    restore_context();
	printf("\n\nContexto restaurado do processo %i\n",BCP.pid);
	print_regs();
	program_clock_int(500); /* program next clock interruption (ex.: 500 ticks) */
}

/* Bad instruciton handler */
void bad_instruction()
{
}

/* Division by zero exception handler */
void div_by_zero_exception()
{
}

/* System call handler. The system call number is on r[12] register.
   All programs must do at least the sys_exit() call. Our OS will use the number 0 (zero) for sys_exit(). */
void system_call()
{
	/* Register r[12] has the system call number. Registers r[13-15] have the system call arguments. */

	switch( REGS.r[12] )
	{
		case 0: /* sys_exit() */
			terminate_process();
			break;
		default:
			printf("\nMONO-OS: **Invalid system call**");
	}
	
}

/* Init OS and hardware */
void init_os(int argc, const char** argv)
{
	printf("\nMONO-OS: starting system...\n\n");

	init_arch();

	translate_addr_handler = translate_addr;
	clock_int_handler = clock_int;
	bad_instruction_handler = bad_instruction;
	div_by_zero_exception_handler = div_by_zero_exception;
	system_call_handler = system_call;

	NEXT_PROGRAM = 1;
	LAST_PROGRAM = argc;
	PROGRAMS = argv;	
	process_row=(struct s_bcp*)calloc(sizeof(struct s_bcp),LAST_PROGRAM);
}

/* OS is going down... */
void halt_os()
{
	printf("\nMONO-OS: system halted...\n\n");
}

/* For debugging purposes ... */
void print_mem() 
{
	int i, j;

	/* prints only first 512 bytes */
    for(i=0; i<32; i++) 
    {
		printf("\n%02X: ", i*16);

        for(j=0; j<4; j++)
        {
            printf("%08X", ((uint32_t*) MEMORY)[i*4+j]);

            if( j != 3 )
                printf(" ");
        }
    }
}
/*Função que carrega os programas-argumentos na memoria*/
int load_program_mine(const char *arquivo){

	FILE 	  *arq;
	uint32_t lines;
	int size_lines=0;
	int 		 i;

	ocupado_base =0;
	if((arq = fopen(arquivo, "r"))==NULL)/*Abrindo arquivo*/
			return -1;

	while(!feof(arq)){/*contando instruções*/
		fread(&lines, sizeof(uint32_t), 1, arq);
		size_lines++;
	}
	size_lines--;
	livre = make_free(ocupado);/*Monta lista de vazios*/
	ocupado_base = search_free(livre, size_lines*4);/*procura espaço para alocar*/

	if(ocupado_base != -1){/*caso espaço disponivel*/

		ocupado = insertion(ocupado,ocupado_base, size_lines*4);/*insere e organiza as listas*/
	    ocupado = contigous_busy_list_make(ocupado);
	    livre = make_free(ocupado);

		rewind(arq);	
		/*escreveno na memoria*/
		for (i = ocupado_base; i < (ocupado_base+(size_lines*4))-1 ; ++i){
			/* code */
			fread(&MEMORY[i], sizeof(uint8_t), 1, arq);
		}
		fclose(arq);
		return size_lines;
	}
	else{
		return -1;
	}
}
/* Create a new process trying to load a new program at 0 address (works only for mono-programmed version) */
int create_process()
{
	int size;
	list *aux;

	do{

		if( NEXT_PROGRAM > LAST_PROGRAM )
		{
			stop_execution(); /* stop CPU execution */
			return 0;
		}

		size = load_program_mine(PROGRAMS[NEXT_PROGRAM]);
		
		BCP.pid = NEXT_PROGRAM++;

		if( size > 0 )
		{
			/*INICIALIZA BCP*/
			bzero(&(BCP.regs), sizeof(BCP.regs));
			BCP.regs.base = ocupado_base;
			BCP.regs.limit = ocupado_base + (size << 2)-1;
			/*INICIALIZA LISTA*/
			process_row[id_process_row].pid = BCP.pid;
			bzero(&process_row[id_process_row].regs,sizeof(REGS));
			process_row[id_process_row].regs.base = BCP.regs.base;
			process_row[id_process_row].regs.limit = BCP.regs.limit;
			id_process_row++;
				
			printf("\nProcesso %i carregado...\n\n", BCP.pid);
			if(NEXT_PROGRAM == LAST_PROGRAM)/*CASO TODOS PROGRAMAS POSSIVEIS TENHAM SIDO CARREGADOS*/
				NEXT_PROGRAM++;

			/*condição de saida da função*/				
			if((NEXT_PROGRAM > LAST_PROGRAM || (search_free(livre, size*4))==-1) && id_process_row!=0){
				memcpy(&(BCP.regs), &(process_row[0].regs), sizeof(REGS));
				BCP.pid=process_row[0].pid;
				bzero(&REGS,sizeof(REGS));
				memcpy(&REGS,&(BCP.regs), sizeof(REGS));
				printf("\nGerenciamento de memoria :\n");
				aux = ocupado;
				while(aux!=NULL){
					printf("Memoria ocupada do %d(byte) ate %d(byte)\n", aux->base, aux->limit);
					aux = aux->prox;
				}
				aux = livre;
				while(aux!=NULL){
					printf("Memoria livre do %d(byte) ate %d(byte)\n", aux->base, aux->limit);
					aux = aux->prox;
				}	
				return 1;
			}
		}

	}while( 1 );
}

/* Terminates the current (only) process */
void terminate_process()
{
	unsigned i=0;
	printf("\n\nProcesso %i terminou...\nRegistradores finais\n\n", BCP.pid);
	print_regs();
	/*Processo terminou processo excluido da memoria*/
	if(ocupado != NULL){
		ocupado = free_space(ocupado, BCP.regs.base, BCP.regs.limit);
	}

	/*anda com a fila*/
	while(i<id_process_row){
		memcpy(&(process_row[i].regs), &(process_row[i+1].regs), sizeof(REGS));
		process_row[i].pid=process_row[i+1].pid;
		i++;
	}
	id_process_row--;
	/*atualiza BCP*/
	memcpy(&(BCP.regs), &(process_row[0].regs), sizeof(REGS));
	BCP.pid=process_row[0].pid;
	memcpy(&REGS,&(BCP.regs), sizeof(REGS));
	/*verifica necessidade de criar mais processos*/
	if((NEXT_PROGRAM<LAST_PROGRAM)||(id_process_row==0)){	
		livre = make_free(ocupado);
		ocupado_base = 0; 
		if( create_process() )
			schedule();
	}
	/*caso nao haja a necessiade somente escalona*/
	else if(id_process_row > 0){
		schedule();
	}

}

/* Save context: it's not necessary  for this mono-programmed OS simulator */
void save_context()
{
	unsigned i=0;
	struct s_bcp aux;	
	/*Salva contexto do processo atuantes(registradores de hardware)*/
	memcpy(&(BCP.regs),&REGS, sizeof(REGS));
	/*copia estado*/
	memcpy(&(process_row[0].regs), &BCP.regs, sizeof(REGS));/*salvando contexto*/
	process_row[0].pid=BCP.pid;
	bzero(&aux.regs,sizeof(REGS));
	memcpy(&(aux.regs),&(process_row[0].regs), sizeof(REGS));
	aux.pid=process_row[0].pid;
	/*atualiza fila*/
	while(i<id_process_row){/*atualizando process_row de processos*/
		memcpy(&(process_row[i].regs), &(process_row[i+1].regs), sizeof(REGS));
		process_row[i].pid=process_row[i+1].pid;
		i++;
	}
	/*ultima posição*/
	memcpy(&(process_row[i-1].regs), &(aux.regs), sizeof(REGS));
	process_row[i-1].pid=aux.pid;
}

/* Restore context to execute a new program */
void restore_context()
{
	/*salva o contexto*/
	save_context();
	/*atualiza BCP*/
	memcpy(&(BCP.regs), &(process_row[0].regs), sizeof(REGS));
	BCP.pid=process_row[0].pid;
	bzero(&REGS,sizeof(REGS));
	memcpy(&REGS, &(BCP.regs), sizeof(REGS));
}

/* CPU scheduler: very simple for mono-programmed OS ... */
void schedule()
{
	restore_context();

	program_clock_int(500); /* Example: program next clock interruption (next 500 ticks) */


}
/* Main OS program */
int main(int argc, const char* argv[])
{	


	ocupado = NULL;
	livre = NULL;
	

	init_os(argc, argv);
		
		if( create_process() )
		{	
			schedule();
			start_execution();/* Start execution of programs. It's a loop and must be called ONLY ONE TIME in the whole program */
		}		
	
	printf("\n\nMemoria : \n\n");
	print_mem();

	
	halt_os();

	return 0;
}
