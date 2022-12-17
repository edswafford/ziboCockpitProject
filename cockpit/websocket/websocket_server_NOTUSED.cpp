

#include "websocket_server.hpp"

#include <functional>

WebsocketServer::WebsocketServer()
{


	//Wire up our event handlers
	this->endpoint.set_open_handler(std::bind(&WebsocketServer::onOpen, this, std::placeholders::_1));
	this->endpoint.set_close_handler(std::bind(&WebsocketServer::onClose, this, std::placeholders::_1));
	this->endpoint.set_message_handler(std::bind(&WebsocketServer::onMessage, this, std::placeholders::_1, std::placeholders::_2));

	//Initialise the Asio library
	this->endpoint.init_asio(&(this->eventLoop));
}

void WebsocketServer::run(int port)
{
	//Listen on the specified port number and start accepting connections
	endpoint.listen(port);
	endpoint.start_accept();

	//Start the Asio event loop
	endpoint.run();
}

void WebsocketServer::stop()
{
	endpoint.stop();
}

void WebsocketServer::onOpen(ClientConnection conn)
{
	std::clog << "Client connected." << std::endl;
	
	{
		//Prevent concurrent access to the list of open connections from multiple threads
		std::lock_guard<std::mutex> lock(this->connectionListMutex);
	
		//Add the connection handle to our list of open connections
		openConnections.push_back(conn);
	}
	//Invoke any registered handlers
	for (auto handler : this->connectHandlers) {
		handler(conn);
	}
}

void WebsocketServer::onClose(ClientConnection conn)
{
	std::clog << "Client disconnected." << std::endl;
	
	{
		//Prevent concurrent access to the list of open connections from multiple threads
		std::lock_guard<std::mutex> lock(this->connectionListMutex);
	
	
		//Remove the connection handle from our list of open connections
		auto connVal = conn.lock();
		auto newEnd = std::remove_if(openConnections.begin(), openConnections.end(), [&connVal](ClientConnection elem)
		{
			//If the pointer has expired, remove it from the vector
			if (elem.expired() == true) {
				return true;
			}
	
			//If the pointer is still valid, compare it to the handle for the closed connection
			auto elemVal = elem.lock();
			if (elemVal.get() == connVal.get()) {
				return true;
			}
	
			return false;
		});
	
		//Truncate the connections vector to erase the removed elements
		openConnections.resize(std::distance(openConnections.begin(), newEnd));
	}
	
	//Invoke any registered handlers
	for (auto handler : this->disconnectHandlers) {
		handler(conn);
	}
}

void WebsocketServer::onMessage(ClientConnection conn, WebsocketEndpoint::message_ptr msg)
{
	//Validate that the incoming message contains valid JSON
	std::string const payload = msg->get_payload();
	try {
		const json json_message = json::parse(payload.begin(), payload.end());
		if (!json_message.empty())
		{
			const auto text_message = TextMessage::text_message_factory(json_message);
	
			//Output the details
			std::clog << "Received message of type \"" << text_message.text << R"(".)" << std::endl;
			std::clog << "Message payload:" << std::endl;
	
			std::string messageType = "__MESSAGE__";
			json reply;
			reply[messageType] = text_message.text;
			sendMessage(conn, json_message.dump());
	
			//If any handlers are registered for the message type, invoke them
			auto& handlers = this->messageHandlers[messageType];
			for (auto handler : handlers) {
				handler(conn, json_message);
			}
		}
	}
	
	catch (const std::exception& ex) {
		std::clog << "Error parsing json " << payload << std::endl;
		std::clog << ex.what() << std::endl;
	}
}

void WebsocketServer::sendMessage(ClientConnection conn, std::string message)
{
	this->endpoint.send(conn, message, websocketpp::frame::opcode::text);
}

void WebsocketServer::broadcastMessage(json msg)
{
	//Prevent concurrent access to the list of open connections from multiple threads
	std::lock_guard<std::mutex> lock(this->connectionListMutex);
	
	for (auto conn : this->openConnections) {
		this->sendMessage(conn, msg.dump());
	}
}

size_t WebsocketServer::numConnections()
{
	//Prevent concurrent access to the list of open connections from multiple threads
	std::lock_guard<std::mutex> lock(this->connectionListMutex);
	return this->openConnections.size();
}

