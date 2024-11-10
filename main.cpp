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
        DrawTexture(teksturyFigur[figura], posX, posY, WHITE);
    }
}

void narysujSzachowniceFEN(const string& fen) {
    int szerokoscSzachownicy = GetScreenHeight() * 0.9;
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

void menuGlowne()
{
    bool wUstawieniach = false;
    bool wWyborzePoziomow = false;

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(kolorTla);

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
            CloseWindow();
        }

        EndDrawing();

        if (wUstawieniach)
        {
            ustawieniaMenu();
            wUstawieniach = false;
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
    int wybranyKolorTla = 1;
    int wybranyKolorPola = 1;

    while (wMenu && !WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(kolorTla);

        DrawText("Glosnosc Muzyki:", 200, 50, 20, WHITE);
        DrawText(TextFormat("%i", glosnoscMuzyki), 450, 50, 20, WHITE);

        if (IsKeyPressed(KEY_LEFT)) glosnoscMuzyki = max(glosnoscMuzyki - 10, 0);
        if (IsKeyPressed(KEY_RIGHT)) glosnoscMuzyki = min(glosnoscMuzyki + 10, 100);

        if (rysujPrzycisk("Przelacz Pelen Ekran", szerokoscOkna / 2 - 100, 100, 200, 50, DARKGRAY, GRAY))
        {
            ToggleFullscreen();
            pelnyEkran = !pelnyEkran;
        }

        // Background color options...

        if (rysujPrzycisk("Wroc", szerokoscOkna / 2 - 100, 500, 200, 50, DARKGRAY, GRAY))
        {
            wMenu = false;
        }

        EndDrawing();
    }
}

void poziom1()
{
    string fen = "rnbqkbnr/pppppppp/8/8/2PP4/5NP1/PP2PPBP/RNBQ1RK1";
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(kolorTla);
        narysujSzachowniceFEN(fen);
        EndDrawing();
    }
}

void wybierzpoziom() {
    bool wybranoPoziom = false;

    while (!wybranoPoziom && !WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(kolorTla);

        if (rysujPrzycisk("Poziom 1", szerokoscOkna / 2 - 100, wysokoscOkna / 2 - 100, 200, 50, DARKGRAY, GRAY)) {
            poziom1();
            wybranoPoziom = true;
        }

        if (rysujPrzycisk("Wroc", szerokoscOkna / 2 - 100, wysokoscOkna / 2 + 100, 200, 50, DARKGRAY, GRAY)) {
            wybranoPoziom = true;
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
