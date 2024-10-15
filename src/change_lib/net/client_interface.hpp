#pragma once
#include "connect_queue.hpp"
#include "connection.hpp"
#include "net_message.hpp"

namespace wallchanger::net {

template <typename T> class client_interface {
  constexpr static std::string_view logger_name = "client_logger";

public:
  client_interface() {
    LOGGER_CREATE(logger_name);
#ifdef _DEBUG
    auto log_file = std::filesystem::path(log_directory()) /
                    std::filesystem::path(logger_name);
    LOGGER_SET_FILE(logger_name, log_file.string() + ".txt");
#endif
  }
  ~client_interface() { disconnect(); }

  client_interface(const client_interface &) = delete;
  client_interface &operator=(const client_interface &) = delete;

  [[nodiscard]] bool connect(const std::string &host_url, uint16_t port) {
    try {
      asio::ip::tcp::resolver resolver(m_context);
      auto endpoint = resolver.resolve(host_url, std::to_string(port));

      m_connection = std::make_unique<connection<T>>(
          connection<T>::owner::client, m_context,
          asio::ip::tcp::socket(m_context), m_incomming);
      m_connection->connect_to_server(endpoint);

      m_execution_thread = std::thread([this]() { m_context.run(); });
      return true;
    } catch (std::exception &e) {
      LOG_ERR(logger_name, e.what());
      return false;
    }
  }

  void disconnect() {
    if (is_connected()) {
      m_connection->disconnect();
    }
    if (!m_context.stopped()) {
      m_context.stop();
    }
    if (m_execution_thread.joinable()) {
      m_execution_thread.join();
    }
    m_connection.release();
    LOG_INFO(logger_name, "client disconnected");
  }

  [[nodiscard]] bool is_connected() {
    if (m_connection) {
      return m_connection->is_connected();
    }
    return false;
  }

  void send_message(const message<T> &msg) {
    if (is_connected()) {
      m_connection->send_message(msg);
    }
  }

  [[nodiscard]] connect_queue<owned_message<T>> &incomming() {
    return m_incomming;
  }

  [[nodiscard]] std::string_view get_logger_name() const { return logger_name; }

protected:
  asio::io_context m_context;
  std::thread m_execution_thread;
  std::unique_ptr<connection<T>> m_connection;

private:
  connect_queue<owned_message<T>> m_incomming;
};
} // namespace wallchanger::net