// lab8_brockmanRachel_cgt215.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFPhysics.h>
#include <vector>

using namespace std;
using namespace sf;
using namespace sfp;

const float KB_SPEED = 0.2;
const int MAX_SHOTS = 5;

void LoadTex(Texture& tex, string filename) {
    if (!tex.loadFromFile(filename)) {
        cout << "Could not load " << filename << endl;
    }
}

Vector2f GetTextSize(const Text& text) {
    FloatRect r = text.getGlobalBounds();
    return Vector2f(r.width, r.height);
}

int main() {
    RenderWindow window(VideoMode(800, 600), "Duck Hunter");
    World world(Vector2f(0, 0));
    int score = 0;
    int arrows = MAX_SHOTS;
    bool gameEnded = false;

    PhysicsSprite crossbow;
    Texture cbowTex;
    LoadTex(cbowTex, "crossbow.png");
    crossbow.setTexture(cbowTex);
    crossbow.setCenter(Vector2f(400, 550));

    PhysicsSprite arrow;
    Texture arrowTex;
    LoadTex(arrowTex, "arrow.png");
    arrow.setTexture(arrowTex);
    bool drawingArrow = false;

    PhysicsRectangle rightWall;
    rightWall.setSize(Vector2f(10, 600));
    rightWall.setCenter(Vector2f(795, 300));
    rightWall.setStatic(true);
    world.AddPhysicsBody(rightWall);

    PhysicsRectangle topWall;
    topWall.setSize(Vector2f(800, 10));
    topWall.setCenter(Vector2f(400, 5));
    topWall.setStatic(true);
    world.AddPhysicsBody(topWall);

    Texture duckTex;
    LoadTex(duckTex, "duck.png");
    PhysicsShapeList<PhysicsSprite> ducks;
    int duckSpawnTimer = 2000;
    Clock spawnClock;

    rightWall.onCollision = [&ducks, &world](PhysicsBodyCollisionResult result) {
        for (PhysicsSprite& duck : ducks) {
            if (result.object == duck) {
                world.RemovePhysicsBody(duck);
                ducks.QueueRemove(duck);
            }
        }
        };

    topWall.onCollision = [&drawingArrow, &world, &arrow](PhysicsBodyCollisionResult result) {
        if (result.object == arrow) {
            drawingArrow = false;
            world.RemovePhysicsBody(arrow);
        }
        };

    Font fnt;
    if (!fnt.loadFromFile("arial.ttf")) {
        cout << "Could not load font." << endl;
        return -1;
    }

    Clock clock;
    Time lastTime = clock.getElapsedTime();
    Time currentTime;

    while (window.isOpen()) {
        currentTime = clock.getElapsedTime();
        Time deltaTime = currentTime - lastTime;
        long deltaMS = deltaTime.asMilliseconds();

        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) {
                window.close();
            }
            if (event.type == Event::KeyPressed && gameEnded && event.key.code == Keyboard::Space) {
                window.close();
            }
        }

        if (gameEnded) {
            window.clear();
            Text gameOverText("GAME OVER", fnt, 30);
            int sz = GetTextSize(gameOverText);
            gameOverText.setPosition(400 - (sz.x / 2), 300 - (sz.y / 2));
            window.draw(gameOverText);
            window.display();
            continue;
        }

        if (deltaMS > 9) {
            lastTime = currentTime;
            world.UpdatePhysics(deltaMS);

            if (Keyboard::isKeyPressed(Keyboard::Space) && !drawingArrow && arrows > 0) {
                drawingArrow = true;
                arrows--;
                arrow.setCenter(crossbow.getCenter());
                arrow.setVelocity(Vector2f(0, -1)); 
                world.AddPhysicsBody(arrow);
            }

    
            if (spawnClock.getElapsedTime().asMilliseconds() >= duckSpawnTimer) {
                spawnClock.restart();
                PhysicsSprite& duck = ducks.Create();
                duck.setTexture(duckTex);
                Vector2f sz = duck.getSize();
                duck.setCenter(Vector2f(-sz.x / 2, 50 + (sz.y / 2)));
                duck.setVelocity(Vector2f(0.15, 0));
                world.AddPhysicsBody(duck);

                duck.onCollision = [&drawingArrow, &world, &arrow, &duck, &ducks, &score](PhysicsBodyCollisionResult result) {
                    if (result.object2 == arrow) {
                        drawingArrow = false;
                        world.RemovePhysicsBody(arrow);
                        world.RemovePhysicsBody(duck);
                        ducks.QueueRemove(duck);
                        score += 10;
                    }
                    };
            }

            window.clear();

   
            if (drawingArrow) {
                window.draw(arrow);
            }

    
            ducks.DoRemovals();

            for (PhysicsShape& duck : ducks) {
                window.draw((PhysicsSprite&)duck);
            }

            window.draw(crossbow);

         
            Text scoreText("Score: " + to_string(score), fnt, 20);
            scoreText.setPosition(Vector2f(790 - GetTextSize(scoreText).x, 580));
            window.draw(scoreText);

            Text arrowCountText("Arrows: " + to_string(arrows), fnt, 20);
            arrowCountText.setPosition(Vector2f(10, 580));
            window.draw(arrowCountText);

            window.display();

            if (arrows <= 0 && !drawingArrow) {
                gameEnded = true;
            }
        }
    }
    return 0;
}


// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
