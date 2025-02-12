# SYSTEM KATALONSKI
Program służy jako pomoc do poprawienia swojego poziomu gry w szachach, skupiając się na strukturach powstałych po systemie katalońskim.
Składa się z wybranych zadań szachowych, które użytkownik ma za zadanie rozwiązać.
Autorem programu jest Handwerkier Paweł. Program został zrealizowany w ramach przedmiotu "Projekt C" prowadzonego przez Doktora Juszczaka Cezarego.
Grafiki oraz wybrane dźwięki zostały zapożyczone ze strony internetowej chess.com.
Program stworzony w c++, używając biblioteki raylib i docelowo jest na system Windows
>> Uruchamianie z pliku .exe

Należy pobrać wszystkie pliki z githuba i je rozpakować do jednego folderu. Pliki dzielą się na dźwięki, grafiki oraz kod

Aby uruchomić program należy wejść w folder Projekcik i uruchomić plik projekcik.exe




>> Otwieranie projektu w Visual Studio i podłączanie bibliotek:

Uruchamianie w Visual Studio 2022.
Kliknij "Otwórz projekt lub rozwiązanie".
Wskaż plik .sln w folderze Projekcik i kliknij "Otwórz".

Podłączenie bibliotek raylib (jeśli są w tym samym folderze co projekt):

Wejdź w Project > Properties.
Przejdź do C/C++ > General > Additional Include Directories i dodaj:

raylib-5.0_win64_msvc16\include

Przejdź do Linker > General > Additional Library Directories i dodaj:

raylib-5.0_win64_msvc16\lib

Przejdź do Linker > Input > Additional Dependencies i dopisz:

raylib.lib


I teraz program powinien się kompilować.


>> Korzystanie z aplikacji:

Do korzystania z aplikacji wystarczy myszka. Po programie sterujemy używając lewego przycisku myszy.
Wybieramy odpowiednie przyciski lub odpowiednie figury oraz pole, na które chcemy ją przesunąć.
Poprawność rozwiązanego zadania widzimy od razu po wykonaniu ruchu.
W pozycji możemy dowolnie przesuwać figury, zgodnie z zasadami szachów, aby przeanalizować pozycję.