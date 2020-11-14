#To simulate a ring topology

#Create new simulator object
set ns [new Simulator]

#Open the nam trace file
set nf [open Ring.nam w]
$ns namtrace-all $nf

set tf [open Ring.tr w]
$ns trace-all $tf

#Define finish procedure
proc finish {} {
    global ns nf tf
    $ns flush-trace
    close $nf
    close $tf
    exec nam Ring.nam &
    exit 0
}

#Create the topology
set n0 [$ns node]
set n1 [$ns node]
set n2 [$ns node]
set n3 [$ns node]
set n4 [$ns node]
set n5 [$ns node]

#Create links
$ns duplex-link $n0 $n1 1Mb 10ms DropTail
$ns duplex-link $n1 $n2 1Mb 10ms DropTail
$ns duplex-link $n2 $n3 1Mb 10ms DropTail
$ns duplex-link $n3 $n4 1Mb 10ms DropTail
$ns duplex-link $n4 $n5 1Mb 10ms DropTail
$ns duplex-link $n5 $n0 1Mb 10ms DropTail

#Create a TCP agent & attach to n0
set tcp [new Agent/TCP/Newreno]
$tcp set class_ 1
$ns attach-agent $n0 $tcp

#Create TCP sink & attach to n4
set sink [new Agent/TCPSink]
$ns attach-agent $n4 $sink

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
