#pragma once
#include "connect_queue.h"
#include "connection.h"
#include "net_message.h"

namespace wallchanger::net {

template <typename T> class server_interface {
  constexpr static std::string_view logger_name = "server_logger";

public:
  explicit server_interface(uint16_t port)
      : m_acceptor(m_context,
                   asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)) {
    LOGGER_CREATE(logger_name);
#ifdef _DEBUG
    auto log_file = std::filesystem::path(log_directory()) /
                    std::filesystem::path(logger_name);
    LOGGER_SET_FILE(logger_name, log_file.string() + ".txt");
#endif
  }
  server_interface(const server_interface &) = delete;
  server_interface &operator=(const server_interface &) = delete;

  virtual ~server_interface() { stop(); }

  [[nodiscard]] bool start() {
    try {
      wait_for_client_connection();
      m_execution_thread = std::thread([this]() { m_context.run(); });
      LOG_INFO(logger_name, "server started");
      return true;
    } catch (std::exception &e) {
      LOG_ERR(logger_name, e.what());
      return false;
    }
  }

  void stop() {
    if (!m_context.stopped()) {
      m_context.stop();
    }
    if (m_execution_thread.joinable()) {
      m_execution_thread.join();
    }
    LOG_INFO(logger_name, "server stopped");
  }

  void wait_for_client_connection() {
    m_acceptor.async_accept(
        [this](std::error_code ec, asio::ip::tcp::socket socket) {
          if (!ec) {

            LOG_INFO(logger_name, "New Connection Address:{} Port:{}",
                     socket.remote_endpoint().address().to_string(),
                     socket.remote_endpoint().port());

            auto new_con = std::make_shared<connection<T>>(
                connection<T>::owner::server, m_context, std::move(socket),
                m_incomming);

            if (on_client_connect(new_con)) {
              m_connections.push_back(std::move(new_con));
              m_connections.back()->connect_to_client(this, helper::gen_id());
              LOG_INFO(logger_name, "connection accepted id:{}",
                       m_connections.back()->get_id());

            } else {
              LOG_INFO(logger_name, "connection denied");
            }

          } else {
            LOG_ERR(logger_name, ec.message());
          }
          wait_for_client_connection();
        });
  }

  void message_client(std::shared_ptr<connection<T>> client,
                      const message<T> &msg) {
    if (client && client->is_connected()) {
      client->send_message(msg);
    } else {
      on_client_disconnect(client);
      client.reset();
      m_connections.erase(
          std::remove(m_connections.begin(), m_connections.end(), client),
          m_connections.end());
    }
  }

  void
  message_all_client(const message<T> &msg,
                     std::shared_ptr<connection<T>> ignore_client = nullptr) {
    bool invalid_client = false;
    for (auto &&client : m_connections) {
      if (client && client->is_connected()) {
        if (client != ignore_client) {
          client->send_message(msg);
        }
      } else {
        on_client_disconnect(client);
        client.reset();
        invalid_client = true;
      }
    }
    if (invalid_client) {
      m_connections.erase(
          std::remove(m_connections.begin(), m_connections.end(), nullptr),
          m_connections.end());
    }
  }

  void update(size_t max_msg = -1, bool wait = false) {

    if (wait) {
      m_incomming.wait();
    }

    size_t msg_count = 0;
    while (msg_count < max_msg && !m_incomming.empty()) {
      auto msg = m_incomming.pop_front();
      on_message(msg.remote, msg.msg);
      msg_count++;
    }
  }

  [[nodiscard]] std::string_view get_logger_name() const { return logger_name; }

protected:
  virtual bool
  on_client_connect([[maybe_unused]] std::shared_ptr<connection<T>> client) {
    return false;
  }
  virtual void
  on_client_disconnect([[maybe_unused]] std::shared_ptr<connection<T>> client) {
  }
  virtual void
  on_message([[maybe_unused]] std::shared_ptr<connection<T>> client,
             [[maybe_unused]] message<T> &msg) {}

public:
  virtual void
  on_client_validated([[maybe_unused]] std::shared_ptr<connection<T>> client) {}

protected:
  connect_queue<owned_message<T>> m_incomming;
  std::deque<std::shared_ptr<connection<T>>> m_connections;
  asio::io_context m_context;
  std::thread m_execution_thread;
  asio::ip::tcp::acceptor m_acceptor;
};
} // namespace wallchanger::net
