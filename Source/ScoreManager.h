#pragma once

class ScoreManager
{
private:
    int score = 0;
    float timer = 0.0f;
public:
    void addScore(int amount) { score += amount; }
    void resetScore() { score = 0; }
    int getScore() const { return score; }
};