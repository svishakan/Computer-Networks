import socket

def client_program():

	host = socket.gethostname()
	port = 8000

	sockfd = socket.socket()
	sockfd.connect((host, port))

	msg = input("Enter a message: ")

	while msg.lower().strip() != 'bye':
		sockfd.send(msg.encode())
		data = sockfd.recv(1024).decode()

		print("Received from server:", data)

		msg = input("Enter a message: ")

	sockfd.close()

if __name__ == "__main__":
	client_program()