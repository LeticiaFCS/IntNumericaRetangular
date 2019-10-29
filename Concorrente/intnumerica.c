#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include "timer.h"

#include<pthread.h>

#define debug 
#define print_usuario 
#define NUM_FUNC 7
#define ZERO 0.0000000000
#define POSSIVEL 2

typedef struct str_intervalo{
	double a,b;
	double e;
	
	int possivel_retornar; //2 se é possível, 0 ou 1 se não é	
	double valor_retorno;
	
	double area_retangulo;

	struct str_intervalo * pai;
	double (*func)(double);
}intervalo;

typedef struct str_no{
	intervalo *inter;
	struct str_no *proximo;
}no;

int resolvendo, esperando;

double resposta;

no *topo = NULL;

int pilha_vazia();
void push(intervalo *inter);
intervalo * pop();

void init(intervalo *inter);

intervalo *construtor_intervalo();
void destrutor_intervalo(intervalo *inter);
void define_intervalo(intervalo *inter, double a, double b, double e, double (*func)(double), intervalo *pai);


double a(double x);
double b(double x);
double c(double x);
double d(double x);
double e(double x);
double f(double x);
double g(double x);


int igual(double a, double b, double e);
void define_area_retangulo(intervalo *inter);
void * integral(void *in);

void boba();


double (*funcoes[])(double) = {a,b,c,d,e,f,g};

pthread_mutex_t mutex;
pthread_cond_t cond;


int main(int argc, char *argv[]){
	
	double a, b, e;
	int t, i, j;
	
	pthread_t * t_integral;
	int * tid;
	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&cond, NULL);
	
	
	double ini_inicializacao, fim_inicializacao;
	double ini_threads, fim_threads;
	
	for(i=0; i<NUM_FUNC; i++){
		GET_TIME(ini_inicializacao);
		
		
		intervalo *inter = construtor_intervalo();
		print_usuario("FUNCAO %c\n", (char) (i+'a'));
		
		print_usuario("\tDigite o comeco do intervalo: ");
		scanf("%lf",&a);

		print_usuario("\tDigite o fim do intervalo: ");
		scanf("%lf",&b);

		print_usuario("\tDigite o erro maximo aceitavel: ");
		scanf("%lf", &e);
		
		print_usuario("\tDigite o numero de threads: ");
		scanf("%d", &t);

		define_intervalo(inter,a,b,e,funcoes[i],NULL);
		init(inter);
		
		
		GET_TIME(fim_inicializacao);
		ini_threads = fim_inicializacao;
		
		t_integral = malloc(sizeof(pthread_t)*t);
		tid = malloc(sizeof(int)*t);	
		if(t_integral == NULL || tid == NULL){
			printf("Erro malloc()\n");
			exit(-1);
		}
		
		for(j=0;j<t;j++){
			tid[j]=j;
			
			if(pthread_create(&t_integral[j],NULL,integral, (void *) &tid[j])){
				printf("Erro pthread_create()\n");
				exit(-1);
			}
		}
		for(int j=0;j<t;j++){
			if(pthread_join(t_integral[j],NULL)){
				printf("Erro pthread_join()\n");
				exit(-1);
			}
		}
		free(t_integral);
		
		
		GET_TIME(fim_threads);
	
			
		printf("Funcao %c com a=%.10lf b=%.10lf e=%.10lf\n",(char) (i+'a'), a, b, e);
		printf("Integral = %.10lf\n", resposta);
		printf("\ttempo para inicializacao = %.10lf\n",fim_inicializacao - ini_inicializacao);
		printf("\ttempo de processamento = %.10lf\n",fim_threads - ini_threads);
		printf("\ttempo total = %.10lf\n\n",fim_threads - ini_threads);
		
	}
	
	pthread_mutex_destroy(&mutex);
	pthread_cond_destroy(&cond);
	pthread_exit(NULL);

	return 0;
}


//funções da pilha
int pilha_vazia(){
	return topo == NULL;
}

void push(intervalo *inter){
	no * n = malloc(sizeof(no));
	if(n == NULL){
		exit(-1);
	}
	n->inter = inter;
	n->proximo = topo;
	topo = n;
}

intervalo * pop(){
	if(pilha_vazia()){
		debug("pilha vazia\n");
		exit(-1);
	}
	intervalo * inter = topo->inter;
	no * n = topo;
	topo = topo->proximo;
	free(n);
	return inter;
}

void init(intervalo *inter){
	resolvendo=0;
	resposta=ZERO;
	esperando=1;
	while(!pilha_vazia){
		intervalo * aux = pop();
		destrutor_intervalo(aux);
	}
	push(inter);
}

//funções de intervalo
intervalo *construtor_intervalo(){
	intervalo *inter = malloc(sizeof(intervalo));
	return inter;
}

void destrutor_intervalo(intervalo *inter){
	free(inter);
}

void define_intervalo(intervalo *inter, double a, double b, double e, double (*func)(double), intervalo *pai){
	inter->a=a;
	inter->b=b;
	inter->e=e;
	inter->possivel_retornar=0;
	inter->valor_retorno=ZERO;
	inter->func=func;
	
	define_area_retangulo(inter);
	
	inter->pai=pai;
	boba();
}

//funções para as quais iremos calcular as integrais
double a(double x){
	return 1+x;
}
double b(double x){
	return sqrt(1-x*x);
}
double c(double x){
	return sqrt(1+x*x*x*x);
}
double d(double x){
	return sin(x*x);
}
double e(double x){
	return cos(exp(-x));
}
double f(double x){
	return e(x)*x;
}
double g(double x){
	return e(x)*(0.005*x*x*x+1);
}


//funções auxiliares
int igual(double a, double b, double e){
	return fabs(a-b) <= e;
}


void define_area_retangulo(intervalo *inter){
	double a=inter->a;
	double b=inter->b;
	double m=(a+b)/2;
	
	inter->area_retangulo = (b-a) * inter->func(m);
}


//integral
void *integral(void *arg){
	
	void subproblema_resolvido(intervalo *t);
	
	while(1){
		
		pthread_mutex_lock(&mutex);
		
		while((esperando+resolvendo != 0) && (esperando == 0)){
			pthread_cond_wait(&cond, &mutex);
		}
		
		if(esperando+resolvendo == 0){
			pthread_mutex_unlock(&mutex);
			break;
		}
		
		intervalo *t = pop();


		esperando--;
		resolvendo++;
		
		pthread_mutex_unlock(&mutex);
			
		if(t->possivel_retornar==POSSIVEL){
		
			pthread_mutex_lock(&mutex);
			subproblema_resolvido(t);
			resolvendo--; 
			pthread_cond_broadcast(&cond);
			pthread_mutex_unlock(&mutex);
			
			destrutor_intervalo(t);
		
		}else{
			intervalo *inter_menor1 = construtor_intervalo(),
					  *inter_menor2 = construtor_intervalo();
					  
			double m = (t->a + t->b)/2;
			
			define_intervalo(inter_menor1, t->a, m, t->e, t->func, t);
			define_intervalo(inter_menor2, m, t->b, t->e, t->func, t);
			
			double area_maior = t->area_retangulo,
			som_areas_menores = inter_menor1->area_retangulo
			                  + inter_menor2->area_retangulo;
			
			if(igual(area_maior, som_areas_menores, t->e)){
			
				t->valor_retorno = t->area_retangulo;
				
				pthread_mutex_lock(&mutex);
				subproblema_resolvido(t);
				resolvendo--;
				pthread_cond_broadcast(&cond);
				pthread_mutex_unlock(&mutex);
				
				destrutor_intervalo(t);
				destrutor_intervalo(inter_menor1);
				destrutor_intervalo(inter_menor2);
				
				 
			}
			else{
				
				pthread_mutex_lock(&mutex);
				push(inter_menor1);
				push(inter_menor2);
				resolvendo--;
				esperando +=2 ;
				pthread_cond_broadcast(&cond);
				pthread_mutex_unlock(&mutex);
				
			}
		
		}	
			
	}
	pthread_exit(NULL);
}

void subproblema_resolvido(intervalo *t){
	
	if(t->pai==NULL){
		resposta = t->valor_retorno;
	}
	else{
		
		t->pai->valor_retorno += t->valor_retorno;
		t->pai->possivel_retornar++;
		if(t->pai->possivel_retornar == POSSIVEL){
			esperando++;
			push(t->pai);
		}
			
			
	} 	
	
}

void boba(){
	int i;
	for(i=0;i<500;i++);
}


