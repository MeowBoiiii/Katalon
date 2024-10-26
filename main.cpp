#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include "raylib.h"

using namespace std;
const int ROZMIAR_SZACHOWNICY = 8;
const int ROZMIAR_POLA = 80;
const Color KOLOR_JASNY = LIGHTGRAY;
const Color KOLOR_CIEMNY = DARKGRAY;

// Ustawienia rozmiaru okna
int szerokoscOkna = 1280;
int wysokoscOkna = 720;
Color kolorTla = LIME;  // Domy�lny kolor t�a

// Deklaracja funkcji ustawieniaMenu, aby by�a rozpoznawana w funkcji menuGlowne()
void ustawieniaMenu();
void wybierzpoziom();

//--------------------------------------FUNKCJA RYSUJ�CA TEKST-------------------------------------------------
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

// Funkcja pomocnicza do rysowania szachownicy
void rysujSzachownice()
{
    for (int y = 0; y < ROZMIAR_SZACHOWNICY; y++)
    {
        for (int x = 0; x < ROZMIAR_SZACHOWNICY; x++)
        {
            // Wybieramy kolor dla bie��cego pola
            Color kolorPola = ((x + y) % 2 == 0) ? KOLOR_JASNY : KOLOR_CIEMNY;
            DrawRectangle(x * ROZMIAR_POLA, y * ROZMIAR_POLA, ROZMIAR_POLA, ROZMIAR_POLA, kolorPola);
        }
    }
}

// Funkcja pomocnicza do rysowania figury w polu na szachownicy
void rysujFigure(char figura, int x, int y)
{
    string symbol;
    switch (figura)
    {
    case 'P': symbol = "P"; break; // Pion
    case 'R': symbol = "R"; break; // Wie�a
    case 'N': symbol = "N"; break; // Skoczek
    case 'B': symbol = "B"; break; // Goniec
    case 'Q': symbol = "Q"; break; // Hetman
    case 'K': symbol = "K"; break; // Kr�l
    default: return;
    }

    // Rysowanie symbolu figury na �rodku pola
    int posX = x * ROZMIAR_POLA + ROZMIAR_POLA / 2 - 10;
    int posY = y * ROZMIAR_POLA + ROZMIAR_POLA / 2 - 10;
    DrawText(symbol.c_str(), posX, posY, 20, BLACK);
}// Funkcja do parsowania PGN i rysowania szachownicy na jej podstawie
void narysujSzachownice(const char* pgn)
{
    // Rysowanie planszy
    rysujSzachownice();

    // Ustawienie figur w pozycji pocz�tkowej (tutaj mo�na rozszerzy� o analiz� PGN)
    vector<string> pozycjaStartowa = {
        "RNBQKBNR",
        "PPPPPPPP",
        "        ",
        "        ",
        "        ",
        "        ",
        "pppppppp",
        "rnbqkbnr"
    };

    for (int y = 0; y < ROZMIAR_SZACHOWNICY; y++)
    {
        for (int x = 0; x < ROZMIAR_SZACHOWNICY; x++)
        {
            char figura = pozycjaStartowa[y][x];
            if (figura != ' ')
            {
                rysujFigure(figura, x, y);
            }
        }
    }
}

//-------------------------------------FUNKCJA RYSUJ�CA PRZYCISKI---------------------------------------------
bool rysujPrzycisk(const char* tekst, int x, int y, int szerokosc, int wysokosc, Color kolorklikniecia, Color kolorprzycisku)
{
    Rectangle przyciskRect = { (float)x, (float)y, (float)szerokosc, (float)wysokosc };

    // Sprawdzenie, czy mysz jest na przycisku
    bool nadPrzyciskiem = CheckCollisionPointRec(GetMousePosition(), przyciskRect);

    // Rysowanie przycisku (zmiana koloru po najechaniu myszk�)
    Color kolorPrzycisku = nadPrzyciskiem ? kolorklikniecia : kolorprzycisku;
    DrawRectangleRec(przyciskRect, kolorPrzycisku);
    DrawText(tekst, x + 20, y + 10, 20, RAYWHITE);

    // Zwr�cenie true, je�li przycisk zosta� klikni�ty
    return nadPrzyciskiem && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}

// Funkcja menu g��wnego
void menuGlowne()
{
    bool wUstawieniach = false;
    bool wWyborzePoziomow = false;

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(kolorTla);

        // Rysowanie przycisk�w
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
            rysujTekstNaSrodku("Pawe� Handwerkier", 30, DARKGRAY);
        }
        if (rysujPrzycisk("Wyjdz", szerokoscOkna / 2 - 100, wysokoscOkna / 2 + 100, 200, 50, DARKGRAY, GRAY))
        {
            CloseWindow(); // Wyj�cie z aplikacji
        }

        EndDrawing();

        // Je�li u�ytkownik wybra� opcj� "Ustawienia", wywo�ujemy odpowiedni� funkcj�
        if (wUstawieniach)
        {
            ustawieniaMenu();  // Funkcja do wy�wietlenia menu ustawie�
            wUstawieniach = false; // Powr�t do g��wnego menu po wyj�ciu z ustawie�
        }
        if (wWyborzePoziomow)
        {
            wybierzpoziom();
            wWyborzePoziomow = false;
        }
    }
}

// Funkcja wy�wietlaj�ca menu ustawie�
void ustawieniaMenu()
{
    bool wMenu = true;
    int glosnoscMuzyki = 50;
    bool pelnyEkran = false;

    while (wMenu && !WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(kolorTla);

        DrawText("Glosnosc Muzyki:", 200, 200, 20, WHITE);
        DrawText(TextFormat("%i", glosnoscMuzyki), 450, 200, 20, WHITE);

        if (IsKeyPressed(KEY_LEFT)) glosnoscMuzyki = max(glosnoscMuzyki - 10, 0);
        if (IsKeyPressed(KEY_RIGHT)) glosnoscMuzyki = min(glosnoscMuzyki + 10, 100);

        if (rysujPrzycisk("Przelacz Pelen Ekran", szerokoscOkna / 2 - 100, 300, 200, 50, DARKGRAY, GRAY))
        {
            ToggleFullscreen();
            pelnyEkran = !pelnyEkran;
        }

        DrawText("Wybierz Kolor Tla:", 200, 350, 20, WHITE);
        if (rysujPrzycisk("Zielony", 200, 400, 200, 50, DARKGRAY, DARKGREEN)) kolorTla = LIME;
        if (rysujPrzycisk("Niebieski", 450, 400, 200, 50, DARKGRAY, BLUE)) kolorTla = SKYBLUE;
        if (rysujPrzycisk("Czerwony", 700, 400, 200, 50, DARKGRAY, MAROON)) kolorTla = RED;

        if (rysujPrzycisk("Wroc", szerokoscOkna / 2 - 100, 500, 200, 50, DARKGRAY, GRAY))
        {
            wMenu = false;
        }

        EndDrawing();
    }
}
void narysuj_szachownice()
{

}
// Funkcje poziom�w
void poziom1()
{
    cout << "Wybrano poziom 1" << endl;

    bool poziomAktywny = true;

    while (poziomAktywny && !WindowShouldClose())  // P�tla, kt�ra utrzymuje poziom aktywny
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Rysowanie planszy
        rysujSzachownice();

        // Ustawienie figur w pozycji pocz�tkowej
        vector<string> pozycjaStartowa = {
            "RNBQKBNR",  // 1. linia bia�ych
            "PPPPPPPP",  // 2. linia bia�ych
            "        ",  // Puste linie
            "        ",
            "        ",
            "        ",
            "pppppppp",  // 7. linia czarnych
            "rnbqkbnr"   // 8. linia czarnych
        };

        // Rysowanie figur w pocz�tkowej pozycji
        for (int y = 0; y < ROZMIAR_SZACHOWNICY; y++)
        {
            for (int x = 0; x < ROZMIAR_SZACHOWNICY; x++)
            {
                char figura = pozycjaStartowa[y][x];
                if (figura != ' ')
                {
                    rysujFigure(figura, x, y);
                }
            }
        }

        // Przycisk "Wr��", kt�ry pozwala na wyj�cie z poziomu
        if (rysujPrzycisk("Wroc", 50, 50, 150, 50, DARKGRAY, GRAY))
        {
            poziomAktywny = false;
        }

        EndDrawing();
    }
}


void poziom2()
{
    cout << "Wybrano poziom 2" << endl;
}

void poziom3()
{
    cout << "Wybrano poziom 3" << endl;
}

// Funkcja rysuj�ca przyciski wyboru poziom�w
void narysujPrzyciskiDoWyboruPoziomu()
{
    int liczbaPrzyciskow = 50;
    int kolumny = 10;
    int rozmiarPrzycisku = 100;
    int odstep = 10;

    // Tablica wska�nik�w do funkcji poziom�w
    void (*poziomy[3])() = { poziom1, poziom2, poziom3 };

    for (int i = 0; i < liczbaPrzyciskow; i++)
    {
        int x = 100 + (i % kolumny) * (rozmiarPrzycisku + odstep);
        int y = 100 + (i / kolumny) * (rozmiarPrzycisku + odstep);

        if (i < 3 && rysujPrzycisk(TextFormat("%d", i + 1), x, y, rozmiarPrzycisku, rozmiarPrzycisku, MAGENTA, DARKPURPLE))
        {
            poziomy[i]();  // Wywo�anie odpowiedniej funkcji poziomu
        }
    }
}

// Funkcja wyboru poziomu
void wybierzpoziom()
{
    bool wybierzPoziom = true;

    while (wybierzPoziom && !WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(kolorTla);

        narysujPrzyciskiDoWyboruPoziomu();

        if (rysujPrzycisk("Wroc", szerokoscOkna - 200, 50, 150, 50, DARKGRAY, GRAY))
        {
            wybierzPoziom = false;
        }

        EndDrawing();
    }
}

int main()
{
    InitWindow(szerokoscOkna, wysokoscOkna, "System Katalonski");

    rysujTekstNaSrodku("System Katalonski", 50, DARKGRAY);
    std::this_thread::sleep_for(std::chrono::seconds(2));

    menuGlowne();

    CloseWindow();

    return 0;
}
