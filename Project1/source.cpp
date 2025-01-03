#include <SFML/Graphics.hpp>
#include<vector>
#include<iostream>
#include<algorithm>
using namespace sf;
using namespace std;
string globalState = "free";
int select = -1;
int damaged = -1;
//string turn = "us"; Данный код будет нужен, когда враги не будут болванками для избиения
struct active_ability {
    int range;
    string target;//can be:earth, friend, enemy, nontarget
    vector<string> removedStatus;
    vector<string> addedStatus;
    int hpChange;
};
struct passive_ability {
    string trigger;
    vector<status> effects;
    string target;
    int area;
    int hpChange;

};//passives can triggered: when take damage, deal damage, move, start turn, end turn, get attacked, attack
/*
all stats are :
movespeed
dmg
hpMax
hp
initiative
*/
struct status {
    int msChange;
    int dmgChange;
    int hpmaxChange;
    int intiativeChange;
    bool applied = false;
    int time;
    vector<passive_ability> added_passives;
};
struct unit {
    Texture txtt;
    int hp;
    int ms;
    int dmg;
    bool alive = true;
    int positionx;
    int positiony;
    int nowmovex;
    int nowmovey;
    int initiative;
    int hpMax;
    bool hasUnappliedStatus = false;
    Sprite txt;
    vector<active_ability> actives;
    vector<passive_ability> passives_base;
    vector<passive_ability> passives;
    vector<status> statuses;
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
struct frienUnit : public unit {
    frienUnit(int hpn, int msn, int dmgn, int posx, int posy, int ini) {
        hp = hpn;
        ms = msn;
        dmg = dmgn;
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
            //if (true) {
                zhertva.takedmg(dmg, bfstate);
                cout << "LOS" << endl;
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



int main()
{
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
    

    /*int n = 1, hpin = 1, msin = 1, dmgin = 1, posxin = 1, posyin = 1, inin = 1;
    // cout << "enter size of your army:" << endl;
    // cin >> n;
    vector<frienUnit> us;
    for (int i = 0; i < n; i++) {
        // cout << " enter hp, movespeed, damage, position x, position y and initiative of your "<<i+1<<"st unit"<<endl;
        // cin >> hpin >> msin >> dmgin >> posxin >> posyin >> inin;
        frienUnit scytheofvyse(hpin, msin, dmgin, posxin-1, posyin-1,inin);
        battlefieldState[posyin-1][posxin-1] = "friend";
        us.push_back(scytheofvyse);
    }
    n = 1, hpin = 2, msin = 2, dmgin = 2, posxin = 2, posyin = 2, inin = 2;
    // cout << "enter size of enemy army:" << endl;
    // cin >> n;
    vector<enemyUnit> they;
    for (int i = 0; i < n; i++) {
        // cout << " enter hp, movespeed, damage, position x, position y and initiative of enemy " << i + 1 << "st unit" << endl;
        // cin >> hpin >> msin >> dmgin >> posxin >> posyin >> inin;
        enemyUnit scytheofvyse(hpin, msin, dmgin, posxin - 1, posyin - 1, inin);
        battlefieldState[posyin - 1][posxin - 1] = "enemy";
        they.push_back(scytheofvyse);
    }



    */




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
