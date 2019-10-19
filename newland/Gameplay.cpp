#include "Gameplay.hpp"
#include "Controls.hpp"
#include "Game.hpp"
#include "Pickup.hpp"
#include "Sound.hpp"
#include <string>

std::wstring Gameplay::lastObject;
std::wstring Gameplay::straznikTekst[6];

Gameplay::Gameplay()
{
    straznikTekst[0] = L"Mam nadzieję że nic nie kombinujesz!";
    straznikTekst[1] = L"Ach jak ja kocham to miasto!";
    straznikTekst[2] = L"Przestępcy miejcie się na baczności!";
    straznikTekst[3] = L"Słyszałeś że kupcy na farmie oferują niższe ceny?\nMuszę się kiedyś tam wybrać!";
    straznikTekst[4] = L"Gdzie się ukrywa ten zabójca, może wkrótce się pokaże!";
    straznikTekst[5] = L"Czekam aż skończy się moja służba!";
}

// Procedura obslugujaca interakcje gracza ze swiatem gry
void Gameplay::Interaction()
{
    bool trigger = false;
    UserInterface::Clear();

    // Wyswietlenie ekwipunku
    if (Controls::IsPressed(Controls::Inventory))
        UserInterface::ShowInventory();

    std::vector<tmx::MapObject>* triggers = Maps::currentLocation->GetTriggers();
    std::vector<Npc*>* npcs = Maps::currentLocation->GetNpcs();

    if (triggers != NULL)
        for (int i = 0; i < triggers->size(); i++)
            // Jezeli gracz stoi na obiekcie wywolujacym - przekazujemy dzialanie funkcji Trigger z parametrem danego
            // obiektu
            if (Game::player.GetCollisionPoints().intersects(triggers->at(i).getAABB()))
                Trigger(&triggers->at(i)), trigger = true;

    if (npcs != NULL)
        for (int i = 0; i < npcs->size(); i++) {
            if (npcs->at(i)->player.talkable) {
                npcs->at(i)->interaction_message = false;
                // Jezeli gracz stoi przy danym NPC - przekazujemy dzialanie funkcji Trigger z parametrem danego NPC
                if (npcs->at(i)->GetInteractionRect().intersects(Game::player.GetCollisionPoints())) {
                    if (!npcs->at(i)->interaction) {
                        npcs->at(i)->interaction_message = true;
                        if (Controls::IsPressed(Controls::Interaction))
                            npcs->at(i)->interaction = true;
                    } else {
                        Trigger(npcs->at(i)->GetName()), trigger = true;
                        // Fix przed rozmawianiem z dwoma npcami naraz. Rozmawiam z tym ktory jest wyzej w tabeli
                        break;
                    }
                } else {
                    npcs->at(i)->interaction = false;
                }
            }
        }

    for (int i = 0; i < Maps::currentLocation->pickups.size(); i++) {
        if (Maps::currentLocation->pickups[i]->intersects(Game::player.GetCollisionPoints())) {
            UserInterface::InteractionPopup(L"Podnieś przedmiot",
                sf::Vector2f(Maps::currentLocation->pickups[i]->itemSprite.getPosition().x + 16,
                    Maps::currentLocation->pickups[i]->itemSprite.getPosition().y + 16),
                UserInterface::Kwadrat, 0.5f, sf::Color::Yellow, true);
            if (Controls::IsPressed(Controls::Pickup)) {
                Maps::currentLocation->pickups[i]->PickupItem();
                Maps::currentLocation->pickups.erase(Maps::currentLocation->pickups.begin() + i);
            }
        }
    }

    if (!trigger)
        lastObject = L"";

    if (lastObject != L"piano") {
        Sound::pianino.stop();
        Game::bgMusic.setVolume(25);
    }
}

// Funkcja przekazujaca dzialania do poszczegolnych procedur dla kazdego swiata
void Gameplay::Trigger(tmx::MapObject* object)
{
    std::string location = Maps::currentLocation->GetParent();

    if (location == "galicja")
        Galicja(pugi::as_wide(object->getName()), Maps::currentLocation->GetName(), object->getCentre());
    else if (location == "farma")
        Farma(pugi::as_wide(object->getName()), Maps::currentLocation->GetName(), object->getCentre());
    else if (location == "pustynia")
        Pustynia(pugi::as_wide(object->getName()), Maps::currentLocation->GetName(), object->getCentre());
    else if (location == "plaza")
        Plaza(pugi::as_wide(object->getName()), Maps::currentLocation->GetName(), object->getCentre());

    lastObject = pugi::as_wide(object->getName());
}

// j.w
void Gameplay::Trigger(std::wstring objName)
{
    std::string location = Maps::currentLocation->GetParent();
    Npc* npc = Maps::currentLocation->GetNpc(objName);

    if (npc != NULL)
        npc->interaction = true;
    else
        return;

    if (location == "galicja")
        Galicja(objName, Maps::currentLocation->GetName(), npc->player.GetPosition());
    else if (location == "farma")
        Farma(objName, Maps::currentLocation->GetName(), npc->player.GetPosition());
    else if (location == "pustynia")
        Pustynia(objName, Maps::currentLocation->GetName(), npc->player.GetPosition());
    else if (location == "plaza")
        Plaza(objName, Maps::currentLocation->GetName(), npc->player.GetPosition());

    lastObject = objName;
}

void Gameplay::Farma(std::wstring objName, std::string location, sf::Vector2f objCenterPos)
{
    Npc* npc = Maps::currentLocation->GetNpc(objName);

    if (location == "farma") {
        if (objName == L"galicja") {
            if (Controls::IsPressed(Controls::Up))
                Game::LoadMap("galicja", "farma");
        } else if (objName == L"Handlarz") {
            // ui.ShowInventory();
            UserInterface::MessageWait(L"Witaj!\nNaciśnij odpowiedni klawisz aby dokonać zakupu:\n\n[1] Podręczny "
                                       L"zegarek - 50$\n[2] - Mikstura życia - 20$",
                npc, 0.f);

            if (Controls::IsPressed(Controls::Choose1)) {
                if (Inventory::money >= 50) {
                    if (Inventory::GetItemInfo("zegarek").count > 0) {
                        UserInterface::Popup(L"Posiadasz już zegarek!", &Game::player, 1.0f, sf::Color::Red, true);
                    } else {
                        Inventory::AddItem("zegarek", false);
                        Inventory::money -= 50;
                        UserInterface::Popup(L"Kupiłeś zegarek za 50$", &Game::player, 4.0f, sf::Color::White, true);
                    }
                } else {
                    UserInterface::Popup(
                        L"Niewystarczająca ilość pieniędzy!", &Game::player, 3.0f, sf::Color::Red, true);
                }
            } else if (Controls::IsPressed(Controls::Choose2)) {
                if (Inventory::money >= 30) {
                    Inventory::AddItem("hp_pot", false);
                    Inventory::money -= 30;
                    UserInterface::Popup(L"Kupiłeś miksture za 30$", &Game::player, 4.0f, sf::Color::White, true);
                } else {
                    UserInterface::Popup(
                        L"Niewystarczająca ilość pieniędzy!", &Game::player, 3.0f, sf::Color::Red, true);
                }
            }
        }
    }
}

void Gameplay::Pustynia(std::wstring objName, std::string location, sf::Vector2f objCenterPos)
{
    Npc* npc = Maps::currentLocation->GetNpc(objName);

    if (location == "pustynia") {
        if (objName == L"galicja") {
            if (Controls::IsPressed(Controls::Right)) {
                if (GameplayInfo::GetInfo("cutscenka", "pustynia").count == 0) {
                    GameplayInfo::AddInfo("cutscenka", "pustynia");
                    UserInterface::Cutscenka("pustynia");
                }
                Game::LoadMap("galicja", "pustynia");
            }
        } else if (objName == L"plaza") {
            if (Controls::IsPressed(Controls::Left))
                Game::LoadMap("plaza", "pustynia");
        } else if (objName == L"lochy") {
            if (Controls::IsPressed(Controls::Up)) {
                if (GameplayInfo::GetInfo("cutscenka", "lochy").count == 0) {
                    GameplayInfo::AddInfo("cutscenka", "lochy");
                    UserInterface::Cutscenka("lochy");
                }
                Game::LoadMap("lochy");
            }
        } else if (objName == L"skrzynia") {
            if (GameplayInfo::GetInfo("skrzynia", "zabrano").count == 0) {
                UserInterface::InteractionPopup(
                    L"Weź przedmioty ze skrzynki", objCenterPos, UserInterface::Kwadrat, 0.5f, sf::Color::Yellow, true);
                if (Controls::IsPressed(Controls::Pickup)) {
                    GameplayInfo::SetInfo("skrzynia", "zabrano", 1);
                    Inventory::AddItem("patyk");
                    Inventory::AddItem("hp_pot");
                    Inventory::money += 50;
                    UserInterface::Popup(
                        L"Podniesiono:\n- Patyk\n- Mikstura\n- 50$", &Game::player, 4.0f, sf::Color::White);
                }
            }
        } else if (objName == L"skrzynia_roza") {
            if (GameplayInfo::GetInfo("roza", "quest").count == 1) {
                if (GameplayInfo::GetInfo("skrzynia_roza", "przedmioty").count == 0) {
                    UserInterface::InteractionPopup(
                        L"Zabierz skarby", objCenterPos, UserInterface::Kwadrat, 0.5f, sf::Color::Yellow, true);
                    if (Controls::IsPressed(Controls::Pickup)) {
                        GameplayInfo::SetInfo("skrzynia_roza", "przedmioty", 1);
                        Inventory::AddItem("lekarstwa");
                        Inventory::money += 100;
                        UserInterface::Popup(
                            L"Podniesiono:\n - Lekarstwa\n - 100$", &Game::player, 4.0f, sf::Color::White);
                    }
                } else {
                    if (Controls::IsPressed(Controls::Pickup)) {
                        UserInterface::Popup(L"Zabrałeś już skarby!", objCenterPos, 2.0f, sf::Color::Red, true);
                    }
                }
            }
        } else if (objName == L"zaklecie") {
            if (!GameplayInfo::GetInfo("grota", "otworzona").count && Inventory::GetItemInfo("zaklecie").count > 0) {
                Inventory::DeleteItem("zaklecie");
                GameplayInfo::SetInfo("grota", "otworzona", 1);

                for (int i = 0; i < Maps::GetLocation("pustynia")->GetObjects()->size(); i++) {
                    if (Maps::GetLocation("pustynia")->GetObjects()->at(i).getName() == "grota1"
                        || Maps::GetLocation("pustynia")->GetObjects()->at(i).getName() == "grota2") {
                        Maps::GetLocation("pustynia")->GetObjects()->at(i).move(sf::Vector2f(-64.f, 0.f));
                    }
                }
            }
        } else if (objName == L"Wygnaniec") {
            if (lastObject != L"Wygnaniec")
                GameplayInfo::SetInfo("wygnaniec", "rozmowa", 0);

            if (GameplayInfo::GetInfo("wygnaniec", "quest").count == 0) {
                int rozmowa = GameplayInfo::GetInfo("wygnaniec", "rozmowa").count;

                if (rozmowa == 0) {
                    UserInterface::MessageWait(L"Witaj podróżniku jestem ciężko chory i potrzebuję Twojej "
                                               L"Pomocy!\n\n[1] - Co mam zrobić?       [2] - Nie mam na to czasu!",
                        npc, 0.f);
                    if (Controls::IsPressed(Controls::Choose1)) {
                        GameplayInfo::SetInfo("wygnaniec", "rozmowa", 2);
                    } else if (Controls::IsPressed(Controls::Choose2)) {
                        GameplayInfo::SetInfo("wygnaniec", "rozmowa", 1);
                    }
                }

                // nie mam na to czasu
                else if (rozmowa == 1) {
                    UserInterface::MessageWait(L"Podróżniku proszę nie zapomnij o mnie i wróć kiedy znajdziesz "
                                               L"czas.\nPozostało mi niewiele czasu...",
                        npc, 0.f);
                }

                // co mam zrobic
                else if (rozmowa == 2) {
                    UserInterface::MessageWait(
                        L"Kilka lat temu za drobne przewinienie wyrzucono mnie z miasta\ni zostałem skazany na "
                        L"banicję. Czy możesz zabrać tę butelkę\ni napełnić ją wodą z miejscowej studni? Już od kilku "
                        L"dni nic nie piłem.\n\n[1] - Dlaczego Cię wygnano?\n[2] - Chętnie Ci pomogę!\n[3] - Nie mam "
                        L"na to czasu!",
                        npc, 0.f);
                    if (Controls::IsPressed(Controls::Choose1)) {
                        GameplayInfo::SetInfo("wygnaniec", "rozmowa", 3);
                    } else if (Controls::IsPressed(Controls::Choose2)) {
                        GameplayInfo::SetInfo("wygnaniec", "rozmowa", 6);
                    } else if (Controls::IsPressed(Controls::Choose3)) {
                        GameplayInfo::SetInfo("wygnaniec", "rozmowa", 1);
                    }
                } else if (rozmowa == 3) {
                    UserInterface::MessageWait(
                        L"A więc, jeżeli już musisz wiedzieć kilka lat temu, gdy ta kraina była jedną\nz "
                        L"najspokojniejszych na całym świecie a mieszkańcy New Land nie\nmartwili się potworami, "
                        L"pracowałem w gospodzie. Pewnej nocy gdy\ndozorca jeszcze spał zakradłem się do jego piwnicy "
                        L"i zjadłem odrobinę\nprzysmaków jakie zostały sprowadzone z innych krain.\n\n[ENTER] - Dalej",
                        npc, 0.f);
                    if (Controls::IsPressed(Controls::Accept)) {
                        GameplayInfo::SetInfo("wygnaniec", "rozmowa", 4);
                    }
                } else if (rozmowa == 4) {
                    UserInterface::MessageWait(
                        L"Nie spodziewałem się że mój spisek wyjdzie na jaw i co gorsza\nże zostanę oskarżony o "
                        L"przywoływanie duchów. Jak później się\ndowiedziałem gospodarz przechowywał zakazane relikwie "
                        L"dla\njakieś znachorki i całą winę zrzucił na mnie.\n\n[1] - To naprawdę niesprawiedliwe "
                        L"oczywiście że ci pomogę!\n[2] - Nic z tego nie będe pomagać przestępcy!",
                        npc, 0.f);
                    if (Controls::IsPressed(Controls::Choose1)) {
                        GameplayInfo::SetInfo("wygnaniec", "rozmowa", 6);
                    } else if (Controls::IsPressed(Controls::Choose2)) {
                        GameplayInfo::SetInfo("wygnaniec", "rozmowa", 5);
                    }
                } else if (rozmowa == 5) {
                    UserInterface::MessageWait(
                        L"Nie sądziłem że nie będziesz taki niewyrozumiały.\nNaprawdę na ciebie liczyłem...", npc, 0.f);
                } else if (rozmowa == 6) {
                    UserInterface::MessageWait(L"Dobrze a więc weź tą butelkę i napełnij ją wodą w miejscowej "
                                               L"studni.\nWprawdzie nie jestem już teraz zbyt bogaty,\nale postaram "
                                               L"się odwdzięczyć.\n\n[ENTER] - Dobrze, ide napełnić butelke!",
                        npc, 0.f);
                    if (Controls::IsPressed(Controls::Accept)) {
                        GameplayInfo::SetInfo("wygnaniec", "rozmowa", 7);
                    }
                } else if (rozmowa == 7) {
                    GameplayInfo::SetInfo("wygnaniec", "quest", 1);
                    Inventory::AddItem("butelka_pusta");
                    UserInterface::Popup(L"Zadanie rozpoczęte!", &Game::player, 4.f, sf::Color::Green);
                }
            } else if (GameplayInfo::GetInfo("wygnaniec", "quest").count == 1) {
                if (Inventory::GetItemInfo("butelka_pelna").count < 1) {
                    if (lastObject != L"Wygnaniec")
                        UserInterface::Popup(L"Proszę przynieś mi jak najszybciej te wodę!", &npc->player);
                } else {
                    Inventory::DeleteItem("butelka_pelna");
                    UserInterface::Popup(L"Dziękuje ci podróżniku!\nPrzyjmij to w nagrodę!", &npc->player, 5.f);
                    Inventory::money += 30;
                    Inventory::AddItem("hp_pot");
                    UserInterface::Popup(L"Otrzymano:\n- Mikstura\n- 30$", &Game::player, 4.f, sf::Color::Green);
                    GameplayInfo::SetInfo("wygnaniec", "quest", 2);
                }
            } else if (GameplayInfo::GetInfo("wygnaniec", "quest").count == 2) {
                if (lastObject != L"Wygnaniec") {
                    UserInterface::Popup(L"Witaj!", &npc->player);
                }
            }
        }
    } else if (location == "lochy") {
        if (objName == L"pustynia") {
            if (Controls::IsPressed(Controls::Down))
                Game::LoadMap("pustynia", "lochy");
        } else if (objName == L"grobowiec") {
            if (GameplayInfo::GetInfo("znachorka", "quest").count == 1) {
                if (GameplayInfo::GetInfo("grobowiec", "kosci").count == 0) {
                    UserInterface::InteractionPopup(
                        L"Zabierz kości", objCenterPos, UserInterface::Kwadrat, 0.5f, sf::Color::Yellow, true);
                    if (Controls::IsPressed(Controls::Pickup)) {
                        GameplayInfo::SetInfo("grobowiec", "kosci", 1);
                        Inventory::AddItem("kosci");
                        UserInterface::Popup(L"Podniesiono:\n - Kości", &Game::player, 4.0f, sf::Color::White);
                    }
                } else {
                    if (Controls::IsPressed(Controls::Pickup)) {
                        UserInterface::Popup(L"Zabrałeś już kości!", objCenterPos, 2.0f, sf::Color::Red, true);
                    }
                }
            }
        }
    }
}

void Gameplay::Plaza(std::wstring objName, std::string location, sf::Vector2f objCenterPos)
{
    Npc* npc = Maps::currentLocation->GetNpc(objName);

    if (location == "plaza") {
        if (objName == L"pustynia") {
            if (Controls::IsPressed(Controls::Right)) {
                if (GameplayInfo::GetInfo("cutscenka", "plaza").count == 0) {
                    GameplayInfo::AddInfo("cutscenka", "plaza");
                    UserInterface::Cutscenka("plaza");
                }
                Game::LoadMap("pustynia", "plaza");
            }
        } else if (objName == L"sterowanie") {
            if (GameplayInfo::GetInfo("sterowanie", "zobaczono").count == 0) {
                UserInterface::Message(
                    L"Witaj! Czy chcesz zobaczyć konfigurację sterowania?\n\n[ENTER] - Tak\n[ESCAPE] - Pomiń");
                if (Controls::IsPressed(Controls::Accept)) {
                    UserInterface::Sterowanie();
                    GameplayInfo::SetInfo("sterowanie", "zobaczono", 1);
                } else if (Controls::IsPressed(Controls::Back)) {
                    GameplayInfo::SetInfo("sterowanie", "zobaczono", 1);
                }
            }
        }
    }
}

// Triggery dla lokacji galicja
void Gameplay::Galicja(std::wstring objName, std::string location, sf::Vector2f objCenterPos)
{
    Npc* npc = Maps::currentLocation->GetNpc(objName);

    if (location == "galicja") {

        if (objName == L"farma") {
            if (Controls::IsPressed(Controls::Down))
                Game::LoadMap("farma");
        } else if (objName == L"pustynia") {
            if (Controls::IsPressed(Controls::Left))
                Game::LoadMap("pustynia", "galicja");
        } else if (objName == L"karczma") {
            if (Controls::IsPressed(Controls::Up)) {
                if (GameplayInfo::GetInfo("cutscenka", "karczma").count == 0) {
                    GameplayInfo::AddInfo("cutscenka", "karczma");
                    UserInterface::Cutscenka("karczma");
                }
                Game::LoadMap("karczma");
            }
        } else if (objName == L"domek1") {
            if (Controls::IsPressed(Controls::Up))
                Game::LoadMap("domek1");
        } else if (objName == L"domek2") {
            if (Controls::IsPressed(Controls::Up))
                Game::LoadMap("domek2");
        } else if (objName == L"fontanna") {
            if (GameplayInfo::GetInfo("wygnaniec", "quest").count == 1) {
                UserInterface::InteractionPopup(
                    L"Napełnij butelke", objCenterPos, UserInterface::Kwadrat, 0.5f, sf::Color::Yellow, true);
                if (Controls::IsPressed(Controls::Pickup)) {
                    Inventory::DeleteItem("butelka_pusta");
                    Inventory::AddItem("butelka_pelna");
                    UserInterface::Popup(L"Napełniono butelke!", &Game::player, 4.0f, sf::Color::White);
                }
            }
        } else if (objName == L"Adelard" || objName == L"Galien") {
            if (lastObject != L"Adelard" && lastObject != L"Galien") {
                int tekst = rand() % 6;
                UserInterface::Popup(straznikTekst[tekst], &npc->player, 3.0f, sf::Color::White, true);
            }
        } else if (objName == L"Kapłan") {
            UserInterface::MessageWait(
                L"Witaj, mogę Ciebie całkowicie uleczyć za drobną opłatą...\n\n[1] - Skorzystaj z usługi (50$)", npc,
                0.f);
            if (Controls::IsPressed(Controls::Choose1)) {
                if (Game::player.hp == Game::player.maxHp) {
                    UserInterface::Popup(L"Posiadasz już maksymalne zdrowie!", &npc->player, 4.f);
                } else {
                    if (Inventory::money >= 50) {
                        Inventory::money -= 50;
                        Game::player.hp = Game::player.maxHp;
                        UserInterface::Popup(L"Uleczono!", &Game::player, 3.0f, sf::Color::Green);
                        Sound::leczenie.play();
                    } else {
                        UserInterface::Popup(
                            L"Niewystarczająca ilość pieniędzy!", &Game::player, 3.0f, sf::Color::Red, true);
                    }
                }
            }
        } else if (objName == L"Znachorka") {
            if (GameplayInfo::GetInfo("znachorka", "quest").count == 0) {
                if (GameplayInfo::GetInfo("grota", "otworzona").count == 1) {
                    if (lastObject != L"Znachorka")
                        GameplayInfo::SetInfo("znachorka", "rozmowa", 0);

                    int rozmowa = GameplayInfo::GetInfo("znachorka", "rozmowa").count;

                    if (rozmowa == 0) {
                        UserInterface::MessageWait(L"Czy możesz zrobić dla mnie przysługę młody chłopcze?\n\n[1] - "
                                                   L"Tak, O co chodzi?    [2] - Nie mam na to czasu!",
                            npc, 0.f);
                        if (Controls::IsPressed(Controls::Choose1)) {
                            GameplayInfo::SetInfo("znachorka", "rozmowa", 2);
                        } else if (Controls::IsPressed(Controls::Choose2)) {
                            GameplayInfo::SetInfo("znachorka", "rozmowa", 1);
                        }
                    }

                    // nie mam na to czasu
                    else if (rozmowa == 1) {
                        UserInterface::MessageWait(L"Och niewdzięczniku! Gdzie są prawdziwi bohaterowie\nna tym "
                                                   L"świecie!\n\nMam nadzieję że jeszcze zmienisz zdanie!",
                            npc, 0.f);
                    }

                    // tak o co chodzi
                    else if (rozmowa == 2) {
                        UserInterface::MessageWait(
                            L"Wyglądasz mi na silnego wojownika.\nPotrzebuję właśnie kogoś takiego jak ty.\nJestem "
                            L"znachorką, zajmuję się leczeniem ludzi, ale\nw dzisiejszych niegodziwych czasach ludzie "
                            L"oczekują\node mnie więcej… Potrzebuję kośći zmarłego.\n\n[1] - Po co ci to "
                            L"cholerstwo?\n[2] - Gdzie mogę coś takiego znaleźć?",
                            npc, 0.f);
                        if (Controls::IsPressed(Controls::Choose1)) {
                            GameplayInfo::SetInfo("znachorka", "rozmowa", 3);
                        } else if (Controls::IsPressed(Controls::Choose2)) {
                            GameplayInfo::SetInfo("znachorka", "rozmowa", 4);
                        }
                    }

                    // po co ci to cholerstwo
                    else if (rozmowa == 3) {
                        UserInterface::MessageWait(
                            L"Jeśli już musisz wiedzieć… Do stworzenia mikstury regeneracji.\nBez niej moi słudzy "
                            L"umrą\n\n[1] - Słudzy? Co masz na myśli?\n[2] - Dobra biore się za to!",
                            npc, 0.f);
                        if (Controls::IsPressed(Controls::Choose1)) {
                            GameplayInfo::SetInfo("znachorka", "rozmowa", 5);
                        } else if (Controls::IsPressed(Controls::Choose2)) {
                            GameplayInfo::SetInfo("znachorka", "rozmowa", 4);
                        }
                    }

                    // gdzie moge cos takiego znalezc
                    else if (rozmowa == 4) {
                        UserInterface::MessageWait(
                            L"W opuszczonej jaskini na pustyni znajduje się wejście do groty.\nTam szukaj kości przy "
                            L"grobowcu.\n\n [1] - A więc ide przeszukać jaskinię!",
                            npc, 0.f);
                        if (Controls::IsPressed(Controls::Choose1)) {
                            GameplayInfo::SetInfo("znachorka", "rozmowa", 6);
                        }
                    }

                    // sludzy co masz na mysli
                    else if (rozmowa == 5) {
                        UserInterface::MessageWait(
                            L"Nie twoja sprawa. Pomożesz mi czy nie?\n\n[1] - Nie, obawiam się że w tej sytuacji nie "
                            L"mogę ci pomóc.\n[2] - Dobra biorę się za to!",
                            npc, 0.f);
                        if (Controls::IsPressed(Controls::Choose1)) {
                            GameplayInfo::SetInfo("znachorka", "rozmowa", -1);
                        } else if (Controls::IsPressed(Controls::Choose2)) {
                            GameplayInfo::SetInfo("znachorka", "rozmowa", 4);
                        }
                    }

                    // a wiec ide przeszukac jaskinie
                    else if (rozmowa == 6) {
                        UserInterface::Popup(L"Bezpiecznej drogi podróżniku!", &npc->player, 4.f);
                        GameplayInfo::SetInfo("znachorka", "quest", 1);
                        UserInterface::Popup(L"Zadanie rozpoczęte!", &Game::player, 4.f, sf::Color::Green);
                        GameplayInfo::SetInfo("znachorka", "rozmowa", -1);
                    }
                } else {
                    UserInterface::Popup(
                        L"Kiedy wreszcie ktoś odnajdzie wejście\ndo tej przeklętej groty!", &npc->player, 4.f);
                }
            } else if (GameplayInfo::GetInfo("znachorka", "quest").count == 1) {
                // przyjscie po zaakceptowaniu zadania
                if (Inventory::GetItemInfo("kosci").count > 0) {
                    UserInterface::MessageWait(L"Wiedziałam że mogę na tobie polegać!\nW nagrode dostaniesz troche "
                                               L"monet i coś co może\nci się przydać!\n\n[ENTER] - Akceptuj",
                        npc, 0.f);
                    if (Controls::IsPressed(Controls::Accept)) {
                        Inventory::money += 50;
                        Inventory::AddItem("zegarek");
                        Inventory::AddItem("hp_pot");
                        UserInterface::Popup(
                            L"Otrzymano:\n- Zegarek\n- Mikstura\n- 50$", &Game::player, 4.0f, sf::Color::White);
                        GameplayInfo::SetInfo("znachorka", "quest", 2);
                        Inventory::DeleteItem("kosci");
                    }
                } else {
                    if (lastObject != L"Znachorka")
                        UserInterface::Popup(
                            L"Wroć kiedy znajdziesz kości!\nPotrzebuje szybko zrobić miksture!", &npc->player, 3.0f);
                }
            } else if (GameplayInfo::GetInfo("znachorka", "quest").count == 2) {
                if (lastObject != L"Znachorka")
                    UserInterface::Popup(L"Witaj dobry człowieku!", &npc->player);
            }
        } else if (objName == L"Handlarz") {
            // ui.ShowInventory();
            UserInterface::MessageWait(
                L"Witaj!\nNaciśnij odpowiedni klawisz aby dokonać zakupu:\n\n[1] Przepustka - Most 100$", npc, 0.f);

            if (Controls::IsPressed(Controls::Choose1)) {
                if (Inventory::money >= 100) {
                    if (Inventory::GetItemInfo("przepustka").count > 0) {
                        UserInterface::Popup(L"Posiadasz już przepustke!", &Game::player, 3.f, sf::Color::Red, true);
                    } else {
                        Inventory::AddItem("przepustka");
                        Inventory::money -= 100;
                        UserInterface::Popup(L"Kupiłeś przepustkę za 100$", &Game::player, 4.f, sf::Color::White, true);
                    }
                } else {
                    UserInterface::Popup(
                        L"Niewystarczająca ilość pieniędzy!", &Game::player, 3.0f, sf::Color::Red, true);
                }
            }
        } else if (objName == L"Strażnik") {
            if (GameplayInfo::GetInfo("beczki", "przesuniete").count == 0) {
                if (lastObject != L"Strażnik")
                    GameplayInfo::SetInfo("straznik", "rozmowa", 0);

                int rozmowa = GameplayInfo::GetInfo("straznik", "rozmowa").count;

                if (rozmowa == 0) {
                    UserInterface::MessageWait(L"Witaj!\n\nNaciśnij [ENTER], aby okazać przepustkę!", npc, 0.f);
                    if (Controls::IsPressed(Controls::Accept)) {
                        GameplayInfo::SetInfo("straznik", "rozmowa", 1);
                    }
                }
                if (rozmowa == 1) {
                    if (Inventory::GetItemInfo("przepustka").count > 0) {

                        for (int i = 0; i < Maps::currentLocation->GetObjects()->size(); i++) {
                            if (Maps::currentLocation->GetObjects()->at(i).getName() == "beczka1"
                                || Maps::currentLocation->GetObjects()->at(i).getName() == "beczka2") {
                                Maps::currentLocation->GetObjects()->at(i).move(sf::Vector2f(0, -64.0f));
                            }
                        }
                        GameplayInfo::AddInfo("beczki", "przesuniete");
                        Inventory::DeleteItem("przepustka");

                        UserInterface::Popup(L"Przechodź śmiało!", &npc->player, 4.f, sf::Color::Yellow);
                    } else {
                        UserInterface::Popup(L"Potrzebujesz przepustki!", &npc->player, 4.f, sf::Color::Red);
                    }
                }
            } else {
                if (lastObject != L"Strażnik")
                    UserInterface::Popup(
                        L"Nie zawracaj mi głowy, pilnuje przejścia.", &npc->player, 1.5f, sf::Color::White, true);
            }
        }
    } else if (location == "karczma") {
        if (objName == L"quit") {
            if (Controls::IsPressed(Controls::Down))
                Game::LoadMap("galicja", "karczma");
        } else if (objName == L"Róża") {
            if (GameplayInfo::GetInfo("roza", "quest").count == 0) {
                if (lastObject != L"Róża")
                    GameplayInfo::SetInfo("roza", "rozmowa", 0);

                int rozmowa = GameplayInfo::GetInfo("roza", "rozmowa").count;

                if (rozmowa == 0) {
                    UserInterface::MessageWait(L"Witaj chłopcze czy możesz mi pomóc i dostarczyć coś ważnego?\n\n[1] - "
                                               L"Tak o co chodzi?\n[2] - Nie, nie jestem chłopcem na posyłki!",
                        npc, 0.f);
                    if (Controls::IsPressed(Controls::Choose1)) {
                        GameplayInfo::SetInfo("roza", "rozmowa", 2);
                    } else if (Controls::IsPressed(Controls::Choose2)) {
                        GameplayInfo::SetInfo("roza", "rozmowa", 1);
                    }
                }

                else if (rozmowa == 1) {
                    UserInterface::MessageWait(L"Szkoda, chciałam Ci się odwdzięczyć...\n\n[1] - W takim razie "
                                               L"zmieniłem zdanie!\n[2] - Nic z tego, żegnaj!",
                        npc, 0.f);

                    if (Controls::IsPressed(Controls::Choose1)) {
                        GameplayInfo::SetInfo("roza", "rozmowa", 2);
                    } else if (Controls::IsPressed(Controls::Choose2)) {
                        GameplayInfo::SetInfo("roza", "rozmowa", -1);
                    }
                }

                else if (rozmowa == 2) {
                    UserInterface::MessageWait(
                        L"A więc kilka tygodni mój mąż udał się na pustynie w poszukiwaniu\nskarbów, lecz nigdy nie "
                        L"wrócił. Proszę weź ten klucz, przed jego\nśmiercią umówiliśmy się że ukryje tam wszystkie "
                        L"skarby, wśród nich\npowinny być także leki, których dla mnie szukał. Proszę weź "
                        L"wszystko\ndla siebie i wróc tutaj a odkupię od ciebie te lekarstwa. Bardzo mi na\ntym "
                        L"żależy. Skarby nie są dla mnie tak ważne...\n\n[1] - Jasne że ci pomogę!      [2] - Przykro "
                        L"mi, ale nie mam czasu.",
                        npc, 0.f);

                    if (Controls::IsPressed(Controls::Choose1)) {
                        GameplayInfo::SetInfo("roza", "rozmowa", 3);
                    } else if (Controls::IsPressed(Controls::Choose2)) {
                        GameplayInfo::SetInfo("roza", "rozmowa", 1);
                    }
                }

                else if (rozmowa == 3) {
                    UserInterface::Popup(L"Powodzenia podróżniku, spokojnej podróży!", &npc->player, 3.f);
                    GameplayInfo::SetInfo("roza", "quest", 1);
                    UserInterface::Popup(L"Zadanie rozpoczęte!", &Game::player, 4.f, sf::Color::Green);
                }

            } else if (GameplayInfo::GetInfo("roza", "quest").count == 1) {
                // przyjscie po zaakceptowaniu zadania
                if (Inventory::GetItemInfo("lekarstwa").count > 0) {
                    UserInterface::MessageWait(
                        L"Dziękuje!\nTo dzięki takim jak ty New Land jest\ntak wspaniałą krainą. Bardzo mi pomogłeś.",
                        npc, 5.f);
                    Inventory::DeleteItem("lekarstwa");
                    GameplayInfo::SetInfo("roza", "quest", 2);
                } else {
                    if (lastObject != L"Róża")
                        UserInterface::Popup(L"Wroć kiedy znajdziesz lekarstwa!", &npc->player, 3.0f);
                }
            } else if (GameplayInfo::GetInfo("roza", "quest").count == 2) {
                if (lastObject != L"Róża")
                    UserInterface::Popup(L"Cześć, co u Ciebie?!", &npc->player);
            }
        }
    } else if (location == "domek1") {

        if (objName == L"quit") {
            if (Controls::IsPressed(Controls::Down))
                Game::LoadMap("galicja", "domek1");
        } else if (objName == L"Zabójca") {
            if (GameplayInfo::GetInfo("zabojca", "quest").count == 0) {
                if (lastObject != L"Zabójca")
                    GameplayInfo::SetInfo("zabojca", "rozmowa", 0);

                int rozmowa = GameplayInfo::GetInfo("zabojca", "rozmowa").count;

                if (rozmowa == 0) {
                    UserInterface::MessageWait(
                        L"Witaj! Potrzebuje czaszek szkieletów do uzupełnienia mojej wystawy...\nW zamian otrzymasz "
                        L"nagrodę...\n\n[1] - Okej, mogę spróbować!     [2] - Nie mam czasu na takie zabawy.",
                        npc, 0.f);
                    if (Controls::IsPressed(Controls::Choose1)) {
                        GameplayInfo::SetInfo("zabojca", "rozmowa", 2);
                    } else if (Controls::IsPressed(Controls::Choose2)) {
                        GameplayInfo::SetInfo("zabojca", "rozmowa", 1);
                    }
                }

                // nie mam na to czasu
                else if (rozmowa == 1) {
                    UserInterface::MessageWait(L"Dobra, w takim razie wyjdź zanim stanie ci się krzywda!", npc, 0.f);
                }

                // tak o co chodzi
                else if (rozmowa == 2) {
                    UserInterface::MessageWait(
                        L"Dobra słuchaj... Masz tu zaklęcie które wypowiesz stojąc między dwoma\nfilarami ze słońcem i "
                        L"księżycem. Znajdują się one obok portu na pustyni.\nPowinna ci się ukazać grota do której "
                        L"wejdziesz.\nW środku znajdziesz szkielety z których możesz otrzymać czaszki.\nPotrzebuje 5 "
                        L"takich czaszek\n[1] - Dobra, rozumiem, biorę się do roboty\n[2] - To jednak nie dla mnie",
                        npc, 0.f);
                    if (Controls::IsPressed(Controls::Choose1)) {
                        GameplayInfo::SetInfo("zabojca", "rozmowa", 3);
                    } else if (Controls::IsPressed(Controls::Choose2)) {
                        GameplayInfo::SetInfo("zabojca", "rozmowa", 1);
                    }
                }

                // po co ci to cholerstwo
                else if (rozmowa == 3) {
                    UserInterface::Popup(L"Powodzenia życzę!", &npc->player, 4.f);
                    GameplayInfo::SetInfo("zabojca", "quest", 1);
                    UserInterface::Popup(
                        L"Zadanie rozpoczęte!\nOtrzymano:\n - Stare zaklęcie", &Game::player, 4.f, sf::Color::Green);
                    GameplayInfo::SetInfo("zabojca", "rozmowa", -1);
                    Inventory::AddItem("zaklecie");
                }

            } else if (GameplayInfo::GetInfo("zabojca", "quest").count == 1) {
                // przyjscie po zaakceptowaniu zadania
                if (Inventory::GetItemInfo("czaszka").count >= 5) {
                    UserInterface::MessageWait(
                        L"Okej dzięki wielkie. Już ide na piętro, żeby je udekorować.\nMasz tu klucz do skrzynki w "
                        L"mojej sypialni...\nMoże coś ci się sprzyda.\n[ENTER] - Akceptuj",
                        npc, 0.f);
                    if (Controls::IsPressed(Controls::Accept)) {
                        Inventory::AddItem("klucz_zabojca");
                        UserInterface::Popup(
                            L"Otrzymano:\n- Klucz do skrzyni zabójcy", &Game::player, 4.0f, sf::Color::White);
                        GameplayInfo::SetInfo("zabojca", "quest", 2);
                        Inventory::DeleteItem("czaszka", 5);

                        Maps::GetLocation("domek1_pietro")->SetLayerVisible("Kosci", true);
                    }
                } else {
                    if (lastObject != L"Zabójca")
                        UserInterface::Popup(L"Wroć kiedy zbierzesz wszystkie 5 czaszek!", &npc->player, 3.0f);
                }
            } else if (GameplayInfo::GetInfo("zabojca", "quest").count == 2) {
                if (lastObject != L"Zabójca")
                    UserInterface::Popup(L"Czego tu znów szukasz?", &npc->player);
            }
        } else if (objName == L"drabina") {
            if (Controls::IsPressed(Controls::Up))
                Game::LoadMap("domek1_pietro");
        } else if (objName == L"schody") {
            if (Controls::IsPressed(Controls::Left))
                Game::LoadMap("domek1_piwnica");
        } else if (objName == L"skrzynia") {
            if (GameplayInfo::GetInfo("skrzynia_zabojca", "zabrano").count == 0
                && Inventory::GetItemInfo("klucz_zabojca").count > 0) {
                UserInterface::InteractionPopup(
                    L"Weź przedmioty ze skrzynki", objCenterPos, UserInterface::Kwadrat, 0.5f, sf::Color::Yellow, true);
                if (Controls::IsPressed(Controls::Pickup)) {
                    GameplayInfo::SetInfo("skrzynia_zabojca", "zabrano", 1);
                    Inventory::DeleteItem("klucz_zabojca");
                    Inventory::AddItem("miecz");
                    Inventory::AddItem("hp_pot", 2);
                    Inventory::money += 50;
                    UserInterface::Popup(
                        L"Podniesiono:\n- Miecz\n- 2x Mikstura\n- 50$", &Game::player, 4.0f, sf::Color::White);
                }
            }
        }
    } else if (location == "domek1_pietro") {
        if (objName == L"drabina") {
            if (Controls::IsPressed(Controls::Down))
                Game::LoadMap("domek1", "drabina");
        }
    } else if (location == "domek1_piwnica") {
        if (objName == L"schody") {
            if (Controls::IsPressed(Controls::Right))
                Game::LoadMap("domek1", "schody");
        } else if (objName == L"piano") {
            UserInterface::InteractionPopup(
                L"Zagraj na pianinie", objCenterPos, UserInterface::Trojkat, 0.5f, sf::Color::Yellow, true);
            if (Controls::IsPressed(Controls::Interaction)) {
                Sound::pianino.play();
                Game::bgMusic.setVolume(0);
            }
        }
    } else if (location == "domek2") {
        if (objName == L"quit") {
            if (Controls::IsPressed(Controls::Down))
                Game::LoadMap("galicja", "domek2");
        } else if (objName == L"schody") {
            if (Controls::IsPressed(Controls::Up))
                Game::LoadMap("domek2_pietro", "schody");
        }
    } else if (location == "domek2_pietro") {
        if (objName == L"schody") {
            if (Controls::IsPressed(Controls::Down))
                Game::LoadMap("domek2", "schody");
        }
    }
}