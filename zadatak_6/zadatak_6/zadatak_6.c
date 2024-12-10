/*
6. Napisati program koji èita datoteku racuni.txt u kojoj su zapisani nazivi svih datoteka koji
predstavljaju pojedini raèun.Na poèetku svake datoteke je zapisan datum u kojem vremenu je
raèun izdat u formatu YYYY-MM-DD. Svaki sljedeæi red u datoteci predstavlja artikl u formatu
naziv, kolièina, cijena.

Potrebno je formirati vezanu listu raèuna sortiranu po datumu. Svaki èvor vezane liste sadržava 
vezanu listu artikala sortiranu po nazivu artikla. 

Nakon toga potrebno je omoguæiti upit kojim æe korisnik saznati koliko je novaca sveukupno potrošeno na specifièni
artikl u odreðenom vremenskom razdoblju i u kojoj je kolièini isti kupljen.

*/

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define MAX 20
#define ERROR_OPENING_FILE -1
#define DATE_LENGTH 8

struct _article;
struct _receipt;
struct _listReceipt;
typedef struct _listReceipt* positionList;
typedef struct _article* positionArticle;
typedef struct _receipt* positionReceipt;

typedef struct _article {
    char name[MAX];
    int quant;
    int price;

    positionArticle next;
}article;

typedef struct _receipt {
    char receiptDate[DATE_LENGTH];
    positionReceipt next;
    positionArticle articleHead;
}receipt;

typedef struct _listReceipt {
    char receipt[MAX];

    positionList next;
}listReceipt;

int createListReceipt(positionList head, char list[MAX]);
int readFile(positionList headList, positionReceipt haedReceipt, int temp);
int createReceipt(positionReceipt headReceipt, char receiptDate[DATE_LENGTH+1], FILE *file);
//int insertSorted(positionReceipt headReceipt, positionReceipt newReceipt);
int createArticle( positionArticle headArticle, char name[MAX], int quant, int price);
int outputArticle(positionArticle headArticle);
int deleteArticle(positionArticle headArticle, char name[MAX], positionArticle tempArt);
positionArticle findPrev(positionArticle head, char name[MAX], positionArticle tempArt);
int receiptBetweenDates(char newArticle[MAX], char dateStart[MAX], char dateEnd[MAX], positionReceipt headReceipt);
int findArticle(positionArticle headArticle, char newArticle[MAX]);


int main()
{
    const char* file = "racuni.txt";

    FILE* fileReceipt = fopen(file, "r");
    if (!fileReceipt) {
        printf("Datoteka se ne moze otvoriti");
        return ERROR_OPENING_FILE;
    }
    printf("datoteka se otvorila!\n");

    positionList headList = NULL;
    headList = (positionList)malloc(sizeof(listReceipt));
    if (headList == NULL)
    {
        printf("Pogreska prilikom alokacije memorije\n");
        return ERROR_OPENING_FILE;
    }
    headList->next = NULL;

    printf("\nLista recepata\n");

    char buffer[1024];
    positionList tempList = headList;
    while (fscanf(fileReceipt, " %s", buffer) == 1) {
        createListReceipt(tempList, buffer);
    }

    positionReceipt headReceipt = NULL;
    headReceipt = (positionReceipt)malloc(sizeof(receipt));
    if (headReceipt == NULL)
    {
        printf("Pogreska prilikom alokacije memorije\n");
        return ERROR_OPENING_FILE;
    }
    headReceipt->next = NULL;

    for (int i = 0; i < 5; i++)
    {
        readFile(headList, headReceipt, i);
    }


    printf("\n\n\nUPIT\n\n");
    printf("Unesite artikl: ");
    char newArticle[MAX];
    fgets(newArticle, MAX, stdin);
    printf("%s", newArticle);

    printf("Pocetni datum (oblika YYYYMMDD): ");
    char dateStart[DATE_LENGTH+1];
    fgets(dateStart, DATE_LENGTH+1, stdin);
    printf("%s", dateStart);

    printf("\nZavrsni datum (oblika YYYYMMDD): ");
    char dateEnd[DATE_LENGTH + 1];
    scanf(" %[^\n]%*c", dateEnd);
    printf("%s", dateEnd);

    int totalPrice, totalQuant;
    totalQuant = receiptBetweenDates(newArticle, dateStart, dateEnd, headReceipt);
    printf("\n%d", totalQuant);

    return 0;
}


int createListReceipt(positionList head, char list[MAX])
{
    positionList newElement = NULL;
    newElement = (positionList)malloc(sizeof(listReceipt));
    if (newElement == NULL)
    {
        printf("pogreska prilikom alokacije memorije\n");
        return ERROR_OPENING_FILE;
    }
    newElement->next = head->next;
    head->next = newElement;
    strcpy(newElement->receipt, list);
    printf("\n%s", newElement->receipt);

    return EXIT_SUCCESS;
}

int readFile(positionList headList, positionReceipt headReceipt, int temp)
{
    if (temp < 0 || temp > 4)
        return EXIT_FAILURE;

    positionList tempList = headList->next;
    const char* file = NULL;
    for (int i = 0; i < 5; i++)
    {
        if (i == temp)
        {
            file = tempList->receipt;
        }
        tempList = tempList->next;
    }

    if (!file)
    {
        printf("greska!");
        return EXIT_FAILURE;
    }

    FILE* fileReceipt = fopen(file, "r");
    if (!fileReceipt) 
    {
        printf("Datoteka se ne moze otvoriti");
        return ERROR_OPENING_FILE;
    }

    char buffer[1024];
    char receiptDate[DATE_LENGTH+1] = {0};
    if (fscanf(fileReceipt, " %s", buffer) == 1)
    {
        int iDate = 0;
        int i = 0;
        while (buffer[i] && iDate < DATE_LENGTH )
        {
            if (buffer[i] >= '0' && buffer[i] <= '9')
            {
                receiptDate[iDate] = buffer[i];
                iDate++;
            }
            i++;
        }
        
    }

    createReceipt(headReceipt, receiptDate, fileReceipt);
    return EXIT_SUCCESS;
}

int createReceipt(positionReceipt headReceipt, char receiptDate[DATE_LENGTH+1], FILE* file)
{
    positionReceipt newReceipt = NULL;
    newReceipt = (positionReceipt)malloc(sizeof(receipt));
    if (newReceipt == NULL)
    {
        printf("pogreska prilikom alokacije memorije\n");
        return ERROR_OPENING_FILE;
    }
    newReceipt->next = headReceipt->next;
    headReceipt->next = newReceipt;
    strcpy(newReceipt->receiptDate, receiptDate);

    positionArticle headArticle = NULL;
    headArticle = (positionArticle)malloc(sizeof(article));
    if (headArticle == NULL)
    {
        printf("Pogreska prilikom alokacije memorije\n");
        return ERROR_OPENING_FILE;
    }
    headArticle->next = NULL;
    newReceipt->articleHead = headArticle;
    char name[MAX];
    int quant, price;
    printf("\n%s", newReceipt->receiptDate);
    while (fscanf(file, " %s %d %d", name, &quant, &price) == 3)
    {
        createArticle(headArticle, name, quant, price);
    }
    
   

    outputArticle(headArticle);


    printf("\n");

    //insertSorted(headReceipt, newReceipt);

    return EXIT_SUCCESS;
}
/*
int insertSorted(positionReceipt headReceipt, positionReceipt newReceipt)
{
    positionReceipt temp = headReceipt;
    if (!temp->next)
        printf("nema temp->next");
 
    while (temp->next && strcmp(temp->next->receiptDate, newReceipt->receiptDate)>0)
    {
        temp = temp->next;
    }

    newReceipt->next = temp->next;
    temp->next = newReceipt;

    return EXIT_SUCCESS;
}
*/
int createArticle(positionArticle headArticle, char name[MAX], int quant, int price)
{
    positionArticle newArticle = NULL;
    newArticle = (positionArticle)malloc(sizeof(article));
    if (newArticle == NULL)
    {
        printf("pogreska prilikom alokacije memorije\n");
        return ERROR_OPENING_FILE;
    }
    newArticle->next = headArticle->next;
    headArticle->next = newArticle;


    strcpy(newArticle->name, name);
    newArticle->price = price;
    newArticle->quant = quant;
    
    positionArticle temp = headArticle->next;
    while (temp)
    {
        if (strcmp(temp->name, newArticle->name) == 0 && (temp->next != newArticle->next))
        {
            newArticle->quant += temp->quant;
            deleteArticle(headArticle, name, newArticle);
            return EXIT_SUCCESS;
        }
        temp = temp->next;
    }

    return EXIT_SUCCESS;
}

int outputArticle(positionArticle headArticle)
{
    positionArticle temp = headArticle->next;
    while (temp)
    {
        printf("\n\t%s %d %d", temp->name, temp->quant, temp->price);
        temp = temp->next;
    }
    return EXIT_SUCCESS;
}

int deleteArticle(positionArticle headArticle, char name[MAX], positionArticle tempArt)
{
    positionArticle prev = findPrev(headArticle, name, tempArt);
    if (prev == NULL || prev->next == NULL) {
        printf("Element nije pronaden.\n");
        return EXIT_FAILURE;
    }
    positionArticle elementToDelete = prev->next;
    prev->next = elementToDelete->next;
    free(elementToDelete);

    return EXIT_SUCCESS;
}

positionArticle findPrev(positionArticle head, char name[MAX], positionArticle tempArt)
{
    positionArticle prev = head;
    positionArticle current = head->next;

    while (current && (strcmp(current->name, name) != 0 || (tempArt->next == current->next)))
    {
        prev = current;
        if(current->next)
        current = current->next;
    }

    if (current) return prev;
    return NULL;
}

int receiptBetweenDates(char newArticle[MAX], char dateStart[MAX], char dateEnd[MAX], positionReceipt headReceipt)
{
    printf("uslo u prvu");
    int quant = 0;
    char receiptD[DATE_LENGTH + 1] = {0};
    
    positionReceipt tempReceipt = headReceipt->next;
    int j = 0;
    while(tempReceipt && j < 5)
    {
        for (int i = 0; i < DATE_LENGTH; i++)
        {
            receiptD[i] = tempReceipt->receiptDate[i];
        }
        printf("\n%s", receiptD);

        if (strcmp(receiptD, dateStart) > 0 && strcmp(receiptD, dateEnd) < 0)
        {
            printf("\nPROVJERA");
            quant += findArticle(tempReceipt->articleHead, newArticle);
            tempReceipt = tempReceipt->next;
        }
        if(tempReceipt->next)
        tempReceipt = tempReceipt->next;

        j++;

    }
    return quant;
    
        
}

int findArticle(positionArticle headArticle, char newArticle[MAX])
{
    positionArticle tempArt = headArticle->next;
    printf("uslo u drugu");
    while (tempArt)
    {
        printf("provjera");
        if (strcmp(tempArt->name, newArticle) == 0)
        {
            printf("\nisto");
            return tempArt->quant;
        }
        tempArt = tempArt->next;
    }
    return 0;
}