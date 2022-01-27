/*

Una rete WiFi indoor è costituita da 8 Stazioni Wireless organizzate su una griglia,
avente parametri GridWidth=4, DeltaX=6, DeltaY=6, e da un AP posto al centro della
topologia. L'AP è connesso ad un server attraverso un link p2p avente i seguenti
parametri: Delay=5ms, DataRate=120Mbps.

Si consideri un numero di flussi TCP, variabile da 1 a 8, tra le stazioni WiFi e il server.
Ogni flusso TCP, avente DataRate=1Mbps, è modellabile con applicazione ON-OFF che
inizia all’istante t=1sec avente periodo ON costante per tutta la durata della
simulazione, di durata 30sec. In parallelo, si consideri un numero di flussi UDP,
variabile da 1 a 8, tra ciascuna delle stazioni WiFi e il server. Ogni flusso UDP è
modellabile come traffico CBR avente Data Rate=1Mbps, inizia all’istante t=1sec e si
conclude alla fine della simulazione.

Ad ogni run della simulazione, il numero di flussi TCP e UDP deve essere equivalente.
Ad esempio, al run 1 abbiamo 1 flusso TCP e 1 flusso UDP, al run 2 abbiamo 2 flussi
TCP e 2 flussi UDP, al run 3 abbiamo 3 flussi TCP e 3 flussi UDP, etc.

Prima campagna di simulazione: Nello scenario descritto, si valutino le seguenti
metriche medie: throughput, delay, packet losses, pacchetti ricevuti, al variare del
numero dei flussi e al variare dello standard WiFi: 11a, 11g, 11ac.

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


int 
main (int argc, char *argv[]){

bool verbose = false; 
bool tracing = false;
bool output  =  true;



std::string tcpVariant = "TcpNewReno";                                  
uint32_t nWifi = 8;
uint32_t flows = 16;



  

CommandLine cmd (__FILE__);
cmd.AddValue ("verbose", "Tell applications to log if true", verbose);
cmd.AddValue ("tracing", "Enable pcap tracing", tracing);
cmd.AddValue ("flows", "Number of cbr flows ", flows);
cmd.AddValue ("output", "output in txt", output);
cmd.Parse (argc,argv);



WifiHelper wifi;
wifi.SetStandard (WIFI_PHY_STANDARD_80211g);


/*
if(flows >nWifi){
  std::cout << "the number of flows should be minor or equal to the number of STA that is"+nWifi <<std::endl;
  return 1;
}
*/

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

 
  static const uint32_t packetSize = 1420;

  uint32_t sta=flows/2; 
  
  /*i flussi si incrementano a due a due, 
  con flows = 2 avrò 1 flusso TCP e 1 flusso UDP, entrambi diretti alla stazione 1
  con flows = 4 avrò 2 flussi TCP e 2 flussi UDP, di cui 2 flussi (1 TCP e 1 UDP) sono diretti alla stazione 2 e altri 2 sono diretti alla stazione 1
  e così via fino ad avere 16 flussi, a 2 a 2 diretti nelle 8 stazioni 
*/
 std::cout<<"Simulo "<< flows << " flussi totali di cui: " << sta << " flussi UDP e " << sta << " flussi TCP" << std::endl;


  for(uint32_t j=0; j<sta; j++){
    //FLUSSO UDP BEGIN ******************

	 PacketSinkHelper UDPsink ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), 9));
     ApplicationContainer UDPsinkApp = UDPsink.Install (wifiStaNodes.Get (j)); //STA che riceve i pacchetti dal server
     UDPsinkApp.Start (Seconds (1.0));
     UDPsinkApp.Stop(Seconds (30.0));

     OnOffHelper onoffUDP ("ns3::UdpSocketFactory", InetSocketAddress (interfaces.GetAddress (j), 9));
     onoffUDP.SetAttribute ("PacketSize", UintegerValue (packetSize));
     onoffUDP.SetConstantRate (DataRate ("1Mb/s"), packetSize);
     onoffUDP.SetAttribute ("StartTime", TimeValue (Seconds (1.0)));
     onoffUDP.SetAttribute ("StopTime", TimeValue (Seconds (30.0)) );
     ApplicationContainer UDPappSource = onoffUDP.Install (p2pNodes.Get (0)); //Sul SERVER installo l'applicazione che manda i pkts
     UDPappSource.Start(Seconds(1.0));
     UDPappSource.Stop(Seconds(30.0));


     std::cout<<"Creato flusso UDP verso la stazione "<< wifiStaNodes.Get (j)->GetId()<<std::endl;
     //FLUSSO UDP END *******************


     //FLUSSO TCP BEGIN **************

     PacketSinkHelper TCPsink ("ns3::TcpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), 10));
     ApplicationContainer TCPsinkApp = TCPsink.Install (wifiStaNodes.Get (j));
     TCPsinkApp.Start (Seconds (1.0));
     TCPsinkApp.Stop(Seconds (30.0));


     OnOffHelper server ("ns3::TcpSocketFactory", (InetSocketAddress (interfaces.GetAddress (j), 10)));
     server.SetAttribute ("PacketSize", UintegerValue (packetSize));
     server.SetAttribute ("DataRate", DataRateValue (DataRate ("1Mb/s")));
     server.SetAttribute ("StartTime", TimeValue (Seconds (1.0))); 
     server.SetAttribute ("StopTime", TimeValue (Seconds (30.0)) );

     ApplicationContainer serverApp = server.Install (p2pNodes.Get(0));
     serverApp.Start (Seconds (1.0));
     serverApp.Stop(Seconds (30.0));
     std::cout<<"Creato flusso TCP verso la stazione "<< wifiStaNodes.Get (j)->GetId()<<std::endl;
     //FLUSSO TCP END
  }
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
