#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include <mutex>
#include <deque>
#include <condition_variable>
#include "TrafficObject.h"


enum class TrafficLightPhase {red, green};

// forward declarations to avoid include cycle
class Vehicle;


template <typename T>
class MessageQueue
{
public:
    void send(T &&phase);
    T receive();
private:
    std::deque<T> _messages;
    std::condition_variable _condition;
    std::mutex _mutex;
};

class TrafficLight : public TrafficObject
{
public:
    TrafficLight();
    void waitForGreen();
    void simulate();
    TrafficLightPhase getCurrentPhase();
private:
    MessageQueue<TrafficLightPhase> _trafficLightQueue;
    void cycleThroughPhases();
    TrafficLightPhase _currentPhase;
    std::condition_variable _condition;
    std::mutex _mutex;
};

#endif