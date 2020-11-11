BEGIN {
recvdSize = 0
txsize=0
drpSize=0
startTime = 0
stopTime = 0
thru=0
}
 {
 event = $1
 time = $2
 node_id = $3
 pkt_size = $6
 level = $5
 
 #Store start time
 if (level == "rtProtoLS" && (event == "+" || event == "s") )
   {if (time <= startTime)
     {startTime = time}
     txsize++;
   }
   
#Update total received packetsÃ¢â‚¬â„¢ size and store packets arrival time
 if (level == "rtProtoLS" && event == "r" ) 
 {
  if (time >= stopTime) 
     {stopTime = time}
      recvdSize++
 }
 if (level == "rtProtoLS" && event == "d" ) 
     {drpSize++}
  }
 END {
 printf("\n\t\tLink State Routing - rtProtoLS Packets\n\nAverage Throughput[kbps]\t=\t%.2f\nPackets Sent\t\t\t=\t%.2f\nPackets Dropped\t\t\t=\t%.2f\nPackets Received\t\t=\t%.2f\nStart Time[s]\t\t\t=\t%.2f\nStop Time[s]\t\t\t=\t%.2f\n",(recvdSize/(stopTime - startTime)),txsize,drpSize,recvdSize,startTime,stopTime)}
