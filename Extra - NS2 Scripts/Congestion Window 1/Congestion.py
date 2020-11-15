"""
Python script to find the throughput of the given TCL script's trace file.
"""

def find_throughput(start_node, end_node):
	"""To find throughput. """

	trace_file = open("Congestion.tr",'r')
	lines = trace_file.readlines()
	trace_file.close()

	flag, count = 0, 0
	et, st, th = 0, 0, 0

	for line in lines:
		params = line.split()
	
		if int(params[2]) == start_node and int(params[3]) == end_node:
			if flag == 0:
				st=float(params[1])
				flag = 1

			if params[0] == 'r':
				count+=1
				et=float(params[1])

	time = et - st
	
	if count != 0:
		#Calculate the throughput in kbps (Kilo Bits Per Second)
		#Assuming that packet size is 1000 bytes

		th = (count * 8) / time		#Byte to Bit conversion
		print("%-5d %-5d %10.2f %12d %22.2f" %(start_node, end_node, time, count, th))

if __name__ == "__main__":
	"""Driver code. """

	num = int(input("Enter the number of nodes: "))

	print("\nFrom  To    Time (in s)   Pkts Rcvd.   Throughput (in kbps)")

	for i in range(num):
		for j in range(num):
			find_throughput(i, j)

	#Printing dropped packets
	trace_file = open("Congestion.tr", "r")
	lines = trace_file.readlines()
	trace_file.close()

	dropped = 0

	for line in lines:
		params = line.split()
		
		if params[0] == 'd':
			dropped += 1
		
	print("\nNumber of dropped packets:", dropped)	 

