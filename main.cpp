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
int zaznaczonyX = -1, zaznaczonyY = -1;
bool figuraZaznaczona = false;
// Reprezentacja szachownicy jako tablica 8x8
char szachownica[8][8];
struct Ruch {
    int startX, startY, celX, celY;
    char figuraBita;
};

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


std::map<char, Texture2D> teksturyFigur;

bool czyBialeNaRuchu = true;

struct Zadanie {
    string fen;
    string tytul;
    vector<string> opis;
    bool bialeZaczynaja = true; // Domyślnie białe zaczynają
};

// Modyfikacja funkcji inicjalizacji zadania
Zadanie wczytajZadanie(const string& sciezka) {
    Zadanie zadanie;
    ifstream plik(sciezka);
    if (plik.is_open()) {
        getline(plik, zadanie.fen);
        getline(plik, zadanie.tytul);
        string linia;
        while (getline(plik, linia)) {
            if (linia == "BIALE") {
                zadanie.bialeZaczynaja = true;
            }
            else if (linia == "CZARNE") {
                zadanie.bialeZaczynaja = false;
            }
            else {
                zadanie.opis.push_back(linia);
            }
        }
        plik.close();
        czyBialeNaRuchu = zadanie.bialeZaczynaja; // Ustawienie kto zaczyna
    }
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

// Funkcja do konwersji szachownicy na FEN
string konwertujNaFEN() {
    string fen;
    for (int y = 0; y < 8; ++y) {
        int pustePola = 0;
        for (int x = 0; x < 8; ++x) {
            if (szachownica[y][x] == ' ') {
                pustePola++;
            }
            else {
                if (pustePola > 0) {
                    fen += to_string(pustePola);
                    pustePola = 0;
                }
                fen += szachownica[y][x];
            }
        }
        if (pustePola > 0) {
            fen += to_string(pustePola);
        }
        if (y < 7) fen += '/';
    }
    return fen;
}

bool czyFiguraAtakujePole(int startX, int startY, int celX, int celY) {
    char figura = szachownica[startY][startX];
    if (figura == ' ') return false;

    // Ruchy pionka
    if (figura == 'P') { // Biały pionek
        return (celY == startY - 1 && abs(celX - startX) == 1);
    }
    else if (figura == 'p') { // Czarny pionek
        return (celY == startY + 1 && abs(celX - startX) == 1);
    }
    // Ruchy skoczka
    else if (figura == 'N' || figura == 'n') {
        int dx = abs(celX - startX), dy = abs(celY - startY);
        return (dx * dx + dy * dy == 5); // Skoczek porusza się w "L"
    }
    // Ruchy wieży
    else if (figura == 'R' || figura == 'r') {
        if (celX == startX || celY == startY) {
            int dx = (celX - startX == 0) ? 0 : (celX - startX) / abs(celX - startX);
            int dy = (celY - startY == 0) ? 0 : (celY - startY) / abs(celY - startY);
            for (int x = startX + dx, y = startY + dy; x != celX || y != celY; x += dx, y += dy) {
                if (szachownica[y][x] != ' ') return false;
            }
            return true;
        }
    }
    // Ruchy gońca
    else if (figura == 'B' || figura == 'b') {
        if (abs(celX - startX) == abs(celY - startY)) {
            int dx = (celX - startX > 0) ? 1 : -1;
            int dy = (celY - startY > 0) ? 1 : -1;
            for (int x = startX + dx, y = startY + dy; x != celX; x += dx, y += dy) {
                if (szachownica[y][x] != ' ') return false;
            }
            return true;
        }
    }
    // Ruchy hetmana
    else if (figura == 'Q' || figura == 'q') {
        if (abs(celX - startX) == abs(celY - startY) || celX == startX || celY == startY) {
            int dx = (celX - startX == 0) ? 0 : (celX - startX) / abs(celX - startX);
            int dy = (celY - startY == 0) ? 0 : (celY - startY) / abs(celY - startY);
            for (int x = startX + dx, y = startY + dy; x != celX || y != celY; x += dx, y += dy) {
                if (szachownica[y][x] != ' ') return false;
            }
            return true;
        }
    }

    return false; // Nieznana figura lub brak ataku
}

bool czySzach(bool bialyKrol) {
    int krolX = -1, krolY = -1;
    char krol = bialyKrol ? 'K' : 'k';

    // Znajdź pozycję króla
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            if (szachownica[y][x] == krol) {
                krolX = x;
                krolY = y;
                break;
            }
        }
        if (krolX != -1) break;
    }

    // Sprawdź, czy jakakolwiek figura przeciwnika atakuje króla
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            char figura = szachownica[y][x];
            if (bialyKrol && figura >= 'a' && figura <= 'z') {
                if (czyFiguraAtakujePole(x, y, krolX, krolY)) {
                    return true;
                }
            }
            else if (!bialyKrol && figura >= 'A' && figura <= 'Z') {
                if (czyFiguraAtakujePole(x, y, krolX, krolY)) {
                    return true;
                }
            }
        }
    }

    return false; // Król nie jest zagrożony
}


bool czyRuchPoprawny(int startX, int startY, int celX, int celY) {
    char figura = szachownica[startY][startX];
    char cel = szachownica[celY][celX];

    // Sprawdzenie podstawowe: cel musi znajdować się w granicach szachownicy i nie może być na miejscu startowym
    if (celX < 0 || celX >= 8 || celY < 0 || celY >= 8) {
        return false;
    }
    if (startX == celX && startY == celY) {
        return false;
    }

    // Sprawdzenie, czy figura docelowa nie jest tego samego koloru
    bool figuraBiala = (figura >= 'A' && figura <= 'Z');
    bool celBialy = (cel >= 'A' && cel <= 'Z');
    bool figuraCzarna = (figura >= 'a' && figura <= 'z');
    bool celCzarny = (cel >= 'a' && cel <= 'z');

    if ((figuraBiala && celBialy) || (figuraCzarna && celCzarny)) {
        return false; // Nie można bić swojej figury
    }
    // Tymczasowy ruch
    szachownica[startY][startX] = ' ';
    szachownica[celY][celX] = figura;

    // Sprawdzenie szacha
    bool wynik = true;
    if (figuraBiala) {
        wynik = !czySzach(true);
    }
    else {
        wynik = !czySzach(false);
    }

    // Cofnięcie ruchu
    szachownica[startY][startX] = figura;
    szachownica[celY][celX] = cel;
    // Ruchy pionka
    if (figura == 'P') { // Biały pionek
        if (cel == ' ' && celX == startX && celY == startY - 1) {
            return true; // Ruch o jedno pole do przodu
        }
        if (cel == ' ' && startY == 6 && celX == startX && celY == startY - 2) {
            return true; // Ruch o dwa pola do przodu z pozycji startowej
        }
        if (cel != ' ' && abs(celX - startX) == 1 && celY == startY - 1) {
            return true; // Bicie na ukos
        }
    }
    else if (figura == 'p') { // Czarny pionek
        if (cel == ' ' && celX == startX && celY == startY + 1) {
            return true; // Ruch o jedno pole do przodu
        }
        if (cel == ' ' && startY == 1 && celX == startX && celY == startY + 2) {
            return true; // Ruch o dwa pola do przodu z pozycji startowej
        }
        if (cel != ' ' && abs(celX - startX) == 1 && celY == startY + 1) {
            return true; // Bicie na ukos
        }
    }
    // Ruchy skoczka
    else if (figura == 'N' || figura == 'n') { // Biały lub czarny skoczek
        int dx = abs(celX - startX);
        int dy = abs(celY - startY);
        if (dx * dx + dy * dy == 5) { // Skoczek porusza się w kształcie litery "L"
            return true;
        }
    }
    // Ruchy wieży
    else if (figura == 'R' || figura == 'r') { // Biała lub czarna wieża
        if (celX == startX || celY == startY) { // Porusza się tylko w pionie lub poziomie
            // Sprawdzenie przeszkód na drodze
            int dx = (celX - startX == 0) ? 0 : (celX - startX) / abs(celX - startX);
            int dy = (celY - startY == 0) ? 0 : (celY - startY) / abs(celY - startY);
            int x = startX + dx, y = startY + dy;
            while (x != celX || y != celY) {
                if (szachownica[y][x] != ' ') return false;
                x += dx;
                y += dy;
            }
            return true;
        }
    }
    // Ruchy gońca
    else if (figura == 'B' || figura == 'b') { // Biały lub czarny goniec
        if (abs(celX - startX) == abs(celY - startY)) { // Porusza się po przekątnych
            int dx = (celX - startX > 0) ? 1 : -1;
            int dy = (celY - startY > 0) ? 1 : -1;
            int x = startX + dx, y = startY + dy;
            while (x != celX && y != celY) {
                if (szachownica[y][x] != ' ') return false;
                x += dx;
                y += dy;
            }
            return true;
        }
    }
    // Ruchy hetmana
    else if (figura == 'Q' || figura == 'q') { // Biały lub czarny hetman
        // Hetman łączy ruchy wieży i gońca
        if (abs(celX - startX) == abs(celY - startY) || celX == startX || celY == startY) {
            int dx = (celX - startX == 0) ? 0 : (celX - startX) / abs(celX - startX);
            int dy = (celY - startY == 0) ? 0 : (celY - startY) / abs(celY - startY);
            int x = startX + dx, y = startY + dy;
            while (x != celX || y != celY) {
                if (szachownica[y][x] != ' ') return false;
                x += dx;
                y += dy;
            }
            return true;
        }
    }
    if (figura == 'K' || figura == 'k') { // Biały lub czarny król
        if (abs(celX - startX) <= 1 && abs(celY - startY) <= 1) {
            // Król porusza się o jedno pole w dowolnym kierunku (standardowy ruch)
            return true;
        }

        // Roszada
        if (startY == celY && (celX == startX + 2 || celX == startX - 2)) {
            // Sprawdzenie, czy jest roszada (król przesuwa się o dwa pola w poziomie)
            if (figura == 'K') { // Roszada białego króla
                if (startX == 4 && (celX == 6 || celX == 2)) {
                    // Warunki roszady:
                    // 1. Król nie może się poruszać przez szach
                    if (czySzach(true)) return false;
                    // 2. Muszą być puste pola między królem a wieżą
                    if (celX == 6) { // Król na e1 i wieża na h1
                        if (szachownica[7][5] != ' ' || szachownica[7][6] != ' ') return false;
                        if (szachownica[7][7] != 'R') return false; // Wieża na h1
                        // Przesunięcie wieży
                        szachownica[7][5] = 'R'; // Wieża przechodzi na f1
                        szachownica[7][7] = ' '; // Pole h1 jest puste
                    }
                    else if (celX == 2) { // Król na e1 i wieża na a1
                        if (szachownica[7][1] != ' ' || szachownica[7][2] != ' ' || szachownica[7][3] != ' ') return false;
                        if (szachownica[7][0] != 'R') return false; // Wieża na a1
                        // Przesunięcie wieży
                        szachownica[7][3] = 'R'; // Wieża przechodzi na d1
                        szachownica[7][0] = ' '; // Pole a1 jest puste
                    }
                    return true; // Roszada białego króla zakończona
                }
            }
            else if (figura == 'k') { // Roszada czarnego króla
                if (startX == 4 && (celX == 6 || celX == 2)) {
                    // Warunki roszady:
                    // 1. Król nie może się poruszać przez szach
                    if (czySzach(false)) return false;
                    // 2. Muszą być puste pola między królem a wieżą
                    if (celX == 6) { // Król na e8 i wieża na h8
                        if (szachownica[0][5] != ' ' || szachownica[0][6] != ' ') return false;
                        if (szachownica[0][7] != 'r') return false; // Wieża na h8
                        // Przesunięcie wieży
                        szachownica[0][5] = 'r'; // Wieża przechodzi na f8
                        szachownica[0][7] = ' '; // Pole h8 jest puste
                    }
                    else if (celX == 2) { // Król na e8 i wieża na a8
                        if (szachownica[0][1] != ' ' || szachownica[0][2] != ' ' || szachownica[0][3] != ' ') return false;
                        if (szachownica[0][0] != 'r') return false; // Wieża na a8
                        // Przesunięcie wieży
                        szachownica[0][3] = 'r'; // Wieża przechodzi na d8
                        szachownica[0][0] = ' '; // Pole a8 jest puste
                    }
                    return true; // Roszada czarnego króla zakończona
                }
            }
        }
    }

    return false; // Domyślnie ruch nie jest poprawny
}


void wypiszSzachownice() {
    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            cout << szachownica[y][x] << " ";
        }
        cout << endl;
    }
    cout << "-----------------" << endl;
}

// Modyfikacja funkcji obslugaRuchow
void obslugaRuchow(Zadanie& zadanie) {
    int szerokoscSzachownicy = min(GetScreenHeight(), GetScreenWidth()) * 0.8;
    int startX = (GetScreenWidth() - szerokoscSzachownicy) / 2;
    int startY = (GetScreenHeight() - szerokoscSzachownicy) / 2;
    int poleRozmiar = szerokoscSzachownicy / ROZMIAR_SZACHOWNICY;

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        Vector2 pozycjaMyszy = GetMousePosition();
        int x = (pozycjaMyszy.x - startX) / poleRozmiar;
        int y = (pozycjaMyszy.y - startY) / poleRozmiar;

        if (x >= 0 && x < 8 && y >= 0 && y < 8) { // Kliknięcie w granicach szachownicy
            if (!figuraZaznaczona) {
                if (szachownica[y][x] != ' ') { // Zaznacz figurę
                    bool bialeFigura = isupper(szachownica[y][x]);
                    if (czyBialeNaRuchu == bialeFigura) { // Sprawdzanie ruchu właściwego gracza
                        zaznaczonyX = x;
                        zaznaczonyY = y;
                        figuraZaznaczona = true;
                        cout << "Zaznaczono figure: " << szachownica[y][x] << " na (" << x << ", " << y << ")" << endl;
                    }
                    else {
                        cout << "Nie mozesz wykonac ruchu ta figura!" << endl;
                    }
                }
            }
            else {
                cout << "Proba ruchu z (" << zaznaczonyX << ", " << zaznaczonyY << ") na (" << x << ", " << y << ")" << endl;
                if (czyRuchPoprawny(zaznaczonyX, zaznaczonyY, x, y)) {
                    szachownica[y][x] = szachownica[zaznaczonyY][zaznaczonyX];
                    szachownica[zaznaczonyY][zaznaczonyX] = ' ';
                    zadanie.fen = konwertujNaFEN(); // Aktualizuj FEN
                    czyBialeNaRuchu = !czyBialeNaRuchu; // Zmiana tury
                    cout << "Ruch wykonany. Nowy FEN: " << zadanie.fen << endl;
                }
                else {
                    cout << "Niepoprawny ruch!" << endl;
                }
                figuraZaznaczona = false;
            }
        }
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

// Funkcja do inicjalizacji szachownicy na podstawie FEN
void inicjalizujSzachownice(const string& fen) {
    int x = 0, y = 0;
    for (char c : fen) {
        if (c == ' ') break;
        if (c == '/') {
            y++;
            x = 0;
        }
        else if (isdigit(c)) {
            int pustePola = c - '0';
            for (int i = 0; i < pustePola; ++i) {
                szachownica[y][x++] = ' '; // Puste pole
            }
        }
        else {
            szachownica[y][x++] = c; // Figura
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

void zobacztworcow()
{
    bool wogladaniutworcow = true; // Flaga kontrolująca pozostanie w menu wyboru poziomów
    while (wogladaniutworcow && !WindowShouldClose())
    {
        obslugaMuzyki();
        BeginDrawing();
        ClearBackground(kolorTla);
        DrawText("Kod - Pawel Handwerkier", 400, 50, 30, DARKGRAY);
        DrawText("Grafika - kradzione z chess.com", 400, 100, 30, DARKGRAY);
        // Dodanie przycisku do powrotu do głównego menu
        if (rysujPrzycisk("Wroc", szerokoscOkna / 2 - 100, GetScreenHeight() - 100, 200, 50, DARKGRAY, GRAY))
        {
            wogladaniutworcow = false; // Wyjście z tworcow
        }

        EndDrawing();

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
        DrawText(TextFormat("%i", (int)glosnoscMuzyki), suwakX + suwakSzerokosc + 100, suwakY - 5, 20, WHITE);

        if (rysujPrzycisk("Nastepny utwor", suwakX + suwakSzerokosc + 20, suwakY - 10, 200, 50, DARKGRAY, GRAY)) {
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
// Funkcje poziomów
void poziom1() {
    Zadanie zadanie = wczytajZadanie("zadania/zadanie 1.txt");
    if (zadanie.fen.empty()) {
        std::cerr << "Nie udało się wczytać zadania!" << std::endl;
        return;
    }
    inicjalizujSzachownice(zadanie.fen);
    bool wPoziomie = true;

    while (wPoziomie && !WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(kolorTla);
        obslugaMuzyki();

        int margines = 20;
        int szerokoscPola = 200;

        DrawRectangle(margines, margines, szerokoscPola, GetScreenHeight() - (margines * 2), ColorAlpha(DARKGRAY, 0.8f));
        DrawRectangleLines(margines, margines, szerokoscPola, GetScreenHeight() - (margines * 2), WHITE);

        DrawText(zadanie.tytul.c_str(), margines + 10, margines + 10, 20, WHITE);
        obslugaRuchow(zadanie);

        for (size_t i = 0; i < zadanie.opis.size(); ++i) {
            DrawText(zadanie.opis[i].c_str(), margines + 10, margines + 40 + i * 20, 16, WHITE);
        }

        narysujSzachowniceFEN(zadanie.fen); // Rysowanie szachownicy z aktualnym FEN

        if (rysujPrzycisk("Powrot", GetScreenWidth() - 150, 10, 140, 40, DARKGRAY, GRAY)) {
            wPoziomie = false;
        }

        if (rysujPrzycisk("Ustawienia", GetScreenWidth() - 150, 60, 140, 40, DARKGRAY, GRAY)) {
            ustawieniaMenu();
        }

        EndDrawing();
    }
}
// Automatyczne generowanie kolejnych poziomów
#define GENERUJ_POZIOM(n)                                   \
void poziom##n() {                                          \
    Zadanie zadanie = wczytajZadanie("zadania/zadanie " #n ".txt"); \
    if (zadanie.fen.empty()) {                              \
        std::cerr << "Nie udało się wczytać zadania!" << std::endl; \
        return;                                             \
    }                                                       \
    inicjalizujSzachownice(zadanie.fen);                    \
    bool wPoziomie = true;                                  \
                                                            \
    while (wPoziomie && !WindowShouldClose()) {             \
        BeginDrawing();                                     \
        ClearBackground(kolorTla);                         \
        obslugaMuzyki();                                    \
                                                            \
        int margines = 20;                                  \
        int szerokoscPola = 200;                            \
                                                            \
        DrawRectangle(margines, margines, szerokoscPola, GetScreenHeight() - (margines * 2), ColorAlpha(DARKGRAY, 0.8f)); \
        DrawRectangleLines(margines, margines, szerokoscPola, GetScreenHeight() - (margines * 2), WHITE); \
                                                            \
        DrawText(zadanie.tytul.c_str(), margines + 10, margines + 10, 20, WHITE); \
        obslugaRuchow(zadanie);                             \
                                                            \
        for (size_t i = 0; i < zadanie.opis.size(); ++i) {  \
            DrawText(zadanie.opis[i].c_str(), margines + 10, margines + 40 + i * 20, 16, WHITE); \
        }                                                   \
                                                            \
        narysujSzachowniceFEN(zadanie.fen);                 \
                                                            \
        if (rysujPrzycisk("Powrot", GetScreenWidth() - 150, 10, 140, 40, DARKGRAY, GRAY)) { \
            wPoziomie = false;                              \
        }                                                   \
                                                            \
        if (rysujPrzycisk("Ustawienia", GetScreenWidth() - 150, 60, 140, 40, DARKGRAY, GRAY)) { \
            ustawieniaMenu();                               \
        }                                                   \
                                                            \
        EndDrawing();                                       \
    }                                                       \
}

GENERUJ_POZIOM(2)
GENERUJ_POZIOM(3)
GENERUJ_POZIOM(4)
GENERUJ_POZIOM(5)
GENERUJ_POZIOM(6)
GENERUJ_POZIOM(7)
GENERUJ_POZIOM(8)
GENERUJ_POZIOM(9)
GENERUJ_POZIOM(10)
GENERUJ_POZIOM(11)
GENERUJ_POZIOM(12)
GENERUJ_POZIOM(13)
GENERUJ_POZIOM(14)
GENERUJ_POZIOM(15)
GENERUJ_POZIOM(16)
GENERUJ_POZIOM(17)
GENERUJ_POZIOM(18)
GENERUJ_POZIOM(19)
GENERUJ_POZIOM(20)
GENERUJ_POZIOM(21)
GENERUJ_POZIOM(22)
GENERUJ_POZIOM(23)
GENERUJ_POZIOM(24)
GENERUJ_POZIOM(25)
GENERUJ_POZIOM(26)
GENERUJ_POZIOM(27)
GENERUJ_POZIOM(28)
GENERUJ_POZIOM(29)
GENERUJ_POZIOM(30)







void wybierzpoziom()
{
    // Tablica wskaźników do funkcji poziomów
    void (*poziomy[30])() = {
        poziom1, poziom2, poziom3, poziom4, poziom5, poziom6, poziom7, poziom8, poziom9, poziom10,
        poziom11, poziom12, poziom13, poziom14, poziom15, poziom16, poziom17, poziom18, poziom19, poziom20,
        poziom21, poziom22, poziom23, poziom24, poziom25, poziom26, poziom27, poziom28, poziom29, poziom30
    };

    bool wMenuWyboruPoziomu = true; // Flaga kontrolująca pozostanie w menu wyboru poziomów
    int offsetY = 0;                // Przesunięcie pionowe do przewijania poziomów
    const int maxOffsetY = 0;     // Maksymalne przesunięcie (dla góry listy)
    const int minOffsetY = -100 * ((30 + 4) / 5 - 3); // Dla 3 widocznych rzędów

    while (wMenuWyboruPoziomu && !WindowShouldClose())
    {
        obslugaMuzyki();
        BeginDrawing();
        ClearBackground(kolorTla);

        DrawText("Wybierz poziom:", szerokoscOkna / 2 - 100, 50, 30, WHITE);

        int liczbaPrzyciskow = 30;  // Liczba dostępnych poziomów
        int kolumny = 5;           // Liczba przycisków w jednym rzędzie
        int rozmiarPrzycisku = 100;
        int odstep = 20;

        // Rysowanie przycisków poziomów z uwzględnieniem przesunięcia offsetY
        for (int i = 0; i < liczbaPrzyciskow; i++) {
            int x = (szerokoscOkna / 2 - (rozmiarPrzycisku + odstep) * kolumny / 2)
                + (i % kolumny) * (rozmiarPrzycisku + odstep);
            int y = 150 + (i / kolumny) * (rozmiarPrzycisku + odstep) + offsetY;

            // Tylko przyciski widoczne na ekranie
            if (y + rozmiarPrzycisku > 150 && y < GetScreenHeight() - 100) {
                if (rysujPrzycisk(TextFormat("Poziom %d", i + 1), x, y, rozmiarPrzycisku, rozmiarPrzycisku, MAGENTA, DARKPURPLE)) {
                    poziomy[i]();
                }
            }
        }

        // Strzałka przewijania w górę
        if (rysujPrzycisk("Gora", szerokoscOkna - 120, 10, 100, 40, DARKGRAY, GRAY)) {
            offsetY += 150; // Przesunięcie o jeden rząd w górę
            if (offsetY > maxOffsetY) offsetY = maxOffsetY; // Ograniczenie przesunięcia
        }

        // Strzałka przewijania w dół
        if (rysujPrzycisk("Dol", szerokoscOkna - 120, GetScreenHeight() - 60, 100, 40, DARKGRAY, GRAY)) {
            offsetY -= 150; // Przesunięcie o jeden rząd w dół
            if (offsetY < minOffsetY) offsetY = minOffsetY; // Ograniczenie przesunięcia
        }

        // Dodanie przycisku do powrotu do głównego menu
        if (rysujPrzycisk("Wroc", szerokoscOkna / 2 - 100, GetScreenHeight() - 100, 200, 50, DARKGRAY, GRAY))
        {
            wMenuWyboruPoziomu = false; // Wyjście z menu wyboru poziomów
        }

        EndDrawing();
    }
}

// Funkcja menu głównego
void menuGlowne()
{
    bool wUstawieniach = false;
    bool wWyborzePoziomow = false;
    bool wTworcach = false;

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
            wTworcach = true;
            //rysujTekstNaSrodku("Paweł Handwerkier", 30, DARKGRAY);
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
        if (wTworcach)
        {
            zobacztworcow();
            wTworcach = false;
        }
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