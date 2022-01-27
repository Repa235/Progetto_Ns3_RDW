/*

Seconda campagna di simulazione: Nello stesso ambiente di rete, si consideri un
nuovo scenario applicativo in cui esiste solo un flusso TCP, tra il server e la stazione
zero, avente DataRate=10Mbps, modellabile con applicazione ON-OFF che inizia
all’istante t=1sec avente periodo ON costante per tutta la durata della simulazione, di
durata 30sec. Si grafici il throughput istantaneo in questa condizione, e quando il
flusso TCP coesiste con un flusso UDP-CBR avente DataRate=10Mbps, inizia all’istante
t=1sec e si conclude alla fine della simulazione.

Output: Un grafico con Throughput istantaneo (asse Y) al variare del tempo in secondi
(asse X). Nel grafico dovranno apparire due curve, una per il caso “solo flusso TCP”,
una per il caso in cui il flusso TCP coesiste con quello UDP.

*/

#include "ns3/core-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/mobility-module.h"
#include "ns3/internet-module.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/ssid.h"

#include "ns3/flow-monitor-helper.h"
#include "ns3/ipv4-flow-classifier.h"

#include <fstream>


using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("ScriptAntonioRepaci");
	std::string nomefile = "Script_Repaci";

int printMyString(std::string a, double b){ 
std::cout << a << " " << b  <<std::endl;
return 0;
}

Ptr<PacketSink> sinkTCP;                         /* Pointer to the packet TCP sink application */
uint64_t lastTotalRxTCP = 0;                     /* The value of the last total received bytes */

Ptr<PacketSink> sinkUDP;                         /* Pointer to the packet UDP sink application */
uint64_t lastTotalRxUDP = 0;                     /* The value of the last total received bytes */

void
CalculateThroughputTCP () //we select a target TCP flow and calculate the instant throughput
{
  Time now = Simulator::Now ();                                         /* Return the simulator's virtual time. */
  double cur = (sinkTCP->GetTotalRx () - lastTotalRxTCP) * (double) 8 / 1e5;     /* Convert Application RX Packets to MBits. */

  lastTotalRxTCP = sinkTCP->GetTotalRx ();

  std::ofstream myfile; //open file without overwrite previous stats
  myfile.open ("instant_ThroughputTCP.txt", std::ios_base::app);

  myfile << now.GetSeconds () << " \t" << cur << std::endl; //print time and Mbis/sec for stats!
  myfile.close();

  Simulator::Schedule (MilliSeconds (100), &CalculateThroughputTCP);

}

void
CalculateThroughputUDP () ////we select a target UDP flow and calculate the instant throughput
{
  Time now = Simulator::Now ();                                         /* Return the simulator's virtual time. */
  double cur = (sinkUDP->GetTotalRx () - lastTotalRxUDP) * (double) 8 / 1e5;     /* Convert Application RX Packets to MBits. */
  lastTotalRxUDP = sinkUDP->GetTotalRx ();

  std::ofstream myfile; //open file without overwrite previous stats
  myfile.open ("instant_ThroughputUDP.txt", std::ios_base::app);

  myfile << now.GetSeconds () << " \t" << cur << std::endl; //print time and Mbis/sec for stats!
  myfile.close();


  Simulator::Schedule (MilliSeconds (100), &CalculateThroughputUDP);

}

int 
main (int argc, char *argv[]){

bool verbose = false; 
bool tracing = false;
bool output  =  true;



std::string tcpVariant = "TcpNewReno";                                  
uint32_t nWifi = 8;
uint32_t flows = 4;
std::string dataRate = "10Mbps";    
static const uint32_t packetSize = 1420;

  

CommandLine cmd (__FILE__);
cmd.AddValue ("verbose", "Tell applications to log if true", verbose);
cmd.AddValue ("tracing", "Enable pcap tracing", tracing);
cmd.AddValue ("flows", "Number of cbr flows ", flows);
cmd.AddValue ("output", "output in txt", output);
cmd.Parse (argc,argv);



WifiHelper wifi;
wifi.SetStandard (WIFI_PHY_STANDARD_80211a);


/*
if(flows >nWifi){
  std::cout << "the number of flows should be minor or equal to the number of STA that is"+nWifi <<std::endl;
  return 1;
}
*/

	//before starting, let's clean old statistics if any

	if( remove( "instant_ThroughputTCP.txt" ) != 0 )
		    perror( "Trying to delete instant_ThroughputTCP, but file not available" );
		  else
		    puts( "File instant_ThroughputTCP successfully deleted" );

	if( remove( "instant_ThroughputUDP.txt" ) != 0 )
		    perror( "Trying to delete instant_ThroughputUDP, but file not available" );
		  else
		    puts( "File instant_ThroughputUDP successfully deleted" );

if (verbose){
  LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
  LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);
  LogComponentEnable ("OnOffApplication", LOG_LEVEL_INFO);
  LogComponentEnable ("PacketSink", LOG_LEVEL_INFO);
}

/***CONFIGURAZIONE DEI NODI***/
NodeContainer p2pNodes; 
p2pNodes.Create (2); 
PointToPointHelper pointToPoint;
pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("120Mbps"));
pointToPoint.SetChannelAttribute ("Delay", StringValue ("5ms"));
NetDeviceContainer p2pDevices;
p2pDevices = pointToPoint.Install (p2pNodes);

NodeContainer wifiStaNodes;
wifiStaNodes.Create (nWifi);
NodeContainer wifiApNode = p2pNodes.Get (1); //Un server remoto si collega all'AP attraverso un link p2p


YansWifiChannelHelper channel = YansWifiChannelHelper::Default ();
YansWifiPhyHelper phy= YansWifiPhyHelper::Default (); //indoor wifi
phy.SetChannel (channel.Create ());
wifi.SetRemoteStationManager ("ns3::MinstrelHtWifiManager");

WifiMacHelper mac;
Ssid ssid = Ssid ("my-WIFI");
mac.SetType ("ns3::StaWifiMac",
              "Ssid", SsidValue (ssid),
              "ActiveProbing", BooleanValue (false));

NetDeviceContainer staDevices;
staDevices = wifi.Install (phy, mac, wifiStaNodes);
mac.SetType ("ns3::ApWifiMac",
              "Ssid", SsidValue (ssid));

NetDeviceContainer apDevices;
apDevices = wifi.Install (phy, mac, wifiApNode);

/***Mobilità***/
MobilityHelper mobility;
mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
                                "MinX", DoubleValue (0.0),
                                "MinY", DoubleValue (0.0),
                                "DeltaX", DoubleValue (6.0),
                                "DeltaY", DoubleValue (6.0),
                                "GridWidth", UintegerValue (4), //GridWidth: The number of objects laid out on a line
                                "LayoutType", StringValue ("RowFirst"));
mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
mobility.Install (wifiStaNodes);

Ptr<ListPositionAllocator> positionAlloc = CreateObject <ListPositionAllocator>();
positionAlloc ->Add(Vector(9, 3, 0)); // Access point
mobility.SetPositionAllocator(positionAlloc);
mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
mobility.Install(wifiApNode);
  
Ptr<ListPositionAllocator> positionAlloc1 = CreateObject <ListPositionAllocator>();
positionAlloc1 ->Add(Vector(15, 3, 0)); // Server
mobility.SetPositionAllocator(positionAlloc1);
mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
mobility.Install(p2pNodes.Get(0));

//Stack
InternetStackHelper stack;
stack.Install (wifiStaNodes);
stack.Install(p2pNodes);

Ipv4AddressHelper address;
address.SetBase ("10.1.3.0", "255.255.255.0");


Ipv4InterfaceContainer interfaces;
interfaces = address.Assign (staDevices);
address.Assign (apDevices);
address.SetBase ("10.1.1.0", "255.255.255.0");


Ipv4InterfaceContainer p2pInterfaces;
p2pInterfaces = address.Assign (p2pDevices);






//Applications begin **************************************************************************************


  // Install TCP Receiver on the access point 
    PacketSinkHelper sinkHelper ("ns3::TcpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), 9));
    ApplicationContainer sinkApp = sinkHelper.Install (wifiStaNodes.Get(0));
    sinkTCP = StaticCast<PacketSink> (sinkApp.Get (0)); //for tracing instant throughput
    sinkApp.Start (Seconds (0.0));
     sinkApp.Stop (Seconds (30.0));
    


    // Install TCP Transmitter on the station 
    OnOffHelper server ("ns3::TcpSocketFactory", (InetSocketAddress (interfaces.GetAddress (0), 9))); //indirizzo e porta di destinazione
    //Note. InetSocket holds an Ipv4Address and a port number to form an ipv4 transport endpoint.
    server.SetAttribute ("PacketSize", UintegerValue (packetSize));
    server.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
    server.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
    server.SetAttribute ("DataRate", DataRateValue (DataRate (dataRate)));
    ApplicationContainer serverApp = server.Install (p2pNodes.Get(0));

    serverApp.Start (Seconds (1.0));
    serverApp.Stop (Seconds (30.0));

    Simulator::Schedule (Seconds (0.0), &CalculateThroughputTCP);
/*
    PacketSinkHelper sink1 ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), 10));
  	ApplicationContainer apps_sink1 = sink1.Install (wifiStaNodes.Get(0));
  	sinkUDP = StaticCast<PacketSink> (apps_sink1.Get (0)); //for tracing instant throughput
  	apps_sink1.Start(Seconds (0.0));
    apps_sink1.Stop(Seconds (30.0));


  	OnOffHelper onoff ("ns3::UdpSocketFactory", InetSocketAddress (interfaces.GetAddress (0), 10)); //indirizzo e porta destinazione
  	onoff.SetAttribute ("PacketSize", UintegerValue (packetSize));
    onoff.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
    onoff.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
  	onoff.SetAttribute ("DataRate", DataRateValue (DataRate (dataRate)));
    ApplicationContainer apps_source1 = onoff.Install (p2pNodes.Get(0)); //sending pkts

  	apps_source1.Start(Seconds(1.0));
    apps_source1.Stop(Seconds(30.0));
  	Simulator::Schedule (Seconds (0.0), &CalculateThroughputUDP);
*/


//Applications end **************************************************************************************

//AAA
Ipv4GlobalRoutingHelper::PopulateRoutingTables ();




// AAA Install FlowMonitor on all nodes before Stopping
FlowMonitorHelper flowmon;
Ptr<FlowMonitor> monitor = flowmon.InstallAll ();

Simulator::Stop (Seconds (30.0));

if (tracing == true){
AsciiTraceHelper ascii;
Ptr<OutputStreamWrapper> stream = ascii.CreateFileStream (nomefile + ".tr");
phy.EnableAsciiAll (stream);
}

Simulator::Run ();

//AAA - Statistics to print
double avg_throughput = 0;
double avg_rx_pkts = 0;
double avg_pkts_lost = 0;
double avg_delay = 0;


  // Print per flow statistics
    monitor->CheckForLostPackets ();
    Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowmon.GetClassifier ());
    FlowMonitor::FlowStatsContainer stats = monitor->GetFlowStats ();
    for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = stats.begin (); i != stats.end (); ++i)
    {
     Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (i->first);
     std::cout << "Flow " << i->first << " (" << t.sourceAddress << " -> " << t.destinationAddress << ")\n";
     std::cout << "  Tx Packets: " << i->second.txPackets << "\n";
     std::cout << "  Tx Bytes:   " << i->second.txBytes << "\n";
     std::cout << "  TxOffered:  " << i->second.txBytes * 8.0 / 9.0 / 1000 / 1000  << " Mbps\n";
     std::cout << "  TxThroughp: " << i->second.txBytes * 8.0 / (i->second.timeLastRxPacket.GetSeconds () - i->second.timeFirstTxPacket.GetSeconds ()) / 1024 / 1024  << " Mbps\n";
     std::cout << "  Rx Packets: " << i->second.rxPackets << "\n";
     std::cout << "  Rx Bytes:   " << i->second.rxBytes << "\n";
     std::cout << "  Throughput: " << i->second.rxBytes * 8.0 / 9.0 / 1000 / 1000  << " Mbps\n";
     std::cout << "  Mean delay: " << i->second.delaySum.GetSeconds () / i->second.rxPackets << " s\n";

     double throughput= i->second.rxBytes * 8.0 / (i->second.timeLastRxPacket.GetSeconds () - i->second.timeFirstTxPacket.GetSeconds ()) / 1024 / 1024;
     double pkts_lost =  i->second.txPackets - i->second.rxPackets;
     double delay = i->second.delaySum.GetSeconds () / i->second.rxPackets;

     std::cout << "\n";
     avg_throughput = avg_throughput + throughput;
     avg_rx_pkts = avg_rx_pkts + i->second.rxPackets;
     avg_pkts_lost = avg_pkts_lost  + pkts_lost;

     avg_delay = avg_delay + delay;

   }

  if(output){

    	std::ofstream myfile; //open file without overwrite previous stats
    	myfile.open ("avg_throughput.txt", std::ios_base::app);
    	myfile <<flows<< " "<<avg_throughput/flows<<"\n";
    	myfile.close();

    	myfile.open ("avg_rx_pkts.txt",  std::ios_base::app);
    	myfile <<flows<< " "<< avg_rx_pkts/flows << "\n";
    	myfile.close();

    	myfile.open ("avg_lost_pkts.txt",  std::ios_base::app);
    	myfile <<flows<< " "<< avg_pkts_lost/flows << "\n";
    	myfile.close();

    	myfile.open ("avg_delay.txt",  std::ios_base::app);
    	myfile <<flows<< " "<< avg_delay/flows << "\n";
    	myfile.close();

  }

  Simulator::Destroy ();
  return 0;
}
