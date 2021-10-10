#include <http.h>
#include <nlohmann/json.hpp>
#include <iostream>
#include <cstdio>
#include <iomanip>
#include <ctime>
#include <list>
#include <map>

#define LOG(format, ...) \
do {\
std::time_t t=time(nullptr);\
std::tm tm=*std::localtime(&t);\
std::put_time(&tm,"%Y-%m-%d %H:%M:%S ");\
std::printf(format, ##__VA_ARGS__);\
std::putchar('\n');\
} while(0)

struct room;
struct user;

struct user {
    http_ws_t *ws = nullptr;
    const std::string username; // FIXME: unnecessary, should be optimized
    // std::map<std::string, struct room>::iterator room;
    struct room *room = nullptr;
    // for removing
    std::list<struct user *>::iterator list_itr;
};

struct room {
    const std::string name; // FIXME: unnecessary, should be optimized
    std::list<struct user *> user_list;
};

std::map<const std::string, struct room> rooms;
std::map<const std::string, struct user> users;

http_header_field_t header_field_of_json = {
        .key={.len=12, .data=(unsigned char *) "Content-Type"},
        .value={.len=16, .data=(unsigned char *) "application/json"}
};

void get_room(http_context_t *context) {
    if (context->request.method != HTTP_GET) {
        context->response.status_code = 400;
        http_response(context, nullptr);
    }
    nlohmann::json res = nlohmann::json::array();
    for (const auto &itr: rooms) {
        res.push_back({
                              {"name",   itr.first},
                              {"people", itr.second.user_list.size()}
                      });
    }
    auto res_str = new std::string(res.dump());
    context->response.status_code = 200;
    context->response.body.len = res_str->size();
    context->response.body.data = (unsigned char *) res_str->c_str();
    context->response.headers = {1, 0, &header_field_of_json};
    context->userdata = res_str;
    http_response(context, [](http_context_t *context) {
        delete (std::string *) context->userdata;
        LOG("[http] GET /room 200");
    });
}

void ws_message(http_ws_t *ws, uint8_t opcode, const uint8_t *message, size_t len) {
    if (opcode != 1 || len >= 200) {
        // ignore
        return;
    }
    if (ws->userdata == nullptr) {
        // must be register
        nlohmann::json j = nlohmann::json::parse(std::string((const char *) message, len));
        auto username_j = j["username"], room_name_j = j["roomName"];
        if (!(username_j.is_string() && room_name_j.is_string())) {
            // error
            http_close_connection(ws->http_context);
            return;
        }
        // search username
        const std::string username_str = username_j;
        if (username_str.size() < 3) {
            http_close_connection(ws->http_context);
            return;
        }
        auto user_itr = users.find(username_str);
        if (user_itr != users.end()) {
            // used, close
            http_close_connection(ws->http_context);
            return;
        }
        // create user and insert to users table
        struct user user = {
                .ws=ws, .username=username_str, .room=nullptr
        };
        user_itr = users.insert({username_str, user}).first;
        struct user *user_ptr = &user_itr->second;
        // search room
        const std::string room_name_str = room_name_j;
        auto room_itr = rooms.find(room_name_str);
        if (room_itr == rooms.end()) {
            // create new room and insert to rooms table
            room_itr = rooms.insert({room_name_str, {room_name_str, std::list<struct user *>(1, user_ptr)}}).first;
        } else {
            // join
            room_itr->second.user_list.push_back(user_ptr);
        }
        // set user member room and list_node
        user_itr->second.room = &room_itr->second;
        user_itr->second.list_itr = prev(room_itr->second.user_list.end());
        // set ws
        ws->userdata = user_ptr;
        // tell room member
        nlohmann::json message_j = {{"username", username_str},
                                    {"action",   "join"}
        };
        const std::string message_str = message_j.dump();
        for (const auto &u: room_itr->second.user_list) {
            http_ws_write(u->ws, 1, (unsigned char *) message_str.c_str(), message_str.size());
        }
        LOG("[ws] [%.*s] join [%.*s] room",
            (int) user_ptr->username.size(),
            user_ptr->username.c_str(),
            (int) user_ptr->room->name.size(),
            user_ptr->room->name.c_str());
    } else {
        // send message
        auto user = (struct user *) ws->userdata;
        nlohmann::json message_j = {{"username", user->username},
                                    {"message",  std::string((const char *) message, len)}
        };
        const std::string message_str = message_j.dump();
        for (const auto &u: user->room->user_list) {
            http_ws_write(u->ws, 1, (unsigned char *) message_str.c_str(), message_str.size());
        }
        LOG("[ws] [%.*s] send %zu bytes to [%.*s] room",
            (int) user->username.size(),
            user->username.c_str(),
            len,
            (int) user->room->name.size(),
            user->room->name.c_str()
        );
    }
}

void ws_close(http_ws_t *ws) {
    if (ws->userdata == nullptr) {
        // not register
        return;
    }
    // remove from room
    auto user_ptr = (struct user *) ws->userdata;
    user_ptr->room->user_list.erase(user_ptr->list_itr);
    // tell room member
    nlohmann::json j = {{"username", user_ptr->username},
                        {"action",   "leave"}
    };
    const std::string message_str = j.dump();
    for (const auto &u: user_ptr->room->user_list) {
        http_ws_write(u->ws, 1, (unsigned char *) message_str.c_str(), message_str.size());
    }
    // remove rooms table if no member
    if (user_ptr->room->user_list.empty()) {
        rooms.erase(user_ptr->room->name);
    }
    // remove from users table
    users.erase(user_ptr->username);
    LOG("[ws] [%.*s] leave [%.*s] room",
        (int) user_ptr->username.size(),
        user_ptr->username.c_str(),
        (int) user_ptr->room->name.size(),
        user_ptr->room->name.c_str()
    );
}

void ws_conn(http_ws_t *ws) {
    // set ws userdata to nullptr
    ws->userdata = nullptr;
}

http_ws_handlers_t ws_join_handlers = {
        .on_connection=ws_conn, .on_message=ws_message, .on_close=ws_close
};

void static_dir(http_context_t *context) {
    http_send_dir(context, "/Users/ziyihuang/source/smallchat/fe/dist", "index.html");
}

void err_handler(int err) {
    perror("error");
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        return -1;
    }
    http_server_t *server = http_create_server();
    server->port = std::atoi(argv[1]);
    server->err_handler = err_handler;
    http_register_url(server, "/", static_dir);
    http_register_url(server, "/room", get_room);
    http_register_ws(server, "/message", &ws_join_handlers);
    std::cout << "Start server at port " << server->port << std::endl;
    http_server_run_single_process(server);
    return 0;
}
