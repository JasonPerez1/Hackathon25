//AIControlledGame.hpp
#pragma once
#include <SFML/Graphics.hpp>
#include "GeneticAlgorithm.hpp"  // Replace forward declaration
#include "NeuralNetwork.hpp"     // Replace forward declaration
#include <vector>

// Forward declarations
struct Individual;

struct Platform {
    sf::Vector2f position;
    sf::Vector2f size;
};


class NeuralNetwork;

class AIControlledGame {
public: 

    // Add ground sprites like in the example
    sf::Sprite ground1;
    sf::Sprite ground2;
    AIControlledGame(bool createWindow = true);
    void runSimulation(Individual& individual, bool render = false);
    ~AIControlledGame() {
        if (window) {
            window->close();
            delete window;
            window = nullptr;
        }
    }
    bool hasJumped = false;   // True if Mario has already jumped since the last landing.
    bool wasOnGround = false; // To detect the landing event.
    float jumpedfrom = 0;
private:

    float calculateVerticalDistanceToGround() const;
    void reset();
    std::vector<double> getInputs() const;
    void aiControl();
    void handlePhysics();
    void handleCollisions();
    void handleCoinCollection();
    void updateView(bool render);
    void updatePlatforms();
    void initializePlatforms();
    sf::Font font;
    sf::Text timerText;
    sf::Text scoreText;
    std::vector<double> getGridInputs() const;
    
    // Game state
    sf::RenderWindow* window;
    NeuralNetwork* currentBrain = nullptr;
    sf::Texture marioTexture, blockTexture, coinTexture;
    sf::Sprite mario;
    std::vector<sf::Sprite> grounds;
    std::vector<sf::Sprite> coins;
    std::vector<bool> coinCollected;
    float velocityY = 0;
    bool isOnGround = false;
    bool isDead;
    int score = 0;
    sf::Clock gameClock;
    sf::View view;
    
    // Platform definitions
    struct Platform {
        sf::Vector2f position;
        sf::Vector2f size;
    };    
    std::vector<Platform> platforms;

    
};