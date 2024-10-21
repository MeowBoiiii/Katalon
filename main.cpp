#include <iostream>
#include <thread>      // Dodanie biblioteki do funkcji sleep
#include "raylib.h"    // Do³¹czenie nag³ówka raylib

using namespace std;

// Ustawienia rozmiaru okna
int szerokoscOkna = 1280;
int wysokoscOkna = 720;
Color kolorTla = LIME;  // Domyœlny kolor t³a

// Deklaracja funkcji ustawieniaMenu, aby by³a rozpoznawana w funkcji menuGlowne()
void ustawieniaMenu();

//--------------------------------------FUNKCJA RYSUJ¥CA TEKST-------------------------------------------------
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

//-------------------------------------FUNKCJA RYSUJ¥CA PRZYCISKI---------------------------------------------
bool rysujPrzycisk(const char* tekst, int x, int y, int szerokosc, int wysokosc)
{
    Rectangle przyciskRect = { (float)x, (float)y, (float)szerokosc, (float)wysokosc };

    // Sprawdzenie, czy mysz jest na przycisku
    bool nadPrzyciskiem = CheckCollisionPointRec(GetMousePosition(), przyciskRect);

    // Rysowanie przycisku (zmiana koloru po najechaniu myszk¹)
    Color kolorPrzycisku = nadPrzyciskiem ? DARKGRAY : GRAY;
    DrawRectangleRec(przyciskRect, kolorPrzycisku);
    DrawText(tekst, x + 20, y + 10, 20, RAYWHITE);

    // Zwrócenie true, jeœli przycisk zosta³ klikniêty
    return nadPrzyciskiem && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}

// Funkcja menu g³ównego
void menuGlowne()
{
    bool wUstawieniach = false;

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(kolorTla);

        // Rysowanie przycisków
        if (rysujPrzycisk("Ustawienia", szerokoscOkna / 2 - 100, wysokoscOkna / 2 - 100, 200, 50))
        {
            wUstawieniach = true;
        }
        if (rysujPrzycisk("Tworcy", szerokoscOkna / 2 - 100, wysokoscOkna / 2, 200, 50))
        {
            rysujTekstNaSrodku("Pawe³ Handwerkier", 30, DARKGRAY);
        }
        if (rysujPrzycisk("Wyjdz", szerokoscOkna / 2 - 100, wysokoscOkna / 2 + 100, 200, 50))
        {
            CloseWindow(); // Wyjœcie z aplikacji
        }

        EndDrawing();

        // Jeœli u¿ytkownik wybra³ opcjê "Ustawienia", wywo³ujemy odpowiedni¹ funkcjê
        if (wUstawieniach)
        {
            ustawieniaMenu();  // Funkcja do wyœwietlenia menu ustawieñ
            wUstawieniach = false; // Powrót do g³ównego menu po wyjœciu z ustawieñ
        }
    }
}

// Funkcja wyœwietlaj¹ca menu ustawieñ
void ustawieniaMenu()
{
    bool wMenu = true;
    int glosnoscMuzyki = 50;
    bool pelnyEkran = false;

    while (wMenu && !WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(kolorTla);  // U¿ywamy zmiennej kolorTla zamiast DARKGRAY

        // Opcja do regulacji g³oœnoœci
        DrawText("Glosnosc Muzyki:", 200, 200, 20, WHITE);
        DrawText(TextFormat("%i", glosnoscMuzyki), 450, 200, 20, WHITE);

        if (IsKeyPressed(KEY_LEFT)) glosnoscMuzyki = max(glosnoscMuzyki - 10, 0);  // Zmniejszenie g³oœnoœci
        if (IsKeyPressed(KEY_RIGHT)) glosnoscMuzyki = min(glosnoscMuzyki + 10, 100); // Zwiêkszenie g³oœnoœci

        // Opcja do prze³¹czania trybu pe³noekranowego
        if (rysujPrzycisk("Przelacz Pelen Ekran", szerokoscOkna / 2 - 100, 300, 200, 50))
        {
            ToggleFullscreen();
            pelnyEkran = !pelnyEkran;
        }

        // Dodanie opcji zmiany koloru t³a
        DrawText("Wybierz Kolor Tla:", 200, 350, 20, WHITE);
        if (rysujPrzycisk("Zielony", 200, 400, 200, 50)) kolorTla = LIME;
        if (rysujPrzycisk("Niebieski", 450, 400, 200, 50)) kolorTla = SKYBLUE;
        if (rysujPrzycisk("Czerwony", 700, 400, 200, 50)) kolorTla = RED;

        // Opcja powrotu do g³ównego menu
        if (rysujPrzycisk("Wroc", szerokoscOkna / 2 - 100, 500, 200, 50))
        {
            wMenu = false; // Wyjœcie z menu ustawieñ
        }

        EndDrawing();
    }
}

int main()
{
    // Inicjalizacja okna
    InitWindow(szerokoscOkna, wysokoscOkna, "System Katalonski");

    // Wywo³anie animacji startowej
    rysujTekstNaSrodku("System Katalonski", 50, DARKGRAY);
    std::this_thread::sleep_for(std::chrono::seconds(2));

    // G³ówne menu
    menuGlowne();

    // Zamkniêcie okna i zwolnienie zasobów
    CloseWindow();

    return 0;
}
