#To simulate a star topology

#Create new simulator object
set ns [new Simulator]

#Open the nam trace file
set nf [open Star.nam w]
$ns namtrace-all $nf

set tf [open Star.tr w]
$ns trace-all $tf

#Define finish procedure
proc finish {} {
    global ns nf tf
    $ns flush-trace
    close $nf
    close $tf
    exec nam Star.nam &
    exit 0
}

#Create the topology
set n0 [$ns node]
set n1 [$ns node]
set n2 [$ns node]
set n3 [$ns node]
set n4 [$ns node]
set n5 [$ns node]

#Change centre node shape
$n0 shape square

#Create links
$ns duplex-link $n0 $n1 1Mb 10ms DropTail
$ns duplex-link $n0 $n2 1Mb 10ms DropTail
$ns duplex-link $n0 $n3 1Mb 10ms DropTail
$ns duplex-link $n0 $n4 1Mb 10ms DropTail
$ns duplex-link $n0 $n5 1Mb 10ms DropTail


#Create a TCP agent & attach to n1
set tcp [new Agent/TCP/Newreno]
$tcp set class_ 1
$ns attach-agent $n1 $tcp

$tcp set window_ 8000
$tcp set packetSize_ 512

#Create TCP sink & attach to n3
set sink [new Agent/TCPSink/DelAck]
$ns attach-agent $n3 $sink

$ns connect $tcp $sink

#Create FTP & to tcp
set ftp [new Application/FTP]
$ftp set packetSize_ 1024
$ftp attach-agent $tcp

#Schedule events for simulation
$ns at 0.0 "$ftp start"
$ns at 5.0 "$ftp stop"

#Call finish after simulation ends
$ns at 5.0 "finish"

#Run the simulation
$ns run
