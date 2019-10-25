#include<stdio.h>
#include<stdlib.h>
#include<math.h>

#define debug printf
#define print_usuario 
#define NUM_FUNC 1

typedef struct{
	double a,b;
	double e;
	double (*func)(double);
}intervalo;


intervalo *construtor_intervalo();

void destrutor_intervalo(intervalo *inter);

void define_intervalo(intervalo *inter, double a, double b, double e, double (*func)(double));


double a(double x);
double b(double x);
double c(double x);
double d(double x);
double e(double x);
double f(double x);
double g(double x);


int igual(double a, double b, double e);

double area_retangulo(intervalo *inter);

double integral(intervalo *inter);

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

		define_intervalo(inter,a,b,e,funcoes[i]);

		printf("Integral = %.7lf\n", integral(inter));
		destrutor_intervalo(inter);
	}

	
	


	return 0;
}


intervalo *construtor_intervalo(){
	intervalo *inter = malloc(sizeof(intervalo));
	return inter;
}

void destrutor_intervalo(intervalo *inter){
	free(inter);
}

void define_intervalo(intervalo *inter, double a, double b, double e, double (*func)(double)){
	inter->a=a;
	inter->b=b;
	inter->e=e;
	inter->func=func;
}


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

int igual(double a, double b, double e){
	return fabs(a-b) <= e;
}


double area_retangulo(intervalo *inter){
	double a=inter->a;
	double b=inter->b;
	double m=(a+b)/2;
	
	return (b-a) * inter->func(m);
}

double integral(intervalo *inter){

	double a=inter->a;
	double b=inter->b;
	double e=inter->e;
	double m = (a+b)/2;
	
	debug("\ta=%lf b=%lf m=%lf\n",a,b,m);
	
	intervalo *inter_menor1 = construtor_intervalo(), *inter_menor2 = construtor_intervalo();	
	define_intervalo(inter_menor1, a, m, e,inter->func);
	define_intervalo(inter_menor2, m, b, e,inter->func);
	
	double area_maior = area_retangulo(inter);	
	double soma_areas_menores = area_retangulo(inter_menor1) + area_retangulo(inter_menor2);

	
	debug("\tmai=%lf men=%lf\n",area_maior,soma_areas_menores);
	if(igual(soma_areas_menores, area_maior,inter->e)){
	
		destrutor_intervalo(inter_menor1);
		destrutor_intervalo(inter_menor2);
	
		return area_maior;	
	}else{

		double ret = integral(inter_menor1) + integral(inter_menor2);
		
		destrutor_intervalo(inter_menor1);
		destrutor_intervalo(inter_menor2);

		return ret;
	}
}

