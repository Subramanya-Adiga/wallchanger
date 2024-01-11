#pragma once
#include "connect_queue.h"
#include "net_message.h"

namespace wallchanger::net {
template <typename T> class server_interface;

template <typename T>
class connection : public std::enable_shared_from_this<connection<T>> {
  constexpr static std::string_view logger_name = "connection_logger";

public:
  enum class owner { server, client };

  connection(owner parent, asio::io_context &context,
             asio::ip::tcp::socket socket,
             connect_queue<owned_message<T>> &incomming)
      : m_context(context), m_socket(std::move(socket)), m_incomming(incomming),
        m_owner(parent) {
    LOGGER_CREATE(logger_name);
  }
  connection(const connection &) = delete;
  connection &operator=(const connection &) = delete;

  [[nodiscard]] bool is_connected() const {
    return m_socket.is_open() && m_connected;
  }
  [[nodiscard]] uint32_t get_id() const { return m_id; }

  void connect_to_client(server_interface<T> *server, uint32_t id = 0) {
    if (m_owner == owner::server) {
      if (m_socket.is_open()) {
        m_id = id;
        read_header();
      }
    }
  }

  void
  connect_to_server(const asio::ip::tcp::resolver::results_type &endpoint) {
    if (m_owner == owner::client) {
      asio::async_connect(
          m_socket, endpoint,
          [this](std::error_code ec, asio::ip::tcp::endpoint endpoint) {
            if (!ec) {
              m_connected = true;
              read_header();
            } else {
              LOG_ERR(logger_name, "ConnectionError Code:{} {}\n", ec.value(),
                      ec.message());
              m_connected = false;
              disconnect();
            }
          });
    }
  }

  void disconnect() {
    if (is_connected()) {
      asio::post(m_context, [this]() { m_socket.close(); });
    }
  }

  void send_message(const message<T> &msg) {
    if (is_connected()) {
      asio::post(m_context, [this, msg]() {
        bool writing_header = !m_outbound.empty();
        m_outbound.push_back(msg);
        if (!writing_header) {
          write_header();
        }
      });
    }
  }

private:
  void read_header() {
    asio::async_read(
        m_socket, asio::buffer(&m_temp_in.header, sizeof(message_header<T>)),
        [this](std::error_code ec, [[maybe_unused]] std::size_t length) {
          if (!ec) {
            if (m_temp_in.header.size > 0) {
              m_temp_in.body.resize(m_temp_in.header.size);
              read_body();
            } else {
              add_to_incomming();
            }
          } else {
            LOG_ERR(logger_name, "connection id:[{}] ErrorCode:{} {}", m_id,
                    ec.value(), ec.message());
            m_socket.close();
          }
        });
  }

  void read_body() {
    asio::async_read(
        m_socket, asio::buffer(m_temp_in.body.data(), m_temp_in.body.size()),
        [this](std::error_code ec, [[maybe_unused]] std::size_t length) {
          if (!ec) {
            add_to_incomming();
          } else {
            LOG_ERR(logger_name, "connection id:[{}] ErrorCode:{} {} ", m_id,
                    ec.value(), ec.message());
            m_socket.close();
          }
        });
  }

  void write_header() {
    asio::async_write(
        m_socket,
        asio::buffer(&m_outbound.front().header, sizeof(message_header<T>)),
        [this](std::error_code ec, [[maybe_unused]] std::size_t length) {
          if (!ec) {
            if (m_outbound.front().body.size() > 0) {
              write_body();
            } else {
              (void)m_outbound.pop_front();
              if (!m_outbound.empty()) {
                write_header();
              }
            }
          } else {
            LOG_ERR(logger_name,
                    "Write Header connection id:[{}] ErrorCode:{} {} ", m_id,
                    ec.value(), ec.message());
            m_socket.close();
          }
        });
  }

  void write_body() {
    asio::async_write(
        m_socket,
        asio::buffer(m_outbound.front().body.data(),
                     m_outbound.front().body.size()),
        [this](std::error_code ec, [[maybe_unused]] std::size_t length) {
          if (!ec) {
            (void)m_outbound.pop_front();
            if (!m_outbound.empty()) {
              write_header();
            }
          } else {
            LOG_ERR(logger_name,
                    "Write Body connection id:[{}] ErrorCode:{} {} ", m_id,
                    ec.value(), ec.message());
            m_socket.close();
          }
        });
  }

  void add_to_incomming() {
    if (m_owner == owner::server) {
      m_incomming.push_back({this->shared_from_this(), m_temp_in});
    } else {
      m_incomming.push_back({nullptr, m_temp_in});
    }
    read_header();
  }

protected:
  asio::ip::tcp::socket m_socket;
  asio::io_context &m_context;
  connect_queue<message<T>> m_outbound;
  connect_queue<owned_message<T>> &m_incomming;
  message<T> m_temp_in;
  uint32_t m_id{};
  owner m_owner;
  bool m_connected = false;
};
} // namespace wallchanger::net
