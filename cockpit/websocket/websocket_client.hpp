//#pragma once
//
//#include <websocketpp/config/asio_no_tls_client.hpp>
//#include <websocketpp/client.hpp>
//#include <iostream>
//#include <string>
//
//
//
//
//typedef websocketpp::client<websocketpp::config::asio_client> ws_client;
//
//class WebsocketConnection {
//public:
//
//	WebsocketConnection(websocketpp::connection_hdl hdl, std::string uri);
//
//	void on_open(websocketpp::connection_hdl hdl);
//
//	void on_fail(ws_client* ws_client_ptr, websocketpp::connection_hdl hdl);
//
//	void on_close(ws_client* ws_client_ptr, websocketpp::connection_hdl hdl);
//
//	void on_message(websocketpp::connection_hdl, ws_client::message_ptr msg);
//
//	websocketpp::connection_hdl get_hdl() const;
//
//	std::string get_status() const;
//
//	int get_id() const {return id_;}
//
//	friend std::ostream& operator<< (std::ostream& out, WebsocketConnection const& data);
//private:
//	websocketpp::connection_hdl hdl_;
//	std::string status_;
//	int id_{0};
//};
//
//
//
//class WebsocketClientEndpoint {
//public:
//	WebsocketClientEndpoint();
//
//	~WebsocketClientEndpoint();
//
//	bool connect(std::string const& uri);
//
//	void close(websocketpp::close::status::value code, std::string reason);
//
//	bool send(std::string message);
//
//	int get_connection_id()
//	{
//		if (ws_connection_ptr.use_count() > 0) {
//			return ws_connection_ptr->get_id();
//		}
//		return 0;
//	}
//	websocketpp::lib::shared_ptr<WebsocketConnection> ws_connection() const;
//private:
//	ws_client endpoint;
//	websocketpp::lib::shared_ptr<websocketpp::lib::thread> thread;
//	websocketpp::lib::shared_ptr<WebsocketConnection> ws_connection_ptr;
//};
//
//
