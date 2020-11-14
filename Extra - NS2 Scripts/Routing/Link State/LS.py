"""
Python file to extract information from the Link-State Routing TCL script trace file.
"""

def print_stats(protocol, sent, rcvd, dropped, start, end):
	"""Prints the stats."""
	
	print("\n\t\tLink State Routing - {0} packets\n\n".format(protocol.upper()))
	print("Average Throughput\t:\t", round((rcvd/(end - start)), 2), " kbps")
	print("Packets Sent\t\t:\t", sent, " packets")
	print("Packets Dropped\t\t:\t", dropped, " packets")
	print("Packets Received\t:\t", rcvd, " packets")
	print("Start Time\t\t:\t", round(start, 2), " seconds")
	print("End Time\t\t:\t", round(end, 2), " seconds\n")

if __name__ == "__main__":
	"Usage: python ns_script.py <trace_file> <routing_algorithm>"
	
	

	trace_file = open("LS.tr", "r")
	lines = trace_file.readlines()
	trace_file.close()
	
	protocols = ["tcp", "cbr", "rtProtoLS"]
	
	sent, rcvd, dropped = 0, 0, 0
	start, end = 0, 0
	
	for protocol in protocols:
		for line in lines:
			"Format: 0-event 1-time 2-start_node 3-end_node 4-protocol 5-pkt_size"
			param = line.split()
		
			if param[4] == protocols[0] and (param[0] == "+" or param[0] == "s"):
			 	if float(param[1]) <= start:
			 		start = float(param[1])
			 	sent += 1
		
			if param[4] == protocols[0] and param[0] == "r":
				if float(param[1]) >= end:
					end = float(param[1])
				rcvd += 1
		
			if param[4] == protocols[0] and param[0] == "d":
				dropped += 1
			
		print_stats(protocol, sent, rcvd, dropped, start, end)
	
