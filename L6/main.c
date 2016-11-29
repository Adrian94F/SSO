/*
2 konta bankowe
można wpłacać i wypłacać kasę
wpłaty i wypłaty z zewnętrznych wątków
możliwość sprawdzenia stanu konta, z którego się wypłaca
*/
#include <semaphore.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <stdio.h>
#include <time.h>

#define clear() printf("\033[H\033[J")
#define liczbaKlientow 2

float konta[liczbaKlientow];
int n[liczbaKlientow];
int liczbaWplat[liczbaKlientow];
int liczbaWyplat[liczbaKlientow];
int liczbaPrzelewow[liczbaKlientow];
char buf[100];
char hist[100000];
int offset = 0;
pthread_mutex_t blokadaKont = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t zmiennaWarunkowa = PTHREAD_COND_INITIALIZER;

void msleep (unsigned int ms) {
	int microsecs;
	struct timeval tv;
	microsecs = ms * 1000;
	tv.tv_sec  = microsecs / 1000000;
	tv.tv_usec = microsecs % 1000000;
	select (0, NULL, NULL, NULL, &tv);  
}

void *klient(void *nr)
{
	int *numer = (int *)nr;
	//sprintf(buf, "klient %d żyje!", *numer);
	while(1)
	{
		float w = rand() % 100000 / 100.0;
		if (rand() % 3)
		{

			//wpłata na konto
			pthread_mutex_lock(&blokadaKont);
			konta[*numer] += w;
			offset += sprintf(hist+offset, "K%d    %+6.2f zł (%.2f zł)\n", *numer, w, konta[*numer]);
			pthread_mutex_unlock(&blokadaKont);
			liczbaWplat[*numer]++;
			sprintf(buf, "Klient %d wpłacił %.2f zł (stan konta: %.2f zł)\n", *numer, w, konta[*numer]);
		}
		else
		{
			if (rand() % 2)
			{
				//wypłata z konta
				pthread_mutex_lock(&blokadaKont);
				while(konta[*numer] - w < 0)
				{
					offset += sprintf(hist+offset, "~K%d   %6.2f zł (%.2f zł)\n", *numer, -w, konta[*numer]);
					sprintf(buf, "Klient %d próbuje wypłacić %.2f zł (stan konta: %.2f zł)\n", *numer, w, konta[*numer]);
					pthread_cond_wait(&zmiennaWarunkowa, &blokadaKont);
				}
				konta[*numer] -= w;
				offset += sprintf(hist+offset, "K%d    %6.2f zł (%.2f zł)\n", *numer, -w, konta[*numer]);
				pthread_mutex_unlock(&blokadaKont);
				liczbaWyplat[*numer]++;
				sprintf(buf, "Klient %d wypłacił %.2f zł (stan konta: %.2f zł)\n", *numer, w, konta[*numer]);
			}
			else
			{
				//przelew na inne konto
				int adresat;
				do
					adresat = rand() % liczbaKlientow;
				while(adresat == *numer);
				pthread_mutex_lock(&blokadaKont);
				while(konta[*numer] - w < 0)
				{
					offset += sprintf(hist+offset, "~K%d>K%d %6.2f zł (%.2f zł, %.2f zł)\n", *numer, adresat, w, konta[*numer], konta[adresat]);
					sprintf(buf, "Klient %d próbuje przelać klientowi %d kwotę %.2f zł (stan konta: %.2f zł)\n", *numer, adresat, w, konta[*numer]);
					pthread_cond_wait(&zmiennaWarunkowa, &blokadaKont);
				}
				konta[*numer] -= w;
				konta[adresat] += w;
				offset += sprintf(hist+offset, "K%d>K%d  %6.2f zł (%.2f zł, %.2f zł)\n", *numer, adresat, w, konta[*numer], konta[adresat]);
				pthread_mutex_unlock(&blokadaKont);
				liczbaPrzelewow[*numer]++;
				sprintf(buf, "Klient %d przelał klientowi %d kwotę %.2f zł (stan konta: %.2f zł)\n", *numer, adresat, w, konta[*numer]);
			}
			
		}
		msleep(rand() % 5000);
	}
}

void intHandler(int a) 
{
	clear();
	printf("Log:\n\n%s\n", hist);
	exit(0);
}

void init()
{
	signal(SIGINT, intHandler);
	srand(time(NULL));
	for (int i = 0; i < liczbaKlientow; i++)
	{
		konta[i] = 0;
		n[i] = i;
	}
	sprintf(buf, "Zainicjalizowano!\n");
}

void view()
{
	while(1)
	{
		clear();
		printf("Salda:\n---------------------------------------------------------------------\n Klient  Saldo          Wpłaty  Wypłaty  Przelewy\n---------------------------------------------------------------------\n");
		for (int i = 0; i < liczbaKlientow; i++)
		{
			printf(" %6d  %10.2f zł  %6d  %7d  %8d\n", i, konta[i], liczbaWplat[i], liczbaWyplat[i], liczbaPrzelewow[i]);
		}
		printf("---------------------------------------------------------------------\nOstatnia operacja:\n ");
		printf("%s", buf);
		msleep(50);
	}
}

int main()
{
	init();
	pthread_t klienci[liczbaKlientow];
	for (int i = 0; i < liczbaKlientow; i++)
	{
		if(pthread_create(&klienci[i], NULL, klient, &n[i])) 
		{
			perror("Błąd tworzenia wątku!\n");
			return 1;
		}
		else
		{
			printf("Stworzono wątek %d\n", i);
		}
	}
	view();
	exit(0);
}