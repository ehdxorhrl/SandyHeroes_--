#include "stdafx.h"
#include "SessionManager.h"
#include "User.h"


void SessionManager::add(int clientId, std::shared_ptr<Session> sess) {
    std::lock_guard<std::mutex> lk(mtx_);
    sessions[clientId] = std::move(sess);
}

// 技记 炼雀
std::shared_ptr<Session> SessionManager::get(int clientId) {
    std::lock_guard<std::mutex> lk(mtx_);
    auto it = sessions.find(clientId);
    return (it != sessions.end() ? it->second : nullptr);
}

// 技记 力芭
void SessionManager::remove(int clientId) {
    std::lock_guard<std::mutex> lk(mtx_);
    sessions.erase(clientId);
}


Session* SessionManager::GetSessionByPlayerObject(Object* object)
{
    std::lock_guard<std::mutex> lk(mtx_);
    for (auto& pair : sessions)
    {
        auto& session = pair.second;
        if (session && session->get_player_object() == object)
        {
            return session.get();
        }
    }
    return nullptr;
}