#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <map>
#include "raylib.h"

using namespace std;

const int ROZMIAR_SZACHOWNICY = 8;
const int ROZMIAR_POLA = 90;
const Color KOLOR_JASNY = LIGHTGRAY;
const Color KOLOR_CIEMNY = DARKGRAY;
Color kolorPolaJasny = LIGHTGRAY;
Color kolorPolaCiemny = DARKGRAY;

string poczatkowa_pozycja_fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";

int szerokoscOkna = 1280;
int wysokoscOkna = 720;
Color kolorTla = LIME;

void ustawieniaMenu();
void wybierzpoziom();
std::map<char, Texture2D> teksturyFigur;

void rysujTekstNaSrodku(const char* tekst, int rozmiarCzcionki, Color kolor)
{
    int szerokoscTekstu = MeasureText(tekst, rozmiarCzcionki);
    int wysokoscTekstu = rozmiarCzcionki;
    int xTekst = (GetScreenWidth() - szerokoscTekstu) / 2;
    int yTekst = (GetScreenHeight() - wysokoscTekstu) / 2;

    BeginDrawing();
    ClearBackground(kolor);
    DrawText(tekst, xTekst, yTekst, rozmiarCzcionki, RAYWHITE);
    EndDrawing();
}

void ladujTeksturyFigur() {
    string folder = "grafiki_figur/";
    teksturyFigur['P'] = LoadTexture((folder + "wp.png").c_str());
    teksturyFigur['R'] = LoadTexture((folder + "wr.png").c_str());
    teksturyFigur['N'] = LoadTexture((folder + "wn.png").c_str());
    teksturyFigur['B'] = LoadTexture((folder + "wb.png").c_str());
    teksturyFigur['Q'] = LoadTexture((folder + "wq.png").c_str());
    teksturyFigur['K'] = LoadTexture((folder + "wk.png").c_str());
    teksturyFigur['p'] = LoadTexture((folder + "bp.png").c_str());
    teksturyFigur['r'] = LoadTexture((folder + "br.png").c_str());
    teksturyFigur['n'] = LoadTexture((folder + "bn.png").c_str());
    teksturyFigur['b'] = LoadTexture((folder + "bb.png").c_str());
    teksturyFigur['q'] = LoadTexture((folder + "bq.png").c_str());
    teksturyFigur['k'] = LoadTexture((folder + "bk.png").c_str());
}

void zwolnijTekstury() {
    for (auto& para : teksturyFigur) {
        UnloadTexture(para.second);
    }
    teksturyFigur.clear();
}

void rysujFigure(char figura, int x, int y, int startX, int startY, int poleRozmiar) {
    if (teksturyFigur.find(figura) != teksturyFigur.end()) {
        int posX = startX + x * poleRozmiar;
        int posY = startY + y * poleRozmiar;

        // Zmniejszenie figur do rozmiaru pól szachownicy
        Rectangle dest = { (float)posX, (float)posY, (float)poleRozmiar, (float)poleRozmiar };
        Rectangle src = { 0.0f, 0.0f, (float)teksturyFigur[figura].width, (float)teksturyFigur[figura].height };

        DrawTexturePro(teksturyFigur[figura], src, dest, { 0, 0 }, 0.0f, WHITE);
    }
}


void narysujSzachowniceFEN(const string& fen) {
    int szerokoscSzachownicy = min(GetScreenHeight(), GetScreenWidth()) * 0.8; // Dopasowanie do mniejszego wymiaru okna
    int startX = (GetScreenWidth() - szerokoscSzachownicy) / 2;
    int startY = (GetScreenHeight() - szerokoscSzachownicy) / 2;
    int poleRozmiar = szerokoscSzachownicy / ROZMIAR_SZACHOWNICY;

    for (int y = 0; y < ROZMIAR_SZACHOWNICY; y++) {
        for (int x = 0; x < ROZMIAR_SZACHOWNICY; x++) {
            Color kolorPola = ((x + y) % 2 == 0) ? kolorPolaJasny : kolorPolaCiemny;
            DrawRectangle(startX + x * poleRozmiar, startY + y * poleRozmiar, poleRozmiar, poleRozmiar, kolorPola);
        }
    }

    int x = 0, y = 0;
    for (char c : fen) {
        if (c == ' ') break;
        if (c == '/') {
            y++;
            x = 0;
        }
        else if (isdigit(c)) {
            x += c - '0';
        }
        else {
            rysujFigure(c, x, y, startX, startY, poleRozmiar);
            x++;
        }
    }
}


bool rysujPrzycisk(const char* tekst, int x, int y, int szerokosc, int wysokosc, Color kolorklikniecia, Color kolorprzycisku)
{
    Rectangle przyciskRect = { (float)x, (float)y, (float)szerokosc, (float)wysokosc };
    bool nadPrzyciskiem = CheckCollisionPointRec(GetMousePosition(), przyciskRect);
    Color kolorPrzycisku = nadPrzyciskiem ? kolorklikniecia : kolorprzycisku;
    DrawRectangleRec(przyciskRect, kolorPrzycisku);
    DrawText(tekst, x + 20, y + 10, 20, RAYWHITE);
    return nadPrzyciskiem && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}

// Funkcja menu głównego
void menuGlowne()
{
    bool wUstawieniach = false;
    bool wWyborzePoziomow = false;

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(kolorTla);

        // Rysowanie przycisków
        if (rysujPrzycisk("Zadania", szerokoscOkna / 2 - 200, wysokoscOkna / 2 - 300, 400, 100, DARKGRAY, GRAY))
        {
            wWyborzePoziomow = true;
        }
        if (rysujPrzycisk("Ustawienia", szerokoscOkna / 2 - 100, wysokoscOkna / 2 - 100, 200, 50, DARKGRAY, GRAY))
        {
            wUstawieniach = true;
        }
        if (rysujPrzycisk("Tworcy", szerokoscOkna / 2 - 100, wysokoscOkna / 2, 200, 50, DARKGRAY, GRAY))
        {
            rysujTekstNaSrodku("Paweł Handwerkier", 30, DARKGRAY);
        }
        if (rysujPrzycisk("Wyjdz", szerokoscOkna / 2 - 100, wysokoscOkna / 2 + 100, 200, 50, DARKGRAY, GRAY))
        {
            CloseWindow(); // Wyjście z aplikacji
        }

        EndDrawing();

        // Jeśli użytkownik wybrał opcję "Ustawienia", wywołujemy odpowiednią funkcję
        if (wUstawieniach)
        {
            ustawieniaMenu();  // Funkcja do wyświetlenia menu ustawień
            wUstawieniach = false; // Powrót do głównego menu po wyjściu z ustawień
        }
        if (wWyborzePoziomow)
        {
            wybierzpoziom();
            wWyborzePoziomow = false;
        }
    }
}

void ustawieniaMenu()
{
    bool wMenu = true;
    int glosnoscMuzyki = 50;
    bool pelnyEkran = false;

    // Zmienne przechowujące obecne wybory koloru tła i koloru pól
    int wybranyKolorTla = 1;   // 1: Zielony, 2: Niebieski, 3: Czerwony, 4: Fioletowy
    int wybranyKolorPola = 1;  // 1: Szary, 2: Zielony i Biały, 3: Różowy i Biały, 4: Brązowy i Biały

    while (wMenu && !WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(kolorTla);

        DrawText("Glosnosc Muzyki:", 200, 50, 20, WHITE);
        DrawText(TextFormat("%i", glosnoscMuzyki), 450, 50, 20, WHITE);

        // Sterowanie głośnością muzyki
        if (IsKeyPressed(KEY_LEFT)) glosnoscMuzyki = max(glosnoscMuzyki - 10, 0);
        if (IsKeyPressed(KEY_RIGHT)) glosnoscMuzyki = min(glosnoscMuzyki + 10, 100);

        // Przycisk do przełączania pełnego ekranu
        if (rysujPrzycisk("Przelacz Pelen Ekran", szerokoscOkna / 2 - 100, 100, 200, 50, DARKGRAY, GRAY))
        {
            ToggleFullscreen();
            pelnyEkran = !pelnyEkran;
        }

        // Wybór koloru tła
        DrawText("Wybierz Kolor Tla:", 200, 200, 20, WHITE);

        if (rysujPrzycisk("Zielony", 200, 250, 200, 50, DARKGRAY, DARKGREEN))
        {
            kolorTla = LIME;
            wybranyKolorTla = 1;
        }
        if (rysujPrzycisk("Niebieski", 450, 250, 200, 50, DARKGRAY, BLUE))
        {
            kolorTla = SKYBLUE;
            wybranyKolorTla = 2;
        }
        if (rysujPrzycisk("Czerwony", 700, 250, 200, 50, DARKGRAY, MAROON))
        {
            kolorTla = RED;
            wybranyKolorTla = 3;
        }
        if (rysujPrzycisk("Fioletowy", 950, 250, 200, 50, DARKGRAY, DARKPURPLE))
        {
            kolorTla = PURPLE;
            wybranyKolorTla = 4;
        }

        // Wybór koloru pól szachownicy
        DrawText("Wybierz Kolor Pol Szachownicy:", 200, 350, 20, WHITE);

        if (rysujPrzycisk("Szary", 200, 400, 200, 50, DARKGRAY, GRAY))
        {
            kolorPolaJasny = LIGHTGRAY;
            kolorPolaCiemny = DARKGRAY;
            wybranyKolorPola = 1;
        }
        if (rysujPrzycisk("Zielony i Bialy", 450, 400, 200, 50, DARKGRAY, DARKGREEN))
        {
            kolorPolaJasny = WHITE;
            kolorPolaCiemny = DARKGREEN;
            wybranyKolorPola = 2;
        }
        if (rysujPrzycisk("Rozowy i Bialy", 700, 400, 200, 50, DARKGRAY, PINK))
        {
            kolorPolaJasny = WHITE;
            kolorPolaCiemny = PINK;
            wybranyKolorPola = 3;
        }
        if (rysujPrzycisk("Brazowy i Bialy", 950, 400, 200, 50, DARKGRAY, BROWN))
        {
            kolorPolaJasny = WHITE;
            kolorPolaCiemny = BROWN;
            wybranyKolorPola = 4;
        }

        // Pozycje „ptaszków” przy wybranych kolorach
        int ptaszekX = 180;  // Pozycja X dla „ptaszka”
        int ptaszekY_Tla = 275;  // Pozycje Y dla każdego przycisku tła
        int ptaszekY_Pola = 425; // Pozycje Y dla każdego przycisku pól

        // Funkcja rysująca ptaszek w danym miejscu
        auto rysujPtaszek = [](int x, int y) {
            DrawLine(x, y, x + 5, y + 10, WHITE);    // Prawa część "ptaszka"
            DrawLine(x + 5, y + 10, x + 15, y - 5, WHITE); // Lewa część "ptaszka"
            };

        // Rysowanie ptaszka przy wybranym kolorze tła
        switch (wybranyKolorTla)
        {
        case 1: rysujPtaszek(ptaszekX, ptaszekY_Tla); break;          // Zielony
        case 2: rysujPtaszek(ptaszekX + 250, ptaszekY_Tla); break;    // Niebieski
        case 3: rysujPtaszek(ptaszekX + 500, ptaszekY_Tla); break;    // Czerwony
        case 4: rysujPtaszek(ptaszekX + 750, ptaszekY_Tla); break;    // Fioletowy
        }

        // Rysowanie ptaszka przy wybranym kolorze pól
        switch (wybranyKolorPola)
        {
        case 1: rysujPtaszek(ptaszekX, ptaszekY_Pola); break;          // Szary
        case 2: rysujPtaszek(ptaszekX + 250, ptaszekY_Pola); break;    // Zielony i Biały
        case 3: rysujPtaszek(ptaszekX + 500, ptaszekY_Pola); break;    // Różowy i Biały
        case 4: rysujPtaszek(ptaszekX + 750, ptaszekY_Pola); break;    // Brązowy i Biały
        }

        // Przycisk powrotu do menu głównego
        if (rysujPrzycisk("Wroc", szerokoscOkna / 2 - 100, 500, 200, 50, DARKGRAY, GRAY))
        {
            wMenu = false;
        }

        EndDrawing();
    }
}


void poziom1()
{
    string fen = "r1b2rk1/pppp1ppp/2n1pn2/8/3P4/1NQ2NP1/qP2PPBP/2R1K2R w K";
    bool wPoziomie = true;

    while (wPoziomie && !WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(kolorTla);

        int margines = 20;
        int szerokoscPola = 200;

        // Rysowanie przezroczystego pola na informacje
        DrawRectangle(margines, margines, szerokoscPola, GetScreenHeight() - (margines*2), ColorAlpha(DARKGRAY, 0.8f)); // Półprzezroczyste tło

        // Dodanie przyjemniejszego wyglądu – obramowanie
        DrawRectangleLines(margines, margines, szerokoscPola, GetScreenHeight()-(margines*2), WHITE);

        // Nagłówek i informacje
        DrawText("Zadanie 1", margines + 10, 10+margines, 20, WHITE);
        DrawText("Czarne zbily pionla", margines + 10, 80 + margines, 16, WHITE);
        DrawText("na skrzydle hetmanskim,", margines + 10, 100 + margines, 16, WHITE);
        DrawText("jednak nie maja jeszcze", margines + 10, 120 + margines, 16, WHITE);
        DrawText("wyprowadzonych figur", margines + 10, 140 + margines, 16, WHITE);
        DrawText("-Znajdz najlepsze", margines + 10, 170 + margines, 16, WHITE);
        DrawText("posuniecie dla bialego-", margines + 10, 190 + margines, 16, WHITE);

        // Rysowanie szachownicy, przesuniętej w prawo o szerokość pola na tekst + margines
        narysujSzachowniceFEN(fen); // Dodanie odstępu między polem a szachownicą

        // Rysowanie przycisku „Powrót”
        if (rysujPrzycisk("Powrot", GetScreenWidth() - 150, 10, 140, 40, DARKGRAY, GRAY))
        {
            wPoziomie = false; // Wyjście z poziomu
        }

        // Rysowanie przycisku „Ustawienia”
        if (rysujPrzycisk("Ustawienia", GetScreenWidth() - 150, 60, 140, 40, DARKGRAY, GRAY))
        {
            ustawieniaMenu(); // Przejście do ustawień
        }

        EndDrawing();
    }
}



void poziom2()
{
    string fen = "r4rk1/2pn1ppp/p1b1pq2/1p6/P1pP4/2P2NP1/Q3PPBP/R2R2K1 w";
    bool wPoziomie = true;

    while (wPoziomie && !WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(kolorTla);

        int margines = 20;
        int szerokoscPola = 200;

        // Rysowanie przezroczystego pola na informacje
        DrawRectangle(margines, margines, szerokoscPola, GetScreenHeight() - (margines * 2), ColorAlpha(DARKGRAY, 0.8f)); // Półprzezroczyste tło

        // Dodanie przyjemniejszego wyglądu – obramowanie
        DrawRectangleLines(margines, margines, szerokoscPola, GetScreenHeight() - (margines * 2), WHITE);

        // Nagłówek i informacje
        DrawText("Zadanie 2", margines + 10, 10 + margines, 20, WHITE);
        DrawText("Otwarta przekatna a8-h1", margines + 10, 80 + margines, 16, WHITE);
        DrawText("-Znajdz najlepsze", margines + 10, 110 + margines, 16, WHITE);
        DrawText("posuniecie dla bialego-", margines + 10, 130 + margines, 16, WHITE);

        narysujSzachowniceFEN(fen);

        // Rysowanie przycisku „Powrót”
        if (rysujPrzycisk("Powrot", GetScreenWidth() - 150, 10, 140, 40, DARKGRAY, GRAY))
        {
            wPoziomie = false; // Wyjście z poziomu
        }

        // Rysowanie przycisku „Ustawienia”
        if (rysujPrzycisk("Ustawienia", GetScreenWidth() - 150, 60, 140, 40, DARKGRAY, GRAY))
        {
            ustawieniaMenu(); // Przejście do ustawień
        }

        EndDrawing();
    }
}
void poziom3()
{
    string fen = "r4rk1/ppn3pp/2p5/3pP3/6bq/2NB2R1/PP1Q1P1P/2KR4 w";
    bool wPoziomie = true;

    while (wPoziomie && !WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(kolorTla);

        int margines = 20;
        int szerokoscPola = 200;

        // Rysowanie przezroczystego pola na informacje
        DrawRectangle(margines, margines, szerokoscPola, GetScreenHeight() - (margines * 2), ColorAlpha(DARKGRAY, 0.8f)); // Półprzezroczyste tło

        // Dodanie przyjemniejszego wyglądu – obramowanie
        DrawRectangleLines(margines, margines, szerokoscPola, GetScreenHeight() - (margines * 2), WHITE);

        // Nagłówek i informacje
        DrawText("Zadanie 3", margines + 10, 10 + margines, 20, WHITE);
        DrawText("Biale tutaj wyjatkowo", margines + 10, 80 + margines, 16, WHITE);
        DrawText("zdecydowaly sie na 0-0-0,", margines + 10, 100 + margines, 16, WHITE);
        DrawText("czyli dluga roszade", margines + 10, 120 + margines, 16, WHITE);
        DrawText("pozwala im to na atak", margines + 10, 140 + margines, 16, WHITE);
        DrawText("na skrzydle krolewskim", margines + 10, 160 + margines, 16, WHITE);
        DrawText("-Znajdz najlepsze", margines + 10, 190 + margines, 16, WHITE);
        DrawText("posuniecie dla bialego-", margines + 10, 210 + margines, 16, WHITE);

        // Rysowanie szachownicy, przesuniętej w prawo o szerokość pola na tekst + margines
        narysujSzachowniceFEN(fen); // Dodanie odstępu między polem a szachownicą

        // Rysowanie przycisku „Powrót”
        if (rysujPrzycisk("Powrot", GetScreenWidth() - 150, 10, 140, 40, DARKGRAY, GRAY))
        {
            wPoziomie = false; // Wyjście z poziomu
        }

        // Rysowanie przycisku „Ustawienia”
        if (rysujPrzycisk("Ustawienia", GetScreenWidth() - 150, 60, 140, 40, DARKGRAY, GRAY))
        {
            ustawieniaMenu(); // Przejście do ustawień
        }

        EndDrawing();
    }
}

void wybierzpoziom()
{
    bool wMenuWyboruPoziomu = true; // Flaga kontrolująca pozostanie w menu wyboru poziomów

    while (wMenuWyboruPoziomu && !WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(kolorTla);

        DrawText("Wybierz poziom:", szerokoscOkna / 2 - 100, 50, 30, WHITE);

        int liczbaPrzyciskow = 5;  // Liczba dostępnych poziomów (można rozszerzyć)
        int kolumny = 5;          // Liczba przycisków w rzędzie
        int rozmiarPrzycisku = 100;
        int odstep = 20;

        for (int i = 0; i < liczbaPrzyciskow; i++)
        {
            int x = (szerokoscOkna / 2 - (rozmiarPrzycisku + odstep) * kolumny / 2) + (i % kolumny) * (rozmiarPrzycisku + odstep);
            int y = 150;

            if (rysujPrzycisk(TextFormat("Poziom %d", i + 1), x, y, rozmiarPrzycisku, rozmiarPrzycisku, MAGENTA, DARKPURPLE))
            {
                // Wywołanie odpowiedniej funkcji poziomu
                switch (i)
                {
                case 0:
                    poziom1();
                    break;
                case 1:
                    poziom2();
                    break;
                case 2:
                    poziom3();
                    break;
                }
            }
        }

        // Dodanie przycisku do powrotu do głównego menu
        if (rysujPrzycisk("Wroc", szerokoscOkna / 2 - 100, 300, 200, 50, DARKGRAY, GRAY))
        {
            wMenuWyboruPoziomu = false; // Wyjście z menu wyboru poziomów
        }

        EndDrawing();
    }
}


int main()
{
    InitWindow(szerokoscOkna, wysokoscOkna, "Szachy C++");
    SetTargetFPS(60);

    ladujTeksturyFigur();

    menuGlowne();

    zwolnijTekstury();
    CloseWindow();

    return 0;
}