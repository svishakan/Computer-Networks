"""
Python script to perform the following:

c. Using any scripting language, find the total number of bytes received by each node in TCP Flow 1.

d. Find how much time it takes for each packet to reach receiver in UDP flow2.
"""

trace_file = open("CW.tr", "r")
lines = trace_file.readlines()
trace_file.close()

def calculate_bytes_received(node):
	"""To find the number of bytes received by a node."""
	
	#trace_file = open("CW.tr", "r")
	#lines = trace_file.readlines()
	#trace_file.close()
	
	global lines
	
	rcvd_bytes = 0
	
	for line in lines:
		params = line.split()
		
		#TCP Flow 1 has a Flow ID of 0 in the TCL file
		#params[5] in the file has the no. of bytes of the packet
		if int(params[3]) == node and int(params[7]) == 0:
			if params[0] == "r":
				rcvd_bytes += int(params[5])
		
	print("Node {0} received {1} bytes of information.".format(node, rcvd_bytes))
	

def time_taken(pkt_id):
	"""To find the time taken by a given packet to reach the destination node of UDP Flow 2, i.e. node N10."""
	
	#trace_file = open("CW.tr", "r")
	#lines = trace_file.readlines()
	#trace_file.close()
	
	global lines
	
	st, et, flag = 0, 0, 0
	
	for line in lines:
		params = line.split()
		
		#Packet ID is stored in the last value of the parameters list
		#UDP Flow 2 has Flow ID 3 in the TCP script file
		if int(params[-1]) == pkt_id and int(params[7]) == 3:
		
			#To_Node in the parameters list is 9 for node N10
			#Receive event at node N10
			if int(params[3]) == 9 and params[0] == "r":
				et = float(params[1])
				time = round(et - st, 2)		
				return time * 1000			#time in ms
				#print("Time taken for Packet {0} to reach node N10: {1} ms".format(pkt_id, time * 1000))
			
			#Send event, from another node
			elif int(params[2]) != 9 and params[0] == "+" and flag == 0:
				st = float(params[1])
				flag = 1
	
	return -1	#Packet not of UDP Flow 2
	
	

def get_number_of_packets():
	"""To find the number of packets sent/rcvd. in an NS2 trace file."""

	#trace_file = open("CW.tr", "r")
	#lines = trace_file.readlines()
	#trace_file.close()
	
	global lines
	
	pkts = 0
	
	for line in lines:
		params = line.split()
		
		pkts = max(int(params[-1]), pkts)
		
	
	return pkts

if __name__ == "__main__":
	"""Driver code."""
	
	num = int(input("Enter the number of nodes: "))
	
	print("\nNumber of bytes received at each node in TCP Flow 1:\n")
	
	for i in range(num):
		calculate_bytes_received(i)
		
	
	pkts = get_number_of_packets()
	
	print("\nTotal number of packets sent/received:", pkts)
	
	
	count, tt, time = 0, 0, 0
	
	#Code takes a while to execute, depending upon the file size.
	#Needs to be optimized for faster output
	for i in range(pkts + 1):
		time = time_taken(i)
		
		if time != -1:	#Of UDP Flow 2
			count += 1
			tt += time
			
	avg_time = round(tt / count, 2)
	print("Average time taken for each UDP Flow 2 packet to reach node N10:", avg_time, "ms\n")
		
		
		
"""
OUTPUT:

Enter the number of nodes: 10

Number of bytes received at each node in TCP Flow 1:

Node 0 received 43040 bytes of information.
Node 1 received 0 bytes of information.
Node 2 received 1172520 bytes of information.
Node 3 received 0 bytes of information.
Node 4 received 0 bytes of information.
Node 5 received 0 bytes of information.
Node 6 received 1161080 bytes of information.
Node 7 received 0 bytes of information.
Node 8 received 1118040 bytes of information.
Node 9 received 0 bytes of information.

Total number of packets sent/received: 6381
Average time taken for each UDP Flow 2 packet to reach node N10: 46.53 ms

"""			

