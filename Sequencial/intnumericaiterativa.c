#include<stdio.h>
#include<stdlib.h>
#include<math.h>

#define debug printf
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

double ans;

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
void integral(intervalo *inter);


double (*funcoes[])(double) = {a,b,c,d,e,f,g};


int main(int argc, char *argv[]){
	
	double a, b, e;
	for(int i=0; i<NUM_FUNC; i++){
		intervalo *inter = construtor_intervalo();
		print_usuario("FUNCAO %c\n", (char) (i+'a'));
		
		print_usuario("\tDigite o comeco do intervalo: ");
		scanf("%lf",&a);

		print_usuario("\tDigite o fim do intervalo: ");
		scanf("%lf",&b);

		print_usuario("\tDigite o erro maximo aceitavel: ");
		scanf("%lf", &e);

		define_intervalo(inter,a,b,e,funcoes[i],NULL);
		init(inter);
		integral(inter);
		printf("Integral = %.7lf\n", ans);
	}
	
	
	

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
	ans=ZERO;
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
void integral(intervalo *inter){
	
	void subproblema_resolvido(intervalo *t);
	
	while(1){
		
		if(esperando+resolvendo == 0){
		
			break;
		}
		
		intervalo *t = pop();
		esperando--;
		resolvendo++;
		
			
		if(t->possivel_retornar==POSSIVEL){
		
			subproblema_resolvido(t);
			resolvendo--; 
			
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
			
			if(igual(area_maior, som_areas_menores, inter->e)){
			
				t->valor_retorno = t->area_retangulo;
			
				subproblema_resolvido(t);
				resolvendo--;
				
				destrutor_intervalo(t);
				destrutor_intervalo(inter_menor1);
				destrutor_intervalo(inter_menor2);
				
				 
			}
			else{
			
				push(inter_menor1);
				push(inter_menor2);
				esperando +=2 ;
				
			}
		
		}	
			
	}
}

void subproblema_resolvido(intervalo *t){
	if(t->pai==NULL){
		ans = t->valor_retorno;
	}
	else{
		
		t->pai->valor_retorno += t->valor_retorno;
		t->pai->possivel_retornar++;
		if(t->pai->possivel_retornar == POSSIVEL)
			push(t->pai);
			
			
	} 	
	
}


