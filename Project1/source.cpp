#include <SFML/Graphics.hpp>
#include<vector>
#include<iostream>
using namespace sf;
using namespace std;
struct player {
    int dvig = 0;
    Texture txt1;
    Texture txt2;
    Sprite spr;
 /*   player(Texture t1, Texture t2) {
        txt1 = t1;
        txt2 = t2;               Данный код более уместен в seal1/2
        spr.setTexture(txt1);
    }
    player() {
    
    }*/
    /*player(const player& obj) {
        txt1 = obj.txt1;
        txt2 = obj.txt2;     Данный код не работает по необъяснимым мне причиснам, так что я сделал через костыли
        spr.setTexture(txt1);
    }*/  

    Clock rattler;
    int nowtxt = 1; 
    void check() {

        if (dvig > 0 && nowtxt == 1) {
            spr.move(2, 0);
            dvig--;
            cout << dvig << endl;
        }
        else if (dvig > 0 && nowtxt==2) {
            spr.move(-2, 0);
            dvig--;
        }
        else if (((Mouse::isButtonPressed(Mouse::Left) && (Mouse::getPosition().x > spr.getPosition().x && Mouse::getPosition().x < spr.getPosition().x + txt1.getSize().x && Mouse::getPosition().y > spr.getPosition().y && Mouse::getPosition().y < spr.getPosition().y + txt1.getSize().y)) && dvig == 0) &&  nowtxt==1){  
                dvig = 50;  
        }else if (((Mouse::isButtonPressed(Mouse::Left) && (Mouse::getPosition().x > spr.getPosition().x && Mouse::getPosition().x < spr.getPosition().x + txt1.getSize().x && Mouse::getPosition().y > spr.getPosition().y && Mouse::getPosition().y < spr.getPosition().y + txt1.getSize().y)) && dvig == 0) && nowtxt == 2) {
    
                dvig = 50;
         
        } else if (((Mouse::isButtonPressed(Mouse::Right) && (Mouse::getPosition().x > spr.getPosition().x && Mouse::getPosition().x < spr.getPosition().x + txt1.getSize().x && Mouse::getPosition().y > spr.getPosition().y && Mouse::getPosition().y < spr.getPosition().y + txt1.getSize().y))) && nowtxt == 2) {
            if (rattler.getElapsedTime().asSeconds() > 1) {
                nowtxt = 1;
                rattler.restart();
            }
        }
        else if (((Mouse::isButtonPressed(Mouse::Right) && (Mouse::getPosition().x > spr.getPosition().x && Mouse::getPosition().x < spr.getPosition().x + txt1.getSize().x && Mouse::getPosition().y > spr.getPosition().y && Mouse::getPosition().y < spr.getPosition().y + txt1.getSize().y))) && nowtxt == 1) {
            if (rattler.getElapsedTime().asSeconds() > 1) {
                nowtxt = 2;
                rattler.restart();
            }
        }
        
    }
    void draw(RenderWindow& win) {
        if (nowtxt == 1) {
            spr.setTexture(txt1);
        }
        else {
            spr.setTexture(txt2);
        }
        win.draw(spr);
    }
    pair<Texture, Texture> gettxts() const {
        return { txt1, txt2 };
    }
};
struct seal1 : public player {    
    seal1() {
        txt1.loadFromFile("seal.jpg");
        txt2.loadFromFile("images.jpg");
    }
};
struct seal2 : public player {
    seal2() {
        txt1.loadFromFile("seal.jpg");
        txt2.loadFromFile("images.jpg");
        nowtxt = 2;
    }
};

int main()
{
    RenderWindow window(VideoMode(1500, 1000), L"Новый проект", Style::Fullscreen);
    window.setVerticalSyncEnabled(true);

    Texture tul;
    Texture frs;
    Texture sect;
    if (!tul.loadFromFile("seal.jpg") || !frs.loadFromFile("forest.jpg") || !sect.loadFromFile("images.jpg")) {
        return EXIT_FAILURE;
    }   
        tul.loadFromFile("seal.jpg");
        frs.loadFromFile("forest.jpg");
        sect.loadFromFile("images.jpg");
        vector<player> seals;
        sf::Sprite bg;
        bg.setTexture(frs);
        Vector2u razm = window.getSize();
        bg.setScale(2,2);
        int dvig = 0;
        sf::RectangleShape create1(Vector2f(100,100));
        create1.setFillColor(Color::Cyan);
        create1.setPosition(0, window.getSize().y - 100);
        sf::RectangleShape create2(Vector2f(100, 100));
        create2.setFillColor(Color::Cyan);
        create2.setPosition(window.getSize().x - 100, window.getSize().y - 100);
        Clock rat;
        while (window.isOpen())
        {
            Event event;
            while (window.pollEvent(event))
            {
                if (event.type == sf::Event::Closed) {
                    window.close();
                }
            }
            if (Mouse::isButtonPressed(Mouse::Left) && Mouse::getPosition().x<100 && Mouse::getPosition().y >  window.getSize().y - 100 && rat.getElapsedTime().asSeconds() > 1) {
                seals.push_back(seal1());
                rat.restart();
                cout << seals.size() << endl;
            }
            else if (Mouse::isButtonPressed(Mouse::Left) && Mouse::getPosition().x > window.getSize().x - 100 && Mouse::getPosition().y > window.getSize().y - 100 && rat.getElapsedTime().asSeconds() > 1) {
                seals.push_back(seal2());
                rat.restart();
            }
            window.draw(bg);
            window.draw(create1);
            window.draw(create2);
            for (int i = 0; i < seals.size();i++) {
                seals[i].check();
                seals[i].draw(window);
            }
            window.display();
        }

}