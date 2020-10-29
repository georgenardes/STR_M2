/*
Implementado por:
	Diogo Marchi
	George Nardes

*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sched.h>
#include <unistd.h>
#include <omp.h>

#ifndef _POSIX_THREAD_PRIO_PROTECT
#endif

#define MAX_OBJ 1500
#define FREQ 3000000000


/* struct para agrupar valores */
typedef struct {
	int num_objeto;
	int *pesos;
} data_t;

/* variavel para controle de partida */
static int go = 0;

/* id de cada thread */
static int _id = 0;

/* contadores de cada thread */
static int contadores[3];

/* variavel para mudar posição de escrita */
static int pos = 1;

/* variavel para finalizar programa */
static int finaliza = 1;

void gotoxy(int x,int y){
    printf("%c[%d;%df",0x1B,y+pos,x);
}

void *conta_objeto(void *t_pesos_num_obj_ptr){
	// id da thread
	int id = _id++;
	
	// contagem da esteira
	contadores[id] = 0;

	/* cast de parametros */
	data_t *t_pesos_num_obj = (data_t *)t_pesos_num_obj_ptr;
	
	// para contagem do tempo minimo e maximo  
	static double tempo_MAX = 0.00;
	static double tempo_MIN = 1.00;
	
    static double tempo_somar_pesos = 0.00;
	
	// cabeçalho total de prod
	gotoxy(0, 3);		
	printf("Total produtos |est 1|est 2|est 3|");
	
	while(finaliza == 1){
	
		// enquanto não detectar produto
		while(rand() % 10 != 1);	
		
		// tempo inicial 
		double tempo_i = (double) clock();
				
		//Inicia sessão critica
		#pragma omp critical
		{		
			// se atingir limite, realiza a soma dos pesos
			if((*t_pesos_num_obj).num_objeto >= MAX_OBJ){
				
				double tempo_i_p = (double) clock();		
							
				// soma os pesos dos produtos 			
				int soma = 0;
				for(int i = 0; i < MAX_OBJ; i++){
					soma += (*t_pesos_num_obj).pesos[i];			
				}
				
				tempo_somar_pesos = ((double) clock() - tempo_i_p)/ FREQ;
							
				gotoxy(50, 4);		
				printf("O peso total é de: %d Kg", soma);
							
				gotoxy(80, 4);
				printf("tempo MIN de contagem %.15lf", tempo_MIN);
				gotoxy(80, 5);
				printf("tempo MAX de contagem %.15lf", tempo_MAX);
				gotoxy(80, 6);
				printf("para somar pesos %.15lf", tempo_somar_pesos);				
				
				// reseta variaveis de medição de tempo
				tempo_MIN = 1.0;
				tempo_MAX = 0.0;
				
				// reinicia variavel de contagem 
				(*t_pesos_num_obj).num_objeto = 0;	
				
				// reseta contadores 
				contadores[0] = 0;
				contadores[1] = 0;
				contadores[2] = 0;
				
				// nova linha para printar 
				pos += 3;			
																		
			} 		
			
			// pesos[num_objeto] = peso random
			(*t_pesos_num_obj).pesos[(*t_pesos_num_obj).num_objeto] = rand() % 10 + 1;
							
			// num_objeto += 1;
			(*t_pesos_num_obj).num_objeto++;
			
			// adiciona no contador da esteira atual 
			contadores[id]++;			
			
			// contagem do tempo para somar um produto 
			double tempo_f = ((double) clock() - tempo_i) / FREQ;		
			
			// valores min e max
			if (tempo_f < tempo_MIN)
				tempo_MIN = tempo_f;			
			if (tempo_f > tempo_MAX)
				tempo_MAX = tempo_f;
				
			// printa soma de produtos
			gotoxy(5, 4);
			printf("%d", (*t_pesos_num_obj).num_objeto);	
			
			// printa contagem da esteira 
			gotoxy(18+(id*5), 4);
			printf("|%d", contadores[id]);		
		
		//termina sessao critica			
		}	
		
		/* sleep para simular esteira */
		usleep(1000);
				
	}				
	
	/* the function must return something - NULL will do */
	return NULL;
}


void *verifica_teclado(){
	gotoxy(0, 0);
	printf("Pressione enter para finalizar a simulação");

	do{
		getchar();
		finaliza = 0;
	}while(finaliza == 1);
	

	return NULL;
}

int main(){
	system("clear");
	/* para gerar valores aleatorios */
	srand(time(NULL));
	
	/* estrutura com contador de objetos e pesos */
	data_t t_pesos_num_obj;

	/* inicializa contador de obejtos = 0*/
	t_pesos_num_obj.num_objeto = 0;
	
	/* inicializa vetor de pesos */
	t_pesos_num_obj.pesos = malloc(MAX_OBJ * sizeof(int));
	

    #pragma omp parallel shared(t_pesos_num_obj) num_threads(4) 	
	{
		 #pragma omp single nowait
            {
                verifica_teclado();
		}
    	conta_objeto(&t_pesos_num_obj);
	}
	gotoxy(0,20);
	
	return 0;
}
