package com.hsjavaclass.systemsprog;

import java.io.*;
import java.net.Socket;
import java.net.UnknownHostException;

public class DatabaseClient {
	private DataInputStream in;
	private DataOutputStream out;
	private BufferedReader console;

	public DatabaseClient(String server, int port) {
		try {
			Socket conn = new Socket(server, port);
			in = new DataInputStream(conn.getInputStream());
			out = new DataOutputStream(conn.getOutputStream());
			console = new BufferedReader(new InputStreamReader(System.in));

			Thread sendMessage = new Thread(() -> {
				while (true) {
					String message;
					try {
						if ((message = console.readLine()) != null) {
							out.writeUTF(message);
							out.flush();
						}
					} catch (IOException e) {
						e.printStackTrace();
					}
				}
			});

			Thread readMessage = new Thread(() -> {
				while (true) {
					try {
						if (in.available() > 0)
							System.out.println(in.readUTF());
					} catch (IOException e) {

						e.printStackTrace();
					}
				}
			});

			sendMessage.start();
			readMessage.start();
		} catch (UnknownHostException e) {
			System.out.println("Server not found!");
		} catch (IOException e) {
			System.out.println("Lost connection to server.");
		}
	}

	public static void main(String[] args) {
		new DatabaseClient("127.0.0.1", 8085);
	}
}