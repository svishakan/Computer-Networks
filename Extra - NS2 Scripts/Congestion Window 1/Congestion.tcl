#Simulation of congestion control in a network using a bottlenecked link

#To see different congestion control algorithms in action:
#Change the TCP agent to Reno, Newreno, Vegas etc.
#Default TCP agent is Tahoe.

#Creating a new simulator object
set ns [new Simulator]

#Setting the colors for different packets
$ns color 0 violet
$ns color 1 green
$ns color 2 orange
$ns color 3 red

#Create the topology
set n0 [$ns node]
set n1 [$ns node]
set n2 [$ns node]
set n3 [$ns node]
set n4 [$ns node]
set n5 [$ns node]
set n6 [$ns node]
set n7 [$ns node]

#Open the trace files
set f [open Congestion.tr w]
$ns trace-all $f
set nf [open Congestion.nam w]
$ns namtrace-all $nf

#Define the finish procedure
proc finish {} {
	global ns f nf
	$ns flush-trace
	close $f
	close $nf
	puts "Executing NAM..."
	exec nam Congestion.nam &
	puts "Plotting using XGraph..."
	
	#Give correct path to XGraph
	exec /home/vishakan/XGraph4.38_linux64/bin/xgraph Congestion.xg &
	exit 0
}

#N1-N2 is a bottlenecked link
$ns duplex-link $n0 $n1 5Mb 2ms DropTail
$ns duplex-link $n1 $n2 1.5Mb 10ms DropTail
$ns duplex-link $n2 $n3 5Mb 2ms DropTail
$ns duplex-link $n1 $n4 5Mb 2ms DropTail
$ns duplex-link $n2 $n5 5Mb 2ms DropTail
$ns duplex-link $n1 $n6 5Mb 2ms DropTail
$ns duplex-link $n2 $n7 5Mb 2ms DropTail

$ns duplex-link-op $n0 $n1 orient right
$ns duplex-link-op $n1 $n2 orient right
$ns duplex-link-op $n2 $n5 orient right
$ns duplex-link-op $n1 $n6 orient up
$ns duplex-link-op $n4 $n1 orient up
$ns duplex-link-op $n2 $n3 orient up
$ns duplex-link-op $n7 $n2 orient up

#Creating UDP agents with CBR traffic
set udp0 [new Agent/UDP]
$ns attach-agent $n0 $udp0
$udp0 set class_ 0

set cbr0 [new Application/Traffic/CBR]
$cbr0 attach-agent $udp0

set null0 [new Agent/Null]
$ns attach-agent $n3 $null0
$ns connect $udp0 $null0

set udp1 [new Agent/UDP]
$ns attach-agent $n6 $udp1
$udp1 set class_ 1

set cbr1 [new Application/Traffic/CBR]
$cbr1 attach-agent $udp1

set null1 [new Agent/Null]
$ns attach-agent $n3 $null1
$ns connect $udp1 $null1

#Creating TCP agents with FTP application
set tcp1 [new Agent/TCP]
$tcp1 set class_ 2
$ns attach-agent $n4 $tcp1

set sink1 [new Agent/TCPSink]
$ns attach-agent $n7 $sink1
$ns connect $tcp1 $sink1

set ftp1 [new Application/FTP]
$ftp1 attach-agent $tcp1

set tcp2 [new Agent/TCP]
$tcp2 set class_ 3
$ns attach-agent $n5 $tcp2

set sink2 [new Agent/TCPSink]
$ns attach-agent $n6 $sink2
$ns connect $tcp2 $sink2

set ftp2 [new Application/FTP]
$ftp2 attach-agent $tcp2

#Print some details
puts "TCP Packet Size\t:\t[$tcp1 set packetSize_]"

#For the XGraph
set outfile [open "Congestion.xg" w]

proc plotWindow {tcpSource outfile} {
	#Use to plot the congestion window at every 0.1 second interval
	global ns
	set now [$ns now]
	set cwnd [$tcpSource set cwnd_]
	puts $outfile "$now $cwnd"
	$ns at [expr $now + 0.1] "plotWindow $tcpSource $outfile"
}



#Run the simulation
$ns at 0.0 "plotWindow $tcp1 $outfile"

$ns at 0.5 "$cbr0 start"
$ns at 1.5 "$cbr1 start"

$ns at 0.25 "$ftp1 start"
$ns at 1.75 "$ns detach-agent $n4 $tcp1 ; $ns detach-agent $n7 $sink1"

$ns at 0.75 "$ftp2 start"
$ns at 2.25 "$ns detach-agent $n5 $tcp2 ; $ns detach-agent $n6 $sink2"

$ns at 2.5 "finish"

$ns run
