package com.hsjavaclass.systemsprog;

import java.io.IOException;
import java.net.ServerSocket;
import java.util.Set;
import java.util.concurrent.ConcurrentHashMap;
import java.util.Map;
import java.util.HashMap;

public class DatabaseServer {
	private Set<DatabaseClientHandler> clients = ConcurrentHashMap.newKeySet();
	private Map<String, String> database = new HashMap<>();

	public DatabaseServer(int port) {
		try (ServerSocket server = new ServerSocket(port)) {
			while (true) {
				DatabaseClientHandler client = new DatabaseClientHandler(this, server.accept());
				clients.add(client);
				client.start();
			}
		} catch (IOException e) {
			System.out.println("The server failed on port: " + port + ".");
		}
	}

	public synchronized void broadcast(String message) {
		String response = "Answer: ";

		String[] args = message.split(" ");

		if (args[0].equals("get")) {
			if (database.get(args[1]) != null)
				response += database.get(args[1]) + "\n";
			else
				response += "Key not found!\n";
		} else if (args[0].equals("put")) {
			if (args.length == 3) {
				database.put(args[1], args[2]);
				response += "Key-Value added!\n";
			} else
				response += "Invalid arguments!\n";
		} else if (args[0].equals("list")) {
			if (database.size() > 0) {
				response += "Current pairs:\n";
				for (String key : database.keySet()) {
					response += key + ": " + database.get(key) + "\n";
				}
			} else
				response += "The database is empty."
		} else
			response += "Invalid command.";

		for (DatabaseClientHandler client : clients)
			client.sendMessage(response);
	}

	public void clientDisconnected(DatabaseClientHandler client) {
		clients.remove(client);
	}

	public static void main(String[] args) {
		DatabaseServer server = new DatabaseServer(8085);
	}
}