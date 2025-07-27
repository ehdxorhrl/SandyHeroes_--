#pragma once
#include "User.h"

class Session;

// �̱������� ����
class SessionManager { 
public:
    static SessionManager& getInstance() {
        static SessionManager inst;
        return inst;
    }

    const std::unordered_map<int, std::shared_ptr<Session>>&
        getAllSessions() const {
        return sessions;
    }

    SessionManager(const SessionManager&) = delete;
    SessionManager& operator=(const SessionManager&) = delete;

    void add(int clientId, std::shared_ptr<Session> sess);

    std::shared_ptr<Session> get(int clientId);

    void remove(int clientId);

    Session* GetSessionByPlayerObject(Object* object);

private:
    SessionManager() = default;             
    ~SessionManager() = default;

    std::mutex mtx_;
    std::unordered_map<int, std::shared_ptr<Session>> sessions;
};
