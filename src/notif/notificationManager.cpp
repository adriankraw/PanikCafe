#include "InfoNotification.cpp"
#include "AlertNotification.cpp"
#include "OrderNotification.cpp"
#include "raylib.h"
#include <iostream>
#include <list>
#include <memory>

enum NotificationEnum {
    INFO,
    ALERT,
    ORDER,
};

class NotificationManager {
  public:
    NotificationManager() {};
    NotificationManager(NotificationManager &&) = default;
    NotificationManager(const NotificationManager &) = default;
    NotificationManager &operator=(NotificationManager &&) = default;
    NotificationManager &operator=(const NotificationManager &) = default;
    ~NotificationManager() {};

    void Update();
    void AddNotification(NotificationEnum, std::string);
    std::list<std::unique_ptr<notification>> notifications;

  private:
};

void NotificationManager::Update() {
    int count = 0;
    for (auto &noti:notifications) {
        if (noti->visible) {
            count++;
            noti->AddAge(GetFrameTime());
            noti->Show(GetScreenWidth() - 200, 60 * (count - 1));
        }
    }
}
void NotificationManager::AddNotification(NotificationEnum notificationEnum, std::string text) {
    switch (notificationEnum) {
    case NotificationEnum::INFO:
        notifications.push_back(std::unique_ptr<InfoNotification>(new InfoNotification(text)));
        break;
    case NotificationEnum::ALERT:
        notifications.push_back(std::unique_ptr<AlertNotification>(new AlertNotification(text)));
        break;
    case NotificationEnum::ORDER:
        notifications.push_back(std::unique_ptr<OrderNotification>(new OrderNotification(text)));
    break;
    default:
        notifications.push_back(std::unique_ptr<InfoNotification>(new InfoNotification(text)));
    break;
    }
}
