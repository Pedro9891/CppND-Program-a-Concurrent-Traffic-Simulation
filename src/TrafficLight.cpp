#include <iostream>
#include <random>
#include "TrafficLight.h"

/* Implementation of class "MessageQueue" */

template <typename T>
void MessageQueue<T>::send(T &&phase)
{
    std::lock_guard<std::mutex> lock(_mutex);
    _messages.clear();
    _messages.push_back(std::move(phase));
    _condition.notify_one();
}

template <typename T>
T MessageQueue<T>::receive()
{
    std::unique_lock<std::mutex> lock(_mutex);
    _condition.wait(lock, [this] { return !_messages.empty(); });

    T p = std::move(_messages.back());
    return p;
}


/* Implementation of class "TrafficLight" */

TrafficLight::TrafficLight()
{
    _currentPhase = TrafficLightPhase::red;
}

void TrafficLight::waitForGreen()
{
    while (true)
    {
        TrafficLightPhase p = _trafficLightQueue.receive();
        if (p == TrafficLightPhase::green)
            return;
        
    }
    
}

void TrafficLight::cycleThroughPhases()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(4000,6000);
    int cycleDuration = dist(gen);
    auto lastTime = std::chrono::steady_clock::now();

    while (true)
    {
        auto currentTime = std::chrono::steady_clock::now();
        auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastTime).count();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        if (elapsedTime > cycleDuration)
        {
            _currentPhase = (_currentPhase == TrafficLightPhase::red) ? TrafficLightPhase::green : TrafficLightPhase::red;
            _trafficLightQueue.send(std::move(_currentPhase));

            lastTime = std::chrono::steady_clock::now();
            cycleDuration = dist(gen);
        }
    }
}

void TrafficLight::simulate()
{
    threads.emplace_back(&TrafficLight::cycleThroughPhases, this);
}

TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}