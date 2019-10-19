/* New Land */

/* Autor: Piotr Szczygieł, Podrzecze (Małopolska) */
/* Początek: Sierpień 2014 */

/* Styczeń 2015 - Pierwsza grywalna wersja gry */

/* Pomoc w tworzeniu gry: */
/* Kamil Poręba - pomoc w tworzeniu lokacji, fabuły, grafiki oraz innych rzeczy */
/* Arkadiusz Kania, Mateusz Wieczorek - pomoc w tworzeniu lokacji */
/* Emil Mordarski - Dubbing */

/* Gra RPG 2D wykonana przy użyciu bibliotek:*/

/* SFML - Simple Fast Multimedia Library: Podstawowe funkcje graficzne */
/* tmxloader - Biblioteka parsująca pliki lokacji o rozszerzeniu .tmx */
/* gzlib - Biblioteka umożliwiająca kompresje i dekompresje danych */
/* pugixml - Biblioteka obsługi danych w formacie XML */

#include "GameEngine.hpp"

int main()
{
    bool full = false;
    int maxsize = sf::Texture::getMaximumSize();

    if (maxsize < 8192) {
        std::cout << "Ta karta graficzna obsluguje tekstury o niewiekszym rozmiarze niz " << maxsize
                  << ".\nMoga wystepic problemy z wyswietlaniem niektorych tekstur.\nPotrzebny rozmiar to 8192.\n\n"
                  << std::endl;
    } else {
        std::cout << "Karta graficzna obsluguje maksymalny rozmiar tekstur " << maxsize
                  << ".\nWszystko powinno dzialac dobrze.\n\n";
    }

    std::cout << "Czy uruchomic gre w trybie pelnoekranowym?\n(t/n)";
    std::string odp;
    std::cin >> odp;
    if (odp[0] == 't' || odp[0] == 'T')
        full = true;

    GameEngine::Initialize(640, 480, "New Land - Gra na Piknik Naukowy", full, "plaza");
    int returnCode = GameEngine::Run();

    return 0;
}
