#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

int c_socket;

int ClientCreate(int port, int anyip, char IPADDR[]) // Return 1 for error
{
    c_socket = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in client;
    memset(&client, 0, sizeof(client));
    client.sin_family = AF_INET;
    client.sin_port = htons(port);
    if (anyip == 0)
        client.sin_addr.s_addr = inet_addr(IPADDR);
    else
        client.sin_addr.s_addr = INADDR_ANY;
    if (connect(c_socket, (struct sockaddr *)&client, sizeof(client)) == -1)
    {
        printf("Connection Issue.\n");
        return 1;
    }
    else
        return 0;
}


struct node *rptr = NULL;

struct node
{
    struct node *lcptr;
    char symbol;
    int frequency;
    struct node *rcptr;
};

struct lookup
{
    char s;
    char code[200];
};

char s[200];
char ds[200];
int f[200];
int n;
int nc;
struct lookup LookUpTable[200];
int lookup_size;

char encodedseq[200] = "";


void InsertionSort()
{
    for (int i = 1; i < nc; i++)
    {
        for (int j = i; j > 0; j--)
        {
            if (f[j - 1] > f[j])
            {
                int temp = f[j];
                f[j] = f[j - 1];
                f[j - 1] = temp;

                char temp2 = ds[j];
                ds[j] = ds[j - 1];
                ds[j - 1] = temp2;
            }
            else
                break;
        }
    }
}


void Read()
{
    printf("Enter the string: ");
    scanf(" %[^\n]", s);
    n = strlen(s);
    lookup_size = 0;
}

void count()
{
    nc = 0;
    for (int i = 0; i < n; i++)
    {
        int flag = 0;
        //ds[0]=0;
        for (int j = 0; j < nc; j++)
        {
            if (s[i] == ds[j])
            {
                flag = 1;
                break;
            }
        }
        if (flag == 0)
        {
            ds[nc] = s[i];
            nc++;
        }
    }
    for (int i = 0; i < nc; i++)
    {
        int cou = 0;
        for (int j = 0; j < n; j++)
        {
            if (ds[i] == s[j])
            {
                cou++;
            }
        }
        f[i] = cou;
    }

    InsertionSort();
}

void BuildTree(int a[], char c[], int size, struct node *ptrarr[])
{
    if (size > 1)
    {
        int b[200];
        char t[200];
        struct node *nparr[200];
        struct node *ptr = malloc(sizeof(struct node));

        int temp = a[0] + a[1];
        int tempi = 0;

        for (int i = 2; i < size; i++)
        {
            if (temp >= a[i])
            {
                tempi = i - 1;
            }
        }
        int ct = 2;
        for (int i = 0; i < tempi; i++)
        {
            b[i] = a[ct];
            t[i] = c[ct];
            nparr[i] = ptrarr[ct];
            ct++;
        }
        b[tempi] = temp;
        nparr[tempi] = ptr;
        t[tempi] = 0;
        for (int i = tempi + 1; i < nc - 1; i++)
        {
            b[i] = a[ct];
            t[i] = c[ct];
            nparr[i] = ptrarr[ct];
            ct++;
        }

        struct node *lfptr = malloc(sizeof(struct node));
        struct node *rgptr = malloc(sizeof(struct node));

        lfptr->symbol = c[0];
        lfptr->frequency = a[0];

        if (ptrarr[0] != NULL)
        {
            lfptr = ptrarr[0];
        }

        rgptr->symbol = c[1];
        rgptr->frequency = a[1];

        if (ptrarr[1] != NULL)
        {
            rgptr = ptrarr[1];
        }

        ptr->symbol = 0;
        ptr->frequency = temp;
        ptr->lcptr = lfptr;
        ptr->rcptr = rgptr;
        rptr = ptr;

        BuildTree(b, t, size - 1, nparr);
    }
}

void LookupTable(struct node *ptr, char en[], int ensize, int choice, char chars[])
{
    if (choice == 1)
    {
        if (ptr != NULL)
        {
            if (ptr->lcptr == NULL && ptr->rcptr == NULL)
            {
                lookup_size++;
                int index = (int)(ptr->symbol);
                LookUpTable[index].s = ptr->symbol;
                for (int i = 0; i < ensize; i++)
                    LookUpTable[index].code[i] = en[i];
            }
            else
            {
                ensize++;
                en[ensize - 1] = '0';
                LookupTable(ptr->lcptr, en, ensize, 1, "");

                en[ensize - 1] = '1';
                LookupTable(ptr->rcptr, en, ensize, 1, "");
            }
        }
    }
    else if (choice == 0)
    {
        int maxnoofbits = 10;

        int noofchars = strlen(chars);

        int noofbits = 1;
        int twopow = 2;
        for (int i = 0; i < maxnoofbits; i++) 
        {
            if (noofchars <= twopow)
            {
                i = maxnoofbits;
            }
            else
            {
                twopow *= 2;
                noofbits++;
            }
        }

        char code[noofbits];
        for (int i = 0; i < noofbits; i++)
            code[i] = '0';

        for (int i = 0; i < noofchars; i++)
        {
            for (int j = 0; j < noofbits; j++)
            {
                if (code[j] == '1')
                    code[j] = '0';
                else if (code[j] == '0')
                {
                    code[j] = '1';
                    j = noofbits;
                }
            }
            LookUpTable[(int)chars[i]].s = chars[i];
            strcpy(LookUpTable[(int)chars[i]].code, code);
            printf("Code: %c - %s\n", chars[i], LookUpTable[(int)chars[i]].code);
        }
    }
}

void Encoder()
{
    printf("Encoded Sequence: ");
    char temp[400];

    for (int i = 0; i < n; i++)
    {
        int index = (int)(s[i]);

        printf("%s", LookUpTable[index].code);
        strcat(encodedseq, LookUpTable[index].code);
    }
    printf("\n");
}

void Decoder(char seq[], int nd)
{
    int cn = 0;
    printf("Decoded sequence: ");
    while (cn < nd)
    {
        struct node *ptr = rptr;
        while (ptr != NULL)
        {
            if (ptr->lcptr == NULL && ptr->rcptr == NULL)
            {
                printf("%c", ptr->symbol);
                ptr = NULL;
            }
            else
            {
                if (seq[cn] == '0')
                    ptr = ptr->lcptr;
                else if (seq[cn] == '1')
                    ptr = ptr->rcptr;
                cn++;
            }
        }
    }
    printf("\n");
}

void print(struct node *ptr)
{
    if (ptr != NULL)
    {
        print(ptr->lcptr);
        printf("\n");
        printf("%c - %d", ptr->symbol, ptr->frequency);
        printf("\n");
        print(ptr->rcptr);
    }
}

int main()
{
    printf("----------------------Huffman Encoding Transmitter-----------------------------\n");

    rptr = NULL;

    int error = ClientCreate(9009, 1, "");
    if (error == 1)
    {
        close(c_socket);
        printf("Connection Issue.\n");
        return 0;
    }
    else
    {
        int choice = 0;
        printf("Enter choice of Huffman Encoding(\n0-const, 1-var):\n ");
        scanf("%d", &choice);

        Read();
        count();

        printf("\nCharecters: %s\n", ds);
        for (int i = 0; i < nc; i++)
        {
            printf("char %c - frequency %d\n", ds[i], f[i]);
        }

        if (choice == 1)
        {
            struct node *ptrarr[200];
            for (int i = 0; i < nc; i++)
                ptrarr[i] = NULL;
            BuildTree(f, ds, nc, ptrarr);
            printf("\n\n");

            printf("\n------------ Variable Size ----------\n");
            char en[200];
            LookupTable(rptr, en, 0, 1, "");
            Encoder();
        }
        else if (choice == 0)
        {
            printf("\n------------ Constant Size ----------\n");
            char en[200];
            LookupTable(rptr, en, 0, 0, ds);
            Encoder();
        }

        char s[1];
        char code[100];

        send(c_socket, ds, sizeof(ds), 0);

        for (int i = 0; i < strlen(ds); i++)
        {
            s[0] = LookUpTable[(int)ds[i]].s;
            strcpy(code, LookUpTable[(int)ds[i]].code);

            send(c_socket, s, sizeof(s), 0);
            send(c_socket, code, sizeof(code), 0);
        }

        send(c_socket, encodedseq, sizeof(encodedseq), 0);
    }

    close(c_socket);

    return 0;
}
