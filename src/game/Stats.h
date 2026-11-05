#ifndef STATS_H
#define STATS_H

#include <string>

class Stats {
    private:
        int _gamesPlayed   = 0;
        int _bestDistance  = 0;
        int _totalDistance = 0;

    public:
        static Stats load(const std::string& path);

        void recordGame(int distance);
        void save(const std::string& path) const;

        int gamesPlayed()   const;
        int bestDistance()  const;
        int totalDistance() const;
};

#endif