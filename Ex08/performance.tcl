#Performance Evaluation of TCP and UDP sharing a bottleneck

#Create a new simulator object
set ns [new Simulator]

#Define different colors for data flows (for NAM)
$ns color 1 Blue
$ns color 2 Red

#Open the NAM trace file
set nf [open out_Perf.nam w]
$ns namtrace-all $nf

#Define a 'finish' procedure
proc finish {} {
    global ns nf
    $ns flush-trace

    #Close the NAM trace file
    close $nf
    
    #Execute NAM on the trace file
    exec nam out_Perf.nam &
    exit 0
}

#Create the six nodes
set n0 [$ns node]
set n1 [$ns node]
set n2 [$ns node]
set n3 [$ns node]
set n4 [$ns node]
set n5 [$ns node]

#Create links between the nodes
$ns	duplex-link     $n0	$n2	2Mbps	10ms	DropTail
$ns	duplex-link     $n1 $n2 2Mbps	10ms	DropTail
$ns	simplex-link    $n2	$n3	0.3Mbps	100ms	DropTail
$ns	simplex-link    $n3	$n2	0.3Mbps	100ms	DropTail
$ns	duplex-link     $n3	$n4	0.5Mbps	40ms	DropTail
$ns	duplex-link     $n3	$n5	0.5Mbps	40ms	DropTail

#Give node position (for NAM)
$ns	duplex-link-op	$n0	$n2	orient	right-down
$ns	duplex-link-op	$n1	$n2	orient	right-up
$ns	simplex-link-op	$n2	$n3	orient	right
$ns	simplex-link-op	$n3	$n2	orient	right
$ns	duplex-link-op	$n3	$n4	orient	left-up
$ns	duplex-link-op	$n3	$n5	orient	left-down

#Set Queue size of link n2-n3 to 10
$ns queue-limit $n2 $n3 10

#Monitor the queue for the link n2-n3
$ns simplex-link-op $n2 $n3 queuePos 0.5
$ns simplex-link-op $n3 $n2 queuePos 1.5

#Set up a TCP connection over n0 & n4 and its flow id, window size and packet size
set tcp [new Agent/TCP]
$ns attach-agent $n0 $tcp

set tcp_sink [new Agent/TCPSink/DelAck]
$ns attach-agent $n4 $tcp_sink

$ns connect $tcp $tcp_sink

$tcp set fid_ 1
$tcp set window_ 8000
$tcp set packetSize_ 512

#Set up a FTP over TCP connection
set ftp [new Application/FTP]
$ftp attach-agent $tcp
$tcp set type_ TCP

#Set up a UDP connection over n1 & n5 and its flow id
set udp [new Agent/UDP]
$ns attach-agent $n1 $udp

set null [new Agent/Null]
$ns attach-agent $n5 $null

$ns connect $udp $null

$udp set fid_ 2

#Set up a CDR over UDP connection with type, packet size, rate, random fields
set cbr [new Application/Traffic/CBR]

$cbr attach-agent $udp
$cbr set type_ CBR
$cbr set packet_size_ 1024
$cbr set rate_ 0.01Mb
$cbr set random_ false

#Schedule events for the CBR and FTP agents
$ns at 0.1 "$cbr start"
$ns at 1.0 "$ftp start"
$ns at 4.5 "$ftp stop"
$ns at 5.0 "$cbr stop"

#Detach TCP and Sink agents (not necessary)
$ns at 4.5 "$ns detach-agent $n0 $tcp; $ns detach-agent $n4 $tcp_sink"

#Call the finish procedure after 5 seconds of simulation time
$ns at 5.0 "finish"

#Print CBR packet size and interval
puts "CBR Packet Size   = [$cbr set packet_size_]"
puts "CBR Interval      = [$cbr set interval_]"

#Run the simulation
puts "Running the simulation..."
$ns run
