//В конце разработки надо будет как то распихать всё это чудо по файлам и систематизировать, а также понаделать анимаций + звуков
#include <SFML/Graphics.hpp>
#include<vector>
#include<iostream>
#include<algorithm>
//#include <cstdlib>//если рандом не захочет работать
#include<ctime>
#include<string>
#include<map>
using namespace sf;
using namespace std;
string globalState = "free";
vector<pair<string, string>> allPassives = { {"poisoned","status"}, {"Poison touch", "attack"}, {"running", "status"}, {"runrunrun", "turn start"}};
int select = -1;
int damaged = -1;
//string turn = "us"; Данный код будет нужен, когда враги не будут болванками для избиения
struct unit;
struct statBlock {
    int msChange = 0;
    int dmgChange = 0;
    int hpmaxChange = 0;
    int tickdmg = 0;
    int initiativeChange = 0;
    void applyDelta(unit& owner);
    void disapplyDelta(unit& owner);
    void tick(unit& owner, vector<vector<string>>& battlefieldState, int id);
    statBlock(int ms, int dmg, int hpmax, int tickdmgA, int initi);
    statBlock() {}
};
struct passive {
    int id;

    string type;
        int time;
        bool (*condition)(unit&, vector<vector<string>>&) = [](unit& owner, vector<vector<string>>& battlefieldstate) {return true; };//бывашая rtechicallythetruth
        string trigger;
        string target = "";
    
        int dmg = 0;//для триггеринг
        vector<passive> effects_enemy = {};//для триггеринг
        vector<passive> effects_friends = {};
        statBlock enemyStats;//для триггеринг
        statBlock friendStats;
    
        bool operator==(int& another) {
            return this->id == another;
        }
        const bool operator==(const passive& another) {
            return this->id == another.id;
        }
        bool operator==(passive& another) {
            return this->id == another.id;
        }
    
        void triggering(unit& owner, vector<vector<string>>& battlefieldstate, int targetx, int targety, vector<unit>& zhertvi);
        void triggering(unit& owner, vector<vector<string>>& battlefieldstate);
        void apply(unit& owner);
        void disapply(unit& owner);
        void tick(unit& owner, vector<vector<string>>& battlefieldstate);
        passive() {};



};
struct active_ability;
struct unit {
    bool hasActive = false;

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


    RectangleShape activeButton;
private:
    String type;
public:
    String getType() {
        return type;
    }
    void setType(String& dano) {
        type = dano;
        if (type == "friend") {
            txtt.loadFromFile("seal.jpg");
            txt.setScale(0.1, 0.1);
            txt.move(positionx * 150.f + 163.f, positiony * 150.f + 47.f);
        }
        if (type == "enemy") {
            txtt.loadFromFile("images.jpg");
            txt.setScale(0.3, 0.3);
            txt.move(positionx * 150.f + 170.f, positiony * 150.f + 48.f);
        }
    }
    vector<int> Status_duration;//-1, если статуса нет, иначе рил длительность, i - id
    unit() {}
    unit(int hpn, int msn, int dmgn, int posx, int posy, int ini) {
        hp = hpn;
        ms = msn;
        dmg = dmgn;
        mindmg = max(0, dmgn - deltadmg);
        maxdmg = dmgn + deltadmg;
        positionx = posx;
        positiony = posy;
        initiative = ini;
        activeButton.setSize(Vector2f(300.f, 150.f));
        activeButton.setFillColor(Color::Blue);
        activeButton.setPosition(Vector2f(1615.f, 925.f));
        Status_duration = { -1,-1,-1,-1, -1, -1 };
    }

    
    /*map<string, int> passives;
    map<string, int> actives;*/

    //везде где i - id неплохо бы сделать дефолт размер равный колву статусов
    //vector<pair<bool, int>> applied_status = {};//i - id статуса
    
    vector<active_ability> actives;//это все кажется можно сделать сетами и это будет оптимальнее
    vector<bool> hasActiveById;
    vector<int> cooldowns;
    
    vector<passive> statuses = {};//мб сделать глобальным тк мы эт не меняем, а меняем только duration
    vector<passive> passives_whenTurnStart_base = {};
    vector<passive> passives_whenTurnStart = {};
    vector<passive> passives_whenAttack_base = {};
    vector<passive> passives_whenAttack = {};
    void turnStartPassives(vector<vector<string>>& battlefieldState) {
        for (int i = 0; i < passives_whenTurnStart.size(); i++) {
            passives_whenTurnStart[i].triggering(*this, battlefieldState);
        }
    }
    void attackPassives(vector<vector<string>>& battlefieldState, int targetx, int targety, vector<unit>& zhertvi) {
        for (int i = 0; i < passives_whenAttack.size(); i++) {
            passives_whenAttack[i].triggering(*this, battlefieldState, targetx, targety, zhertvi);
        }
    }
    
    void tick(vector<vector<string>>& battlefieldState) {
        for (int i = 0; i < cooldowns.size();i++) {
            if (cooldowns[i] > 0) {
                cooldowns[i]--;
            }
        }
        for (int i = 0; i < Status_duration.size(); i++) {
            if (Status_duration[i] >= 0) {
                statuses[i].tick(*this, battlefieldState);
            }
        }
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
            if (hasActive) {
                wind.draw(activeButton);
            }
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
    pair<int, int> hodi[] = { {0,1}, {0, -1},{1, 0}, {-1, 0} };
    for (int i = 0; i < 4; i++) {
        if (x + hodi[i].first >= 0 && x + hodi[i].first < 11 && y + hodi[i].second >= 0 && y + hodi[i].second < 6 && battlefieldState[y + hodi[i].second][x + hodi[i].first] == searching) {
            return true;
        }
    }
    return false;
}
/*struct frienUnit : public unit {
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
    
};
struct enemyUnit : public unit {

    enemyUnit(int hpn, int msn, int dmgn, int posx, int posy, int ini) {
        hp = hpn;
        ms = msn;
        dmg = dmgn;
        positionx = posx;
        positiony = posy;
        initiative = ini;
        
    }
};
*/



void statBlock::applyDelta(unit& owner) {
            owner.ms = max(0, owner.ms + msChange);
            owner.dmg = max(0, owner.dmg + dmgChange);
            //owner.hpMax = max(0,owner.hpMax + hpmaxChange);
            owner.initiative = max(0, owner.initiative + initiativeChange);
}
void statBlock::disapplyDelta(unit& owner) {
            owner.ms = max(0, owner.ms-msChange);
            owner.dmg = max(0, owner.dmg - dmgChange);
            //owner.hpMax =  max(0, owner.hpMax - hpmaxChange);
            owner.initiative = max(0, owner.initiative - initiativeChange);
        }
        void statBlock::tick(unit& owner, vector<vector<string>>& battlefieldState, int id) {
            owner.takedmg(tickdmg, battlefieldState);
            owner.Status_duration[id]--;
            if (owner.Status_duration[id] == 0) {
                this->disapplyDelta(owner);
                owner.Status_duration[id] = -1;
            }
        }
        statBlock::statBlock (int ms, int dmg, int hpmax, int tickdmgA, int initi) {
            msChange = ms;
            dmgChange = dmg;
            hpmaxChange = hpmax;
            tickdmg = tickdmgA;
            initiativeChange = initi;
        }



        void passive::triggering (unit& owner, vector<vector<string>>& battlefieldState, int targetx, int targety, vector<unit>& zhertvi) {
            bool possible = condition(owner, battlefieldState);
                    if (possible && time == 0 && type == "ability" && target=="area") {
                        //тут будет срабатывание на всех в области, но я ленивый (пока что)
                        return;
                    }
                    if (possible && time == 0 && type == "ability" && target == "self") {
                        for (int i = 0; i < effects_friends.size(); i++) {
                            if (owner.Status_duration[effects_friends[i].id] == -1) {
                                owner.Status_duration[effects_friends[i].id] = time;
                                effects_friends[i].apply(owner);
                            } else {
                                owner.Status_duration[effects_friends[i].id] += time;
                            }
                        }
                        return;
                    }
                    if (possible && time == 0 && type == "ability" && target == "attacked") {
            
                            for (int i = 0; i < zhertvi.size(); i++) {
                                if (zhertvi[i].positionx == targetx && zhertvi[i].positiony == targety && zhertvi[i].alive==true) {
                                    for (int j = 0; j < effects_enemy.size(); j++) {
                                        if (zhertvi[i].Status_duration[effects_enemy[i].id] == -1) {
                                            zhertvi[i].Status_duration[effects_enemy[i].id] = time;
                                            effects_enemy[i].apply(zhertvi[i]);
                                        }
                                        else {
                                            zhertvi[i].Status_duration[effects_enemy[i].id] += time;
                                        }
                                    }
                                }
                            }
                    }
        }
        void passive::triggering(unit& owner, vector<vector<string>>& battlefieldState) {
            bool possible = condition(owner, battlefieldState);
                    if (possible && time == 0 && type == "ability" && target == "area") {
                        //тут будет срабатывание на всех в области, но я ленивый (пока что)
                        return;
                    }
                    if (possible && type == "ability" && target == "self") {
                        for (int i = 0; i < effects_friends.size(); i++) {
                                effects_friends[i].apply(owner);
                        }
                        return;
                    }
                }
        void passive::apply(unit& owner) {
                    if (type == "status") {
                        if (owner.Status_duration[id] == -1) {
                            friendStats.applyDelta(owner);
                            owner.Status_duration[id] = time;
                        } else {
                            owner.Status_duration[id] += time;
                        }
                        return;
                    }
                    if (type == "ability") {
                        friendStats.applyDelta(owner);
                        for (int i = 0; i < effects_friends.size(); i++) {
                            if (effects_friends[i].trigger == "attack") {
                                if (find(owner.passives_whenAttack.begin(), owner.passives_whenAttack.end(), effects_friends[i]) == owner.passives_whenAttack.end()) {
                                    owner.passives_whenAttack.push_back(effects_friends[i]);
                                }
                            }
                            if (effects_friends[i].trigger == "turn start") {
                                if (find(owner.passives_whenTurnStart.begin(), owner.passives_whenTurnStart.end(), effects_friends[i]) == owner.passives_whenTurnStart.end()) {
                                    owner.passives_whenTurnStart.push_back(effects_friends[i]);
                                }
                            }
                        }
                            return;
                    }
                }
      /*  void passive::disapply(unit& owner) {
                    if (type == "status") {
                        friendStats.disapplyDelta(owner);
                        return;
                    }
                    if (type == "ability") {
                        if (trigger == "attack") {
                            if (find(owner.passives_whenAttack.begin(), owner.passives_whenAttack.end(), *this) != owner.passives_whenAttack.end()) {
                                owner.passives_whenAttack.erase(find(owner.passives_whenAttack.begin(), owner.passives_whenAttack.end(), *this));
                            }
                        }
                        if (trigger == "turn start" && find(owner.passives_whenTurnStart.begin(), owner.passives_whenTurnStart.end(), *this) != owner.passives_whenTurnStart.end()) {
                            owner.passives_whenTurnStart.erase(find(owner.passives_whenTurnStart.begin(), owner.passives_whenTurnStart.end(), *this));
                        }
                        return;
                    }
                }*/
                void passive::tick(unit& owner, vector<vector<string>>& battlefieldState) {
                    if (type == "status") {
                        friendStats.tick(owner, battlefieldState, id);
                        
                        return;
                    }
                }
struct active_ability {
    int id;
    int range;
    int area;//радиус от клетки таргета (те для три на три area = 1)
    string target;//can be:earth, friend, enemy, nontarget
    vector<passive> removedStatus = {};
    vector<passive> addedStatus = {};
    int hpChange;
    bool isBarrier = false;
    bool operator==(active_ability& another) {
        return id == another.id;
    }
    bool operator==(int& another) {
        return id == another;
    }
    void activate(unit& owner, vector<vector<string>>& battlefieldState, vector<unit>& they, vector<unit>& us, int targety, int targetx) {
        if (target == "earth" && (targety - owner.positiony) * (targety - owner.positiony) + (targetx - owner.positionx) * (targetx - owner.positionx) <= range * range) {
            for (int y = max(0, targety - area); y < min(5, targety + area); y++) {
                for (int x = max(0, targetx - area); x < min(10, targetx + area); x++) {
                    for (int i = 0; i < they.size(); i++) {
                        if (they[i].alive == true && they[i].positionx == x && they[i].positiony == y) {
                            they[i].takedmg(hpChange, battlefieldState, isBarrier);
                        }
                    }
                    for (int i = 0; i < us.size(); i++) {
                        if (us[i].alive == true && us[i].positionx == x && us[i].positiony == y) {
                            us[i].takedmg(hpChange, battlefieldState, isBarrier);
                        }
                    }
                }
            }
        }
    }
};



//passives can triggered: when take damage, deal damage, move, start turn, end turn, get attacked, attack
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
    fireball (урон по области 3 на 3 на расстоянии до 4)
    poison touch (отравление на 3 хода, в каждый ход получает по 1 урону)
    RUNRUNRUN (ms+2 если рядом со врагом)
    */
    active_ability fireball;
    fireball.range = 4;
    fireball.addedStatus = {};
    fireball.removedStatus = {};
    fireball.target = "earth";
    fireball.hpChange = 4;
    fireball.area = 1;
    fireball.id = 0;
    passive poisoned;
    poisoned.type = "status";
    poisoned.id = 0;
    poisoned.dmg = 1;
    poisoned.time = 3;
    passive poison_touch;
    poison_touch.id = 1;
    poison_touch.trigger = "attack";
    poison_touch.target = "enemy";
    poison_touch.effects_enemy.push_back(poisoned);
    poison_touch.type = "ability";
    passive running;
    running.id = 2;
    running.type = "status";
    running.time = 1;
    statBlock runningStats;
    runningStats.msChange = 2;
    running.friendStats = runningStats;
    passive runrunrun; ;
    runrunrun.condition = isEnemyNear;
    runrunrun.id = 3;
    runrunrun.effects_friends.push_back(running);
    runrunrun.trigger = "turn start";
    runrunrun.type = "ability";
    runrunrun.target = "self";
    runrunrun.time = 1;
    vector<passive> statuses = { poisoned, poison_touch, running, runrunrun };
    //неработает нафиг триггеринги твои (а вот кондиции ок)
    









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
    /*int n, hpin, msin, dmgin, posxin, posyin, inin;
    cout << "enter size of your army:" << endl;
    cin >> n;*/
    vector<unit> us;
    /*for (int i = 0; i < n; i++) {
        cout << " enter hp, movespeed, damage, position x, position y and initiative of your "<<i+1<<"st unit"<<endl;
        cin >> hpin >> msin >> dmgin >> posxin >> posyin >> inin;
        unit scytheofvyse(hpin, msin, dmgin, posxin-1, posyin-1,inin);
        String mysticStaff = "friend";
        scytheofvyse.setType(mysticStaff);
        battlefieldState[posyin-1][posxin-1] = "friend";
        us.push_back(scytheofvyse);
    }
    cout << "enter size of enemy army:" << endl;
    cin >> n;*/
    vector<unit> they;
    /*for (int i = 0; i < n; i++) {
        cout << " enter hp, movespeed, damage, position x, position y and initiative of enemy " << i + 1 << "st unit" << endl;
        cin >> hpin >> msin >> dmgin >> posxin >> posyin >> inin;
        unit scytheofvyse(hpin, msin, dmgin, posxin - 1, posyin - 1, inin);
        String mysticStaff = "enemy";
        scytheofvyse.setType(mysticStaff);
        battlefieldState[posyin - 1][posxin - 1] = "enemy";
        they.push_back(scytheofvyse);
    }*/

    //мб сделать таки массив в котором чары хранятся а не тупо поляну и отдельно массив чаров
    unit scytheofvyse(1, 2, 1, 0, 0, 1);
    String mysticStaff = "friend";
    scytheofvyse.setType(mysticStaff);
    battlefieldState[0][0] = "friend";
    scytheofvyse.passives_whenTurnStart.push_back(runrunrun);
    scytheofvyse.statuses = statuses;
    us.push_back(scytheofvyse);

    scytheofvyse = unit(2, 2, 1, 1, 1, 1);
    mysticStaff = "enemy";
    scytheofvyse.setType(mysticStaff);
    battlefieldState[1][1] = "enemy";
    scytheofvyse.statuses = statuses;
    they.push_back(scytheofvyse);


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
            for (size_t i = 0; i < us.size(); i++) {
                us[i].tick(battlefieldState);
            }
            while (select == -1) {
                if (pointer_they >= they.size() && pointer_us >= us.size()) {
                    pointer_they = 0;
                    pointer_us = 0;
                }
                else if (pointer_they >= they.size() && pointer_us < us.size()) {
                    select = pointer_us;
                    /*for (int i = 0; i < us.size(); i++) {
                        us[i].turnStartPassives(battlefieldState);
                    }*/
                    pointer_us++;
                }
                else if (pointer_us >= us.size() && pointer_they < they.size()) {
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
            if (select != -1) { 
                us[select].turnStartPassives(battlefieldState);
                //they когда враги будут ходить
            }
        }
        /*for (int i = 0; i < 6; i++) {
            if (Mouse::isButtonPressed(Mouse::Right) && us[i].txt.getGlobalBounds().contains(Mouse::getPosition().x, Mouse::getPosition().y) && globalState=="free") {
                select = i;
            }
        }*/


        for (int i = 0; i < they.size(); i++) {
            if (globalState == "free" &&they[i].alive==true && select != -1 && Mouse::isButtonPressed(Mouse::Left) && battlefield[they[i].positiony][they[i].positionx].getGlobalBounds().contains(Mouse::getPosition().x, Mouse::getPosition().y)) {
                if ((they[i].positiony - us[select].positiony) * (they[i].positiony - us[select].positiony) + (they[i].positionx - us[select].positionx) * (they[i].positionx - us[select].positionx) <= (us[select].ms) * (us[select].ms)+1 && rattle.getElapsedTime().asMilliseconds() >= 500) {
                    battlefieldState[us[select].positiony][us[select].positionx] = "free";
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
                    battlefieldState[us[select].positiony + targety][us[select].positionx + targetx] = "friend";
                    us[select].attack(targetx, targety);
                    damaged = i;
                    rattle.restart();
                }
            }
            else if (damaged != -1 && select != -1 && globalState == "damage" && rattle.getElapsedTime().asMilliseconds() >= 10) {
                us[select].attackPassives(battlefieldState, they[damaged].positionx, they[damaged].positiony, they);
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
            cout << us[i].alive << endl;;
        }
        for (int i = 0; i < they.size(); i++) {
                they[i].draw(window);
        }
        window.display();
    }
}
