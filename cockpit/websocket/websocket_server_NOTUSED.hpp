
//We need to define this when using the Asio library without Boost
#define ASIO_STANDALONE

#include <functional>
#include <string>
#include <utility>
#include <vector>
#include <map>
#include <thread>
#include <mutex>
#include <iostream>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

#include <json.hpp>
using json = nlohmann::json;

typedef websocketpp::server<websocketpp::config::asio> WebsocketEndpoint;
typedef websocketpp::connection_hdl ClientConnection;

//Registers a callback for when a client connects
template <typename CallbackTy>
void connect(CallbackTy handler) {
	this->connectHandlers.push_back(handler);
}

//Registers a callback for when a client disconnects
template <typename CallbackTy>
void disconnect(CallbackTy handler) {
	this->disconnectHandlers.push_back(handler);
}

//Registers a callback for when a particular type of message is received
template <typename CallbackTy>
void message(const std::string& messageType, CallbackTy handler) {
	this->messageHandlers[messageType].push_back(handler);
}


class TextMessage
{
public:
	std::string text;
	TextMessage() = default;
	TextMessage(std::string text) : text(std::move(text)) {}

	static TextMessage text_message_factory(const json& j)
	{
		TextMessage text_message;
		j.at("__MESSAGE__").get_to(text_message.text);
		return text_message;
	}

	json to_json() {
		return json{ {"__MESSAGE__", text}, };
	}
};



class WebsocketServer
{
public:

	WebsocketServer();

	void run(int port);
	void stop();


	//Registers a callback for when a client connects
	template <typename CallbackTy>
	void connect(CallbackTy handler)
	{
		//Make sure we only access the handlers list from the networking thread
		this->eventLoop.post([this, handler]() {
			this->connectHandlers.push_back(handler);
			});
	}

	//Registers a callback for when a client disconnects
	template <typename CallbackTy>
	void disconnect(CallbackTy handler)
	{
		//Make sure we only access the handlers list from the networking thread
		this->eventLoop.post([this, handler]() {
			this->disconnectHandlers.push_back(handler);
			});
	}

	//Registers a callback for when a particular type of message is received
	template <typename CallbackTy>
	void message(const std::string& messageType, CallbackTy handler)
	{
		//Make sure we only access the handlers list from the networking thread
		this->eventLoop.post([this, messageType, handler]() {
			this->messageHandlers[messageType].push_back(handler);
			});
	}

protected:

	void onOpen(ClientConnection conn);

	void onClose(ClientConnection conn);


	void onMessage(ClientConnection conn, WebsocketEndpoint::message_ptr msg);
public:

	void sendMessage(ClientConnection conn, std::string message);

	void broadcastMessage(json msg);

	size_t numConnections();

private:
	WebsocketEndpoint endpoint;
	std::vector<ClientConnection> openConnections;
	std::vector<std::function<void(ClientConnection)>> connectHandlers;
	std::vector<std::function<void(ClientConnection)>> disconnectHandlers;
	std::map<std::string, std::vector<std::function<void(ClientConnection, json value)>>> messageHandlers;
	asio::io_service eventLoop;
	std::mutex connectionListMutex;

};
