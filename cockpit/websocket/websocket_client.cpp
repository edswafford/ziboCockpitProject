//#include "websocket_client.hpp"
//#include "../common/logger.hpp"
//extern logger LOG;
//
//
//WebsocketConnection::WebsocketConnection(websocketpp::connection_hdl hdl, std::string uri):
//	hdl_(hdl),
//	status_("Initiating")
//{}
//
//void WebsocketConnection::on_open(websocketpp::connection_hdl hdl)
//{
//	status_ = "Open";
//	std::cout << "Websocket Open\n";
//}
//
//void WebsocketConnection::on_fail(ws_client* ws_client_ptr, websocketpp::connection_hdl hdl)
//{
//	status_ = "Failed";
//
//	const ws_client::connection_ptr connection = ws_client_ptr->get_con_from_hdl(hdl);
//	std::cout << "Websocket Failed" << "\n";
//	std::cout << "Connection State " << connection->get_state() << "\n";
//	std::cout << "Local Close Code " << connection->get_local_close_code() << "\n";
//	std::cout << "Local Close Reason " << connection->get_local_close_reason() << "\n";
//	std::cout << "Remove Close Code " << connection->get_remote_close_code() << "\n";
//	std::cout << "Remote Close Reason " << connection->get_remote_close_reason() << "\n";
//	std::cout << "Error code " << connection->get_ec() << ", Error message "
//			<< connection->get_ec().message() << "\n";
//}
//
//void WebsocketConnection::on_close(ws_client* ws_client_ptr, websocketpp::connection_hdl hdl)
//{
//	status_ = "Closed";
//	const ws_client::connection_ptr connection = ws_client_ptr->get_con_from_hdl(hdl);
//	std::cout << "Websocket Closed" << "\n";
//	std::cout << "Connection State " << connection->get_state() << "\n";
//	std::cout << "Local Close Code " << connection->get_local_close_code() << "\n";
//	std::cout << "Local Close Reason " << connection->get_local_close_reason() << "\n";
//	std::cout << "Remove Close Code " << connection->get_remote_close_code() << "\n";
//	std::cout << "Remote Close Reason " << connection->get_remote_close_reason() << "\n";
//}
//
//void WebsocketConnection::on_message(websocketpp::connection_hdl hdl, ws_client::message_ptr msg)
//{
//	LOG() << "Websocket on_message NOT Implemented";
//	//if (msg->get_opcode() == websocketpp::frame::opcode::text) {
//
//	//	auto j = nlohmann::json::parse(msg->get_payload());
//	//	j.at("id").get_to(id_);
//	//	std::cout << "Received: " << msg->get_payload() << "\n";
//	//}
//	//else {
//	//	std::cout << "Binary Message: " << websocketpp::utility::to_hex(msg->get_payload());
//	//}
//}
//
//websocketpp::connection_hdl WebsocketConnection::get_hdl() const
//{
//	return hdl_;
//}
//
//std::string WebsocketConnection::get_status() const
//{
//	return status_;
//}
//
//WebsocketClientEndpoint::WebsocketClientEndpoint()
//{
//	endpoint.clear_access_channels(websocketpp::log::alevel::all);
//	endpoint.clear_error_channels(websocketpp::log::elevel::all);
//
//	endpoint.init_asio();
//	endpoint.start_perpetual();
//
//	thread = websocketpp::lib::make_shared<websocketpp::lib::thread>(&ws_client::run, &endpoint);
//}
//
//WebsocketClientEndpoint::~WebsocketClientEndpoint()
//{
//	endpoint.stop_perpetual();
//
//	if (ws_connection_ptr.use_count() > 0) {
//
//		if (ws_connection_ptr->get_status() == "Open") {
//			std::cout << "Closing Websocket Connection\n";
//
//			websocketpp::lib::error_code ec;
//			endpoint.close(ws_connection_ptr->get_hdl(), websocketpp::close::status::going_away, "", ec);
//			if (ec) {
//				std::cout << "Error Closing Websocket Connection: " << ec.message() << "\n";
//			}
//		}
//	}
//	std::cout << "Websocket Connection Terminated\n";
//	thread->join();
//}
//
//bool WebsocketClientEndpoint::connect(std::string const& uri)
//{
//	websocketpp::lib::error_code ec;
//
//	ws_client::connection_ptr connection = endpoint.get_connection(uri, ec);
//
//	if (ec) {
//		std::cout << "> Connect initialization error: " << ec.message() << std::endl;
//		return false;
//	}
//
//	ws_connection_ptr = websocketpp::lib::make_shared<WebsocketConnection>(connection->get_handle(), uri);
//
//	connection->set_open_handler(websocketpp::lib::bind(
//	                                                    &WebsocketConnection::on_open,
//	                                                    ws_connection_ptr,
//	                                                    websocketpp::lib::placeholders::_1
//	                                                   ));
//	connection->set_fail_handler(websocketpp::lib::bind(
//	                                                    &WebsocketConnection::on_fail,
//	                                                    ws_connection_ptr,
//	                                                    &endpoint,
//	                                                    websocketpp::lib::placeholders::_1
//	                                                   ));
//	connection->set_close_handler(websocketpp::lib::bind(
//	                                                     &WebsocketConnection::on_close,
//	                                                     ws_connection_ptr,
//	                                                     &endpoint,
//	                                                     websocketpp::lib::placeholders::_1
//	                                                    ));
//	connection->set_message_handler(websocketpp::lib::bind(
//	                                                       &WebsocketConnection::on_message,
//	                                                       ws_connection_ptr,
//	                                                       websocketpp::lib::placeholders::_1,
//	                                                       websocketpp::lib::placeholders::_2
//	                                                      ));
//
//	endpoint.connect(connection);
//
//	return true;
//}
//
//void WebsocketClientEndpoint::close(websocketpp::close::status::value code, std::string reason)
//{
//	websocketpp::lib::error_code ec;
//
//	if (ws_connection_ptr.use_count() > 0) {
//		endpoint.close(ws_connection_ptr->get_hdl(), code, reason, ec);
//		if (ec) {
//			std::cout << "> Error initiating close: " << ec.message() << std::endl;
//		}
//	}
//}
//
//bool WebsocketClientEndpoint::send(std::string message)
//{
//	websocketpp::lib::error_code ec;
//
//	if (ws_connection_ptr.use_count() > 0) {
//		endpoint.send(ws_connection_ptr->get_hdl(), message, websocketpp::frame::opcode::text, ec);
//		if (ec) {
//			std::cout << "> Error sending message: " << ec.message() << std::endl;
//			return false;
//		}
//
//		std::cout << "Sent: " << message << "\n";
//		return true;
//	}
//}
//
//websocketpp::lib::shared_ptr<WebsocketConnection> WebsocketClientEndpoint::ws_connection() const
//{
//	return ws_connection_ptr;
//}
