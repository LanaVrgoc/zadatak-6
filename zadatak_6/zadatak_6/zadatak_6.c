/*
6. Napisati program koji èita datoteku racuni.txt u kojoj su zapisani nazivi svih datoteka koji
predstavljaju pojedini raèun. Na poèetku svake datoteke je zapisan datum u kojem vremenu je
raèun izdat u formatu YYYY-MM-DD. Svaki sljedeæi red u datoteci predstavlja artikl u formatu
naziv, kolièina, cijena. Potrebno je formirati vezanu listu raèuna sortiranu po datumu. Svaki èvor
vezane liste sadržava vezanu listu artikala sortiranu po nazivu artikla. Nakon toga potrebno je
omoguæiti upit kojim æe korisnik saznati koliko je novaca sveukupno potrošeno na specifièni
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
int readFile(positionList headList, positionReceipt* headReceipt, int temp);
int createReceipt(positionReceipt* headReceipt, char receiptDate[DATE_LENGTH + 1], FILE* file);
int insertSorted(positionReceipt* headReceipt, positionReceipt newReceipt);
int createArticle(positionArticle headArticle, char name[MAX], int quant, int price);
int outputArticle(positionArticle headArticle);
int deleteArticle(positionArticle headArticle, char name[MAX], positionArticle tempArt);
positionArticle findPrev(positionArticle head, char name[MAX], positionArticle tempArt);
int receiptBetweenDates(char newArticle[MAX], char dateStart[MAX], char dateEnd[MAX], positionReceipt headReceipt, int* pTotalPrice);
int findArticle(positionArticle headArticle, char newArticle[MAX], int*pTotalPrice);


int main()
{
    const char* file = "racuni.txt";

    FILE* fileReceipt = fopen(file, "r");
    if (!fileReceipt) {
        printf("Datoteka se ne moze otvoriti");
        return ERROR_OPENING_FILE;
    }

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
        readFile(headList, &headReceipt, i);
    }

    printf("\n\n\nUPIT\n\n");
    printf("Unesite artikl: ");
    char newArticle[MAX];
    fgets(newArticle, MAX, stdin);
    newArticle[strcspn(newArticle, "\n")] = '\0'; 

    printf("Pocetni datum (oblika YYYYMMDD): ");
    char dateStart[DATE_LENGTH + 1];
    fgets(dateStart, DATE_LENGTH + 1, stdin);
    dateStart[strcspn(dateStart, "\n")] = '\0'; 

    printf("\nZavrsni datum (oblika YYYYMMDD): ");
    char dateEnd[DATE_LENGTH + 1];
    if (scanf(" %c", dateEnd) < 0)
    {
        printf("\nGreska!\n");
        return EXIT_FAILURE;
    }

    int totalQuant;
    int totalPrice;
    int* pTotalPrice = &totalPrice;
    totalQuant = receiptBetweenDates(newArticle, dateStart, dateEnd, headReceipt, pTotalPrice);

    printf("\nukupna kolicina: %d, ukupna cijena: %u", totalQuant, (* pTotalPrice) * totalQuant);

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

int readFile(positionList headList, positionReceipt* headReceipt, int temp)
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
    char receiptDate[DATE_LENGTH + 1] = { 0 };
    if (fscanf(fileReceipt, " %s", buffer) == 1)
    {
        int iDate = 0;
        int i = 0;
        while (buffer[i] && iDate < DATE_LENGTH)
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

int createReceipt(positionReceipt* headReceipt, char receiptDate[DATE_LENGTH + 1], FILE* file)
{
    positionReceipt newReceipt = (positionReceipt)malloc(sizeof(receipt));
    if (!newReceipt)
    {
        printf("Greška: neuspjela alokacija memorije za novi raèun.\n");
        return ERROR_OPENING_FILE;
    }

    strcpy(newReceipt->receiptDate, receiptDate);
    newReceipt->articleHead = (positionArticle)malloc(sizeof(article));
    if (!newReceipt->articleHead)
    {
        printf("Greška: neuspjela alokacija memorije za èlanke.\n");
        free(newReceipt);
        return ERROR_OPENING_FILE;
    }
    newReceipt->articleHead->next = NULL;

    char name[MAX];
    int quant, price;
    while (fscanf(file, " %s %d %d", name, &quant, &price) == 3)
    {
        createArticle(newReceipt->articleHead, name, quant, price);
    }

    newReceipt->next = (*headReceipt)->next;
    (*headReceipt)->next = newReceipt;

    printf("\n\n");

    outputArticle(newReceipt->articleHead);
    return EXIT_SUCCESS;
}

int insertSorted(positionReceipt* headReceipt, positionReceipt newReceipt)
{
    positionReceipt current = *headReceipt;

    while (current->next && strcmp(current->next->receiptDate, newReceipt->receiptDate) < 0)
    {
        current = current->next;
    }

    newReceipt->next = current->next;
    current->next = newReceipt;

    return EXIT_SUCCESS;
}

int receiptBetweenDates(char newArticle[MAX], char dateStart[MAX], char dateEnd[MAX], positionReceipt headReceipt, int* pTotalPrice)
{
    int totalQuantity = 0;
    char receiptDate[DATE_LENGTH + 1] = { 0 };

    positionReceipt currentReceipt = headReceipt->next;

    while (currentReceipt)
    {
        strncpy(receiptDate, currentReceipt->receiptDate, DATE_LENGTH);
        receiptDate[DATE_LENGTH] = '\0'; 

        if (strcmp(receiptDate, dateStart) >= 0 && strcmp(receiptDate, dateEnd) <= 0)
        {
            totalQuantity += findArticle(currentReceipt->articleHead, newArticle, pTotalPrice);
        }

        currentReceipt = currentReceipt->next;
    }

    return totalQuantity;
}

int findArticle(positionArticle headArticle, char newArticle[MAX], int* pTotalPrice)
{
    int quantity = 0;
    positionArticle currentArticle = headArticle->next;

    while (currentArticle)
    {
        if (strcmp(currentArticle->name, newArticle) == 0)
        {
            quantity += currentArticle->quant;
            *pTotalPrice = currentArticle->price;
        }
        currentArticle = currentArticle->next;
    }

    return quantity;
}
int createArticle(positionArticle headArticle, char name[MAX], int quant, int price)
{
    positionArticle newArticle = NULL;

    newArticle = (positionArticle)malloc(sizeof(article));
    if (!newArticle)
    {
        printf("Greška prilikom alokacije memorije.\n");
        return ERROR_OPENING_FILE;
    }

    strcpy(newArticle->name, name);
    newArticle->quant = quant;
    newArticle->price = price;

    positionArticle current = headArticle->next;
    while (current)
    {
        if (strcmp(current->name, newArticle->name) == 0)
        {
            current->quant += newArticle->quant;
            free(newArticle); 
            return EXIT_SUCCESS;
        }
        current = current->next;
    }

    newArticle->next = headArticle->next;
    headArticle->next = newArticle;

    return EXIT_SUCCESS;
}

int outputArticle(positionArticle headArticle)
{
    positionArticle current = headArticle->next;

    if (!current)
    {
        printf("Lista artikala je prazna.\n");
        return EXIT_FAILURE;
    }

    printf("Artikli:\n");
    while (current)
    {
        printf("Naziv: %s, Kolicina: %d, Cijena: %d\n", current->name, current->quant, current->price);
        current = current->next;
    }

    return EXIT_SUCCESS;
}

int deleteArticle(positionArticle headArticle, char name[MAX], positionArticle tempArt)
{
    positionArticle prev = findPrev(headArticle, name, tempArt);

    if (!prev || !prev->next)
    {
        printf("Artikl s imenom '%s' nije pronaðen.\n", name);
        return EXIT_FAILURE;
    }

    positionArticle toDelete = prev->next;
    prev->next = toDelete->next;
    free(toDelete);

    return EXIT_SUCCESS;
}

positionArticle findPrev(positionArticle head, char name[MAX], positionArticle tempArt)
{
    positionArticle prev = head;
    positionArticle current = head->next;

    while (current)
    {
        if (strcmp(current->name, name) == 0 && current->next == tempArt->next)
        {
            return prev;
        }
        prev = current;
        current = current->next;
    }

    return NULL;
}
