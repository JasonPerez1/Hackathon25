

// AIControlledGame.cpp

#include <iostream>
using namespace std;
#include <SFML/Graphics.hpp>
#include "GeneticAlgorithm.hpp"  // Replace forward declaration
#include "NeuralNetwork.hpp"     // Replace forward declaration
#include "AIControlledGame.hpp"
#include <sstream>
#include <iomanip>
#include <cfloat>
#include <vector>
#include <cmath>

constexpr float JUMP_FORCE = -9.0f;  // Increased from -9.0
constexpr float GRAVITY = 0.15f;       // Reduced from 0.2
constexpr int WINDOW_WIDTH = 1600;
constexpr int WINDOW_HEIGHT = 1200;

AIControlledGame::AIControlledGame(bool createWindow) : window(nullptr) {
    if (createWindow) {
        window = new sf::RenderWindow(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "AI Mario");
        window->setPosition(sf::Vector2i(100, 50));
    
        // Load textures
        if(!marioTexture.loadFromFile("resources/mario.png") || 
           !blockTexture.loadFromFile("resources/groundtexture.png") ||
           !coinTexture.loadFromFile("resources/coin.png")) {
            // Handle error
        }
        initializePlatforms(); // Initialize predefined platforms

        // Initialize Mario
        mario.setTexture(marioTexture);
        mario.setScale(0.05f, 0.05f);
    }
    // In AIControlledGame constructor
    // platforms = {
    //     {{0.0f, 500.0f}, {180.0f, 50.0f}},    
    //     {{-300.0f, 500.0f}, {180.0f, 50.0f}},  
    //     {{400.0f, 375.0f}, {180.0f, 50.0f}},
    //     {{500.0f, 375.0f}, {180.0f, 50.0f}},
    //     {{600.0f, 400.0f}, {180.0f, 50.0f}},
    //     {{1200.0f, 500.0f}, {180.0f, 50.0f}}
    // };

        // Generate 20 initial platforms 
    // platforms.clear();
    // float currentX = 0.0f;
    // float currentY = 500.0f;
    
    // Create initial platform
    // platforms.push_back({{currentX, currentY}, {400.0f, 50.0f}});
    
    // // Generate subsequent platforms with huge gaps
    // for(int i = 1; i < 20; i++) {
    //     // Massive gap parameters
    //     float gap = 400.0f + static_cast<float>(rand() % 300); // 400-700 gap
    //     float width = 200.0f + static_cast<float>(rand() % 100); // 200-300 width
    //     float heightChange = static_cast<float>(rand() % 300 - 150); // -150 to +150 change
        
    //     // Calculate new position
    //     currentX += gap;
    //     currentY = std::max(250.0f, 
    //                 std::min(currentY + heightChange, 600.0f));
        
    //     platforms.push_back({{currentX, currentY}, {width, 50.0f}});
    // }
    
  // Initialize coins
// coins.emplace_back(coinTexture);
// coins.back().setPosition(550.0f, 250.0f);  // Coin positioned above second platform
// coins.back().setScale(0.5f, 0.5f);
// coinCollected.push_back(false);
    
    // Initilize timer
    if (!font.loadFromFile("resources/arial.ttf")) {
        std::cerr << "Error loading font\n";
    }
    
    timerText.setFont(font);
    timerText.setCharacterSize(24);
    timerText.setFillColor(sf::Color::White);
    timerText.setPosition(650, 10); // Positioned in top-right

    //Initilize score view
    scoreText.setFont(font);
    scoreText.setCharacterSize(24);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition(650, 40);  // Positioned below timer

    // Initialize view safely
    if (window) {
               view = window->getDefaultView();
    } else {
        view.reset(sf::FloatRect(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT));
    }
    
    // Seed random generator for platform generation
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
}
void AIControlledGame::updatePlatforms() {
    // Remove platforms that are no longer visible (i.e. to the left of the view)
    platforms.erase(
        std::remove_if(platforms.begin(), platforms.end(), [this](const Platform& p) {
            return (p.position.x + p.size.x) < (view.getCenter().x - WINDOW_WIDTH);
        }),
        platforms.end()
    );

    // Determine the farthest x-coordinate from existing platforms
    float farthestX = 0.0f;
    for (const auto& platform : platforms) {
        farthestX = std::max(farthestX, platform.position.x + platform.size.x);
    }

    // Generate new platforms until we cover the area ahead
    // We set the right edge as some offset ahead of the current view
    const float viewRightEdge = view.getCenter().x + WINDOW_WIDTH * 0.75f;
    while (farthestX < viewRightEdge) {
        // If there are no platforms, set default parameters.
        const Platform* previousPlatform = platforms.empty() ? nullptr : &platforms.back();

        // Determine horizontal gap parameters
        const float minGap = 120.0f;    
        const float maxGap = 200.0f;    
        float gap = minGap + static_cast<float>(rand() % static_cast<int>(maxGap - minGap + 1));

        // Determine platform width parameters
        const float minWidth = 150.0f;  
        const float maxWidth = 300.0f;  
        float width = minWidth + static_cast<float>(rand() % static_cast<int>(maxWidth - minWidth + 1));

        // Determine vertical position.
        // If we have a previous platform, vary its height by up to +/- maxHeightChange.
        // Otherwise, start at a default height.
        const float maxHeightChange = 100.0f;
        float newY = (previousPlatform) ? previousPlatform->position.y : 400.0f;
        newY += static_cast<float>((rand() % static_cast<int>(maxHeightChange * 2 + 1)) - maxHeightChange);
        // Clamp newY between reasonable limits (e.g., 300 and 550)
        newY = std::max(300.0f, std::min(newY, 550.0f));

        // Create and add the new platform
        Platform newPlatform;
        newPlatform.position = sf::Vector2f(farthestX + gap, newY);
        newPlatform.size = sf::Vector2f(width, 50.0f);
         // Create and add the new platform
         platforms.push_back(newPlatform);

         // Add a coin for the new platform
         coins.emplace_back(coinTexture);
         auto& coin = coins.back();
         coin.setPosition(
             newPlatform.position.x + newPlatform.size.x/2 - coin.getGlobalBounds().width/2,
             newPlatform.position.y - 50.0f
         );
         coin.setScale(0.5f, 0.5f);
         coinCollected.push_back(false);
        // Update farthestX to the end of this new platform
        farthestX = newPlatform.position.x + width;
    }
}

void AIControlledGame::initializePlatforms() {
    platforms = {
        {sf::Vector2f(100.0f, 500.0f), sf::Vector2f(300.0f, 20.0f)},
        {sf::Vector2f(500.0f, 400.0f), sf::Vector2f(300.0f, 20.0f)},
        {sf::Vector2f(800.0f, 300.0f), sf::Vector2f(250.0f, 20.0f)},
        {sf::Vector2f(1000.0f, 200.0f), sf::Vector2f(350.0f, 20.0f)},
        {sf::Vector2f(1500.0f, 200.0f), sf::Vector2f(350.0f, 20.0f)},
        {sf::Vector2f(1800.0f, 200.0f), sf::Vector2f(350.0f, 20.0f)}
    };

    // Add coins for each platform
    coins.clear();
    coinCollected.clear();
    for(const auto& platform : platforms) {
        coins.emplace_back(coinTexture);
        auto& coin = coins.back();
        coin.setPosition(
            platform.position.x + platform.size.x/2 - coin.getGlobalBounds().width/2,
            platform.position.y - 50.0f  // 50 pixels above platform
        );
        coin.setScale(0.5f, 0.5f);
        coinCollected.push_back(false);
    }
}

void AIControlledGame::runSimulation(Individual& individual, bool render) {
    currentBrain = new NeuralNetwork(individual.chromosome, {33, 16, 4});
    reset();

    while((!render || (window && window->isOpen())) &&  // Modified condition
          gameClock.getElapsedTime().asSeconds() < 15.0f && 
          !isDead) 
    {
        if(render) {  // Only process events if rendering
            sf::Event event;
            while(window->pollEvent(event)) {
                if(event.type == sf::Event::Closed)
                    window->close();
            }
        }

        aiControl();
        handlePhysics();
        handleCollisions();
        handleCoinCollection();
        updatePlatforms(); // Ensure platforms extend ahead
        scoreText.setPosition(mario.getPosition().x + 250, scoreText.getPosition().y);
        timerText.setPosition(mario.getPosition().x + 250, timerText.getPosition().y);
        updateView(render);
        // updatePlatforms();

        // // Calculate fitness
        // if(isDead) {
        //     individual.fitness = 0;  // Heavy penalty for dying
        // } else {
        individual.fitness = mario.getPosition().x + (score * 1000000);
        // if(isDead) {
        //         individual.fitness = individual.fitness*0.25; 
        // };

        if(render) {
            // window->clear(sf::Color(135, 206, 235));
            // for(const auto& platform : platforms) {
            //     sf::Sprite ground(blockTexture);
            //     ground.setPosition(platform.position);
            //     ground.setScale(
            //         blockTexture.getSize().x,
            //         blockTexture.getSize().y
            //     );
            //     window->draw(ground);
            // }
            for(size_t i = 0; i < coins.size(); i++) {
                if(!coinCollected[i]) window->draw(coins[i]);
            }
            // Update timer text
            float elapsedTime = gameClock.getElapsedTime().asSeconds();
            std::ostringstream ss;
            ss << "Time: " << std::fixed << std::setprecision(1) << elapsedTime;
            timerText.setString(ss.str());

            // Update score text
            std::ostringstream scoreStream;
            scoreStream << "Score: " << score;
            scoreText.setString(scoreStream.str());
            

            // Draw
            window->clear(sf::Color(135, 206, 235));
            // Draw platforms
            for (const auto& platform : platforms) {
                sf::RectangleShape rect(sf::Vector2f(platform.size.x, platform.size.y));
                rect.setPosition(platform.position.x, platform.position.y);
                rect.setFillColor(sf::Color::Green);
                window->draw(rect);
            }

            view.setCenter(mario.getPosition().x, view.getCenter().y);
            window->setView(view);
            window->draw(timerText);
            window->draw(scoreText);
            window->draw(mario);
            for(size_t i = 0; i < coins.size(); i++) {
                if(!coinCollected[i]) window->draw(coins[i]);  // Only draw if not collected
            }
            view.setCenter(mario.getPosition().x, view.getCenter().y);
            window->setView(view);
            window->display();

            
        }
    }
    
    delete currentBrain;
    // cout << individual.fitness<< endl;
    // cout << mario.getPosition().x<< endl;

    // cout << individual.fitness<< endl;
}


void AIControlledGame::reset() {
    mario.setPosition(100, 400);
    velocityY = 0;
    isOnGround = false;
    score = 0;
    gameClock.restart();
    coinCollected = std::vector<bool>(coins.size(), false);
    isDead = false;
}

// // Method to update platforms
// void AIControlledGame::updatePlatforms() {
//     // Remove platforms that are far off-screen
//     platforms.erase(std::remove_if(platforms.begin(), platforms.end(), [this](const Platform& p) {
//         return (p.position.x + p.size.x) < (view.getCenter().x - WINDOW_WIDTH);
//     }), platforms.end());

//     // Find current farthest platform edge
//     float farthestX = 0;
//     for (const auto& platform : platforms) {
//         farthestX = std::max(farthestX, platform.position.x + platform.size.x);
//     }

//     // Generate new platforms until we cover the visible area ahead
//     const float viewRightEdge = view.getCenter().x + WINDOW_WIDTH * 0.75f;
//     while (farthestX < viewRightEdge) {
//         // Get previous platform reference
//         const Platform& previousPlatform = platforms.back();
        
//         // Calculate new platform parameters
//         const float minGap = 120.0f;    // Minimum horizontal spacing
//         const float maxGap = 220.0f;    // Maximum horizontal spacing
//         const float gap = static_cast<float>(rand() % (int)(maxGap - minGap)) + minGap;
        
//         const float minWidth = 150.0f;  // Minimum platform length
//         const float maxWidth = 300.0f;  // Maximum platform length
//         const float width = static_cast<float>(rand() % (int)(maxWidth - minWidth)) + minWidth;
        
//         const float maxHeightChange = 200.0f;
//         const float newY = std::max(300.0f, 
//             std::min(previousPlatform.position.y + (rand() % (int)maxHeightChange * 2 - maxHeightChange),
//             550.0f));

//         // Create new platform with explicit struct type
//         platforms.push_back(Platform{
//             {farthestX + gap, newY},  // position
//             {width, 50.0f}            // size
//         });

//         farthestX = platforms.back().position.x + width;
//     }
// }

float AIControlledGame::calculateVerticalDistanceToGround() const {
    float minDistance = FLT_MAX;
    sf::FloatRect marioBounds = mario.getGlobalBounds();
    
    for(const auto& platform : platforms) {
        sf::FloatRect platformBounds(
            platform.position.x,
            platform.position.y,
            platform.size.x,
            platform.size.y
        );
        
        // Check if Mario is above this platform
        if(marioBounds.intersects(platformBounds)) {
            // Directly on platform
            return 0.0f;
        }
        
        // Check if platform is below Mario
        if(platform.position.y > mario.getPosition().y) {
            // Calculate vertical distance to platform top
            float verticalDistance = platform.position.y - 
                                   (mario.getPosition().y + marioBounds.height);
            
            // Check horizontal overlap
            bool horizontalOverlap = (mario.getPosition().x + marioBounds.width > platform.position.x) &&
                                   (mario.getPosition().x < platform.position.x + platform.size.x);
            
            if(horizontalOverlap && verticalDistance < minDistance) {
                minDistance = verticalDistance;
            }
        }
    }
    
    // If no platform found below, return distance to bottom of screen
    if(minDistance == FLT_MAX) {
        minDistance = WINDOW_HEIGHT - (mario.getPosition().y + marioBounds.height);
    }
    
    return minDistance;
}

std::vector<double> AIControlledGame::getGridInputs() const {
    const int gridRows = 5;
    const int gridCols = 5;
    const float cellSize = 50.0f; // Each cell = 50x50 pixels

    std::vector<double> gridInputs(gridRows * gridCols, 0.0);
    sf::Vector2f marioPos = mario.getPosition();

    // Grid centered on Mario's position
    float startX = marioPos.x - (gridCols * cellSize)/2.0f;
    float startY = marioPos.y - (gridRows * cellSize)/2.0f;

    for (int row = 0; row < gridRows; row++) {
        for (int col = 0; col < gridCols; col++) {
            sf::FloatRect cellRect(
                startX + col * cellSize,
                startY + row * cellSize,
                cellSize,
                cellSize
            );

            bool platformDetected = false;
            for (const auto& platform : platforms) {
                sf::FloatRect platformBounds(
                    platform.position.x,
                    platform.position.y,
                    platform.size.x,
                    platform.size.y
                );

                if (cellRect.intersects(platformBounds)) {
                    platformDetected = true;
                    break;
                }
            }
            gridInputs[row * gridCols + col] = platformDetected ? 1.0 : 0.0;
        }
    }
        return gridInputs;
}



// Revised getInputs() function
std::vector<double> AIControlledGame::getInputs() const {
    std::vector<double> inputs;

    // 1. Mario's X position (normalized)
    float marioX = mario.getPosition().x;
    float marioY = mario.getPosition().y;
    inputs.push_back(marioX / WINDOW_WIDTH);

    // 2. Mario's Y position (normalized)
    inputs.push_back(marioY / WINDOW_HEIGHT);

    // 3. Mario's vertical velocity (normalized)
    inputs.push_back(velocityY / 10.0f);

    // 4. Horizontal distance to the next platform (normalized)
    float nextPlatformDist = WINDOW_WIDTH;
    for (const auto& platform : platforms) {
        if (platform.position.x > marioX) {
            float dist = platform.position.x - marioX;
            if (dist < nextPlatformDist)
                nextPlatformDist = dist;
        }
    }
    inputs.push_back(nextPlatformDist / WINDOW_WIDTH);

    // 5. Vertical difference to that same next platform (normalized)
    float nextPlatformVertDiff = WINDOW_HEIGHT;
    for (const auto& platform : platforms) {
        if (platform.position.x > marioX) {
            float vertDiff = std::abs(platform.position.y - marioY);
            if (vertDiff < nextPlatformVertDiff)
                nextPlatformVertDiff = vertDiff;
        }
    }
    inputs.push_back(nextPlatformVertDiff / WINDOW_HEIGHT);

    // 6. Euclidean distance to the nearest coin (normalized)
    float nearestCoinDist = std::numeric_limits<float>::max();
    for (size_t i = 0; i < coins.size(); i++) {
        if (coinCollected[i])
            continue;
        float dx = coins[i].getPosition().x - marioX;
        float dy = coins[i].getPosition().y - marioY;
        float dist = std::sqrt(dx * dx + dy * dy);
        if (dist < nearestCoinDist)
            nearestCoinDist = dist;
    }
    // Maximum possible distance in the window (diagonal)
    float maxDist = std::sqrt(WINDOW_WIDTH * WINDOW_WIDTH + WINDOW_HEIGHT * WINDOW_HEIGHT);
    inputs.push_back(nearestCoinDist / maxDist);

    // 7. Direction to the nearest coin (using normalized angle)
    // If no coin is available, default to 0.
    double normalizedAngle = 0.0;
    if (nearestCoinDist < std::numeric_limits<float>::max()) {
        float coinDiffX = 0.0f, coinDiffY = 0.0f;
        for (size_t i = 0; i < coins.size(); i++) {
            if (coinCollected[i])
                continue;
            float dx = coins[i].getPosition().x - marioX;
            float dy = coins[i].getPosition().y - marioY;
            float dist = std::sqrt(dx * dx + dy * dy);
            if (std::abs(dist - nearestCoinDist) < 0.001f) {
                coinDiffX = dx;
                coinDiffY = dy;
                break;
            }
        }
        // Compute angle (range: -pi to pi), then normalize to [-1,1]
        double angle = std::atan2(coinDiffY, coinDiffX);
        normalizedAngle = angle / 3.14159265;
    }
    inputs.push_back(normalizedAngle);

    // 8. Sign of vertical velocity: +1 if falling or 0, -1 if moving upward.
    inputs.push_back(velocityY >= 0 ? 1.0 : -1.0);

    // Append the grid-based inputs (5x5 grid, 25 values)
    std::vector<double> gridInputs = getGridInputs();
    inputs.insert(inputs.end(), gridInputs.begin(), gridInputs.end());

    // cout << inputs[0] <<" "<< inputs[1] <<" "<< inputs[2] <<" " << inputs[3] <<" "<< inputs[4] <<" " << inputs[5] <<" "<< inputs[6] <<" " << inputs[7] <<" " << endl;

    return inputs;
}



void AIControlledGame::aiControl() {
    auto inputs = getInputs();
    auto outputs = currentBrain->activate(inputs);
    
    // Left/Right movement
    if(outputs[0] > 0.5) {
        mario.move(-2, 0);
    }
    if(outputs[1] >= 0.5) {
        mario.move(2, 0);
    }
    // Jump control
    if(outputs[2] > 0.45 && isOnGround && currentBrain->jumpReleased) {
        velocityY = JUMP_FORCE;
        isOnGround = false;
        currentBrain->jumpReleased = false;
    }
    else if(outputs[2] <= 0.5) {
        currentBrain->jumpReleased = true;
    }
// // Horizontal movement
// float moveSpeed = 2.0f;
// if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
//     mario.move(-moveSpeed, 0);
// }
// if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
//     mario.move(moveSpeed, 0);
// }

// // Jumping
// if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && isOnGround) {
//     velocityY = JUMP_FORCE;
//     isOnGround = false;
// }
    
}

void AIControlledGame::handlePhysics() {
    velocityY += GRAVITY;
    mario.move(0, velocityY);

    // Add vertical death check
    if(mario.getPosition().y > WINDOW_HEIGHT + 100) {
        isDead = true;
    }
}

void AIControlledGame::handleCollisions() {
    isOnGround = false;
    
    // Get Mario's collision bounds
    sf::FloatRect marioBounds = mario.getGlobalBounds();
    //marioBounds.height -= 2;  // Keep small tolerance

    for (const auto& platform : platforms) {
        sf::FloatRect platformBounds(
            platform.position.x,
            platform.position.y,
            platform.size.x,
            platform.size.y
        );

        if (marioBounds.intersects(platformBounds)) {
            // Calculate penetration depths for all sides
            float overlapLeft = marioBounds.left + marioBounds.width - platformBounds.left;
            float overlapRight = platformBounds.left + platformBounds.width - marioBounds.left;
            float overlapTop = marioBounds.top + marioBounds.height - platformBounds.top;
            float overlapBottom = platformBounds.top + platformBounds.height - marioBounds.top;

            // Only consider positive overlaps
            bool validOverlapLeft = overlapLeft > 0;
            bool validOverlapRight = overlapRight > 0;
            bool validOverlapTop = overlapTop > 0;
            bool validOverlapBottom = overlapBottom > 0;

            // Find smallest valid overlap
            float minOverlap = FLT_MAX;
            sf::Vector2f correction(0, 0);

            if (validOverlapTop && overlapTop < minOverlap) {
                minOverlap = overlapTop;
                correction.y = -overlapTop;
                isOnGround = true;  // Mario is on the ground
            }
            if (validOverlapBottom && overlapBottom < minOverlap) {
                minOverlap = overlapBottom;
                correction.y = overlapBottom;
            }
            if (validOverlapLeft && overlapLeft < minOverlap) {
                minOverlap = overlapLeft;
                correction.x = -overlapLeft; // Mario collided with the right side of the platform
            }
            if (validOverlapRight && overlapRight < minOverlap) {
                minOverlap = overlapRight;
                correction.x = overlapRight; // Mario collided with the left side of the platform
            }

            // Apply the smallest valid correction
            mario.move(correction);

            // Update state based on collision direction
            if (correction.y < 0) { // Top collision (landing)
                velocityY = 0;  // Stop downward velocity when landing
            }
            if (correction.x != 0) { // Horizontal collision
                // Stop horizontal movement if Mario collides with platform sides
                if (correction.x < 0) {
                    // Mario hit the right side of the platform, stop movement to the left
                    mario.move(-overlapLeft, 0);
                }
                if (correction.x > 0) {
                    // Mario hit the left side of the platform, stop movement to the right
                    mario.move(overlapRight, 0);
                }
            }
        }
    }
}


void AIControlledGame::handleCoinCollection() {
    for(size_t i = 0; i < coins.size(); i++) {
        if(!coinCollected[i] && mario.getGlobalBounds().intersects(coins[i].getGlobalBounds())) {
            coinCollected[i] = true;
            score += 100;
        }
    }
}

void AIControlledGame::updateView(bool render) {
    if(window && render) {  // Only update view if window exists
        view.setCenter(mario.getPosition().x, view.getCenter().y);
        window->setView(view);
    }
}