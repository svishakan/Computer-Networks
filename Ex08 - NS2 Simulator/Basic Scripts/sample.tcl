# Creating New Simulator
set ns [new Simulator]

# Setting up the traces
set f [open outEx1.tr w]
set nf [open outEx1.nam w]
$ns namtrace-all $nf
$ns trace-all $f



proc finish {} {

     global ns nf f

     $ns flush-trace

     close $nf

     close $f

     exec nam outEx1.nam &

     exit 0

}

#
#Create Nodes
#

set n0 [$ns node]
      puts "n0: [$n0 id]"
set n1 [$ns node]
      puts "n1: [$n1 id]"
set n2 [$ns node]
      puts "n2: [$n2 id]"
set n3 [$ns node]
      puts "n3: [$n3 id]"
set n4 [$ns node]
      puts "n4: [$n4 id]"

#
#Setup Connections
#

$ns duplex-link $n0 $n2 100Mb 5ms DropTail
$ns duplex-link $n2 $n4 54Mb 10ms DropTail
$ns duplex-link $n1 $n2 100Mb 5ms DropTail
$ns duplex-link $n2 $n3 54Mb 10ms DropTail
$ns queue-limit $n2 $n3 40
$ns simplex-link $n3 $n4 10Mb 15ms DropTail
$ns simplex-link $n4 $n3 10Mb 15ms DropTail


#
#Set up Transportation Level Connections
#

set tcp0 [new Agent/TCP]
$ns attach-agent $n1 $tcp0

set udp1 [new Agent/UDP]
$udp1 set dst_addr_ Unicast
$udp1 set fid_ 1
$ns attach-agent $n0 $udp1

set null0 [new Agent/Null]
$ns attach-agent $n3 $null0

set sink0 [new Agent/TCPSink]
$ns attach-agent $n4 $sink0

#
#Setup traffic sources
#

set ftp0 [new Application/FTP]
$ftp0 attach-agent $tcp0

set cbr0 [new Application/Traffic/CBR]
$cbr0 set rate_ 2Mb
$cbr0 set packetSize_ 1000
$cbr0 attach-agent $udp1
$ns connect $udp1 $null0
$udp1 set fid_ 0
$ns connect $tcp0 $sink0
$tcp0 set fid_ 1

#
#Start up the sources
#

$ns at 0.05 "$ftp0 start"
$ns at 0.1 "$cbr0 start"
$ns at 60.0 "$ftp0 stop"
$ns at 60.5 "$cbr0 stop"
$ns at 61.0 "finish"
$ns run



$ns run
