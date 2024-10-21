#include <iostream>
#include <thread>      // Dodanie biblioteki do funkcji sleep
#include "raylib.h"    // Do��czenie nag��wka raylib

using namespace std;

// Ustawienia rozmiaru okna
int szerokoscOkna = 1280;
int wysokoscOkna = 720;
Color kolorTla = LIME;  // Domy�lny kolor t�a

// Deklaracja funkcji ustawieniaMenu, aby by�a rozpoznawana w funkcji menuGlowne()
void ustawieniaMenu();

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

//-------------------------------------FUNKCJA RYSUJ�CA PRZYCISKI---------------------------------------------
bool rysujPrzycisk(const char* tekst, int x, int y, int szerokosc, int wysokosc)
{
    Rectangle przyciskRect = { (float)x, (float)y, (float)szerokosc, (float)wysokosc };

    // Sprawdzenie, czy mysz jest na przycisku
    bool nadPrzyciskiem = CheckCollisionPointRec(GetMousePosition(), przyciskRect);

    // Rysowanie przycisku (zmiana koloru po najechaniu myszk�)
    Color kolorPrzycisku = nadPrzyciskiem ? DARKGRAY : GRAY;
    DrawRectangleRec(przyciskRect, kolorPrzycisku);
    DrawText(tekst, x + 20, y + 10, 20, RAYWHITE);

    // Zwr�cenie true, je�li przycisk zosta� klikni�ty
    return nadPrzyciskiem && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}

// Funkcja menu g��wnego
void menuGlowne()
{
    bool wUstawieniach = false;

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(kolorTla);

        // Rysowanie przycisk�w
        if (rysujPrzycisk("Ustawienia", szerokoscOkna / 2 - 100, wysokoscOkna / 2 - 100, 200, 50))
        {
            wUstawieniach = true;
        }
        if (rysujPrzycisk("Tworcy", szerokoscOkna / 2 - 100, wysokoscOkna / 2, 200, 50))
        {
            rysujTekstNaSrodku("Pawe� Handwerkier", 30, DARKGRAY);
        }
        if (rysujPrzycisk("Wyjdz", szerokoscOkna / 2 - 100, wysokoscOkna / 2 + 100, 200, 50))
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
        ClearBackground(kolorTla);  // U�ywamy zmiennej kolorTla zamiast DARKGRAY

        // Opcja do regulacji g�o�no�ci
        DrawText("Glosnosc Muzyki:", 200, 200, 20, WHITE);
        DrawText(TextFormat("%i", glosnoscMuzyki), 450, 200, 20, WHITE);

        if (IsKeyPressed(KEY_LEFT)) glosnoscMuzyki = max(glosnoscMuzyki - 10, 0);  // Zmniejszenie g�o�no�ci
        if (IsKeyPressed(KEY_RIGHT)) glosnoscMuzyki = min(glosnoscMuzyki + 10, 100); // Zwi�kszenie g�o�no�ci

        // Opcja do prze��czania trybu pe�noekranowego
        if (rysujPrzycisk("Przelacz Pelen Ekran", szerokoscOkna / 2 - 100, 300, 200, 50))
        {
            ToggleFullscreen();
            pelnyEkran = !pelnyEkran;
        }

        // Dodanie opcji zmiany koloru t�a
        DrawText("Wybierz Kolor Tla:", 200, 350, 20, WHITE);
        if (rysujPrzycisk("Zielony", 200, 400, 200, 50)) kolorTla = LIME;
        if (rysujPrzycisk("Niebieski", 450, 400, 200, 50)) kolorTla = SKYBLUE;
        if (rysujPrzycisk("Czerwony", 700, 400, 200, 50)) kolorTla = RED;

        // Opcja powrotu do g��wnego menu
        if (rysujPrzycisk("Wroc", szerokoscOkna / 2 - 100, 500, 200, 50))
        {
            wMenu = false; // Wyj�cie z menu ustawie�
        }

        EndDrawing();
    }
}

int main()
{
    // Inicjalizacja okna
    InitWindow(szerokoscOkna, wysokoscOkna, "System Katalonski");

    // Wywo�anie animacji startowej
    rysujTekstNaSrodku("System Katalonski", 50, DARKGRAY);
    std::this_thread::sleep_for(std::chrono::seconds(2));

    // G��wne menu
    menuGlowne();

    // Zamkni�cie okna i zwolnienie zasob�w
    CloseWindow();

    return 0;
}
