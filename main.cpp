#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <thread>
#include <map>
#include "raylib.h"
#include <algorithm>

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

/*template <typename T>
T Clamp(T value, T min, T max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}
*/



void ustawieniaMenu();
void wybierzpoziom();
std::map<char, Texture2D> teksturyFigur;

struct Zadanie
{
    std::string fen;
    std::string tytul;
    std::vector<std::string> opis;
};

// wczytywanie danych z pliku tekstowego
Zadanie wczytajZadanie(const std::string& sciezkaPliku)
{
    Zadanie zadanie;
    std::ifstream plik(sciezkaPliku);

    if (!plik.is_open())
    {
        cerr << "Nie mozna otworzyc pliku: " << sciezkaPliku << endl;
        return zadanie;
    }

    std::string linia;
    bool pierwszaLinia = true;
    bool tytulUstawiony = false;

    while (std::getline(plik, linia))
    {
        if (pierwszaLinia)
        {
            zadanie.fen = linia; // Pierwsza linia to FEN
            pierwszaLinia = false;
        }
        else if (!tytulUstawiony && !linia.empty())
        {
            zadanie.tytul = linia; // Następna niepusta linia to tytuł
            tytulUstawiony = true;
        }
        else
        {
            zadanie.opis.push_back(linia); // Pozostałe linie to opis
        }
    }

    plik.close();
    return zadanie;
}

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

//MUZYKA

vector<Music> muzyka; // Wektor przechowujący różne utwory
int aktualnyUtwor = 0;     // Indeks aktualnie odtwarzanego utworu
bool muzykaOdtwarzana = true; // Flaga kontrolująca odtwarzanie muzyki
float glosnoscMuzyki = 50.0f; // Domyślna głośność (0-100)

void ladujMuzyke() {
    muzyka.clear();
    muzyka.push_back(LoadMusicStream("muzyka/muzyka1.ogg"));
    muzyka.push_back(LoadMusicStream("muzyka/muzyka2.ogg"));
    muzyka.push_back(LoadMusicStream("muzyka/muzyka3.ogg"));

    if (!muzyka.empty()) {
        aktualnyUtwor = 0;
        PlayMusicStream(muzyka[aktualnyUtwor]);
    }
    else {
        std::cerr << "Nie udalo sie zaladowac muzyki!" << std::endl;
    }
    if (aktualnyUtwor >= 0 && aktualnyUtwor < muzyka.size()) {
        PlayMusicStream(muzyka[aktualnyUtwor]);
    }

}


void zwolnijMuzyke() {
    // Zwolnij pamięć zajmowaną przez utwory
    for (auto& utwor : muzyka) {
        UnloadMusicStream(utwor);
    }
    muzyka.clear();
}
void ustawGlosnoscDlaWszystkich(float poziom) {
    for (auto& utwor : muzyka) {
        SetMusicVolume(utwor, poziom);
    }
}

void obslugaMuzyki() {
    if (muzyka.empty()) return; // Jeśli wektor jest pusty, wyjdź

    UpdateMusicStream(muzyka[aktualnyUtwor]); // Aktualizuj strumień muzyki

    // Sterowanie muzyką
    if (IsKeyPressed(KEY_SPACE)) { // Pauza/Wznowienie
        if (muzykaOdtwarzana) PauseMusicStream(muzyka[aktualnyUtwor]);
        else ResumeMusicStream(muzyka[aktualnyUtwor]);
        muzykaOdtwarzana = !muzykaOdtwarzana;
    }
    if (IsKeyPressed(KEY_RIGHT)) { // Następny utwór
        StopMusicStream(muzyka[aktualnyUtwor]);
        aktualnyUtwor = (aktualnyUtwor + 1) % muzyka.size();
        PlayMusicStream(muzyka[aktualnyUtwor]);
    }
    if (IsKeyPressed(KEY_LEFT)) { // Poprzedni utwór
        StopMusicStream(muzyka[aktualnyUtwor]);
        aktualnyUtwor = (aktualnyUtwor - 1 + muzyka.size()) % muzyka.size();
        PlayMusicStream(muzyka[aktualnyUtwor]);
    }
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
        obslugaMuzyki();

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
        obslugaMuzyki();
        BeginDrawing();
        ClearBackground(kolorTla);

        // Rysowanie suwaka głośności
        DrawText("Glosnosc Muzyki:", 200, 50, 20, WHITE);
        float suwakX = 400; // Pozycja X suwaka
        float suwakY = 50; // Pozycja Y suwaka
        float suwakSzerokosc = 300; // Szerokość suwaka

        // Obsługa myszy dla zmiany głośności
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            Vector2 mousePos = GetMousePosition();
            if (mousePos.x >= suwakX && mousePos.x <= suwakX + suwakSzerokosc &&
                mousePos.y >= suwakY && mousePos.y <= suwakY + 20) {
                glosnoscMuzyki = ((mousePos.x - suwakX) / suwakSzerokosc) * 100.0f;
                //glosnoscMuzyki = Clamp(glosnoscMuzyki, 0.0f, 100.0f); // Utrzymuj wartość w zakresie 0-100
                if (glosnoscMuzyki <= 0.0f) glosnoscMuzyki = 0.0f;
                if (glosnoscMuzyki >= 100.0f) glosnoscMuzyki = 100.0f;
                if (aktualnyUtwor >= 0 && aktualnyUtwor < muzyka.size()) {
                    SetMusicVolume(muzyka[aktualnyUtwor], glosnoscMuzyki / 100.0f);
                }
            }
        }

        // Rysowanie tła suwaka i wskaźnika
        DrawRectangle(suwakX, suwakY, suwakSzerokosc, 20, LIGHTGRAY);
        DrawRectangle(suwakX + (glosnoscMuzyki / 100.0f) * suwakSzerokosc - 5, suwakY - 5, 10, 30, RED);

        // Wyświetlenie wartości głośności
        DrawText(TextFormat("%i", (int)glosnoscMuzyki), suwakX + suwakSzerokosc + 100, suwakY-5, 20, WHITE);

        if (rysujPrzycisk("Nastepny utwor", suwakX+suwakSzerokosc+20, suwakY-10, 200, 50, DARKGRAY, GRAY)) {
            if (!muzyka.empty()) {
                StopMusicStream(muzyka[aktualnyUtwor]);
                aktualnyUtwor = (aktualnyUtwor + 1) % muzyka.size();
                PlayMusicStream(muzyka[aktualnyUtwor]);
            }
        }

        // Przycisk do przełączania pełnego ekranu
        if (rysujPrzycisk("Pelen Ekran", szerokoscOkna / 2 - 100, 100, 200, 50, DARKGRAY, GRAY))
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

// Funkcja poziom1
void poziom1()
{
    // Wczytaj zadanie z pliku
    Zadanie zadanie = wczytajZadanie("zadania/zadanie 1.txt");
    if (zadanie.fen.empty())
    {
        std::cerr << "Nie udalo sie wczytac zadania!" << std::endl;
        return;
    }

    bool wPoziomie = true;

    while (wPoziomie && !WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(kolorTla);
        obslugaMuzyki();

        int margines = 20;
        int szerokoscPola = 200;

        // Rysowanie przezroczystego pola na informacje
        DrawRectangle(margines, margines, szerokoscPola, GetScreenHeight() - (margines * 2), ColorAlpha(DARKGRAY, 0.8f));
        DrawRectangleLines(margines, margines, szerokoscPola, GetScreenHeight() - (margines * 2), WHITE);

        // Rysowanie tytułu zadania
        DrawText(zadanie.tytul.c_str(), margines + 10, margines + 10, 20, WHITE);

        // Rysowanie opisu zadania
        int wysokoscLinii = 20; // Odstęp między liniami tekstu
        for (size_t i = 0; i < zadanie.opis.size(); ++i)
        {
            DrawText(zadanie.opis[i].c_str(), margines + 10, margines + 40 + i * wysokoscLinii, 16, WHITE);
        }

        // Rysowanie szachownicy, przesuniętej w prawo o szerokość pola na tekst + margines
        narysujSzachowniceFEN(zadanie.fen);

        // Rysowanie przycisku „Powrót”
        if (rysujPrzycisk("Powrot", GetScreenWidth() - 150, 10, 140, 40, DARKGRAY, GRAY))
        {
            wPoziomie = false;
        }

        // Rysowanie przycisku „Ustawienia”
        if (rysujPrzycisk("Ustawienia", GetScreenWidth() - 150, 60, 140, 40, DARKGRAY, GRAY))
        {
            ustawieniaMenu();
        }

        EndDrawing();
    }
}




void poziom2()
{
    // Wczytaj zadanie z pliku
    Zadanie zadanie = wczytajZadanie("zadania/zadanie 2.txt");
    if (zadanie.fen.empty())
    {
        std::cerr << "Nie udalo sie wczytac zadania!" << std::endl;
        return;
    }

    bool wPoziomie = true;

    while (wPoziomie && !WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(kolorTla);
        obslugaMuzyki();

        int margines = 20;
        int szerokoscPola = 200;

        // Rysowanie przezroczystego pola na informacje
        DrawRectangle(margines, margines, szerokoscPola, GetScreenHeight() - (margines * 2), ColorAlpha(DARKGRAY, 0.8f));
        DrawRectangleLines(margines, margines, szerokoscPola, GetScreenHeight() - (margines * 2), WHITE);

        // Rysowanie tytułu zadania
        DrawText(zadanie.tytul.c_str(), margines + 10, margines + 10, 20, WHITE);

        // Rysowanie opisu zadania
        int wysokoscLinii = 20; // Odstęp między liniami tekstu
        for (size_t i = 0; i < zadanie.opis.size(); ++i)
        {
            DrawText(zadanie.opis[i].c_str(), margines + 10, margines + 40 + i * wysokoscLinii, 16, WHITE);
        }

        // Rysowanie szachownicy, przesuniętej w prawo o szerokość pola na tekst + margines
        narysujSzachowniceFEN(zadanie.fen);

        // Rysowanie przycisku „Powrót”
        if (rysujPrzycisk("Powrot", GetScreenWidth() - 150, 10, 140, 40, DARKGRAY, GRAY))
        {
            wPoziomie = false;
        }

        // Rysowanie przycisku „Ustawienia”
        if (rysujPrzycisk("Ustawienia", GetScreenWidth() - 150, 60, 140, 40, DARKGRAY, GRAY))
        {
            ustawieniaMenu();
        }

        EndDrawing();
    }
}
void poziom3()
{
    // Wczytaj zadanie z pliku
    Zadanie zadanie = wczytajZadanie("zadania/zadanie 3.txt");
    if (zadanie.fen.empty())
    {
        std::cerr << "Nie udalo sie wczytac zadania!" << std::endl;
        return;
    }

    bool wPoziomie = true;

    while (wPoziomie && !WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(kolorTla);
        obslugaMuzyki();
        int margines = 20;
        int szerokoscPola = 200;

        // Rysowanie przezroczystego pola na informacje
        DrawRectangle(margines, margines, szerokoscPola, GetScreenHeight() - (margines * 2), ColorAlpha(DARKGRAY, 0.8f));
        DrawRectangleLines(margines, margines, szerokoscPola, GetScreenHeight() - (margines * 2), WHITE);

        // Rysowanie tytułu zadania
        DrawText(zadanie.tytul.c_str(), margines + 10, margines + 10, 20, WHITE);

        // Rysowanie opisu zadania
        int wysokoscLinii = 20; // Odstęp między liniami tekstu
        for (size_t i = 0; i < zadanie.opis.size(); ++i)
        {
            DrawText(zadanie.opis[i].c_str(), margines + 10, margines + 40 + i * wysokoscLinii, 16, WHITE);
        }

        // Rysowanie szachownicy, przesuniętej w prawo o szerokość pola na tekst + margines
        narysujSzachowniceFEN(zadanie.fen);

        // Rysowanie przycisku „Powrót”
        if (rysujPrzycisk("Powrot", GetScreenWidth() - 150, 10, 140, 40, DARKGRAY, GRAY))
        {
            wPoziomie = false;
        }

        // Rysowanie przycisku „Ustawienia”
        if (rysujPrzycisk("Ustawienia", GetScreenWidth() - 150, 60, 140, 40, DARKGRAY, GRAY))
        {
            ustawieniaMenu();
        }

        EndDrawing();
    }
}

void wybierzpoziom()
{
    bool wMenuWyboruPoziomu = true; // Flaga kontrolująca pozostanie w menu wyboru poziomów

    while (wMenuWyboruPoziomu && !WindowShouldClose())
    {
        //obslugaMuzyki();
        obslugaMuzyki();
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
    InitAudioDevice();
    ladujMuzyke();

 /*   Music muzyka = LoadMusicStream("muzyka/muzyka1.ogg");
    PlayMusicStream(muzyka);
    muzyka.looping = true;
    */

    menuGlowne();

    zwolnijTekstury();
    zwolnijMuzyke();
    CloseWindow();

    return 0;
}