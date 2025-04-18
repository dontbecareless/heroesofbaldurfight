//В конце разработки надо будет как то распихать всё это чудо по файлам и систематизировать, а также понаделать анимаций + звуков
#include<SFML/Graphics.hpp>
#include<vector>
#include<iostream>
#include<algorithm>
//#include <cstdlib>//если рандом не захочет работать
#include<ctime>
#include<string>
#include<cstring>
#include<deque>
//нужно .h и .cpp файлы делать
using namespace sf;
using namespace std;


enum class PGS { free, active_ability, animation, damage, particle_flying, activating_active, dealing_damage, running};
//PGS = Possible Global States, but i am too lazy to write it every time

deque<pair<int, int>> path;
PGS globalState = PGS::free;
vector<pair<string, string>> allPassives = { {"poisoned","status"}, {"Poison touch", "attack"}, {"running", "status"}, {"runrunrun", "turn start"} };
int select = -1;
int damaged = -1;
int nowtargetx = -1;
int nowtargety = -1;
bool nowBotTurn = false;
const int w = 11;
const int h = 6;
vector<vector<string>> battlefieldState(h, vector<string>(w, "free"));
vector<vector<bool>> reachable(h, vector<bool>(w, false));
vector<vector<bool>> was(h, vector<bool>(w, false));


struct Unit;

vector<Unit> us;
vector<Unit> they;

struct StatBlock {
	int msChange = 0;
	int dmgChange = 0;
	int hpmaxChange = 0;
	int tickdmg = 0;
	int initiativeChange = 0;
	void applyDelta(Unit& owner);
	void disapplyDelta(Unit& owner);
	void tick(Unit& owner, int id);
	StatBlock(int ms, int dmg, int hpmax, int tickdmgA, int initi);
	StatBlock() {}
};

struct Passive {
	int id;
	string type;
	int time;
	bool (*condition)(Unit&) = [](Unit& owner) {return true; };//бывашая rtechicallythetruth
	string trigger;
	string target = "";
	int dmg = 0;//для триггеринг
	vector<Passive> effects_enemy = {};//для триггеринг
	vector<Passive> effects_friends = {};
	StatBlock enemyStats;//для триггеринг
	StatBlock friendStats;
	bool operator==(int& another) {
		return this->id == another;
	}
	const bool operator==(const Passive& another) {
		return this->id == another.id;
	}
	bool operator==(Passive& another) {
		return this->id == another.id;
	}
	void triggering(Unit& owner, int targetx, int targety, vector<Unit>& zhertvi);
	void triggering(Unit& owner);
	void apply(Unit& owner);
	void disapply(Unit& owner);
	void tick(Unit& owner);
	Passive() {};
};

vector<Passive> statuses;

struct ActiveAbility;
struct Particle {
	RectangleShape txt;
	int startSize;
	int endSize;
	int nowmovex;
	int nowmovey;
	int stepSize;
	int numberOfIterations;
	int stepMoveX;
	int stepMoveY;
	int iters;
	int deltaX;
	int deltaY;

	void draw(int targetx, int targety, vector<Unit>& us);
	void draw(bool isActive);
};

void checkLeftTop(int i, int j, string& ok) {
	if (i - 1 >= 0 && (battlefieldState[i][j] == "free" || battlefieldState[i ][j] == ok) && reachable[i - 1][j]) {
		reachable[i][j] = true;
	}
	else if (j - 1 >= 0 && (battlefieldState[i][j] == "free" || battlefieldState[i][j] == ok) && reachable[i][j - 1]) {
		reachable[i][j] = true;
	}
}
void checkBottomRight(int i, int j, string& ok) {
	if (i + 1 < h && (battlefieldState[i][j] == "free" || battlefieldState[i][j] == ok) && reachable[i + 1][j]) {
		reachable[i][j] = true;
	}
	else if (j + 1 < w && (battlefieldState[i][j] == "free" || battlefieldState[i][j] == ok) && reachable[i][j + 1]) {
		reachable[i][j] = true;
	}
}
bool makePath(int& dx, int& dy, int posx, int posy);
struct Unit {
	bool isRangeUnit = false;
	int attackRange;
	Particle bullet;
	bool skippedTurnThisRound = false;
	bool hasActive = false;
	int active;
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
	int defaultActiveCooldown;
	int iters;
	int stepMoveX;
	int stepMoveY;
	int cooldown;
	RectangleShape activeButton;
	vector<Passive> statuses = {};
	vector<Passive> passives_whenTurnStart_base = {};
	vector<Passive> passives_whenTurnStart = {};
	vector<Passive> passives_whenAttack_base = {};
	vector<Passive> passives_whenAttack = {};
private:
	String type;
public:
	String getType() {
		return type;
	}
	void setType(string& dano) {
		type = dano;
		if (type == "enemy") {
			txtt.loadFromFile("seal.jpg");
			txt.setScale(0.1, 0.1);
			txt.move(positionx * 150.f + 163.f, positiony * 150.f + 47.f);
		}
		if (type == "friend") {
			txtt.loadFromFile("images.jpg");
			txt.setScale(0.3, 0.3);
			txt.move(positionx * 150.f + 170.f, positiony * 150.f + 48.f);
		}
	}
	vector<int> Status_duration;//-1, если статуса нет, иначе рил длительность, i - id
	Unit() {
		attackRange = -1;
		isRangeUnit = false;
		skippedTurnThisRound = false;
		activeButton.setSize(Vector2f(300.f, 150.f));
		activeButton.setFillColor(Color::Blue);
		activeButton.setPosition(Vector2f(1615.f, 925.f));
		Status_duration = { -1,-1,-1,-1, -1, -1 };
		hasActive = false;
		active = -1;
		defaultActiveCooldown = -1;
		cooldown = 0;
	}
	Unit(int hpn, int msn, int dmgn, int posx, int posy, int ini) {
		attackRange = -1;
		isRangeUnit = false;
		skippedTurnThisRound = false;
		activeButton.setSize(Vector2f(300.f, 150.f));
		activeButton.setFillColor(Color::Blue);
		activeButton.setPosition(Vector2f(1615.f, 925.f));
		Status_duration = { -1,-1,-1,-1, -1, -1 };
		hasActive = false;
		active = -1;
		defaultActiveCooldown = -1;
		cooldown = 0;
		hp = hpn;
		ms = msn;
		dmg = dmgn;
		mindmg = max(0, dmgn - deltadmg);
		maxdmg = dmgn + deltadmg;
		positionx = posx;
		positiony = posy;
		initiative = ini;
		hpMax = hp;
	}
	void turnStartPassives() {
		for (int i = 0; i < passives_whenTurnStart.size(); i++) {
			passives_whenTurnStart[i].triggering(*this);
		}
	}
	void attackPassives( int targetx, int targety, vector<Unit>& zhertvi) {
		for (int i = 0; i < passives_whenAttack.size(); i++) {
			passives_whenAttack[i].triggering(*this, targetx, targety, zhertvi);
		}
	}
	void tick() {
		if (cooldown > 0) {
			cooldown--;
		}
		for (int i = 0; i < Status_duration.size(); i++) {
			if (Status_duration[i] >= 0) {
				statuses[i].tick(*this);
			}
		}
	}
	void useActive() {
		globalState = PGS::active_ability;
	}
	void takedmg(int tika) {
		hp -= tika;
		if (hp > hpMax) {
			hp = hpMax;
		}
		else if (hp <= 0) {
			alive = false;
			battlefieldState[positiony][positionx] = "free";
			hp = 1;
		}
	}
	void takedmg(int tika, bool barrier) {
		hp -= tika;
		if (hp > hpMax && !barrier) {
			hp = hpMax;
		}
		else if (hp <= 0) {
			alive = false;
			battlefieldState[positiony][positionx] = "free";
			hp = 1;
		}
	}
	void takedmg(int midmg, int madmg, bool barrier) {
		hp -= (midmg + (rand() / ((RAND_MAX + 1u) / (madmg - midmg + 1))));
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
			if (Status_duration[0] > 0) {
				txt.setColor(Color(0, 100, 0, 255));
			}
			wind.draw(txt);
			if (hasActive && globalState == PGS::free && !nowBotTurn && select != -1) {
				if (cooldown == 0) {
					activeButton.setFillColor(Color::White);
				}
				else {
					activeButton.setFillColor(Color::Black);
				}
				wind.draw(activeButton);
			}
			if (hasActive && globalState == PGS::active_ability) {
				activeButton.setFillColor(Color::Cyan);
				wind.draw(activeButton);
			}
		}
	}
	bool operator>(const Unit& another) {
		return initiative > another.initiative;
	}
	bool operator>=(const Unit& another) {
		return initiative >= another.initiative;
	}
	bool operator<(const Unit& another) {
		return initiative < another.initiative;
	}
	bool operator<=(const Unit& another) {
		return initiative <= another.initiative;
	}
	void move(int dx, int dy) {
		if (globalState != PGS::running) {
			nowtargetx = positionx + dx;
			nowtargety = positiony + dy;
		}
		globalState = PGS::animation;
		makePath(dx, dy, positionx, positiony);
		nowmovex = dx * 150.f;
		nowmovey = dy * 150.f;
		iters = max(abs(nowmovex) / 15, abs(nowmovey) / 15);
		if (iters != 0) {
			stepMoveX = nowmovex / iters;
			stepMoveY = nowmovey / iters;
		}
		else {
			stepMoveX = 0;
			stepMoveY = 0;
		}	
		positionx += dx;
		positiony += dy;
	}
	void move() {
		iters--;
		txt.move(stepMoveX, stepMoveY);
		if (iters <= 0) {
			globalState = PGS::running;
			if (positionx == nowtargetx && positiony == nowtargety) {
				globalState = PGS::free;
				select = -1;
				nowtargetx = -1;
				nowtargety = -1;
				if (!nowBotTurn) {
					battlefieldState[positiony][positionx] = "friend";
				} else {
					battlefieldState[positiony][positionx] = "enemy";
				}
			}
		}
	}
	void attack(int dx, int dy) {
		globalState = PGS::damage;
		if (!isRangeUnit) {
			nowmovex = dx * 150.f;
			nowmovey = dy * 150.f;
			iters = max(abs(nowmovex) / 15, abs(nowmovey) / 15);
			if (iters != 0) {
				stepMoveX = nowmovex / iters;
				stepMoveY = nowmovey / iters;
			}
			else {
				stepMoveX = 0;
				stepMoveY = 0;
			}
			positionx += dx;
			positiony += dy;
		} else {
			if (nowBotTurn) {
				bullet.draw(positionx + dx, positiony + dy, they);
			} else {
				bullet.draw(positionx + dx, positiony + dy, us);
			}
		}
	}
	void attack(Unit& zhertva, vector<Unit>& attackers, vector<Unit>& zhertvi) {
		if (!isRangeUnit) {
			iters--;
			txt.move(stepMoveX, stepMoveY);
			if (iters <= 0) {
				attackers[select].attackPassives(zhertvi[damaged].positionx, zhertvi[damaged].positiony, zhertvi);
				zhertva.takedmg(dmg);
				damaged = -1;
				select = -1;
				globalState = PGS::free;
			}
		} else {
			bullet.draw(false);
			if (globalState == PGS::dealing_damage) {
				attackers[select].attackPassives(zhertvi[damaged].positionx, zhertvi[damaged].positiony, zhertvi);
				zhertva.takedmg(dmg);
				damaged = -1;
				select = -1;
				globalState = PGS::free;
			}
		}
	}
};

void buildReachable(string ok = "") {
	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			reachable[i][j] = false;
		}
	}
	Unit& obj = nowBotTurn ? they[select] : us[select];
	int x = obj.positionx;
	int y = obj.positiony;
	reachable[y][x] = true;
	for (int i = y; i < h; i++) {
		for (int j = x; j < w; j++) {
			checkLeftTop(i, j, ok);
		}
	}
	for (int i = h - 1; i >= y; i--) {
		for (int j = w - 1; j >= x; j--) {
			checkBottomRight(i, j, ok);
		}
	}
	for (int i = y; i >= 0; i--) {
		for (int j = x; j < w; j++) {
			checkLeftTop(i, j, ok);
		}
	}
	for (int i = 0; i <= y; i++) {
		for (int j = w - 1; j >= x; j--) {
			checkBottomRight(i, j, ok);
		}


	}
	for (int i = y; i >= 0; i--) {
		for (int j = x; j >= 0; j--) {
			checkBottomRight(i, j, ok);
		}
	}
	for (int i = 0; i <= y; i++) {
		for (int j = 0; j <= x; j++) {
			checkLeftTop(i, j, ok);
		}
	}
	for (int i = y; i < h; i++) {
		for (int j = x; j >= 0; j--) {
			checkBottomRight(i, j, ok);
		}
	}
	for (int i = h - 1; i >= y; i--) {
		for (int j = 0; j <= x; j++) {
			checkLeftTop(i, j, ok);
		}
	}
}
bool makePath(int& dx, int& dy, int posx, int posy) {
	if (path.empty()) {
		for (int i = 0; i < h; i++) {
			for (int j = 0; j < w; j++) {
				was[i][j] = false;
			}
		}
		buildReachable();
		dx = 0;
		dy = 0;
		while (posx != nowtargetx || posy != nowtargety) {
			was[posy][posx] = true;
			if (nowtargetx > posx && posx + 1 < w && reachable[posy][posx + 1] && !was[posy][posx + 1]) {
				path.push_back({ 1, 0 });
				posx++;
				continue;
			}
			if (nowtargetx < posx && posx - 1 >= 0 && reachable[posy][posx - 1] && !was[posy][posx - 1]) {
				path.push_back({ -1, 0 });
				posx--;
				continue;
			}
			if (nowtargety > posy && posy + 1 < h && reachable[posy + 1][posx] && !was[posy + 1][posx]) {
				path.push_back({ 0, 1 });
				posy++;
				continue;
			}
			if (nowtargety < posy && posy - 1 >= 0 && reachable[posy - 1][posx] && !was[posy - 1][posx]) {
				path.push_back({ 0, -1 });
				posy--;
				continue;
			}


			if (posx + 1 < w && reachable[posy][posx + 1] && !was[posy][posx + 1]) {
				path.push_back({ 1, 0 });
				posx++;
				continue;
			}
			if (posy + 1 < h && reachable[posy + 1][posx] && !was[posy + 1][posx]) {
				path.push_back({ 0, 1 });
				posy++;
				continue;
			}
			if (posx - 1 >= 0 && reachable[posy][posx - 1] && !was[posy][posx - 1]) {
				path.push_back({ -1, 0 });
				posx--;
				continue;
			}
			if (posy - 1 >= 0 && reachable[posy - 1][posx] && !was[posy - 1][posx]) {
				path.push_back({ 0, -1 });
				posy--;
				continue;
			}

			posx -= path.back().first;
			posy -= path.back().second;
			path.pop_back();
			if (was[posy][posx + 1] && was[posy][posx - 1] && was[posy + 1][posx] && was[posy - 1][posx]) {
				return false;
			}	
		}
		return true;
	}
	dx = path.back().first;
	dy = path.back().second;
	path.pop_back();
	return true;
}
bool contMouse(RectangleShape& rect) {
	return rect.getGlobalBounds().contains(Mouse::getPosition().x, Mouse::getPosition().y);
}
int sqr(int a) {
	return a * a;
}
int pythagor(int a, int b) {
	return sqr(a) + sqr(b);
}
bool isEnemyNear(Unit& owner) {
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
		if (x + hodi[i].first >= 0 && x + hodi[i].first < w && y + hodi[i].second >= 0 && y + hodi[i].second < h && battlefieldState[y + hodi[i].second][x + hodi[i].first] == searching) {
			return true;
		}
	}
	return false;
}

void StatBlock::applyDelta(Unit& owner) {
	owner.ms = max(0, owner.ms + msChange);
	owner.dmg = max(0, owner.dmg + dmgChange);
	//owner.hpMax = max(0,owner.hpMax + hpmaxChange);
	owner.initiative = max(0, owner.initiative + initiativeChange);
}
void StatBlock::disapplyDelta(Unit& owner) {
	owner.ms = max(0, owner.ms - msChange);
	owner.dmg = max(0, owner.dmg - dmgChange);
	//owner.hpMax =  max(0, owner.hpMax - hpmaxChange);
	owner.initiative = max(0, owner.initiative - initiativeChange);
}
void StatBlock::tick(Unit& owner, int id) {
	owner.takedmg(tickdmg);
	owner.Status_duration[id]--;
	if (owner.Status_duration[id] == 0) {
		this->disapplyDelta(owner);
		owner.Status_duration[id] = -1;
	}
}
StatBlock::StatBlock(int ms, int dmg, int hpmax, int tickdmgA, int initi) {
	msChange = ms;
	dmgChange = dmg;
	hpmaxChange = hpmax;
	tickdmg = tickdmgA;
	initiativeChange = initi;
}

void Passive::triggering(Unit& owner, int targetx, int targety, vector<Unit>& zhertvi) {
	bool possible = condition(owner);
	if (possible && type == "ability" && target == "area") {
		//тут будет срабатывание на всех в области, но я ленивый (пока что)
		return;
	}
	if (possible && type == "ability" && target == "self") {
		for (int i = 0; i < effects_friends.size(); i++) {
			if (owner.Status_duration[effects_friends[i].id] == -1) {
				owner.Status_duration[effects_friends[i].id] = time;
				effects_friends[i].apply(owner);
			}
			else {
				owner.Status_duration[effects_friends[i].id] += time;
			}
		}
		return;
	}
	if (possible && type == "ability" && target == "attacked") {
		for (int i = 0; i < zhertvi.size(); i++) {
			if (zhertvi[i].positionx == targetx && zhertvi[i].positiony == targety && zhertvi[i].alive == true) {
				for (int j = 0; j < effects_enemy.size(); j++) {
					if (zhertvi[i].Status_duration[effects_enemy[i].id] == -1) {
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
void Passive::triggering(Unit& owner) {
	bool possible = condition(owner);
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
void Passive::apply(Unit& owner) {
	if (type == "status") {
		if (owner.Status_duration[id] == -1) {
			friendStats.applyDelta(owner);
			owner.Status_duration[id] = time;
		}
		else {
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
void Passive::tick(Unit& owner) {
	if (type == "status") {
		friendStats.tick(owner, id);
		return;
	}
}

void Particle::draw(int targetx, int targety, vector<Unit>& us) {
	globalState = PGS::particle_flying;
	nowmovex = (targetx - us[select].positionx) * 150.f;
	nowmovey = (targety - us[select].positiony) * 150.f;
	nowmovex += deltaX;
	nowmovey += deltaY;
	iters = max(abs(nowmovex) / 15, abs(nowmovey) / 25);
	stepSize = (endSize - startSize) / iters;
	stepMoveX = nowmovex / iters;
	stepMoveY = nowmovey / iters;
	txt.setSize(Vector2f(startSize, startSize));
	txt.setOrigin(Vector2f(startSize / 2, startSize / 2));
	txt.setPosition(us[select].txt.getPosition());
}
void Particle::draw(bool isActive) {
	txt.move(stepMoveX, stepMoveY);
	iters--;
	if (iters == 0) {
		if (isActive) {
			globalState = PGS::activating_active;
		} else {
			globalState = PGS::dealing_damage;
		}
	}
	txt.setSize(txt.getSize() + Vector2f(stepSize, stepSize));
}


struct ActiveAbility {
	Particle txt;
	int id;
	int range;
	int area;//радиус от клетки таргета (те для три на три area = 1)
	string target;//can be:earth, friend, enemy, nontarget
	vector<Passive> removedStatus = {};
	vector<Passive> addedStatus = {};
	int hpChange;
	bool isBarrier = false;

	bool operator==(ActiveAbility& another) {
		return id == another.id;
	}
	bool operator==(int& another) {
		return id == another;
	}
	void activate(int targetx, int targety, vector<Unit>& us) {
		txt.draw(targetx, targety, us);
		nowtargetx = targetx;
		nowtargety = targety;
	}
	void activate(Unit& owner) {
		if (target == "earth") {
			for (int y = max(0, nowtargety - area); y < min(h, nowtargety + area); y++) {
				for (int x = max(0, nowtargetx - area); x < min(w, nowtargetx + area); x++) {
					for (int i = 0; i < they.size(); i++) {
						if (they[i].alive == true && they[i].positionx == x && they[i].positiony == y) {
							they[i].takedmg(hpChange, isBarrier);
						}
					}
					for (int i = 0; i < us.size(); i++) {
						if (us[i].alive == true && us[i].positionx == x && us[i].positiony == y) {
							us[i].takedmg(hpChange, isBarrier);
						}
					}
				}
			}
			nowtargetx = -1;
			nowtargety = -1;
			select = -1;
			globalState = PGS::free;
		}
	}
};

vector<ActiveAbility> actives;

void chooseSelect(int& pointer_us, int& pointer_they) {
	if (pointer_they >= they.size() && pointer_us >= us.size()) {
		pointer_they = 0;
		pointer_us = 0;
		for (size_t i = 0; i < us.size(); i++) {
			us[i].skippedTurnThisRound = false;
		}
	}
	else if (pointer_they >= they.size() && pointer_us < us.size()) {
		nowBotTurn = false;
		select = pointer_us;
		pointer_us++;
	}
	else if (pointer_us >= us.size() && pointer_they < they.size()) {
		nowBotTurn = true;
		select = pointer_they;
		pointer_they++;
	}
	else if (they[pointer_they] > us[pointer_us]) {
		nowBotTurn = true;
		select = pointer_they;
	}
	else if (they[pointer_they] <= us[pointer_us]) {
		nowBotTurn = false;
		select = pointer_us;
		pointer_us++;
	}
	else {
		nowBotTurn = true;
		select = pointer_they;
		pointer_they++;
	}
}
void activateTurnStartOfSelected() {
	if (!nowBotTurn) {
		us[select].tick();
		us[select].turnStartPassives();
	} else{
		they[select].tick();
		they[select].turnStartPassives();
	}
}
bool usAttackCondition(int i, vector<vector<RectangleShape>>& battlefield) {
	bool inRange;
	if (!us[select].isRangeUnit) {
		inRange = pythagor(they[i].positiony - us[select].positiony, they[i].positionx - us[select].positionx) <= sqr(us[select].ms + 1);
	} else {
		inRange = pythagor(they[i].positiony - us[select].positiony, they[i].positionx - us[select].positionx) <= sqr(us[select].attackRange);
	}
	bool buttonPressed = contMouse(battlefield[they[i].positiony][they[i].positionx]);
	return inRange && buttonPressed;
}
bool moveCondition(vector<vector<RectangleShape>>& battlefield, int i, int j) {
	buildReachable();
	bool inReach = reachable[i][j];
	return contMouse(battlefield[i][j]) && pythagor(i - us[select].positiony, j - us[select].positionx) <= sqr(us[select].ms) && battlefieldState[i][j] == "free" && inReach;
}
bool activeCondition() {
	return us[select].hasActive && contMouse(us[select].activeButton) && us[select].cooldown == 0;
}
bool weCanInteract(Clock& rattle) {
	return globalState == PGS::free && select != -1 && rattle.getElapsedTime().asMilliseconds() >= 500 && Mouse::isButtonPressed(Mouse::Left);
}
void usChoosePositionToAttack(int& targetx, int& targety, int i) {
	if (they[i].positionx > us[select].positionx) {
		targetx--;
	}
	else if (they[i].positionx < us[select].positionx) {
		targetx++;
	}
	else if (us[select].positiony < they[i].positiony) {
		targety--;
	}
	else if (us[select].positiony > they[i].positiony) {
		targety++;
	}
}
void paintDefaultBattlefield(vector<vector<RectangleShape>>& battlefield) {
	for (int inow = 0; inow < h; inow++) {
		for (int jnow = 0; jnow < w; jnow++) {
			battlefield[inow][jnow].setFillColor(Color::Transparent);
		}
	}
}
bool skipTurnCondition(RectangleShape& skipTurnButton) {
	return !us[select].skippedTurnThisRound && contMouse(skipTurnButton);
}
void usPossibleActions(Clock& rattle, vector<vector<RectangleShape>>& battlefield, RectangleShape& skipTurnButton) {
	if (skipTurnCondition(skipTurnButton)) {
		us[select].skippedTurnThisRound = true;
		select = -1;
		rattle.restart();
		return;
	}
	for (int i = 0; i < they.size(); i++) {
		if (usAttackCondition(i,  battlefield)) {
			int targetx = they[i].positionx - us[select].positionx;
			int targety = they[i].positiony - us[select].positiony;
			if (!us[select].isRangeUnit) {
				battlefieldState[us[select].positiony][us[select].positionx] = "free";
				usChoosePositionToAttack(targetx, targety, i);
				battlefieldState[us[select].positiony + targety][us[select].positionx + targetx] = "friend";
			}
			us[select].attack(targetx, targety);
			damaged = i;
			rattle.restart();
			return;
		}
	}
	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			if (moveCondition(battlefield, i, j)) {
				battlefieldState[us[select].positiony][us[select].positionx] = "free";
				us[select].move(j - us[select].positionx, i - us[select].positiony);
				rattle.restart();
				return;
			}
		}
	}
	if (activeCondition()) {
		us[select].useActive();
		paintDefaultBattlefield(battlefield);
		us[select].cooldown = us[select].defaultActiveCooldown;
		rattle.restart();
		return;
	}
}
bool continueAttackAnimationCondition(Clock& rattle) {
	return damaged != -1 && select != -1 && globalState == PGS::damage && rattle.getElapsedTime().asMilliseconds() >= 10;
}
bool continueMoveAnimationCondition(Clock& rattle) {
	return select != -1 && globalState == PGS::animation && rattle.getElapsedTime().asMilliseconds() >= 10;
}
bool cancelAbilityCondition(Clock& rattle) {
	return select != -1 && us[select].hasActive && contMouse(us[select].activeButton) && Mouse::isButtonPressed(Mouse::Left) && globalState == PGS::active_ability && rattle.getElapsedTime().asMilliseconds() >= 500;
}
bool continueParticleAnimationCondition(Clock& rattle) {
	return globalState == PGS::particle_flying && rattle.getElapsedTime().asMilliseconds() >= 10 && damaged == -1;
}
bool activatingActiveCondition() {
	return globalState == PGS::activating_active && nowtargetx != -1 && nowtargety != -1;
}
void paintActiveSight(vector<vector<RectangleShape>>& battlefield, int i, int j) {
	battlefield[i][j].setFillColor(Color(255, 0, 0, 100));
	int minx = max(0, j - actives[us[select].active].area);
	int miny = max(0, i - actives[us[select].active].area);
	int maxx = min(10, j + actives[us[select].active].area);
	int maxy = min(5, i + actives[us[select].active].area);
	for (int inow = 0; inow < h; inow++) {
		for (int jnow = 0; jnow < w; jnow++) {
			if (inow<miny || inow>maxy || jnow<minx || jnow>maxx) {
				battlefield[inow][jnow].setFillColor(Color::Transparent);
			}
			else {
				battlefield[inow][jnow].setFillColor(Color(255, 0, 0, 100));
			}
		}
	}
}
bool legalActiveTarget(vector<vector<RectangleShape>>& battlefield, Clock& rattle, int i, int j) {
	bool goodTime = rattle.getElapsedTime().asMilliseconds() >= 500 && globalState == PGS::active_ability && select != -1;
	if (!goodTime) {
		return false;
	}
	bool goodPosition = contMouse(battlefield[i][j]) && pythagor(us[select].positionx - j, us[select].positiony - i) <= sqr(actives[us[select].active].range);
	return goodTime && goodPosition && Mouse::isButtonPressed(Mouse::Left);
}
void paintBattlefieldWithMoveRange(vector<vector<RectangleShape>>& battlefield, int i, int j) {
	bool inReach = reachable[i][j];
	if (pythagor(i - us[select].positiony, j - us[select].positionx) <= sqr(us[select].ms) && inReach) {
		battlefield[i][j].setFillColor(Color(0, 255, 0, 100));
	} else if (!us[select].isRangeUnit && battlefieldState[i][j] == "enemy" && pythagor(i - us[select].positiony, j - us[select].positionx) <= sqr(us[select].ms) + 1 && inReach) {
		battlefield[i][j].setFillColor(Color(0, 255, 0, 100));
	} else if (us[select].isRangeUnit && battlefieldState[i][j] == "enemy" && pythagor(i - us[select].positiony, j - us[select].positionx) <= sqr(us[select].attackRange && inReach)) {
		battlefield[i][j].setFillColor(Color(0, 255, 0, 100));
	} else {
		battlefield[i][j].setFillColor(Color::Transparent);
	}
}
void colouriseBattlefieldUs(vector<vector<RectangleShape>>& battlefield) {
	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			if (select != -1 && globalState == PGS::free) {
				buildReachable("enemy");
				paintBattlefieldWithMoveRange(battlefield, i, j);
			}
			else {
				if (globalState == PGS::active_ability && select != -1) {
					if (contMouse(battlefield[i][j]) && pythagor(us[select].positionx - j, us[select].positiony - i) <= sqr(actives[us[select].active].range)) {
						paintActiveSight(battlefield, i, j);
					}
					else if (contMouse(battlefield[i][j]) && pythagor(us[select].positionx - j, us[select].positiony - i) > sqr(actives[us[select].active].range)) {
						paintDefaultBattlefield(battlefield);
					}
				}
			}
		}
	}
}
bool aiNeedToChoose() {
	return globalState == PGS::free && select != -1;
}
bool aiCanUseActive() {
	return they[select].hasActive && they[select].cooldown == 0;
}
bool somebodyWin(RenderWindow& window) {
	bool allEnemyAreDead = true;
	bool allFriendsAreDead = true;
	for (size_t i = 0; i < us.size(); i++) {
		if (us[i].alive) {
			allFriendsAreDead = false;
			break;
		}
	}
	for (size_t i = 0; i < they.size(); i++) {
		if (they[i].alive) {
			allEnemyAreDead = false;
			break;
		}
	}
	if (allEnemyAreDead && allFriendsAreDead) {
		cout << "FRIENDSHIP WON!!!!1!";
		window.close();
		return true;
	}
	if (allEnemyAreDead) {
		cout << "U WIN!!!!11!1!!!!";
		window.close();
		return true;
	}
	if (allFriendsAreDead) {
		cout << "Unluchka, unluchka((((";
		window.close();
		return true;
	}
	return false;
}
void findEarthActiveTarget(int& targetx, int& targety) {
	int betterValue = 0;
	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			if (pythagor(j - they[select].positionx, i - they[select].positiony) <= sqr(actives[they[select].active].range)) {
				int countValue = 0;
				for (int y = max(0, i - actives[they[select].active].area); y < min(5, i + actives[they[select].active].area); y++) {
					for (int x = max(0, j - actives[they[select].active].area); x < min(10, j + actives[they[select].active].area); x++) {
						if (battlefieldState[y][x] == "friend") {
							countValue++;
						}
						else if (battlefieldState[y][x] == "enemy") {
							countValue--;
						}
					}
				}
				if (countValue > betterValue) {
					targetx = j;
					targety = i;
				}
			}
		}
	}
}
bool hasTarget(int targetx, int targety) {
	return targetx != -1 && targety != -1;
}
void aiFindTargetToAttack(int& targetx, int& targety, int i) {
	targetx = us[i].positionx - they[select].positionx;
	targety = us[i].positiony - they[select].positiony;
	if (they[select].isRangeUnit) {
		return;
	}
	if (us[i].positionx > they[select].positionx) {
		targetx--;
	}
	else if (us[i].positionx < they[select].positionx) {
		targetx++;
	}
	else if (they[select].positiony < us[i].positiony) {
		targety--;
	}
	else if (they[select].positiony > us[i].positiony) {
		targety++;
	}
}
bool aiCanAttackThis(int i) {
	if (!they[select].isRangeUnit) {
		buildReachable();
		return us[i].alive && pythagor(us[i].positionx - they[select].positionx, us[i].positiony - they[select].positiony) <= sqr(they[select].ms) && reachable[us[i].positiony][us[i].positionx];
	}
	return us[i].alive && pythagor(us[i].positionx - they[select].positionx, us[i].positiony - they[select].positiony) <= sqr(they[select].attackRange);
}
bool aiDestinationBusy(int dx, int dy) {
	buildReachable();
	return battlefieldState[they[select].positiony + dy][they[select].positionx + dx] != "free" || !reachable[they[select].positiony + dy][they[select].positionx + dx];
}
void aiFindTargetToMove(int& dx, int& dy) {
	int minrastkv = -1;
	int tomove = -1;
	for (size_t i = 0; i < us.size(); i++) {
		int rastnow = pythagor(us[i].positionx - they[select].positionx, us[i].positiony - they[select].positiony);
		if ((rastnow < minrastkv || minrastkv == -1) && us[i].alive) {
			minrastkv = rastnow;
			tomove = i;
		}
		else if (us[i].alive && rastnow == minrastkv && us[tomove].dmg < us[i].dmg) {
			tomove = i;
		}
	}
	nowtargetx = us[tomove].positionx;
	nowtargety = us[tomove].positiony;
	int kraz = sqrt(minrastkv / (they[select].ms * they[select].ms));
	dx = (nowtargetx - they[select].positionx) / kraz;
	dy = (nowtargety - they[select].positiony) / kraz;
	if (pythagor(dx, dy) > sqr(they[select].ms)) {
		if (dx >= dy) {
			dx--;
		}
		else {
			dy--;
		}
	}
}
void aiActions() {
	if (aiCanUseActive()) {
		if (actives[they[select].active].target == "earth") {
			int targetx = -1;
			int targety = -1;
			findEarthActiveTarget(targetx, targety);
			if (hasTarget(targetx, targety)) {
				actives[they[select].active].activate(targetx, targety, they);
				return;
			}
		}
	}
	for (size_t i = 0; i < us.size(); i++) {
		if (aiCanAttackThis(i)) {
			int targetx, targety;
			aiFindTargetToAttack(targetx, targety, i);
			if (!they[select].isRangeUnit) {
				battlefieldState[they[select].positiony][they[select].positionx] = "free";
				battlefieldState[they[select].positiony + targety][they[select].positionx + targetx] = "enemy";
			}
			they[select].attack(targetx, targety);
			damaged = i;
			return;
		}
	}
	int dx, dy;
	aiFindTargetToMove(dx, dy);
	if (aiDestinationBusy(dx, dy)) {
		int stepx = (they[select].positionx - nowtargetx) / abs(they[select].positionx - nowtargetx);
		int stepy = (they[select].positiony - nowtargety) / abs(they[select].positiony - nowtargety);
		int i = 0;
		while (aiDestinationBusy(dx, dy)) {
			if (i % 2 == 0) {
				dx += stepx;
			}
			else {
				dy += stepy;
			}
			i++;
		}
	}
	battlefieldState[they[select].positiony][they[select].positionx] = "free";
	they[select].move(dx, dy);
	return;
}
bool continueBulletAnimationCondition(Clock& rattle) {
	return damaged != -1 && (globalState == PGS::particle_flying || globalState== PGS::dealing_damage) && rattle.getElapsedTime().asMilliseconds() >= 10; //mb select
}
bool inMovement() {
	return globalState == PGS::running;
}
void botTurn(Clock& rattle, vector<vector<RectangleShape>>& battlefield) {
	if (!they[select].alive) {
		select = -1;
	}
	if (aiNeedToChoose()) {
		aiActions();
	}
	if (inMovement()) {
		they[select].move(0, 0);
	}
	if (continueMoveAnimationCondition(rattle)) {
		they[select].move();
		rattle.restart();
	}
	if (continueBulletAnimationCondition(rattle)) {
		they[select].attack(us[damaged], they, us);
		rattle.restart();
	}
	if (continueParticleAnimationCondition(rattle)) {
		actives[they[select].active].txt.draw(true);
		rattle.restart();
	}
	if (activatingActiveCondition()) {
		actives[they[select].active].activate(us[select]);
	}
	if (continueAttackAnimationCondition(rattle)) {
		they[select].attack(us[damaged], they, us);
		rattle.restart();
	}
	paintDefaultBattlefield(battlefield);
}
void playerTurn(Clock& rattle, vector<vector<RectangleShape>>& battlefield, RectangleShape& skipTurnButton) {
	if (!us[select].alive) {
		select = -1;
	}
	if (weCanInteract(rattle)) {
		usPossibleActions(rattle, battlefield, skipTurnButton);
	}
	if (inMovement()) {
		us[select].move(0, 0);
	}
	if (continueAttackAnimationCondition(rattle)) {
		us[select].attack(they[damaged], us, they);
		rattle.restart();
	}
	if (continueMoveAnimationCondition(rattle)) {
		us[select].move();
		rattle.restart();
	}
	if (cancelAbilityCondition(rattle)) {
		globalState = PGS::free;
		rattle.restart();
		us[select].cooldown = 0;
	}
	if (continueParticleAnimationCondition(rattle)) {
		actives[us[select].active].txt.draw(true);
		rattle.restart();
	}
	if (continueBulletAnimationCondition(rattle)) {
		us[select].attack(they[damaged], us, they);
		rattle.restart();
	}
	if (activatingActiveCondition()) {
		actives[us[select].active].activate(us[select]);
	}
	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			if (legalActiveTarget(battlefield, rattle, i, j)) {
				actives[us[select].active].activate(j, i, us);
				rattle.restart();
			}
		}
	}
	colouriseBattlefieldUs(battlefield);
}

int main()
{
	Font mainFont;
	//if (!mainFont.loadFromFile("font.ttf")) {
	//	return EXIT_FAILURE;
	//}

	RectangleShape skipTurnButton;
	skipTurnButton.setSize(Vector2f(300.f, 150.f));
	skipTurnButton.setPosition(Vector2f(7.f, 925.f));
	skipTurnButton.setFillColor(Color::White);
	Particle ballOfFire;
	ballOfFire.deltaX = -165;
	ballOfFire.deltaY = -200;
	ballOfFire.startSize = 1;
	ballOfFire.endSize = 450;
	ballOfFire.txt = RectangleShape();
	ballOfFire.txt.setFillColor(Color::Yellow);
	ActiveAbility fireball;
	fireball.txt = ballOfFire;
	fireball.range = 5;
	fireball.addedStatus = {};
	fireball.removedStatus = {};
	fireball.target = "earth";
	fireball.hpChange = 4;
	fireball.area = 1;
	fireball.id = 0;
	StatBlock URPOISONED;
	URPOISONED.tickdmg = 1;
	Passive poisoned;
	poisoned.type = "status";
	poisoned.id = 0;
	poisoned.dmg = 0;
	poisoned.time = 3;
	poisoned.friendStats = URPOISONED;
	Passive poison_touch;
	poison_touch.id = 1;
	poison_touch.trigger = "attack";
	poison_touch.target = "attacked";
	poison_touch.effects_enemy.push_back(poisoned);
	poison_touch.type = "ability";
	Passive running;
	running.id = 2;
	running.type = "status";
	running.time = 1;
	StatBlock runningStats;
	runningStats.msChange = 2;
	running.friendStats = runningStats;
	runningStats.tickdmg = 0;
	Passive runrunrun; ;
	runrunrun.condition = isEnemyNear;
	runrunrun.id = 3;
	runrunrun.effects_friends.push_back(running);
	runrunrun.trigger = "turn start";
	runrunrun.type = "ability";
	runrunrun.target = "self";
	runrunrun.time = 1;
	statuses.push_back(poisoned);
	statuses.push_back(poison_touch);
	statuses.push_back(running);
	statuses.push_back(runrunrun);
	actives.push_back(fireball);

	//srand(time(nullptr));//задаём сид для генерации случайных чисел (что это и почему это желательно сделать можно почитать на cppreferences)

	RenderWindow window(VideoMode(1920, 1080), L"Охота на Ликантропа: драка на поляне", Style::Default);
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

	vector<vector<RectangleShape>> battlefield(h, vector<RectangleShape>(w));
	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			RectangleShape scytheofvyse(Vector2f(150.f, 150.f));
			scytheofvyse.setOutlineColor(Color::Blue);
			scytheofvyse.setOutlineThickness(2.f);
			scytheofvyse.setFillColor(Color::Transparent);
			scytheofvyse.move(j * 150.f + 135.f, i * 150.f + 5);
			battlefield[i][j] = scytheofvyse;
		}
	}

	battlefieldState[1][0] = "wall";
	battlefieldState[0][1] = "wall";


	Unit scytheofvyse(2, 3, 1, 2, 2, 1);
	string mysticStaff = "friend";
	scytheofvyse.setType(mysticStaff);
	battlefieldState[2][2] = "friend";
	scytheofvyse.passives_whenAttack.push_back(poison_touch);
	scytheofvyse.statuses = statuses;
	scytheofvyse.hasActive = true;
	scytheofvyse.active = 0;
	scytheofvyse.defaultActiveCooldown = 3;
	//scytheofvyse.isRangeUnit = true;
	//scytheofvyse.attackRange = 5;
	//Particle bul;
	//bul.txt = RectangleShape();
	//bul.txt.setFillColor(Color::Red);
	//bul.startSize = 20;
	//bul.endSize = 20;
	//scytheofvyse.bullet = bul;
	us.push_back(scytheofvyse);

	scytheofvyse = Unit(4, 2, 0, 3, 2, 1);
	mysticStaff = "enemy";
	//scytheofvyse.hasActive = true;
	//scytheofvyse.active = 0;
	//scytheofvyse.defaultActiveCooldown = 3;
	scytheofvyse.setType(mysticStaff);
	scytheofvyse.isRangeUnit = true;
	scytheofvyse.attackRange = 5;
	Particle bul;
	bul.deltaX = 30;
	bul.deltaY = 30;
	bul.txt = RectangleShape();
	bul.txt.setFillColor(Color::Red);
	bul.startSize = 20;
	bul.endSize = 20;
	scytheofvyse.bullet = bul;
	battlefieldState[2][3] = "enemy";
	scytheofvyse.statuses = statuses;
	they.push_back(scytheofvyse);

	sort(us.begin(), us.end());
	sort(they.begin(), they.end());
	int pointer_they = 0;
	int pointer_us = 0;

	while (window.isOpen())
	{
		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed) {
				window.close();
			}
		}
		if (somebodyWin(window)) {
			window.close();
			break;
		}
		if (select == -1) {
			while (select == -1) {
				chooseSelect(pointer_us, pointer_they);
			}
			activateTurnStartOfSelected();
		}
		if (nowBotTurn) {
			botTurn(rattle, battlefield);
		} else {
			playerTurn(rattle, battlefield, skipTurnButton);
		}
		if (select != -1 && us[select].skippedTurnThisRound) {
			skipTurnButton.setFillColor(Color::Black);
		}
		else if (select != -1 && !us[select].skippedTurnThisRound) {
			skipTurnButton.setFillColor(Color::White);
		}
		window.draw(bg);
		for (int i = 0; i < h; i++) {
			for (int j = 0; j < w; j++) {
				window.draw(battlefield[i][j]);
			}
		}
		for (int i = 0; i < us.size(); i++) {
			us[i].draw(window);
		}
		for (int i = 0; i < they.size(); i++) {
			they[i].draw(window);
		}
		if (globalState == PGS::particle_flying && damaged == -1) {
			window.draw(actives[us[select].active].txt.txt);
		}
		if (globalState == PGS::particle_flying && damaged != -1) {
			if (!nowBotTurn) {
				window.draw(us[select].bullet.txt);
			} else {
				window.draw(they[select].bullet.txt);
			}
		}
		window.draw(skipTurnButton);
		window.display();
	}
}
