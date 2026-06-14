#include <SFML/Graphics.hpp>
#include "GameState.hpp"
#include "MainMenu.hpp"
#include "SlotSelect.hpp"
#include "Settings.hpp"
#include "Game.hpp"
#include "SaveSystem.hpp"

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Dungeon RPG");
    window.setFramerateLimit(60);

    sf::Font font;
    if (!font.loadFromFile("C:/Windows/Fonts/arial.ttf")) return -1;

    GameState state = GameState::MainMenu;
    MainMenu mainMenu(window, font);
    SlotSelect* slotSelect = nullptr;
    Settings* settings = nullptr;
    Game* game = nullptr;

    bool isSavingSlot = false;
    sf::Clock clock;

    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();
        if (dt > 0.05f) dt = 0.05f;

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();

            if (state == GameState::MainMenu) {
                MenuAction action = mainMenu.handleEvent(event);
                switch (action) {
                    case MenuAction::NewGame:
                        isSavingSlot = false;
                        delete slotSelect;
                        slotSelect = new SlotSelect(window, font, false);
                        state = GameState::SaveSlotSelect;
                        break;
                    case MenuAction::Continue:
                        isSavingSlot = false;
                        delete slotSelect;
                        slotSelect = new SlotSelect(window, font, false);
                        state = GameState::SaveSlotSelect;
                        break;
                    case MenuAction::Settings:
                        delete settings;
                        settings = new Settings(window, font);
                        state = GameState::Settings;
                        break;
                    case MenuAction::Quit:
                        window.close();
                        break;
                    default: break;
                }
            }
            else if (state == GameState::SaveSlotSelect && slotSelect) {
                int result = slotSelect->handleEvent(event);
                if (result == -1) {
                    state = GameState::MainMenu;
                } else if (result >= 0) {
                    std::string filename = SaveSystem::slotFilename(result);
                    delete game;
                    game = new Game(window, font);

                    if (isSavingSlot) {
                        game->saveToSlot(filename);
                    } else {
                        if (SaveSystem::hasSave(filename))
                            game->loadFromSlot(filename);
                        else
                            game->newGame();
                    }
                    state = GameState::Playing;
                }
            }
            else if (state == GameState::Settings && settings) {
                bool back = settings->handleEvent(event);
                if (back) {
                    state = GameState::MainMenu;
                }
            }
            else if (state == GameState::Playing && game) {
                bool quit = game->handleEvent(event);
                if (quit) {
                    // 退出到主菜单时先弹存档槽
                    isSavingSlot = true;
                    delete slotSelect;
                    slotSelect = new SlotSelect(window, font, true);
                    state = GameState::SaveSlotSelect;
                }
            }
        }

        // Update
        if (state == GameState::MainMenu) mainMenu.update(dt);
        else if (state == GameState::SaveSlotSelect && slotSelect) slotSelect->update(dt);
        else if (state == GameState::Playing && game) game->update(dt);

        // Render
        window.clear();
        if (state == GameState::MainMenu) mainMenu.draw();
        else if (state == GameState::SaveSlotSelect && slotSelect) slotSelect->draw();
        else if (state == GameState::Settings && settings) settings->draw();
        else if (state == GameState::Playing && game) game->render();
        window.display();
    }

    delete game;
    delete slotSelect;
    delete settings;
    return 0;
}