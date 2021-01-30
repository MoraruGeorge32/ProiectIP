#include <iostream>
#include <cstring>
#include<graphics.h>
using namespace std;
#define LATIME_ECRAN 800
#define LUNGIME_ECRAN 1200
struct pozitie
{
    int x, y;
};
struct nod
{
    char* SirNod;
    nod* st;
    nod* dr;
    pozitie Pozitie;
};
bool EsteMeniul, EsteAplicatie, DesenatMeniu, Gresit;
pozitie PozitieMouse;
nod * ListaNoduri[100]; // retin toate nodurile intr o lista ca mai apoi sa le pot dealoca cand reconstruiesc arborele
int lgExpresie, IndiceDesenare, vfStiva, lgOperanzi, lungimeSirIntermediar, lgListaNoduri, OffsetStartPosX, OffsetStartPosY, DistantaIntreRanduri = 80, DistantaIntreFii, NrPDesc, NrPInc, DIMENSIUNE_TEXT;
char Expresie[100], Operanzi[101][101], StivaOperatori[100], Expresie2[100];
void Push_StivaOperatori(char c)
{
    StivaOperatori[++vfStiva] = c;
}
char Pop_StivaOperatori()
{
    return StivaOperatori[vfStiva--];
}
char Top_StivaOperatori()
{
    return StivaOperatori[vfStiva];
}
void GolireSir(char SirIntermediar[])
{
    int i=0;
    while(SirIntermediar[i+1] != '\0')
    {
        SirIntermediar[i] = '\0';
        i++;
    }
    SirIntermediar[i] = '\0';
}
int Prioritate(char c)
{
    switch(c)
    {
    case '^':
        return 3;
    case '*':
        return 2;
    case '/':
        return 2;
    case '+':
        return 1;
    case '-':
        return 1;
    }
    return -1;
}
void EvalueazaExpresia(char* Expresie)
{
    if(Gresit)
    {
        return;
    }
    char SirIntermediar[100] = {0};
    lgExpresie = strlen(Expresie);
    for(int i = 0 ; i < lgExpresie; i++)
    {
        char c = Expresie[i]; // extrag caracterul curent
        int prioritate = Prioritate(c);// aflu prioritatea caracterului curent
        if(prioritate > 0) // fac asta atat timp cat am un operator
        {

            if(lungimeSirIntermediar > 0) // copiez intermediarul adica operanzii in vectorul de operanzi
            {
                strcpy(Operanzi[++lgOperanzi], SirIntermediar);
                GolireSir(SirIntermediar);
                lungimeSirIntermediar=0;
            }
            int prioritate2 = Prioritate(Top_StivaOperatori()); //prioritatea elementului anterior

            if(prioritate2 > 0) // daca elementul anterior e operator
            {
                while(prioritate2 >= prioritate) // parcurc toti operatorii si ii pun pe toti in stiva operanzi daca prioritatea lor este mai mare sau egala cu prioritatea operatorului curent
                {
                    char p = Pop_StivaOperatori();
                    prioritate2 = Prioritate(p);
                    if(prioritate2 >= prioritate)
                    {
                        Operanzi[++lgOperanzi][0] = p;
                    }
                    else
                    {
                        Push_StivaOperatori(p); // il pun inapoi pe precedentul daca nu este mai mare ca si curentul
                    }
                }
            }
            Push_StivaOperatori(c); // il pun pe operatorul curent in stiva
        }
        else if(c == ')') // daca e paranteza inchisa pun toti operatorii din stiva in vector pana la urmatoarea '('
        {
            if(lungimeSirIntermediar > 0) // inseamna ca data trecuta a fost un operand si trebuie sa l bag in pseudo stiva matrice de operanzi
            {
                strcpy(Operanzi[++lgOperanzi], SirIntermediar);
                GolireSir(SirIntermediar);
                lungimeSirIntermediar = 0;
            }
            while(Top_StivaOperatori() != '(')
            {
                char p;
                p = Pop_StivaOperatori();
                Operanzi[++lgOperanzi][0]=p;
            }
            vfStiva--; // scot si ( din stiva
        }
        else if(c == '(')
        {
            Push_StivaOperatori(c);
        }
        else
        {
            SirIntermediar[lungimeSirIntermediar++] = c;
        }
    }
    if(lungimeSirIntermediar > 0) // inseamna ca a fost operand si trebuie sa l copiez in pseudo stiva matrice de operanzi
    {
        strcpy(Operanzi[++lgOperanzi], SirIntermediar);
        GolireSir(SirIntermediar);
        lungimeSirIntermediar=0;
    }
    while(vfStiva > 0) // la sfarsit pun tot ce a ramas din stiva de operatori in stiva de operanzi
    {
        char p;
        p = Pop_StivaOperatori();
        Operanzi[++lgOperanzi][0]=p;
    }
}
void CorecteazaExpresia(char * expresie) // corectez expresia de chestii de genu 2x = 2*x etc.
{
    char stivaparanteze[100] = {0}; // folosesc asta sa verific corectitudinea parantezelor
    int indice = 0, lg = strlen(expresie), vfStivaParanteze = 0;
    while(indice < lg)
    {
        if(expresie[indice] >= '0' && expresie[indice] <= '9') // caut sa vad daca gasesc o cifra
        {
            if((expresie[indice+1] >= 'A' && expresie[indice+1] <= 'Z' ) || (expresie[indice+1] >= 'a' && expresie[indice+1] <= 'z') || expresie[indice+1] == '(')
            {
                strcpy(Expresie2,expresie+indice+1); // copiez intr un sir auxiliar , pentru ca mai apoi sa pot concatena 2 cu * si cu a sau ( sau oricce alta variabila de langa
                strcpy(expresie+indice+1,"*");
                strcat(expresie,Expresie2);
                lg++;
            }
        }
        indice++;
    }
    bool ok = false;
    for(int i = 0 ; i< lg ; i ++) // parcurg sirul si bag parantezele in stiva parantezele
    {
        if(expresie[i] == '(')
        {
            stivaparanteze[vfStivaParanteze++] = '(';
        }
        else if( expresie[i] == ')' && vfStiva > 0) // daca am gasit paranteza inchisa, si in varfu stivei e o paranteza deschisa , inseamna ca se completeaza si le scot
        {
            if(stivaparanteze[vfStiva - 1] == '(')
            {
                vfStiva--;
            }
            else
            {
                Gresit = true;
                break; // daca nu se completeaza inseamna ca i gresita expresia
            }
        }
        else if(expresie [i]!= '(' && expresie[i]!=')')
        {
            ok = true;
        }
    }
    if(vfStiva > 0 || !ok) // daca au mai ramas paranteze in stiva inseamna ca i gresita expresia din punct de vedere sintactic
    {
        Gresit = true; // ok adica daca nu sunt numai paranteze in expresie
    }
}
void DeseneazaDreptunghi(pozitie pos, int lungime, int latime) // functie speciala ca sa mi desenez propriul dreptunghi folosind funtia de desenare a unui poligon
{
    int puncte[8];
    // trebuie sa tex fiecare coordonata intr un vector si apoi sa l dau ca parametru functiei de creare a poligonului ca mai apoi sa mi poata uni acele puncte
    puncte[0] = pos.x - lungime/2, puncte[1] = pos.y - latime/2, puncte[2] = pos.x - lungime/2, puncte[3] = pos.y + latime/2, puncte[4] = pos.x + lungime/2, puncte[5] = pos.y + latime/2, puncte[6] = pos.x + lungime/2, puncte[7] = pos.y - latime/2, puncte[8] = pos.x - lungime/2, puncte[9] = pos.y - latime/2;
    drawpoly(5,puncte);
    //am folosit poligon si nu simpla functie line pentru ca este mai usor de umplut mai apoi cu culoarea dorita dreptunghiul
    fillpoly(5,puncte);
}
void StergeArboreDinMemorie() // sterg arborele alocat in memorie ca mai apoi sa pot creea altul
{
    for(int i = 0; i < lgListaNoduri ; i ++)
    {
        delete ListaNoduri[i];
    }
    lgListaNoduri = 0;
}
int LatimeSubArbore(nod * nodul) // latimea subarborelui returneaza suma latimilor fiecarui nod din acel subarbore
{
    if(nodul == NULL)
    {
        return 0;
    }
    return textwidth(nodul->SirNod) + DistantaIntreFii + LatimeSubArbore(nodul->dr) + LatimeSubArbore(nodul->st);
}
void DeseneazaTextCentrat(pozitie pos, char* text) // am sa fac o functie care va desena un string intreg centrat inloc sa il deseneze in colt
{
    int pozitieNouay = pos.y - textheight(text)/2;
    int pozitieNouaX = pos.x - textwidth(text)/2;
    outtextxy(pozitieNouaX, pozitieNouay, text);
}
void CreeazaArbore(nod *& nodul) // creez arborele in memorie
{
    IndiceDesenare--;
    nodul = new nod;
    nodul->st = nodul->dr = NULL;
    nodul->SirNod = Operanzi[IndiceDesenare];
    ListaNoduri[lgListaNoduri++] = nodul;
    if(Prioritate(Operanzi[IndiceDesenare][0])> 0)
    {
        CreeazaArbore(nodul->dr);
        CreeazaArbore(nodul->st);
    }
}
void DeseneazaArbore(nod * nodul, pozitie pos)
{
    if(Prioritate(nodul->SirNod[0]) > 0) // vad daca este operator
    {
        // desenez nodul si il atribui in lista
        nodul->Pozitie = pos;

        // setez pozitia urmatorului nod din dreapta
        pozitie PozNouaDrp;
        PozNouaDrp.x = pos.x + LatimeSubArbore(nodul->dr);
        PozNouaDrp.y = pos.y + DistantaIntreRanduri;

        //desenez linia intre nodul curent si urmatorul
        line(pos.x, pos.y, PozNouaDrp.x, PozNouaDrp.y);
        //desenez nodul de operator
        fillellipse(pos.x, pos.y, textwidth(nodul->SirNod), textwidth(nodul->SirNod));

        //desenez subarborele drept
        DeseneazaArbore(nodul -> dr, PozNouaDrp);

        // desenez operand dreapta

        pozitie PozNouaStg;
        PozNouaStg.x = pos.x - LatimeSubArbore(nodul->st);
        PozNouaStg.y = pos.y + DistantaIntreRanduri;
        line(pos.x, pos.y, PozNouaStg.x, PozNouaStg.y);

        fillellipse(pos.x, pos.y, textwidth(nodul->SirNod), textwidth(nodul->SirNod)); // umplu operatorul precedent (fundalul)
        DeseneazaTextCentrat(pos, nodul->SirNod);

        //desenez subarborele stang
        DeseneazaArbore(nodul->st, PozNouaStg);
    }
    else
    {
        DeseneazaDreptunghi(pos, textwidth(nodul->SirNod) + 15, textheight(nodul->SirNod) + 15);
        DeseneazaTextCentrat(pos, nodul->SirNod);
    }
}
void ArboreleExpresiei()
{
    settextstyle(1,0,DIMENSIUNE_TEXT);
    StergeArboreDinMemorie(); // sterg in caz ca a mai fost apelata functia
    //desenez mai intai fundalul
    readimagefile("fundalapp.jpg",0,0,LUNGIME_ECRAN,LATIME_ECRAN);
    // creez in memorie si desenez arborele + textul de peste
    if(!Gresit) // desenez arborele doar daca expresia este corecta sintactic
    {
        nod * b;
        IndiceDesenare = lgOperanzi  + 1;
        CreeazaArbore(b);
        pozitie PozitieStart;
        PozitieStart.x = (LUNGIME_ECRAN / 2) + OffsetStartPosX, PozitieStart.y = 26 + OffsetStartPosY;
        DeseneazaArbore(b, PozitieStart);
    }
    else
    {
        readimagefile("gresit.jpg", LUNGIME_ECRAN/2 - 50, LATIME_ECRAN/2 - 30, LUNGIME_ECRAN/2 + 50, LATIME_ECRAN/2 + 30);
        Gresit = false;
    }
    settextstyle(0,0,3);

    // desenez elementele de interfata
    readimagefile("btninapoi.jpg",1070,700,1170,760); // butonul de inapoi
    readimagefile("btnjos.jpg",70, 700, 120, 750); // butonul de dat in jos arborele
    readimagefile("btnsus.jpg",70, 600, 120, 650); // si tot asa
    readimagefile("btnstg.jpg",45, 650, 95, 700);
    readimagefile("btndrp.jpg",95, 650, 145, 700);
    readimagefile("expndsus.jpg", 200, 700, 250, 750);
    readimagefile("expndlat.jpg", 270, 700, 320, 750);
    readimagefile("expnarb.jpg", 235, 630, 285, 680); // butonul de micsorare/marire al nodurilor
}
void DeseneazaMeniulUI()
{
    if(EsteAplicatie)
    {
        return;
    }
    if(kbhit())
    {
        char tasta = getch();
        if(tasta != '\t'&& tasta != ' ' && tasta != 27) // verific ca tasta apasata sa nu fie tab, spatiu sau escape
        {
            pozitie text_casuta;
            text_casuta.x = 911, text_casuta.y = 310; // pozitia din casuta textului
            if(tasta == '\b') // daca am introdus backspace trebuie sa sterg ultimul caracter
            {
                lgExpresie--;
                if(lgExpresie < 0 ) // lungimea nu trebuie sa fie mai mica de 0
                {
                    lgExpresie = 0;
                }
                char ultim_char = Expresie[lgExpresie - 1];
                if(Expresie[lgExpresie] == ')')
                {
                    NrPInc--;
                }
                else if(Expresie[lgExpresie] == '(')
                {
                    NrPDesc--;
                }
                Expresie[lgExpresie] = '\0';
                if(NrPDesc != NrPInc || Prioritate(ultim_char) > 0 || Expresie[0] == ')') // daca numarul parantezelor deschise difera de cele inchise este clar o greseala in sintaza expresiei
                {
                    Gresit = true;
                    setcolor(COLOR(23, 119, 216));
                    DeseneazaDreptunghi(text_casuta,528,50);
                    setcolor(COLOR(255, 0, 0));
                    DeseneazaTextCentrat(text_casuta,Expresie);
                }
                else
                {
                    Gresit = false;
                    setcolor(COLOR(23, 119, 216));
                    DeseneazaDreptunghi(text_casuta,528,50);
                    setcolor(WHITE);
                    DeseneazaTextCentrat(text_casuta,Expresie);
                }
            }
            else if (textwidth(Expresie) <= 525 && tasta != 13)
            {
                if(tasta == '(')
                {
                    NrPDesc++;
                }
                else if(tasta == ')')
                {
                    NrPInc++;
                }
                Expresie[lgExpresie++] = tasta;
                setfillstyle(SOLID_FILL, COLOR(23, 119, 216));
                setcolor(COLOR(23, 119, 216));
                DeseneazaDreptunghi(text_casuta,528,50);
                if(Prioritate(tasta) > 0 || tasta =='(' || NrPDesc != NrPInc)
                {
                    Gresit = true;
                    setcolor(COLOR(255, 0, 0));
                }
                else
                {
                    Gresit = false;
                    setcolor(WHITE);
                }
                DeseneazaTextCentrat(text_casuta,Expresie);
            }
            else if(tasta == 13) // daca am apasat enter sa intre in aplicatie si sa deseneza arborele
            {
                EsteAplicatie = true;
            }
        }
    }
    if(EsteMeniul)
    {
        return;
    }
    readimagefile("fundal.jpg",0,0,LUNGIME_ECRAN,LATIME_ECRAN);
    setbkcolor(COLOR(23, 119, 216));
    if(lgExpresie > 0 )
    {
        pozitie text_casuta;
        text_casuta.x = 911, text_casuta.y = 310;
        DeseneazaTextCentrat(text_casuta,Expresie);
    }
    EsteMeniul = true;
    DesenatMeniu = false;
}
void DeseneazaAplicatiaUI()
{
    if(!EsteAplicatie) // adica daca sunt in partea de desenat arborele nu ma mai intereseaza meniul
    {
        return;
    }
    if(ismouseclick(WM_LBUTTONDOWN))
    {
        pozitie pozitie_click;
        getmouseclick(WM_LBUTTONDOWN, pozitie_click.x, pozitie_click.y);
        if(pozitie_click.x >= 1070 && pozitie_click.x <= 1170 && pozitie_click.y >= 700 && pozitie_click.y <= 760) // daca am facut click pe butonul inapoi
        {
            // ma intorc in meniu
            EsteAplicatie = false;
            EsteMeniul = false;
            //sterg arborele
            StergeArboreDinMemorie();
            lgListaNoduri = 0;
            // sterg matricea de operanzi
            for(int i = 1; i <= lgOperanzi ; i++)
            {
                GolireSir(Operanzi[i]);
            }
        }
        else if (pozitie_click.x >= 70 && pozitie_click.x <= 120 && pozitie_click.y >= 700 && pozitie_click.y <= 750) // am facut click pe butonul de miscat arborele in jos
        {
            OffsetStartPosY += 20;
            if(OffsetStartPosY > LATIME_ECRAN)
            {
                OffsetStartPosY = 0;
            }
            ArboreleExpresiei();
        }
        else if (pozitie_click.x >= 70 && pozitie_click.x <= 120 && pozitie_click.y >= 600 && pozitie_click.y <= 650) // am facut click pe butonul de miscat arborele in sus
        {
            OffsetStartPosY -= 20;
            if(OffsetStartPosY < 0)
            {
                OffsetStartPosY = LATIME_ECRAN;
            }
            ArboreleExpresiei();
        }
        else if (pozitie_click.x >= 95 && pozitie_click.x <= 145 && pozitie_click.y >= 650 && pozitie_click.y <= 700) // am facut click pe butonul de miscat arborele in dreapta
        {
            OffsetStartPosX += 20;
            if(OffsetStartPosX + LUNGIME_ECRAN/2 > LUNGIME_ECRAN)
            {
                OffsetStartPosX = -LUNGIME_ECRAN/2;
            }
            ArboreleExpresiei();
        }
        else if (pozitie_click.x >= 45 && pozitie_click.x <= 95 && pozitie_click.y >= 650 && pozitie_click.y <= 700) // am facut click pe butonul de miscat arborele in dreapta
        {
            OffsetStartPosX -= 20;
            if(OffsetStartPosX + LUNGIME_ECRAN/2 < 0)
            {
                OffsetStartPosX = LUNGIME_ECRAN/2;
            }
            ArboreleExpresiei();
        }
        else if (pozitie_click.x >= 200 && pozitie_click.x <= 250 && pozitie_click.y >= 700 && pozitie_click.y <= 750) // am facut click pe butonul de marit distanta in inaltime a arborelui
        {
            DistantaIntreRanduri += 10;
            ArboreleExpresiei();
        }
        else if (pozitie_click.x >= 270 && pozitie_click.x <= 320 && pozitie_click.y >= 700 && pozitie_click.y <= 750) // am facut click pe butonul de marit distanta in inaltime a arborelui
        {
            DistantaIntreFii += 10;
            ArboreleExpresiei();
        }
        else if (pozitie_click.x >= 235 && pozitie_click.x <= 285 && pozitie_click.y >= 650 && pozitie_click.y <= 700) // am facut click pe butonul de marit si micsorat dimensiunea nodurilor , textului
        {
            DIMENSIUNE_TEXT ++;
            ArboreleExpresiei();
        }
    }
    else if( ismouseclick(WM_RBUTTONDOWN)) // doua butoane merg si cu click dreapta daca sunt apasate se va face fix inversu lor
    {
        pozitie pozitie_click;
        getmouseclick(WM_RBUTTONDOWN, pozitie_click.x, pozitie_click.y);
        if (pozitie_click.x >= 200 && pozitie_click.x <= 250 && pozitie_click.y >= 700 && pozitie_click.y <= 750) // am facut click pe butonul de marit distanta in inaltime a arborelui
        {
            DistantaIntreRanduri -= 10;
            ArboreleExpresiei();
        }
        else if (pozitie_click.x >= 270 && pozitie_click.x <= 320 && pozitie_click.y >= 700 && pozitie_click.y <= 750) // am facut click pe butonul de marit distanta in inaltime a arborelui
        {
            DistantaIntreFii -= 10;
            ArboreleExpresiei();
        }
    }
    if(!DesenatMeniu)
    {
        CorecteazaExpresia(Expresie);
        EvalueazaExpresia(Expresie);
        ArboreleExpresiei();
        DesenatMeniu = true;
    }
}
void DeseneazaFereastra()
{
    initwindow(LUNGIME_ECRAN, LATIME_ECRAN);
    settextstyle(0,0,3);
    IndiceDesenare = lgOperanzi + 1; // incep de la finalul expresiei evaluate
    setfillstyle(SOLID_FILL, BLUE);
    while(1) // bucla aplicatiei
    {
        PozitieMouse.x = mousex();
        PozitieMouse.y = mousey();
        DeseneazaMeniulUI();
        DeseneazaAplicatiaUI();
    }
    getch();
    closegraph();
}
int main()
{
    DeseneazaFereastra();
    return 0;
}
