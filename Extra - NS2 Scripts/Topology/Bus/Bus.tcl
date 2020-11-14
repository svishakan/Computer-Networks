#To simulate a bus topology

#Create new simulator object
set ns [new Simulator]

#Open the nam trace file
set nf [open Bus.nam w]
$ns namtrace-all $nf

set tf [open Bus.tr w]
$ns trace-all $tf

#Define finish procedure
proc finish {} {
    global ns nf tf
    $ns flush-trace
    close $nf
    close $tf
    exec nam Bus.nam &
    exit 0
}

#Create the topology
set n0 [$ns node]
set n1 [$ns node]
set n2 [$ns node]
set n3 [$ns node]
set n4 [$ns node]

#Create LAN
set lan0 [$ns newLan "$n0 $n1 $n2 $n3 $n4" 0.5Mb 40ms LL Queue/DropTail MAC/Csma/Cd Channel]

#Create a TCP agent & attach to n0
set tcp [new Agent/TCP/Newreno]
$tcp set class_ 1
$ns attach-agent $n0 $tcp

#Create TCP sink & attach to n3
set sink [new Agent/TCPSink]
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
