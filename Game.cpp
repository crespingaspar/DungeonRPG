#include "Game.hpp"
#include <sstream>
#include <algorithm>
#include <cmath>
#include <random>
#include "EnemyTypes.hpp"

static std::mt19937 rng(std::random_device{}());

Game::Game(sf::RenderWindow &window, sf::Font &font)
    : window(window), font(font)
{
}

void Game::newGame()
{
    currentFloor = 1;
    playerAttackPower = 20;
    floorCleared = false;
    wantsToQuit = false;
    player.reset();
    map.generate();

    auto &rooms = map.getRooms();
    if (!rooms.empty())
    {
        float px = rooms[0].centerX() * TILE_SIZE + TILE_SIZE / 2.f;
        float py = rooms[0].centerY() * TILE_SIZE + TILE_SIZE / 2.f;
        player.setPosition(px, py);
    }
    bullets.clear();
    bossBullets.clear();
    damageTexts.clear();
    spawnEnemies();
    spawnItems();

    if (map.getRooms().size() >= 2)
    {
        auto &last = map.getRooms().back();
        float bx = last.centerX() * TILE_SIZE + TILE_SIZE / 2.f;
        float by = last.centerY() * TILE_SIZE + TILE_SIZE / 2.f;
        boss = std::make_unique<Boss>(bx, by);
        bossSpawned = true;
    }
}

void Game::saveToSlot(const std::string &filename)
{
    GameSaveData data = player.getSaveData();
    data.floor = currentFloor;
    data.attackPower = playerAttackPower;
    SaveSystem::save(data, filename);
}

void Game::loadFromSlot(const std::string &filename)
{
    GameSaveData data;
    if (!SaveSystem::load(data, filename))
    {
        newGame();
        return;
    }

    currentFloor = data.floor;
    playerAttackPower = data.attackPower;
    map.generate();
    player.loadSaveData(data);

    auto &rooms = map.getRooms();
    int tx = (int)(data.playerX / TILE_SIZE);
    int ty = (int)(data.playerY / TILE_SIZE);
    if (!map.isWalkable(tx, ty) && !rooms.empty())
    {
        float px = rooms[0].centerX() * TILE_SIZE + TILE_SIZE / 2.f;
        float py = rooms[0].centerY() * TILE_SIZE + TILE_SIZE / 2.f;
        player.setPosition(px, py);
    }

    bullets.clear();
    bossBullets.clear();
    damageTexts.clear();
    spawnEnemies();
    spawnItems();

    if (rooms.size() >= 2)
    {
        auto &last = rooms.back();
        float bx = last.centerX() * TILE_SIZE + TILE_SIZE / 2.f;
        float by = last.centerY() * TILE_SIZE + TILE_SIZE / 2.f;
        boss = std::make_unique<Boss>(bx, by);
        bossSpawned = true;
    }
}

void Game::spawnEnemies()
{
    enemies.clear();
    auto &rooms = map.getRooms();
    std::uniform_int_distribution<int> typeDist(0, 2);

    for (int i = 1; i < (int)rooms.size() - 1; i++)
    {
        int count = 1 + (i % 3 == 0 ? 1 : 0);
        for (int j = 0; j < count; j++)
        {
            float ex = (rooms[i].x + 1 + j * 2) * TILE_SIZE + TILE_SIZE / 2.f;
            float ey = (rooms[i].y + 1) * TILE_SIZE + TILE_SIZE / 2.f;
            int level = currentFloor + i / 3;
            int type = typeDist(rng);
            if (type == 0)
                enemies.push_back(std::make_unique<FastEnemy>(ex, ey, level));
            else if (type == 1)
                enemies.push_back(std::make_unique<TankEnemy>(ex, ey, level));
            else
                enemies.push_back(std::make_unique<Enemy>(ex, ey, level));
        }
    }
}

void Game::spawnItems()
{
    items.clear();
    auto &rooms = map.getRooms();
    std::uniform_int_distribution<int> typeDist(0, 2);

    for (int i = 1; i < (int)rooms.size(); i += 2)
    {
        float ix = (rooms[i].centerX()) * TILE_SIZE + TILE_SIZE / 2.f;
        float iy = (rooms[i].centerY() + 1) * TILE_SIZE + TILE_SIZE / 2.f;
        ItemType t = static_cast<ItemType>(typeDist(rng));
        items.emplace_back(ix, iy, t);
    }
}

bool Game::handleEvent(const sf::Event &event)
{
    if (event.type == sf::Event::KeyPressed)
    {
        if (event.key.code == sf::Keyboard::Escape)
        {
            wantsToQuit = true;
            return true;
        }
        if (event.key.code == sf::Keyboard::R)
        {
            newGame();
        }
        if (event.key.code == sf::Keyboard::Tab)
        {
            // 临时保存到槽0
            saveToSlot(SaveSystem::slotFilename(0));
            if (fontLoaded)
            {
                DamageText d;
                d.text.setFont(font);
                d.text.setString("已保存!");
                d.text.setCharacterSize(20);
                d.text.setFillColor(sf::Color(50, 220, 100));
                d.pos = player.getPosition();
                d.lifetime = 2.0f;
                damageTexts.push_back(d);
            }
        }
    }
    return false;
}

void Game::handleShooting(float dt)
{
    if (playerAttackCooldown > 0.f)
        playerAttackCooldown -= dt;

    bool shooting = sf::Mouse::isButtonPressed(sf::Mouse::Left) ||
                    sf::Keyboard::isKeyPressed(sf::Keyboard::Space);

    if (shooting && playerAttackCooldown <= 0.f && !player.isDead())
    {
        sf::Vector2f cam = getCamera();
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        sf::Vector2f worldMouse(mousePos.x + cam.x, mousePos.y + cam.y);
        sf::Vector2f playerPos = player.getPosition();
        sf::Vector2f dir = worldMouse - playerPos;
        float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
        if (len > 0.f)
            dir /= len;

        bullets.emplace_back(playerPos, dir, playerAttackPower + player.getLevel() * 3);
        playerAttackCooldown = PLAYER_ATTACK_RATE;
    }
}

void Game::handleItems()
{
    sf::Vector2f pPos = player.getPosition();
    for (auto &item : items)
    {
        if (item.isCollected())
            continue;
        sf::Vector2f diff = item.getPosition() - pPos;
        float dist = std::sqrt(diff.x * diff.x + diff.y * diff.y);
        if (dist < 24.f)
        {
            item.collect();
            switch (item.getType())
            {
            case ItemType::HealthPotion:
                player.heal(30);
                if (fontLoaded)
                {
                    DamageText d;
                    d.text.setFont(font);
                    d.text.setString("+30 HP");
                    d.text.setCharacterSize(16);
                    d.text.setFillColor(sf::Color(80, 255, 80));
                    d.pos = pPos;
                    damageTexts.push_back(d);
                }
                break;
            case ItemType::PowerUp:
                playerAttackPower += 10;
                if (fontLoaded)
                {
                    DamageText d;
                    d.text.setFont(font);
                    d.text.setString("攻击力 +10!");
                    d.text.setCharacterSize(16);
                    d.text.setFillColor(sf::Color(255, 180, 0));
                    d.pos = pPos;
                    damageTexts.push_back(d);
                }
                break;
            case ItemType::SpeedUp:
                player.addSpeed(40.f);
                if (fontLoaded)
                {
                    DamageText d;
                    d.text.setFont(font);
                    d.text.setString("速度提升!");
                    d.text.setCharacterSize(16);
                    d.text.setFillColor(sf::Color(50, 220, 180));
                    d.pos = pPos;
                    damageTexts.push_back(d);
                }
                break;
            }
        }
    }
}

void Game::handleBoss(float dt)
{
    if (!boss || boss->isDead())
        return;

    boss->update(dt, map, player.getPosition(), bossBullets);

    // Boss 子弹更新
    for (auto &b : bossBullets)
        b.update(dt, map);

    // Boss 子弹击中玩家
    for (auto &b : bossBullets)
    {
        if (!b.isAlive())
            continue;
        sf::Vector2f diff = b.getPosition() - player.getPosition();
        float dist = std::sqrt(diff.x * diff.x + diff.y * diff.y);
        if (dist < 20.f)
        {
            player.takeDamage(b.getDamage());
            b.kill();
            if (fontLoaded)
            {
                DamageText d;
                d.text.setFont(font);
                d.text.setString("-" + std::to_string(b.getDamage()));
                d.text.setCharacterSize(16);
                d.text.setFillColor(sf::Color(255, 80, 80));
                d.pos = player.getPosition();
                damageTexts.push_back(d);
            }
        }
    }

    bossBullets.erase(std::remove_if(bossBullets.begin(), bossBullets.end(),
                                     [](const Bullet &b)
                                     { return !b.isAlive(); }),
                      bossBullets.end());

    // 玩家子弹击中 Boss
    for (auto &b : bullets)
    {
        if (!b.isAlive())
            continue;
        sf::Vector2f diff = b.getPosition() - boss->getPosition();
        float dist = std::sqrt(diff.x * diff.x + diff.y * diff.y);
        if (dist < 30.f)
        {
            boss->takeDamage(b.getDamage());
            b.kill();
            if (fontLoaded)
            {
                DamageText d;
                d.text.setFont(font);
                d.text.setString("-" + std::to_string(b.getDamage()));
                d.text.setCharacterSize(16);
                d.text.setFillColor(sf::Color(255, 220, 50));
                d.pos = boss->getPosition();
                damageTexts.push_back(d);
            }
            if (boss->isDead())
                player.addExp(boss->getExpReward());
        }
    }

    // Boss 近战攻击
    sf::Vector2f diff = boss->getPosition() - player.getPosition();
    float dist = std::sqrt(diff.x * diff.x + diff.y * diff.y);
    if (dist < 50.f && boss->canAttack())
    {
        player.takeDamage(boss->getMeleeDamage());
        boss->resetMeleeCooldown();
    }
}

void Game::handleCombat(float dt)
{
    for (auto &bullet : bullets)
        bullet.update(dt, map);

    for (auto &bullet : bullets)
    {
        if (!bullet.isAlive())
            continue;
        for (auto &enemy : enemies)
        {
            if (enemy->isDead())
                continue;
            sf::Vector2f diff = bullet.getPosition() - enemy->getPosition();
            float dist = std::sqrt(diff.x * diff.x + diff.y * diff.y);
            if (dist < 20.f)
            {
                enemy->takeDamage(bullet.getDamage());
                bullet.kill();
                if (fontLoaded)
                {
                    DamageText d;
                    d.text.setFont(font);
                    d.text.setString("-" + std::to_string(bullet.getDamage()));
                    d.text.setCharacterSize(16);
                    d.text.setFillColor(sf::Color(255, 220, 50));
                    d.pos = enemy->getPosition();
                    damageTexts.push_back(d);
                }
                if (enemy->isDead())
                    player.addExp(enemy->getExpReward());
                break;
            }
        }
    }

    bullets.erase(std::remove_if(bullets.begin(), bullets.end(),
                                 [](const Bullet &b)
                                 { return !b.isAlive(); }),
                  bullets.end());

    sf::Vector2f playerPos = player.getPosition();
    for (auto &enemy : enemies)
    {
        if (enemy->isDead())
            continue;
        sf::Vector2f diff = enemy->getPosition() - playerPos;
        float dist = std::sqrt(diff.x * diff.x + diff.y * diff.y);
        if (dist < 36.f && enemy->canAttack())
        {
            player.takeDamage(enemy->getAttackPower());
            enemy->resetAttackCooldown();
            if (fontLoaded)
            {
                DamageText d;
                d.text.setFont(font);
                d.text.setString("-" + std::to_string(enemy->getAttackPower()));
                d.text.setCharacterSize(16);
                d.text.setFillColor(sf::Color(255, 80, 80));
                d.pos = playerPos;
                damageTexts.push_back(d);
            }
        }
    }

    enemies.erase(std::remove_if(enemies.begin(), enemies.end(),
                                 [](const std::unique_ptr<Enemy> &e)
                                 { return e->isDead(); }),
                  enemies.end());

    for (auto &d : damageTexts)
    {
        d.lifetime -= dt;
        d.pos.y += d.vy * dt;
    }
    damageTexts.erase(std::remove_if(damageTexts.begin(), damageTexts.end(),
                                     [](const DamageText &d)
                                     { return d.lifetime <= 0.f; }),
                      damageTexts.end());
}

void Game::update(float dt)
{
    if (player.isDead())
    {
        for (auto &b : bossBullets)
            b.update(dt, map);
        bossBullets.erase(std::remove_if(bossBullets.begin(), bossBullets.end(),
                                         [](const Bullet &b)
                                         { return !b.isAlive(); }),
                          bossBullets.end());
        return;
    }

    player.handleInput(dt, map);
    player.updateSkills(dt);
    handleSkills();
    handleShooting(dt);

    for (auto &enemy : enemies)
        enemy->update(dt, map, player.getPosition());

    handleCombat(dt);
    handleItems();
    handleBoss(dt);
    minimap.update(map, player);
    checkFloorClear(dt);
}

sf::Vector2f Game::getCamera()
{
    sf::Vector2f pos = player.getPosition();
    float cx = pos.x - 400.f;
    float cy = pos.y - 300.f;
    cx = std::max(0.f, std::min(cx, (float)(MAP_WIDTH * TILE_SIZE - 800)));
    cy = std::max(0.f, std::min(cy, (float)(MAP_HEIGHT * TILE_SIZE - 600)));
    return sf::Vector2f(cx, cy);
}
void Game::handleSkills()
{
    sf::Vector2f cam = getCamera();
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    sf::Vector2f worldMouse(mousePos.x + cam.x, mousePos.y + cam.y);
    sf::Vector2f playerPos = player.getPosition();

    // Q - 冲刺，朝移动方向
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
    {
        sf::Vector2f dir(0.f, 0.f);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
            dir.y -= 1.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
            dir.y += 1.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
            dir.x -= 1.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
            dir.x += 1.f;
        if (dir.x == 0 && dir.y == 0)
        {
            sf::Vector2f md = worldMouse - playerPos;
            float len = std::sqrt(md.x * md.x + md.y * md.y);
            if (len > 0)
                dir = md / len;
        }
        player.tryDash(dir);
    }

    // E - 三连发
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::E))
    {
        int dmg = playerAttackPower + player.getLevel() * 3;
        if (player.tryMultiShot(worldMouse, bullets, dmg))
        {
            // 显示提示
            if (fontLoaded)
            {
                DamageText d;
                d.text.setFont(font);
                d.text.setString("三连发三连发!");
                d.text.setCharacterSize(14);
                d.text.setFillColor(sf::Color(100, 220, 255));
                d.pos = playerPos;
                damageTexts.push_back(d);
            }
        }
    }

    // F - 护盾
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::F))
    {
        if (player.tryShieldBurst())
        {
            if (fontLoaded)
            {
                DamageText d;
                d.text.setFont(font);
                d.text.setString("护盾!");
                d.text.setCharacterSize(14);
                d.text.setFillColor(sf::Color(100, 200, 255));
                d.pos = playerPos;
                damageTexts.push_back(d);
            }
        }
    }
}

void Game::checkFloorClear(float dt)
{
    if (enemies.empty() && boss && boss->isDead() && !floorCleared)
    {
        floorCleared = true;
        floorClearTimer = 3.f;
    }

    if (floorCleared)
    {
        floorClearTimer -= dt;
        if (floorClearTimer <= 0.f)
        {
            nextFloor();
        }
    }
}

void Game::nextFloor()
{
    currentFloor++;
    floorCleared = false;
    map.generate();
    auto &rooms = map.getRooms();
    if (!rooms.empty())
    {
        float px = rooms[0].centerX() * TILE_SIZE + TILE_SIZE / 2.f;
        float py = rooms[0].centerY() * TILE_SIZE + TILE_SIZE / 2.f;
        player.setPosition(px, py);
        player.heal(player.getMaxHP() / 2); // 过关回一半血
    }
    bullets.clear();
    bossBullets.clear();
    damageTexts.clear();
    spawnEnemies();
    spawnItems();

    if (rooms.size() >= 2)
    {
        auto &last = rooms.back();
        float bx = last.centerX() * TILE_SIZE + TILE_SIZE / 2.f;
        float by = last.centerY() * TILE_SIZE + TILE_SIZE / 2.f;
        boss = std::make_unique<Boss>(bx, by);
        bossSpawned = true;
    }
}
void Game::render()
{
    window.clear(sf::Color(20, 20, 30));
    sf::Vector2f cam = getCamera();

    map.draw(window, cam);

    for (auto &item : items)
        item.draw(window, cam);
    for (auto &enemy : enemies)
        enemy->draw(window, cam);
    if (boss && !boss->isDead())
        boss->draw(window, cam, player.getPosition());
    for (auto &b : bossBullets)
        b.draw(window, cam);
    for (auto &b : bullets)
        b.draw(window, cam);
    player.draw(window, cam);

    for (auto &d : damageTexts)
    {
        sf::Text t = d.text;
        t.setPosition(d.pos - cam);
        window.draw(t);
    }

    drawHUD();
    minimap.draw(window);
    window.display();
}

void Game::drawHUD()
{
    if (!fontLoaded)
        return;

    sf::RectangleShape hpBg(sf::Vector2f(200, 16));
    hpBg.setPosition(10, 10);
    hpBg.setFillColor(sf::Color(80, 0, 0));
    window.draw(hpBg);

    float hpRatio = (float)player.getHP() / player.getMaxHP();
    sf::RectangleShape hpBar(sf::Vector2f(200 * hpRatio, 16));
    hpBar.setPosition(10, 10);
    hpBar.setFillColor(sf::Color(200, 50, 50));
    window.draw(hpBar);

    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(13);
    text.setFillColor(sf::Color::White);

    std::ostringstream ss;
    ss << "HP: " << player.getHP() << "/" << player.getMaxHP()
       << "   等级: " << player.getLevel()
       << "   攻击: " << (playerAttackPower + player.getLevel() * 3)
       << "   敌人: " << enemies.size();
    text.setString(ss.str());
    text.setPosition(10, 30);
    window.draw(text);

    sf::Text ctrl;
    ctrl.setFont(font);
    ctrl.setCharacterSize(12);
    ctrl.setFillColor(sf::Color(160, 160, 160));
    ctrl.setString("[WASD]移动  [鼠标左键/空格]射击  [R]重新开始  [ESC]退出");
    ctrl.setPosition(10, 50);
    window.draw(ctrl);
    // 层数显示
    sf::Text floorText;
    floorText.setFont(font);
    floorText.setCharacterSize(14);
    floorText.setFillColor(sf::Color(255, 200, 50));
    floorText.setString("第 " + std::to_string(currentFloor) + " 层");
    floorText.setPosition(250, 10);
    window.draw(floorText);

    // 技能冷却显示
    auto &skills = player.getSkills();
    std::string skillNames[] = {"Q:冲刺", "E:三连发", "F:护盾"};
    sf::Color skillColors[] = {
        sf::Color(100, 200, 255),
        sf::Color(255, 180, 50),
        sf::Color(100, 255, 180)};

    for (int i = 0; i < 3; i++)
    {
        sf::RectangleShape skillBg(sf::Vector2f(70, 28));
        skillBg.setPosition(10 + i * 78, 85);
        skillBg.setFillColor(sf::Color(40, 40, 60));
        skillBg.setOutlineColor(sf::Color(80, 80, 100));
        skillBg.setOutlineThickness(1);
        window.draw(skillBg);

        if (skills[i].isReady())
        {
            sf::RectangleShape ready(sf::Vector2f(70, 28));
            ready.setPosition(10 + i * 78, 85);
            ready.setFillColor(sf::Color(skillColors[i].r, skillColors[i].g,
                                         skillColors[i].b, 60));
            window.draw(ready);
        }
        else
        {
            float ratio = 1.f - skills[i].currentCooldown / skills[i].cooldown;
            sf::RectangleShape cd(sf::Vector2f(70 * ratio, 28));
            cd.setPosition(10 + i * 78, 85);
            cd.setFillColor(sf::Color(skillColors[i].r, skillColors[i].g,
                                      skillColors[i].b, 40));
            window.draw(cd);
        }

        sf::Text skillText;
        skillText.setFont(font);
        skillText.setCharacterSize(11);
        skillText.setFillColor(skills[i].isReady() ? skillColors[i] : sf::Color(120, 120, 120));
        skillText.setString(skillNames[i]);
        skillText.setPosition(14 + i * 78, 94);
        window.draw(skillText);
    }

    // 过关提示
    if (floorCleared)
    {
        sf::Text clear;
        clear.setFont(font);
        clear.setString("本层通关！\n" + std::to_string((int)floorClearTimer + 1) + " 秒后进入下一层...");
        clear.setCharacterSize(36);
        clear.setFillColor(sf::Color(50, 220, 100));
        clear.setPosition(180, 230);
        window.draw(clear);
    }
    // Boss 存活提示
    if (boss && !boss->isDead())
    {
        sf::Text bossText;
        bossText.setFont(font);
        bossText.setString("!! BOSS !!");
        bossText.setCharacterSize(14);
        bossText.setFillColor(sf::Color(220, 0, 220));
        bossText.setPosition(10, 68);
        window.draw(bossText);
    }

    if (boss && boss->isDead() && enemies.empty() && !floorCleared)
    {
        sf::Text win;
        win.setFont(font);
        win.setString("胜利！\n按R开始新地下城");
        win.setCharacterSize(40);
        win.setFillColor(sf::Color(50, 220, 100));
        win.setPosition(180, 230);
        window.draw(win);
    }

    if (player.isDead())
    {
        sf::Text dead;
        dead.setFont(font);
        dead.setString("你死了\n按R重新开始");
        dead.setCharacterSize(48);
        dead.setFillColor(sf::Color(220, 30, 30));
        dead.setPosition(200, 230);
        window.draw(dead);
    }
}