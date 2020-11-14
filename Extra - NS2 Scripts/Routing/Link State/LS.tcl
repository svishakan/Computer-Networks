#Simulation of Link State Routing

#Defining a new simulator object
set ns [new Simulator]

#Set Colors
$ns color 1 Blue	
#TCP
$ns color 2 Red
#UDP

#Open the nam trace file
set f [open LS.tr w]
$ns trace-all $f
set nf [open LS.nam w]
$ns namtrace-all $nf

#Defining a finish procedure to execute the NAM file
proc finish {} {
	global ns f nf
	$ns flush-trace
	close $f
	close $nf
	puts "Executing the NAM..."
	exec nam LS.nam &
	puts "Throughputs & Packet Information for different packets\n"
	exec python LS.py &
	exit 0
}

#Constructing the topology
set n(1) [$ns node]
set n(2) [$ns node]
set n(3) [$ns node]
set n(4) [$ns node]
set n(5) [$ns node]
set n(6) [$ns node]

$ns duplex-link $n(1) $n(3) 2Mb 10ms DropTail
$ns duplex-link $n(2) $n(3) 2Mb 10ms DropTail
$ns duplex-link $n(3) $n(4) 0.3Mb 100ms DropTail
$ns duplex-link $n(4) $n(5) 0.5Mb 40ms DropTail
$ns duplex-link $n(4) $n(6) 0.5Mb 40ms DropTail

$ns duplex-link-op $n(1) $n(3) orient right-down
$ns duplex-link-op $n(2) $n(3) orient right-up
$ns duplex-link-op $n(3) $n(4) orient right
$ns duplex-link-op $n(4) $n(5) orient right-up
$ns duplex-link-op $n(4) $n(6) orient right-down

#Monitor the queue for the link n3-n4
$ns queue-limit $n(3) $n(4) 10
$ns duplex-link-op $n(3) $n(4) queuePos 0.5


#Creating TCP agent between n2 to n6
#Implementing a Newreno congestion control based TCP
#Implementing a DelAck TCPSink that delays acknowledgements

set tcp [new Agent/TCP/Newreno]
$tcp set class_ 1
$ns attach-agent $n(2) $tcp
set sink [new Agent/TCPSink/DelAck]
$ns attach-agent $n(6) $sink
$ns connect $tcp $sink

#upper limit on advertised window size for TCP connection
$tcp set window_ 8000

#TCP packet size in bytes
$tcp set packetSize_ 512

# Create a FTP application over TCP
set ftp [new Application/FTP]
$ftp attach-agent $tcp
$ftp set type_ FTP

#Creating UDP agent between n1 to n5
set udp0 [new Agent/UDP]
$udp0 set class_ 2
$ns attach-agent $n(1) $udp0
set null0 [new Agent/Null]
$ns attach-agent $n(5) $null0
$ns connect $udp0 $null0

# Create a CBR traffic source and attach it to udp0
#Constant Bit Rate traffic with packet size 1024 bytes, no random size packets, at 0.1Mb rate
set cbr [new Application/Traffic/CBR]
$cbr set type_ CBR
$cbr set packet_size_ 1024
$cbr set rate_ 0.1mb
$cbr set random_ false
$cbr attach-agent $udp0

#Stimulating LS routing protocol
$ns rtproto LS

#The n3-n4 link goes down at 4.0 sec, effectively
#stopping the transmission of packets
$ns rtmodel-at 4.0 down $n(3) $n(4)

#Schedule events for the CBR agent
$ns at 0.0 "$cbr start"
$ns at 1.0 "$ftp start"
$ns at 5.0 "$cbr stop"
$ns at 6.0 "$ftp stop"

#Call the finish procedure after 5 seconds simulation time
$ns at 6.0 "finish"

#Run the simulation
$ns run



