import socket

def server_program():

	host = socket.gethostname()
	port = 8000

	sockfd = socket.socket()
	sockfd.bind((host, port))
	sockfd.listen(2)

	conn, addr = sockfd.accept()

	print("Connection from : ", str(addr))

	while True:
		data = conn.recv(1024).decode()

		if not data:
			break

		print("From Connected User:", str(data))
		data = input("Enter a message: ")
		conn.send(data.encode())

	conn.close()

if __name__ == "__main__":
	server_program()