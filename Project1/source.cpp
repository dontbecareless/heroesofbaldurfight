//В конце разработки надо будет как то распихать всё это чудо по файлам и систематизировать, а также понаделать анимаций + звуков
#include <SFML/Graphics.hpp>
#include<vector>
#include<iostream>
#include<algorithm>
//#include <cstdlib>//если рандом не захочет работать
#include <ctime>
using namespace sf;
using namespace std;
string globalState = "free";
int select = -1;
int damaged = -1;
//string turn = "us"; Данный код будет нужен, когда враги не будут болванками для избиения
struct passive_ability;
struct status;
struct active_ability;
struct unit {
    Texture txtt;
    int hp;
    int ms;
    int dmg;
    int mindmg;
    int maxdmg;
    int deltadmg = 2;
    bool alive = true;
    int positionx;
    int positiony;
    int nowmovex;
    int nowmovey;
    int initiative;
    int hpMax;
    bool hasUnappliedStatus = false;
    Sprite txt;
    vector<pair<bool, int>> applied_status = {};
    vector<active_ability> actives;//это все кажется можно сделать сетами и это будет оптимальнее
    vector<status> statuses = {};
    vector<passive_ability> passives_whenTurnStart_base = {};
    vector<passive_ability> passives_whenTurnStart = {};
    vector<passive_ability> passives_whenAttack_base = {};
    vector<passive_ability> passives_whenAttack = {};

    void applieAllStatus() {
    }
    void turnStartPassives() {
    }
    void attackPassives() {
    }

    void takedmg(int tika, vector<vector<string>>& battlefieldState) {
        hp -= tika;
        if (hp > hpMax) {
            hp = hpMax;
        } else if (hp <= 0) {
            alive = false;
            battlefieldState[positiony][positionx]="free";
            hp = 1;
        }
    }
    void takedmg(int tika, vector<vector<string>>& battlefieldState, bool barrier) {
        hp -= tika;
        if (hp > hpMax && !barrier) {
            hp = hpMax;
        } else if (hp <= 0) {
            alive = false;
            battlefieldState[positiony][positionx] = "free";
            hp = 1;
        }
    }
    void takedmg(int midmg, int madmg, vector<vector<string>>& battlefieldState, bool barrier) {
        hp -= (midmg + (rand() / ((RAND_MAX + 1u) / (madmg-midmg+1))));
        if (hp > hpMax && !barrier) {
            hp = hpMax;
        }
        else if (hp <= 0) {
            alive = false;
            battlefieldState[positiony][positionx] = "free";
            hp = 1;
        }
    }
     void draw(RenderWindow& wind) {
        if (alive == true) {
            txt.setTexture(txtt);
            wind.draw(txt);
        }
    }
    bool operator>(const unit& another) {
        return initiative > another.initiative;
    }
    bool operator>=(const unit& another) {
        return initiative >= another.initiative;
    }
    bool operator<(const unit& another) {
        return initiative < another.initiative;
    }
    bool operator<=(const unit& another) {
        return initiative <= another.initiative;
    }
};
bool rtechnicallythetruth(unit& owner, vector<vector<string>>& battlefieldState) {
    return true;
}
bool isEnemyNear(unit& owner, vector<vector<string>>& battlefieldState) {
    int x = owner.positionx;
    int y = owner.positiony;
    string searching = "";
    if (battlefieldState[y][x] == "enemy") {
        searching = "friend";
    }
    else {
        searching = "enemy";
    }
    for (int iy = max(0, y - 1); iy < max(6, y + 1); y++) {
        for (int ix = max(0, x + 1); ix < max(11, x + 1); x++) {
            if (battlefieldState[iy][ix] == searching) {
                return true;
            }
        }
    }
    return false;
}
struct frienUnit : public unit {
    frienUnit(int hpn, int msn, int dmgn, int posx, int posy, int ini) {
        hp = hpn;
        ms = msn;
        dmg = dmgn;
        mindmg = max(0,dmgn - deltadmg);
        maxdmg = dmgn + deltadmg;
        positionx = posx;
        positiony = posy;
        initiative = ini;
        txtt.loadFromFile("seal.jpg");
        txt.setScale(0.1, 0.1);
        txt.move(posx * 150.f + 163.f, posy * 150.f + 47.f);
    }
    void move(int dx, int dy) {
        globalState = "animation";
        nowmovex = dx * 150.f;
        nowmovey = dy * 150.f;
        positionx += dx;
        positiony += dy;
    }
    void move() {
        if (nowmovex > 0) {
            if (abs(nowmovex) > 16) {
                txt.move(16.f, 0);
                nowmovex -= 16;
            }
            else {
                txt.move(nowmovex, 0);
                nowmovex = 0;
            }
        }
        if (nowmovex < 0) {
            if (abs(nowmovex) > 16) {
                txt.move(-16.f, 0);
                nowmovex += 16;
            }
            else {
                txt.move(nowmovex, 0);
                nowmovex = 0;
            }
        }
        if (nowmovey > 0) {
            if (abs(nowmovey) > 23) {
                txt.move(0, 23.f);
                nowmovey -= 23;
            }
            else {
                txt.move(0, nowmovey);
                nowmovey = 0;
            }
        }
        if (nowmovey < 0) {
            if (abs(nowmovey) > 23) {
                txt.move(0, -23.f);
                nowmovey += 23;
            }
            else {
                txt.move(0, nowmovey);
                nowmovey = 0;
            }
        }
        if (nowmovex == 0 && nowmovey == 0) {
            globalState = "free";
            select = -1;
        }
    }
    void attack(int dx, int dy) {
        globalState = "damage";
        nowmovex = dx * 150.f;
        nowmovey = dy * 150.f;
        cout << nowmovex << ' ' << nowmovey << endl;
        positionx += dx;
        positiony += dy;
    }
   
        void attack(unit& zhertva, vector<vector<string>>& bfstate) {
            if (nowmovex > 0) {
                if (abs(nowmovex) > 16) {
                    txt.move(16.f, 0);
                    nowmovex -= 16;
                }
                else {
                    txt.move(nowmovex, 0);
                    nowmovex = 0;
                }
            }
            if (nowmovex < 0) {
                if (abs(nowmovex) > 16) {
                    txt.move(-16.f, 0);
                    nowmovex += 16;
                }
                else {
                    txt.move(nowmovex, 0);
                    nowmovex = 0;
                }
            }
            if (nowmovey > 0) {
                if (abs(nowmovey) > 23) {
                    txt.move(0, 23.f);
                    nowmovey -= 23;
                }
                else {
                    txt.move(0, nowmovey);
                    nowmovey = 0;
                }
            }
            if (nowmovey < 0) {
                if (abs(nowmovey) > 23) {
                    txt.move(0, -23.f);
                    nowmovey += 23;
                }
                else {
                    txt.move(0, nowmovey);
                    nowmovey = 0;   
                }
            }
            if (nowmovex == 0 && nowmovey == 0) {
                zhertva.takedmg(dmg, bfstate);
                damaged = -1;
                select = -1;
                globalState = "free";
            }
    }
};
struct enemyUnit : public unit {

    enemyUnit(int hpn, int msn, int dmgn, int posx, int posy, int ini) {
        hp = hpn;
        ms = msn;
        dmg = dmgn;
        positionx = posx;
        positiony = posy;
        initiative = ini;
        txtt.loadFromFile("images.jpg");
        txt.setScale(0.3, 0.3);
        txt.move(posx * 150.f + 170.f, posy * 150.f + 48.f);
    }
};

struct status {
    int id;
    int msChange = 0;
    int dmgChange = 0;
    int hpmaxChange = 0;
    int hpChange = 0;
    int initiativeChange = 0;
    int time = 0;
    vector<passive_ability> added_passives = {};
    bool operator==(status& another) {
        return id == another.id;
    }
    void firstApplie(unit& owner) {
        for (int i = 0; i < added_passives.size(); i++) {
            if (added_passives[i].trigger == "attack") {
                if (find(owner.passives_whenAttack.begin(), owner.passives_whenAttack.end(), added_passives[i]) == owner.passives_whenAttack.end()) {
                    owner.passives_whenAttack.push_back(added_passives[i]);
                }
            }
            if (added_passives[i].trigger == "turn start") {
                if (find(owner.passives_whenTurnStart.begin(), owner.passives_whenTurnStart.end(), added_passives[i]) == owner.passives_whenTurnStart.end()) {
                    owner.passives_whenTurnStart.push_back(added_passives[i]);
                }
            }
        }
        owner.ms = max(0, owner.ms - msChange);
        owner.dmg = max(0, owner.dmg - dmgChange);
        owner.hpMax = max(0, owner.hpMax - hpmaxChange);
        owner.hp = min(owner.hp, owner.hpMax);
        owner.initiative -= initiativeChange;
    }
    void removestatus(unit& owner) {
        int posOfStatus = find(owner.statuses.begin(), owner.statuses.end(), *this) - owner.statuses.begin();
        owner.statuses.erase(owner.statuses.begin() + posOfStatus);
        owner.applied_status.erase(owner.applied_status.begin() + posOfStatus);
        for (int i = 0; i < added_passives.size(); i++) {
            added_passives[i].removepassive(owner);
        }
        owner.hpMax -= hpmaxChange;
        owner.ms -= msChange;
        owner.dmg -= dmgChange;
        owner.initiative -= initiativeChange;
    }
    void tick(unit& owner, vector<vector<string>>& battlefieldState) {
        int timeNow=owner.applied_status[find(owner.statuses.begin(), owner.statuses.end(), *this) - owner.statuses.begin()].second--;
        if (timeNow == 0) {
            this->removestatus(owner);
        }
        owner.takedmg(hpChange,battlefieldState);
    }
};
struct active_ability {
    int id;
    int range;
    int area;
    string target;//can be:earth, friend, enemy, nontarget
    vector<status> removedStatus = {};
    vector<status> addedStatus = {};
    int hpChange;
    bool operator==(active_ability& another) {
        return id == another.id;
    }
};
struct passive_ability {
    int id;
    int cd = 0;
    bool (*condition)(unit&, vector<vector<string>>&) = rtechnicallythetruth;
    string trigger;
    vector<status> effects_enemy = {};
    vector<status> effects_friends = {};
    int area;
    int hpChangeEnemy;
    int hpChangeFriends;
    bool operator==(passive_ability& another) {
        return id == another.id;
    }
    void removepassive(unit& owner) {
        if (trigger == "attack") {
            owner.passives_whenAttack.erase(find(owner.passives_whenAttack.begin(), owner.passives_whenAttack.end(), this));
        }
        if (trigger == "turn start") {
            owner.passives_whenTurnStart.erase(find(owner.passives_whenTurnStart.begin(), owner.passives_whenTurnStart.end(), this));
        }
    }

};//passives can triggered: when take damage, deal damage, move, start turn, end turn, get attacked, attack
/*
all stats are :
movespeed
dmg
hpMax
hp
initiative
*/

int main()
{   
    //тут будут описаны способности и сопутствующие им статусы, всё это потом уедет в отдельный файл, с которым мне пока лень разбирваться
    /*
    МИНИМАЛЬНЫЙ НАБОР СПОСОБНОСТЕЙ
    \/fireball (урон по области 3 на 3 на расстоянии до 4)
    \/poison touch (отравление на 3 хода, в каждый ход получает по 1 урону)
    RUNRUNRUN (ms+2 если рядом со врагом)
    */
    active_ability fireball;
    fireball.range = 4;
    fireball.addedStatus = {};
    fireball.removedStatus = {};
    fireball.target = "earth";
    fireball.hpChange = 4;
    fireball.area = 1;
    fireball.id = 1;
    status poisoned;
    poisoned.id = 1;
    poisoned.hpChange = 1;
    poisoned.time = 3;
    passive_ability poison_touch;
    poison_touch.hpChangeFriends = 0;
    poison_touch.hpChangeEnemy = 0;
    poison_touch.area = 0;
    poison_touch.cd = 0;
    poison_touch.id = 1;
    poison_touch.trigger = "attack";
    poison_touch.effects_enemy.push_back(poisoned);
    status running;
    running.id = 2;
    running.time = 1;
    running.msChange = 2;
    passive_ability runrunrun;
    runrunrun.condition = isEnemyNear;
    runrunrun.id = 2;
    runrunrun.area = 0;
    runrunrun.cd = 0;
    runrunrun.effects_friends.push_back(running);
    runrunrun.hpChangeEnemy = 0;
    runrunrun.hpChangeFriends = 0;
    runrunrun.trigger = "turn start";


    









    //w=11
    //h=6

    //способности и сопутствующие статусы закончились
    srand(time(nullptr));//задаём сид для генерации случайных чисел (что это и почему это желательно сделать можно почитать на cppreferences)



    RenderWindow window(VideoMode(1500, 1000), L"Новый проект", Style::Fullscreen);
    window.setVerticalSyncEnabled(true);
    Clock rattle;
    Texture frs;
    Texture friendTexture;
    Texture enemyTexture;
    if (!frs.loadFromFile("forest.jpg") || !friendTexture.loadFromFile("seal.jpg") || !enemyTexture.loadFromFile("images.jpg")) {
        return EXIT_FAILURE;
    }
    frs.loadFromFile("forest.jpg");
    Sprite bg;
    bg.setTexture(frs);
    bg.setScale(2, 2);


    Color hexInRange(0, 255, 0, 100);

    vector<vector<string>> battlefieldState(6, vector<string>(11, "free"));
    //vector<vector<CircleShape>> battlefield(6, vector<CircleShape>(11));
    vector<vector<RectangleShape>> battlefield(6, vector<RectangleShape>(11));
    for (int i = 0; i < 6; i++) {
    for (int j = 0; j < 11; j++) {
        /*CircleShape scytheofvyse(90.f, 6);
        scytheofvyse.setOutlineColor(Color::Blue);
        scytheofvyse.setOutlineThickness(2.f);
        scytheofvyse.setFillColor(Color::Transparent);
        if (i % 2 == 0) {
            scytheofvyse.move(j * 160.f + 30.f, i* 138.f + 5);
        } else {
            scytheofvyse.move(j * 160.f + 110.f, i * 138.f + 5);
        }
        battlefield[i][j]=scytheofvyse;*/
        RectangleShape scytheofvyse(Vector2f(150.f, 150.f));
        scytheofvyse.setOutlineColor(Color::Blue);
        scytheofvyse.setOutlineThickness(2.f);
        scytheofvyse.setFillColor(Color::Transparent);
        scytheofvyse.move(j * 150.f + 135.f, i * 150.f + 5);
        battlefield[i][j] = scytheofvyse;
    }
    }
    //hor 11 vert 6
    int n, hpin, msin, dmgin, posxin, posyin, inin;
    cout << "enter size of your army:" << endl;
    cin >> n;
    vector<frienUnit> us;
    for (int i = 0; i < n; i++) {
        cout << " enter hp, movespeed, damage, position x, position y and initiative of your "<<i+1<<"st unit"<<endl;
        cin >> hpin >> msin >> dmgin >> posxin >> posyin >> inin;
        frienUnit scytheofvyse(hpin, msin, dmgin, posxin-1, posyin-1,inin);
        battlefieldState[posyin-1][posxin-1] = "friend";
        us.push_back(scytheofvyse);
    }
    cout << "enter size of enemy army:" << endl;
    cin >> n;
    vector<enemyUnit> they;
    for (int i = 0; i < n; i++) {
        cout << " enter hp, movespeed, damage, position x, position y and initiative of enemy " << i + 1 << "st unit" << endl;
        cin >> hpin >> msin >> dmgin >> posxin >> posyin >> inin;
        enemyUnit scytheofvyse(hpin, msin, dmgin, posxin - 1, posyin - 1, inin);
        battlefieldState[posyin - 1][posxin - 1] = "enemy";
        they.push_back(scytheofvyse);
    }

    //мб сделать таки массив в котором чары хранятся а не тупо поляну и отдельно массив чаров


    sort(us.begin(), us.end());
    sort(they.begin(), they.end());
    int pointer_they = 0;
    int pointer_us=0;

    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }
        window.draw(bg);

        if (select == -1) {
            if (pointer_they >= they.size() && pointer_us >= us.size()) {
                pointer_they = 0;
                pointer_us = 0;
            }
            else if (pointer_they >= they.size() && pointer_us<us.size()) {
                select = pointer_us;
                pointer_us++;
            }
            else if (pointer_us >= us.size() && pointer_they<they.size()) {
                pointer_they++;
            }
            /*if (they[pointer_they] > us[pointer_us]) {
                turn = "they";
                select = pointer_they;                      Данный код будет нужен, когда враги не будут болванками для избиения
            }*/
            else if (they[pointer_they] <= us[pointer_us]) {
                select = pointer_us;
                pointer_us++;
            }
            else {
                pointer_they++;
            }
        }
        /*for (int i = 0; i < 6; i++) {
            if (Mouse::isButtonPressed(Mouse::Right) && us[i].txt.getGlobalBounds().contains(Mouse::getPosition().x, Mouse::getPosition().y) && globalState=="free") {
                select = i;
            }
        }*/
        for (int i = 0; i < us.size(); i++) {
            us[i].applieAllStatus();
            us[i].turnStartPassives();
        }
        for (int i = 0; i < they.size(); i++) {
            they[i].applieAllStatus();
            they[i].turnStartPassives();
        }

        for (int i = 0; i < they.size(); i++) {
            if (globalState == "free" &&they[i].alive==true && select != -1 && Mouse::isButtonPressed(Mouse::Left) && battlefield[they[i].positiony][they[i].positionx].getGlobalBounds().contains(Mouse::getPosition().x, Mouse::getPosition().y)) {
                if ((they[i].positiony - us[select].positiony) * (they[i].positiony - us[select].positiony) + (they[i].positionx - us[select].positionx) * (they[i].positionx - us[select].positionx) <= (us[select].ms) * (us[select].ms)+1 && rattle.getElapsedTime().asMilliseconds() >= 500) {
                    battlefieldState[us[select].positiony][us[select].positionx] = "free";
                    battlefieldState[they[i].positiony][they[i].positionx] = "friend";
                    int targetx = they[i].positionx - us[select].positionx;
                    int targety = they[i].positiony - us[select].positiony;
                    if (they[i].positionx > us[select].positionx) {
                        targetx--;
                    }
                    else if (they[i].positionx < us[select].positionx){
                        targetx++;
                    }
                    else if(us[select].positiony<they[i].positiony) {
                        targety--;
                    } else if (us[select].positiony > they[i].positiony) {
                        targety++;
                    }
                    us[select].attack(targetx, targety);
                    damaged = i;
                    rattle.restart();
                }
            }
            else if (damaged != -1 && select != -1 && globalState == "damage" && rattle.getElapsedTime().asMilliseconds() >= 10) {
                us[select].attack(they[damaged], battlefieldState);
                rattle.restart();
            }
           /* else if (damaged != -1 && select != -1 && globalState == "damage") {
                they[damaged].takedmg(us[select].dmg, battlefieldState);
                globalState = "free";
                damaged = -1;
                select = -1;
            }*/
        }
        for (int i = 0; i < 6; i++) {
            for (int j = 0; j < 11; j++) {
                if (select != -1 && Mouse::isButtonPressed(Mouse::Left) && battlefield[i][j].getGlobalBounds().contains(Mouse::getPosition().x, Mouse::getPosition().y) && rattle.getElapsedTime().asMilliseconds() >= 500) {
                    if (globalState == "free" && (i - us[select].positiony) * (i - us[select].positiony) + (j - us[select].positionx) * (j - us[select].positionx) <= us[select].ms * us[select].ms ) {
                        if (battlefieldState[i][j] == "free") {
                            battlefieldState[us[select].positiony][us[select].positionx] = "free";
                            us[select].move(j - us[select].positionx, i - us[select].positiony);
                            battlefieldState[i][j] = "friend";
                        }
                        rattle.restart();
                    }
                } else if (globalState=="animation" && rattle.getElapsedTime().asMilliseconds()>=10) {
                  us[select].move();
                  rattle.restart();
                }
                    
            }
        }

        for (int i = 0; i < 6; i++) {
            for (int j = 0; j < 11; j++) {
                if (select != -1 && globalState == "free") {
                    if ((i - us[select].positiony) * (i - us[select].positiony) + (j - us[select].positionx) * (j - us[select].positionx) <= us[select].ms * us[select].ms) {
                        battlefield[i][j].setFillColor(Color(0, 255, 0, 100));
                    }
                    else if (battlefieldState[i][j]=="enemy" && (i - us[select].positiony) * (i - us[select].positiony) + (j - us[select].positionx) * (j - us[select].positionx) <= us[select].ms * us[select].ms + 1) {
                        battlefield[i][j].setFillColor(Color(0, 255, 0, 100));
                    }
                    else {
                        battlefield[i][j].setFillColor(Color::Transparent);
                    }
                }
                window.draw(battlefield[i][j]);
            }
        }
        for (int i = 0; i < us.size(); i++) {
            us[i].draw(window);
        }
        for (int i = 0; i < they.size(); i++) {
                they[i].draw(window);
        }
        window.display();
    }
}
