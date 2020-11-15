#3.Using NS2, simulate a wired network of 10 nodes. Establish two TCP and two UDP flow which passes 
#through a bottleneck link (node 5 to node6). CO1,3, K4

#a. Simulate the given topology.

#b. Draw congestion window and of both TCP flow in the same graph

#c. Using any scripting language, find the total number of bytes received by each node in TCP flow1.

#d. Find how much time it takes for each packet to reach receiver in UDP flow2.

#Initialize the simulator
set ns [new Simulator]

#Setting the colors
$ns color 0 blue
$ns color 1 red
$ns color 2 green
$ns color 3 pink

#Files for trace, namtrace & xgraph
set f [open CW.tr w]
$ns trace-all $f
set nf [open CW.nam w]
$ns namtrace-all $nf
set xf1 [open CW1.xg w]
set xf2 [open CW2.xg w]

#Define the finish procedure
proc finish {} {
    global ns f nf xf1 xf2
    $ns flush-trace
    close $f
    close $nf
    close $xf1
    close $xf2
    puts "Running NAM file..."
    exec nam CW.nam &
    puts "Running the XGraph..."
    exec /home/vishakan/XGraph4.38_linux64/bin/xgraph -color blue CW1.xg -color red CW2.xg &
    #exec /home/vishakan/XGraph4.38_linux64/bin/xgraph CW2.xg &
    exit 0
}

#Setting up the topology
set n1 [$ns node]
set n2 [$ns node]
set n3 [$ns node]
set n4 [$ns node]
set n5 [$ns node]
set n6 [$ns node]
set n7 [$ns node]
set n8 [$ns node]
set n9 [$ns node]
set n10 [$ns node]


#Initalising the links
$ns duplex-link $n1 $n3 5Mb 2ms DropTail
$ns duplex-link $n3 $n7 5Mb 2ms DropTail
$ns duplex-link $n7 $n9 5Mb 2ms DropTail
$ns duplex-link $n9 $n10 5Mb 2ms DropTail
$ns duplex-link $n3 $n2 5Mb 2ms DropTail
$ns duplex-link $n3 $n6 5Mb 2ms DropTail
$ns duplex-link $n7 $n8 5Mb 2ms DropTail
$ns duplex-link $n4 $n3 5Mb 2ms DropTail
$ns duplex-link $n3 $n5 5Mb 2ms DropTail
$ns duplex-link $n5 $n7 5Mb 2ms DropTail

#Bottlenecked link
$ns duplex-link $n5 $n6 0.5Mb 50ms DropTail
$ns queue-limit $n5 $n6 10
$ns duplex-link-op $n5 $n6 queuePos 0.5


#Setting up the TCP connections with FTP application

#N1 - N9 TCP
set tcp_1 [new Agent/TCP]
$ns attach-agent $n1 $tcp_1
$tcp_1 set class_ 0

set sink_1 [new Agent/TCPSink]
$ns attach-agent $n9 $sink_1
$ns connect $tcp_1 $sink_1

set ftp_1 [new Application/FTP]
$ftp_1 attach-agent $tcp_1


#N2 - N8 TCP
set tcp_2 [new Agent/TCP/Newreno]
$ns attach-agent $n2 $tcp_2
$tcp_2 set class_ 1

set sink_2 [new Agent/TCPSink/DelAck]
$ns attach-agent $n8 $sink_2
$ns connect $tcp_2 $sink_2

set ftp_2 [new Application/FTP]
$ftp_2 attach-agent $tcp_2

#Setting up the UDP connections with CBR application

#N3 - N7 UDP
set udp_1 [new Agent/UDP]
$ns attach-agent $n3 $udp_1
$udp_1 set class_ 2

set null_1 [new Agent/Null]
$ns attach-agent $n7 $null_1
$ns connect $udp_1 $null_1

set cbr_1 [new Application/Traffic/CBR]
$cbr_1 attach-agent $udp_1

#N4 - N10 UDP
set udp_2 [new Agent/UDP]
$ns attach-agent $n4 $udp_2
$udp_2 set class_ 3

set null_2 [new Agent/Null]
$ns attach-agent $n10 $null_2
$ns connect $udp_2 $null_2

set cbr_2 [new Application/Traffic/CBR]
$cbr_2 attach-agent $udp_2

#Set the simulation events
$ns at 0.0 "$cbr_1 start"
$ns at 0.0 "$cbr_2 start"
$ns at 0.0 "$ftp_1 start"
$ns at 0.0 "$ftp_2 start"
$ns at 3.0 "$cbr_1 stop"
$ns at 4.0 "$cbr_2 stop"
$ns at 4.5 "$ftp_1 stop"
$ns at 5.0 "$ftp_2 stop"

#Call the finish procedure after execution
$ns at 5.5 "finish"

#Record the cwnd at every 0.1 second interval for the plot
proc record {xgFile tcpFile} {
    global ns
    set now [$ns now]
    set cwnd [$tcpFile set cwnd_]
    puts $xgFile "$now $cwnd"

    $ns at [expr $now + 0.1] "record $xgFile $tcpFile"
}

#Call the record procedure at the start
$ns at 0.0 "record $xf1 $tcp_1"
$ns at 0.0 "record $xf2 $tcp_2"

#Run the simulation
$ns run




