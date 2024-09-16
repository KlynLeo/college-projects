#include <fstream> //fisiere
#include <iostream> //biblioteca standard pt citire si afisare
#include <graphics.h> //pt elementele grafice de la arbori
#include <winbgim.h> //lucrul cu mouse-ul
#include <cstring> //functii cu siruri de caractere
#include <stdlib.h> //functia stoi
#include <string> //pt functia to_string
using namespace std;
ifstream fin("derivata.in");
ofstream fout("deriv.out");

char infix[100], postfixata[100], derivata1[200], derivata2[200];
char sir[50], sir1[50], sir2[50];

int start=1;
int apasat=0;

//STRUCTURI
struct nod
{
    char info;
    nod* urm;
};

struct arb
{
    char info[10];
    arb* st, * dr;
};

struct nod_arb
{
    arb* info;
    nod_arb* urm;
};

struct stiva
{
    nod* varf;
    int nr_elemente;
};

struct stiva_arb
{
    nod_arb* varf;
    int nr_elemente;
};

struct coada
{
    nod* prim;
    nod* ultim;
    int nr_elemente;
};
coada postfix;

void creeaza_stiva(stiva& S)
{
    S.varf = NULL;
    S.nr_elemente = 0;
}

void creeaza_stiva_arb(stiva_arb& S)
{
    S.varf = NULL;
    S.nr_elemente = 0;
}

void creeaza_stiva(coada& C)
{
    C.prim = NULL;
    C.ultim = NULL;
    C.nr_elemente = 0;
}


bool este_vida(stiva S)
{
    return S.nr_elemente == 0;
}

bool este_vida(stiva_arb S)
{
    return S.nr_elemente == 0;
}

bool este_vida(coada C)
{
    return C.nr_elemente == 0;
}


void pop(stiva& S)
{
    if (!este_vida(S))
    {

        nod* varf_nou;
        varf_nou = S.varf->urm;
        delete S.varf;
        S.varf = varf_nou;
        S.nr_elemente--;
    }
}


void pop_arb(stiva_arb& S) // scoate elementul din varful stivei de arbori
{
    if (!este_vida(S))
    {
        nod_arb* varf_nou;
        varf_nou = S.varf->urm;
        delete S.varf;
        S.varf = varf_nou;
        S.nr_elemente--;
    }
}


void push(stiva& S, char element)
{
    nod* nod_nou;

    if (este_vida(S))
    {
        S.nr_elemente = 1;
        S.varf = new nod;
        S.varf->info = element;
        S.varf->urm = NULL;
    }
    else
    {
        S.nr_elemente++;
        nod_nou = new nod;
        nod_nou->info = element;
        nod_nou->urm = S.varf;
        S.varf = nod_nou;
    }
}


void push_arb(stiva_arb& S, arb* element) // adauga un element la stiva de arbori
{
    nod_arb* nod_nou;

    if (este_vida(S))
    {
        S.nr_elemente = 1;
        S.varf = new nod_arb;
        S.varf->info = element;
        S.varf->urm = NULL;
    }
    else
    {
        S.nr_elemente++;
        nod_nou = new nod_arb;
        nod_nou->info = element;
        nod_nou->urm = S.varf;
        S.varf = nod_nou;
    }
}



char coada_pop(coada& C)
{
    char element = C.prim->info;
    nod* primul;
    primul = C.prim;
    C.prim = C.prim->urm;
    delete primul;
    C.nr_elemente--;
    return element;
}


void coada_push(coada& C, char element)
{
    nod* nod_nou;

    if (este_vida(C))
    {
        C.nr_elemente = 1;
        C.prim = new nod;
        C.prim->info = element;
        C.prim->urm = NULL;
        C.ultim = C.prim;
    }
    else
    {
        C.nr_elemente++;
        nod_nou = new nod;
        nod_nou->info = element;
        nod_nou->urm = NULL;
        C.ultim->urm = nod_nou;
        C.ultim = nod_nou;
    }
}

int prioritate(char c) //functie care verifica prioritatea operatorilor (pentru construirea formei postfixate)
{
    if(c=='+'||c=='-')
        return 1;
    else if(c=='*'||c=='/')
        return 2;
    else if(c=='^')
        return 3;
    else if(c=='s'||c=='c'||c=='t'||c=='g'||c=='l')
        return 4;
    else if(c=='(')
        return 5;
    else if(c==')')
        return 6;
    else return 7;
}


//functie care adauga in coada functiile unare
void adauga_in_coada(coada& postfix, stiva& S)
{
    if (S.varf->info == 's')
    {
        coada_push(postfix, 's');
        coada_push(postfix, 'i');
        coada_push(postfix, 'n');
    }
    else if (S.varf->info == 'c')
    {
        coada_push(postfix, 'c');
        coada_push(postfix, 'o');
        coada_push(postfix, 's');
    }
    else if (S.varf->info == 't')
    {
        coada_push(postfix, 't');
        coada_push(postfix, 'g');
    }
    else if (S.varf->info == 'g')
    {
        coada_push(postfix, 'c');
        coada_push(postfix, 't');
        coada_push(postfix, 'g');
    }
    else if (S.varf->info == 'l')
    {
        coada_push(postfix, 'l');
        coada_push(postfix, 'n');
    }
    else
        coada_push(postfix, S.varf->info);

    pop(S);
}


void adaugainstiva(stiva& S, coada& postfix, char& x, int& i)//adauga in stiva operatorii
{

    if (x == 's' && infix[i + 1] == 'i' && infix[i + 2] == 'n')
    {
        push(S, 's');
        i = i + 2;
    }
    else if (x == 'c' && infix[i + 1] == 'o' && infix[i + 2] == 's')
    {
        push(S, 'c');
        i = i + 2;
    }
    else if (x == 't' && infix[i + 1] == 'g')
    {
        push(S, 't');
        i = i + 1;
    }
    else if (x == 'c' && infix[i + 1] == 't' && infix[i + 2] == 'g')
    {
        push(S, 'g');
        i = i + 2;
    }
    else if (x == 'l' && infix[i + 1] == 'n')
    {
        push(S, 'l');
        i = i + 1;
    }
    else
        push(S, x);
}

bool este_operator(char infix[], int i)   // construirea notatie postfixate
{

    if (strchr("+-*/^()", infix[i])) return 1;
    if (strchr("sctglr", infix[i]) && strchr("iogtan", infix[i + 1]))
        return 1;
    return 0;
}

void forma_poloneza(char infix[], coada& postfix) //functie care converteste expresia infixata in postfixata
{

    int i=0;
    int lg = strlen(infix);
    char x;
    stiva S;
    creeaza_stiva(S);
    for (i = 0; i < lg; i++)
    {
        x = infix[i];
        if (!este_operator(infix, i))
            coada_push(postfix, x);
        else
        {

            if (x != '(' && infix[i - 1] != ')' && !strchr("sctglre", x))
                coada_push(postfix, ' ');
            if (x == ')')
            {
                while (S.varf->info != '(')
                {
                    adauga_in_coada(postfix, S);
                    coada_push(postfix, ' ');
                }

                pop(S);
            }
            else if (x != ' ')
            {
                while (!este_vida(S) && (S.varf->info != '(') && (prioritate(S.varf->info) >= prioritate(x)))
                {
                    adauga_in_coada(postfix, S);
                    coada_push(postfix, ' ');
                }
                adaugainstiva(S, postfix, x, i);
            }
        }
    }
    if (!este_operator(infix, i - 1))
        coada_push(postfix, ' ');

    while (S.nr_elemente > 1)
    {
        adauga_in_coada(postfix, S);
        coada_push(postfix, ' ');
    }

    adauga_in_coada(postfix, S);
}


int nrNiveluri(arb* a) //calculeaza cate nivele are un arbore
{

    if (a == NULL)return 0;
    else
    {
        int n1 = nrNiveluri(a->st);
        int n2 = nrNiveluri(a->dr);
        return 1 + max(n1, n2);
    }
}

int nrColoane(arb* a)  //calculeaza cate coloane are un arbore
{

    if (a == NULL)return 0;
    else
    {
        int n1 = nrColoane(a->st);
        int n2 = nrColoane(a->dr);
        return 1 + n1 + n2;
    }
}


int este_operator1(char element[]) //folosim functia la crearea arborelui si verifica daca e operator/functie/operand
{

    if (strchr("+-*/^", element[0]) && element[1] == NULL) return 2;
    if (strchr("sctgl", element[0]) && strchr("iogtn", element[1]) && element[1] != NULL)
        return 1;
    return 0;
}

void adaugaLaarbElement(coada C, stiva_arb& S)//functie care baga forma poloneza in stiva de arbori
{
    int i = 0;
    arb* arb_nou;
    char a[10];
    memset(a, NULL, 10); // punem null pe fiecare element din sir
    while (C.prim != NULL)
    {
        if (C.prim->info == ' ')
        {
            if (!(este_operator1(a)))   //daca este operand (practic elementul dinaintea spatiului care e in sirul a) il adaugam in stiva ca fiind un arbore
            {
                arb_nou = new arb;
                strcpy(arb_nou->info, a);
                arb_nou->st = NULL;
                arb_nou->dr = NULL;
                push_arb(S, arb_nou);
            }
            else if (este_operator1(a) == 2)   //daca este semn il facem arbore si adaugam elementele din stiva(pe care le stergem)apoi adaugam in stiva
            {
                arb_nou = new arb;
                strcpy(arb_nou->info, a);
                arb_nou->dr = S.varf->info;
                pop_arb(S);
                arb_nou->st = S.varf->info;
                pop_arb(S);
                push_arb(S, arb_nou);
            }
            else   //acelasi procedeu pentru functiile unare, dar arborele va contine un singur fiu
            {
                arb_nou = new arb;
                strcpy(arb_nou->info, a);
                arb_nou->dr = NULL;
                arb_nou->st = S.varf->info;
                pop_arb(S);
                push_arb(S, arb_nou);
            }
            memset(a,NULL, strlen(a));
            i = 0;
        }
        else
            a[i++] = C.prim->info; //fiecare element diferit de spatiu intra in a, ca ulterior sa fie vericat mai sus
        C.prim = C.prim->urm;
    }

    if (este_operator1(a) == 2)//repetam pt ultimul element
    {
        arb_nou = new arb;
        strcpy(arb_nou->info, a);
        arb_nou->dr = S.varf->info;
        pop_arb(S);
        arb_nou->st = S.varf->info;
        pop_arb(S);
        push_arb(S, arb_nou);
    }
    else
    {
        arb_nou = new arb;
        strcpy(arb_nou->info, a);
        arb_nou->dr = NULL;
        arb_nou->st = S.varf->info;
        pop_arb(S);
        push_arb(S, arb_nou);
    }
}

void deseneazaNod(char elem[], int xc, int yc, int latime, int inaltime) //functie care deseneaza nodul
{
    char arr[50];
    sprintf(arr, "%s", elem);
    setcolor(WHITE);
    setfillstyle(SOLID_FILL, BLACK);
    fillellipse(xc, yc, 23, 23);
    setbkcolor(BLACK);
    setcolor(WHITE);
    settextjustify(1, 1);
    settextstyle(3, 0, 2);
    outtextxy(xc, yc + 4, arr);
}

void deseneazaarb(arb* a, int niv, int stanga, int latime, int inaltime) //functie care deseneaza arborele
{

    if (a != NULL)
    {
        int n1 = nrColoane(a->st);
        int yc = niv * inaltime - inaltime / 2;
        int xc = stanga + latime * n1 + latime / 2;

        if (a->st != NULL)
        {
            int xcs = stanga + latime * nrColoane(a->st->st) + latime / 2;
            setcolor(WHITE);
            line(xc, yc, xcs, yc + inaltime);
        }

        if (a->dr != NULL)
        {
            int xcd = stanga + latime * (n1 + 1) + latime * nrColoane(a->dr->st) + latime / 2;
            setcolor(WHITE);
            line(xc, yc, xcd, yc + inaltime);
        }
        deseneazaarb(a->st, niv + 1, stanga, latime, inaltime);
        deseneazaarb(a->dr, niv + 1, stanga + latime * (n1 + 1), latime, inaltime);
        deseneazaNod(a->info, xc, yc, latime, inaltime);
    }
}
arb* simplificare(arb* a)//functie care simplifica arborele
{
    int op1 = 0, op2 = 0, rez = 0;
    if (!strcmp(a->info, "*"))
    {
        if (!strcmp(a->dr->info, "*")){
            cout << "suntem aici";
            if (!strchr ("123456789", a->dr->st->info[0])){
                a->st->info[0] = '0' + a->st->dr->info[0] * a->st->info[0];
                strcpy (a->dr->info,a->dr->dr->info);
                a->dr->st = NULL;
                a->dr->dr = NULL;
            }
            if (!strchr ("123456789", a->dr->dr->info[0])){
                a->st->info[0] = '0' + a->dr->dr->info[0] * a->st->info[0];
                a->dr = a->dr->st;
                a->dr->st = NULL;
                a->dr->dr = NULL;
            }
        }
         else if (este_operator1(a->st->info) == 0 && strcmp(a->st->info, "x")
            && este_operator1(a->dr->info) == 0 && strcmp(a->dr->info, "x")){
            op1 = stoi(a->st->info);
            op2 = stoi(a->dr->info);
            rez = op1*op2;
            string str = to_string(rez);
            for (size_t i = 0; i < str.length() && i < 10; i++)
                    a->info[i] = str[i];
            a->info [str.length()] = '\0';
            a->st = NULL;
            a->dr = NULL;
    }
        else if (!strcmp(a->st->info, "0") || !strcmp(a->dr->info, "0"))
        {
            strcpy(a->info, "0");
            a->st = NULL;
            a->dr = NULL;
        }
        else if (!strcmp(a->st->info, "1"))
        {
            a = a->dr;
        }
        else if (!strcmp(a->dr->info, "1"))
        {
            a = a->st;
        }
    }

    if (!strcmp(a->info, "/"))
    {
        if (!strcmp(a->st->info, "0"))
        {
            strcpy(a->info, "0");
            a->st = NULL;
            a->dr = NULL;
        }
        else if (!strcmp(a->dr->info, "1"))
        {
            a = a->st;
        }
    }
    if (!strcmp(a->info, "+"))
    {
        if (este_operator1(a->st->info) == 0 && strcmp(a->st->info, "x")
            && este_operator1(a->dr->info) == 0 && strcmp(a->dr->info, "x")){
            op1 = stoi(a->st->info);
            op2 = stoi(a->dr->info);
            rez = op1+op2;
            string str = to_string(rez);
            for (size_t i = 0; i < str.length() && i < 10; i++)
                    a->info[i] = str[i];
            a->info [str.length()] = '\0';
            a->st = NULL;
            a->dr = NULL;
    }
        else if (!strcmp(a->st->info, "0"))
        {
            a = a->dr;
        }
        else if (!strcmp(a->dr->info, "0"))
        {
            a = a->st;
        }
        else if (!strcmp(a->dr->info, "-"))
        {
            a->dr = a->dr->dr;
            strcpy(a->info, "-");
        }
    }

    if (!strcmp(a->info, "-"))
    {
        if (!strcmp(a->dr->info, "0"))
        {
            a = a->st;
        }
        else if (!strcmp(a->dr->info, "-"))
        {
            a->dr = a->dr->dr;
            strcpy(a->info, "+");
        }
    }

    if (!strcmp(a->info, "^"))
    {
        if (!strcmp(a->st->info, "0"))
        {
            strcpy(a->info, "0");
            a->st = NULL;
            a->dr = NULL;
        }
        else if (!strcmp(a->dr->info, "0"))
        {
            strcpy(a->info, "1");
            a->st = NULL;
            a->dr = NULL;
        }
        else if (!strcmp(a->st->info, "1"))
        {
            strcpy(a->info, "1");
            a->st = NULL;
            a->dr = NULL;
        }
        else if (!strcmp(a->dr->info, "1"))
        {
            a = a->st;
        }
    }

    if (!strcmp(a->info, "ln"))
    {
        if (!strcmp(a->st->info, "e"))
        {
            strcpy(a->info, "1");
            a->st = NULL;
            a->dr = NULL;
        }
    }

    return a;
}

arb* deriveaza(arb* a, arb* d) //functie care deriveaza arborele
{
    arb* a1, * a2, * a3, * a4, * a5, * a6, * a7, * a8;

    if (strchr("0123456789", a->info[0])) strcpy(d->info, "0"); // numere pozitive
    if ((a->info[0] == '-') && strchr("0123456789", a->info[1])) strcpy(d->info, "0"); //numere negative

    if (a->info[0] == 'x') strcpy(d->info, "1"); // daca este variabila

    if (((a->info[0] == '+') || (a->info[0] == '-')) && (a->info[1] == NULL))   // derivam adunarea si scaderea
    {
        a1 = new arb;
        a1->st = NULL;
        a1->dr = NULL;
        a2 = new arb;
        a2->st = NULL;
        a2->dr = NULL;
        deriveaza(a->st, a1);
        a1 = simplificare(a1);
        d->st = a1;
        deriveaza(a->dr, a2);
        a2 = simplificare(a2);
        d->dr = a2;
        strcpy(d->info, a->info);
        d = simplificare(d);
    }

    if (a->info[0] == '*')   // derivam inmultirea
    {
        a1 = new arb;
        a1->st = NULL;
        a1->dr = NULL;
        a2 = new arb;
        a2->st = NULL;
        a2->dr = NULL;
        a3 = new arb;
        a3->st = NULL;
        a3->dr = NULL;
        a4 = new arb;
        a4->st = NULL;
        a4->dr = NULL;
        deriveaza(a->st, a1);
        a1 = simplificare(a1);
        deriveaza(a->dr, a2);
        a2 = simplificare(a2);
        strcpy(a3->info, "*");
        a3->st = a1;
        a3->dr = a->dr;
        a3 = simplificare(a3);
        strcpy(a4->info, "*");
        a4->st = a->st;
        a4->dr = a2;
        a4 = simplificare(a4);
        strcpy(d->info, "+");
        d->st = a3;
        d->dr = a4;
        d = simplificare(d);
    }

    if (a->info[0] == '/')   // derivam impartirea
    {
        a1 = new arb;
        a1->st = NULL;
        a1->dr = NULL;
        a2 = new arb;
        a2->st = NULL;
        a2->dr = NULL;
        a3 = new arb;
        a3->st = NULL;
        a3->dr = NULL;
        a4 = new arb;
        a4->st = NULL;
        a4->dr = NULL;
        a5 = new arb;
        a5->st = NULL;
        a5->dr = NULL;
        a6 = new arb;
        a6->st = NULL;
        a6->dr = NULL;
        a7 = new arb;
        a7->st = NULL;
        a7->dr = NULL;
        strcpy(a7->info, "2");
        deriveaza(a->st, a1);
        a1 = simplificare(a1);
        deriveaza(a->dr, a2);
        a2 = simplificare(a2);
        strcpy(a3->info, "*");
        a3->st = a1;
        a3->dr = a->dr;
        a3 = simplificare(a3);
        strcpy(a4->info, "*");
        a4->st = a->st;
        a4->dr = a2;
        a4 = simplificare(a4);
        strcpy(a5->info, "-");
        a5->st = a3;
        a5->dr = a4;
        a5 = simplificare(a5);
        strcpy(a6->info, "^");
        a6->st = a->dr;
        a6->dr = a7;
        a6 = simplificare(a6);
        strcpy(d->info, "/");
        d->st = a5;
        d->dr = a6;
        d = simplificare(d);
    }

    if (a->info[0] == '^')   // derivam ridicarea la puterea/exponentiala
    {
        if (strchr("0123456789e", a->st->info[0]))   // derivam exponentiala
        {
            a1 = new arb;
            a1->st = NULL;
            a1->dr = NULL;
            a2 = new arb;
            a2->st = a->st;
            a2->dr = NULL;
            strcpy(a2->info, "ln");
            a2 = simplificare(a2);
            a3 = new arb;
            a3->st = a;
            a3->dr = a2;
            strcpy(a3->info, "*");
            a3 = simplificare(a3);
            deriveaza(a->dr, a1);
            a1 = simplificare(a1);
            strcpy(d->info, "*");
            d->st = a3;
            d->dr = a1;
            d = simplificare(d);
        }
        else   // derivam ridicarea la putere a lui x
        {
            a1 = new arb;
            a1->st = NULL;
            a1->dr = NULL;
            a2 = new arb;
            a2->st = NULL;
            a2->dr = NULL;
            strcpy(a2->info, "1");
            a3 = new arb;
            a3->st = a->dr;
            a3->dr = a2;
            strcpy(a3->info, "-");
            a3 = simplificare(a3);
            a3->info[0] = '0' + a3->st->info[0] - a3->dr->info[0];
            a3->st = NULL;
            a3->dr = NULL;
            a4 = new arb;
            a4->st = a->st;
            a4->dr = a3;
            strcpy(a4->info, "^");
            a4 = simplificare(a4);
            a5 = new arb;
            a5->st = a->dr;
            a5->dr = a4;
            strcpy(a5->info, "*");
            a5 = simplificare(a5);
            deriveaza(a->st, a1);
            a1 = simplificare(a1);
            strcpy(d->info, "*");
            d->st = a5;
            d->dr = a1;
            d = simplificare(d);
        }
    }

    if (!strcmp("sin", a->info))   // derivam sinus
    {
        a1 = new arb;
        a1->st = NULL;
        a1->dr = NULL;
        a2 = new arb;
        a2->st = a->st;
        a2->dr = NULL;
        strcpy(a2->info, "cos");
        deriveaza(a->st, a1);
        a1 = simplificare(a1);
        strcpy(d->info, "*");
        d->st = a2;
        d->dr = a1;
        d = simplificare(d);
    }

    if (!strcmp("cos", a->info))   // derivam cosinus
    {
        a1 = new arb;
        a1->st = NULL;
        a1->dr = NULL;
        a2 = new arb;
        a2->st = a->st;
        a2->dr = NULL;
        strcpy(a2->info, "sin");
        a3 = new arb;
        a3->st = NULL;
        a3->dr = NULL;
        deriveaza(a->st, a1);
        a1 = simplificare(a1);
        strcpy(a3->info, "*");
        a3->st = a2;
        a3->dr = a1;
        a3 = simplificare(a3);
        strcpy(d->info, "-");
        d->st = NULL;
        d->dr = a3;
        d = simplificare(d);
    }

    if (!strcmp("tg", a->info))   // derivam tangent
    {
        a1 = new arb;
        a1->st = NULL;
        a1->dr = NULL;
        strcpy(a1->info, "1");
        a2 = new arb;
        a2->st = NULL;
        a2->dr = NULL;
        strcpy(a2->info, "2");
        a3 = new arb;
        a3->st = a->st;
        a3->dr = NULL;
        strcpy(a3->info, "cos");
        a4 = new arb;
        a4->st = a3;
        a4->dr = a2;
        strcpy(a4->info, "^");
        a4 = simplificare(a4);
        a5 = new arb;
        a5->st = a1;
        a5->dr = a4;
        strcpy(a5->info, "/");
        a5 = simplificare(a5);
        a6 = new arb;
        a6->st = NULL;
        a6->dr = NULL;
        deriveaza(a->st, a6);
        a6 = simplificare(a6);
        strcpy(d->info, "*");
        d->st = a5;
        d->dr = a6;
        d = simplificare(d);
    }

    if (!strcmp("ctg", a->info))   // derivam cotangent
    {
        a1 = new arb;
        a1->st = NULL;
        a1->dr = NULL;
        strcpy(a1->info, "1");
        a2 = new arb;
        a2->st = NULL;
        a2->dr = NULL;
        strcpy(a2->info, "2");
        a3 = new arb;
        a3->st = a->st;
        a3->dr = NULL;
        strcpy(a3->info, "sin");
        a4 = new arb;
        a4->st = a3;
        a4->dr = a2;
        strcpy(a4->info, "^");
        a4 = simplificare(a4);
        a5 = new arb;
        a5->st = a1;
        a5->dr = a4;
        strcpy(a5->info, "/");
        a5 = simplificare(a5);
        a6 = new arb;
        a6->st = NULL;
        a6->dr = NULL;
        deriveaza(a->st, a6);
        a6 = simplificare(a6);
        a8 = new arb;
        a8->st = a5;
        a8->dr = a6;
        strcpy(a8->info, "*");
        a8 = simplificare(a8);
        strcpy(d->info, "-");
        d->st = NULL;
        d->dr = a8;
        d = simplificare(d);
    }

    if (!strcmp("ln", a->info))   // derivam logaritm natural
    {
        a1 = new arb;
        a1->st = NULL;
        a1->dr = NULL;
        strcpy(a1->info, "1");
        a2 = new arb;
        a2->st = a1;
        a2->dr = a->st;
        strcpy(a2->info, "/");
        a2 = simplificare(a2);
        a3 = new arb;
        a3->st = NULL;
        a3->dr = NULL;
        deriveaza(a->st, a3);
        a3 = simplificare(a3);
        strcpy(d->info, "*");
        d->st = a2;
        d->dr = a3;
        d = simplificare(d);
    }
    return d;
}

int prioritate1(char c[]) //functie care verifica prioritatea operatorilor (pentru parcurgerea arborelui derivat)
{
    if (!strcmp(c, "+"))
        return 1;
    if (!strcmp(c, "-"))
        return 2;
    if (!strcmp(c, "*"))
        return 3;
    if (!strcmp(c, "^"))
        return 4;
    if (!strcmp(c, "/"))
        return 5;
    else
        return 6;
}

void parcurgere(arb* a, char deriv[]) // functie care parcurge arborele si retine formula intr-un sir de caractere
{
    if (a != NULL)
    {
        if (este_operator1(a->info) == 1)   // parcurge in preordine pentru functii
        {
            strcat(deriv, a->info);
            strcat(deriv, "(");
            parcurgere(a->st, deriv);
            parcurgere(a->dr, deriv);
            strcat(deriv, ")");
        }
        else   // parcurgere in inordine (pentru restul)
        {
            if ((a->st != NULL) && (prioritate1(a->info) > prioritate1(a->st->info)))
            {
                strcat(deriv, "(");
                parcurgere(a->st, deriv);
                strcat(deriv, ")");
            }
            else
                parcurgere(a->st, deriv);
            strcat(deriv, a->info);
            if ((a->dr != NULL) && (prioritate1(a->info) > prioritate1(a->dr->info)))
            {
                strcat(deriv, "(");
                parcurgere(a->dr, deriv);
                strcat(deriv, ")");
            }
            else parcurgere(a->dr, deriv);
        }

    }
}


// REALIZARE MENIU


void butoane()
{
    readimagefile("fundal.jpg", 0, 0, 1265, 1000);
    readimagefile("INFORMATII.jpg",24,600,224,700);
    readimagefile("INTRODUCEFORMULA.jpg",232,600,432,700);
    readimagefile("DERIVATA.jpg",440,600,640,700);
    readimagefile("DERIVATA2.jpg",648,600,848,700);
    readimagefile("ARBORE.jpg",856,600,1056,700);
    readimagefile("FORMULAFISIER.jpg",1064,600,1264,700);
}

// pentru butoane
void Apasa(int& ok)
{
    int coordx, coordy;
    clearmouseclick(WM_LBUTTONDOWN);
    while (!ismouseclick(WM_LBUTTONDOWN))   // verificam daca se apasa mouse-ul
    {
        coordx = mousex();
        coordy = mousey();
    }

    if (coordx > 24 && coordx < 224 && coordy>600 && coordy <700) //daca se apasa pe informatii proiect
        ok = 1;
    else if (coordx > 232 && coordx < 432 && coordy>600 && coordy < 700) //daca se apasa pe introduce formula
        ok = 2;
    else if (coordx > 440 && coordx < 640 && coordy>600 && coordy <700) //daca se apasa derivata
        ok = 3;
    else if (coordx > 648 && coordx < 848 && coordy>600 && coordy <700) //daca se apasa derivata 2
        ok = 4;
    else if (coordx > 856 && coordx < 1056 && coordy>600 && coordy <700 && apasat == 0) //daca se apasa arborele formulei
        ok = 5;
    else if (coordx > 1064 && coordx <1264 && coordy>600 && coordy <700) //daca se apasa formula fisier
        ok = 6;
}

void meniu(int start)
{
    int ok;
    Apasa(ok);
    clearmouseclick(WM_LBUTTONDOWN);
    switch (ok)
    {
        Apasa(ok);
    case 0:
    {
        closegraph();
        break;
    }
    case 1:   // INFORMATII PROIECT
    {
        setactivepage(0);
        butoane();
        setfillstyle(SOLID_FILL,BLACK);
        bar(24, 20, 1265, 600);
        setvisualpage(0);
        settextstyle(10,0,3);
        outtextxy(40, 30, "1.Programul accepta functiile: +, -, *, /, ^, e^, sin, cos, tg, ctg, ln");
        delay(500);
        outtextxy(40, 120, "2. Programul accepta necunoscute precum 'x' ");
        delay(500);
        outtextxy(40,210, "3. Dupa introducerea formulei de la tastatura se apasa ENTER");
        delay(500);
        outtextxy(40, 300, "4. Programul poate citi si expresii matematice apasand butonul derivata.in");
        delay(500);
        outtextxy(40, 390, "5. Daca este scris ceva gresit se poate folosi BACKSPACE");
        delay(500);
        outtextxy(40, 480, "6. Profesor seminar: Ionita Alexandru");
        delay(500);
        outtextxy(40,570,"7. Proiect realizat de Bulai Vali-Danut si Leonte Calin");
        meniu(start);
        break;
    }
    case 2:   // CITIRE DE LA TASTATURA
    {
        setactivepage(0);
        butoane();
        setfillstyle(SOLID_FILL,BLACK);
        bar(24, 20, 1265, 600);
        setvisualpage(0);
        memset(infix, NULL, 100);
        char c;
        int poz = 0;
        settextstyle(10,0,2);
        outtextxy(40, 110, "Introduceti formula de la tastatura:");
        c = getch();
        while (c != 13)
        {
            if (c == 8 && poz)
            {
                poz--;
                infix[poz] = NULL;
                setactivepage(0);
                butoane();
                bar(20, 20, 1265, 600);
                setvisualpage(0);
                outtextxy(40, 110, "Introduceti formula de la tastatura:");
            }
            else if (c != 8)
            {
                infix[poz] = c;
                poz++;
                infix[poz] = NULL;
            }
            outtextxy(40, 132, infix);
            c = getch();
        }
        meniu(start);
        break;
    }

    case 3: //derivata
    {
        apasat = 1;
        setactivepage(0);
        butoane();
        setfillstyle(SOLID_FILL,BLACK);
        bar(24, 20, 1265, 600);
        setvisualpage(0);
        creeaza_stiva(postfix);
        forma_poloneza(infix, postfix);
        stiva_arb S;
        creeaza_stiva_arb(S);
        adaugaLaarbElement(postfix, S);
        arb* d;
        d = new arb;
        d->st = NULL;
        d->dr = NULL;
        d = deriveaza(S.varf->info, d);
        memset(derivata1, NULL, 200);
        parcurgere(d, derivata1);
        settextstyle(10,0,2);
        outtextxy(40, 110, "Derivata expresiei");
        outtextxy(40, 132, infix);
        outtextxy(40, 154, "este:");
        if (strlen(derivata1) < 50)
            outtextxy(40, 176, derivata1);
        else if (strlen(derivata1) < 100)
        {
            strncpy(sir, derivata1, 50);
            sir[50] = NULL;
            outtextxy(40, 176, sir);
            outtextxy(40, 198, derivata1 + 50);
        }
        else
        {
            strncpy(sir1, derivata1, 50);
            sir1[50] = NULL;
            strncpy(sir2, derivata1 + 50, 50);
            sir2[50] = NULL;
            outtextxy(40, 176, sir1);
            outtextxy(40, 198, sir2);
            outtextxy(40, 220, derivata1 + 100);
        }
        setbkcolor(BLACK);
        int coordx, coordy;
        clearmouseclick(WM_LBUTTONDOWN);
        while (!ismouseclick(WM_LBUTTONDOWN))
        {
            coordx = mousex();
            coordy = mousey();
        }
        if (coordx > 856 && coordx < 1056 && coordy>600 && coordy <700 && apasat == 1)
        {
            int height = 697, width = 1200;
            int window2 = initwindow(width, height, "arborele derivatei");
            setcurrentwindow(window2);
            setcolor(WHITE);
            setbkcolor(BLACK);
            cleardevice();
            rectangle(1, 1, width - 1, height - 1);
            int latime, inaltime;
            latime = width / nrColoane(d);
            inaltime = height / nrNiveluri(d);
            deseneazaarb(d, 1, 0, latime, inaltime);
            setbkcolor(BLACK);
            outtextxy(110, 35, "INAPOI");
            int coordx, coordy;
            clearmouseclick(WM_LBUTTONDOWN);
            while (!ismouseclick(WM_LBUTTONDOWN))
            {
                coordx = mousex();
                coordy = mousey();
            }
            if (coordx > 100 && coordx < 300 && coordy > 20 && coordy < 70)
                closegraph(CURRENT_WINDOW);
        }

        fout << "Derivata expresiei " << infix << " este: " << derivata1 << endl;

        setcurrentwindow(0);
        meniu(start);
        break;
    }
    case 4:   // DERIVATA DE ORDINUL 2
    {
        apasat = 2;
        setactivepage(0);
        butoane();
        bar(20, 20, 1265, 600);
        setvisualpage(0);
        creeaza_stiva(postfix);
        forma_poloneza(infix, postfix);
        stiva_arb S;
        creeaza_stiva_arb(S);
        adaugaLaarbElement(postfix, S);
        arb* d;
        d = new arb;
        d->st = NULL;
        d->dr = NULL;
        d = deriveaza(S.varf->info, d);
        arb* d1;
        d1 = new arb;
        d1->st = NULL;
        d1->dr = NULL;
        d1 = deriveaza(d, d1);
        memset(derivata2, NULL, 200);
        parcurgere(d1, derivata2);
        outtextxy(40, 110, "Derivata de ordinul 2 a expresiei");
        outtextxy(40, 132, infix);
        outtextxy(40, 154, "este:");
        if (strlen(derivata2) < 50)
            outtextxy(40, 176, derivata2);
        else if (strlen(derivata2) < 100)
        {
            strncpy(sir, derivata2, 50);
            sir[50] = NULL;
            outtextxy(40, 176, sir);
            outtextxy(40, 198, derivata2 + 50);
        }
        else
        {
            strncpy(sir1, derivata2, 50);
            sir1[50] = NULL;
            strncpy(sir2, derivata2 + 50, 50);
            sir2[50] = NULL;
            outtextxy(40, 176, sir1);
            outtextxy(40, 198, sir2);
            outtextxy(40, 220, derivata2 + 100);
        }
        int coordx, coordy;
        clearmouseclick(WM_LBUTTONDOWN);
        while (!ismouseclick(WM_LBUTTONDOWN))
        {
            coordx = mousex();
            coordy = mousey();
        }
        if (coordx > 856 && coordx < 1056 && coordy>600 && coordy <700 && apasat == 2)
        {
            int height = 697, width = 1265;
            int window3 = initwindow(width, height, "arborele derivatei de ordinul 2");
            setcurrentwindow(window3);
            cleardevice();
            rectangle(1, 1, width - 1, height - 1);
            int latime, inaltime;
            latime = width / nrColoane(d1);
            inaltime = height / nrNiveluri(d1);
            deseneazaarb(d1, 1, 0, latime, inaltime);
            setbkcolor(BLACK);
            outtextxy(110, 35, "INAPOI");
            int coordx, coordy;
            clearmouseclick(WM_LBUTTONDOWN);
            while (!ismouseclick(WM_LBUTTONDOWN))
            {
                coordx = mousex();
                coordy = mousey();
            }
            if (coordx > 100 && coordx < 300 && coordy > 20 && coordy < 70)
                closegraph(CURRENT_WINDOW);
        }

        fout << "Derivata de ordinul 2 a expresiei " << infix << " este: " << derivata2 << endl;

        setcurrentwindow(0);
        meniu(start);
        break;

    }
    case 5:   // arborele FORMULEI
    {

        setactivepage(0);
        butoane();
        setfillstyle(SOLID_FILL,BLACK);
        bar(24, 20, 1265, 600);
        setvisualpage(0);
        creeaza_stiva(postfix);
        forma_poloneza(infix, postfix);
        stiva_arb S;
        creeaza_stiva_arb(S);
        adaugaLaarbElement(postfix, S);
        outtextxy(40, 132, infix);
        int height = 700, width = 1265;
        int window1 = initwindow(width, height, "arborele formulei");
        setcurrentwindow(window1);
        cleardevice();
        rectangle(1, 1, width - 1, height - 1);
        int latime, inaltime;
        latime = width / nrColoane(S.varf->info);
        inaltime = height / nrNiveluri(S.varf->info);
        deseneazaarb(S.varf->info, 1, 0, latime, inaltime);
        setbkcolor(BLACK);
        outtextxy(110, 35, "INAPOI");
        int coordx, coordy;
        clearmouseclick(WM_LBUTTONDOWN);
        while (!ismouseclick(WM_LBUTTONDOWN))
        {
            coordx = mousex();
            coordy = mousey();
        }
        if (coordx > 100 && coordx < 300 && coordy > 20 && coordy < 70)
            closegraph(CURRENT_WINDOW);
        setcurrentwindow(0);
        meniu(start);
        break;
    }
    case 6:
    {
        setactivepage(0);
        butoane();
        setfillstyle(SOLID_FILL,BLACK);
        bar(24, 20, 1265, 600);
        setvisualpage(0);
        memset(infix, NULL, 100);
        fin >> infix;
        settextstyle(10,0,2);
        outtextxy(40, 110, "Formula introdusa din fisierul derivata.in este:");
        outtextxy(40, 132, infix);
        meniu(start);
        break;

    }
    }
}

int main()
{
    initwindow(1265, 700, "DERIVARE");
    setactivepage(0);
    butoane();
    setfillstyle(SOLID_FILL,BLACK);
    bar(24, 20, 1265, 600);
    setvisualpage(0);
    meniu(start);
    return 0;
}
