package com.hsjavaclass.systemsprog;

import java.io.*;
import java.net.Socket;

public class DatabaseClientHandler extends Thread {
	private final DatabaseServer server;
	private final Socket client;
	private final DataInputStream in;
	private final DataOutputStream out;

	public DatabaseClientHandler(DatabaseServer server, Socket socket) throws IOException {
		this.server = server;
		client = socket;
		in = new DataInputStream(socket.getInputStream());
		out = new DataOutputStream(socket.getOutputStream());

		System.out.println("A new client connected: from " + client.getInetAddress());
	}

	@Override
	public void run() {
		try (client; in; out) {
			while (true) {
				if (in.available() > 0)
					server.broadcast(in.readUTF());
			}
		} catch (IOException e) {
			server.clientDisconnected(this);
		}
	}

	public void sendMessage(String message) {
		try {
			out.writeUTF(message);
			out.flush();
		} catch (IOException e) {
			server.clientDisconnected(this);
		}
	}
}