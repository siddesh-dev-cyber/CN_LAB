#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/netanim-module.h"
#include "ns3/point-to-point-module.h"

using namespace ns3;
int main(int argc,char*argv[]){
 CommandLine cmd;
 cmd.Parse(argc,argv);
 
 NodeContainer nodes;
 nodes.Create(2);
 
 PointToPointHelper pointToPoint;
 pointToPoint.SetDeviceAttribute("DataRate",StringValue("10Mbps"));
 pointToPoint.SetChannelAttribute("Delay",StringValue("2ms"));
 
 NetDeviceContainer devices;
 devices=pointToPoint.Install(nodes);
 
 InternetStackHelper stack;
 stack.Install(nodes);
 
 Ipv4AddressHelper address;
 address.SetBase("10.1.1.0","255.255.255.0");
 Ipv4InterfaceContainer interfaces=address.Assign(devices);
 
 UdpServerHelper server(9);
 ApplicationContainer serverApp=server.Install(nodes.Get(1));
 serverApp.Start(Seconds(1.0));
 serverApp.Stop(Seconds(10.0));
 
 UdpClientHelper client(interfaces.GetAddress(1),9);
 client.SetAttribute("MaxPackets",UintegerValue(10));
 client.SetAttribute("Interval",TimeValue(Seconds(1)));
 client.SetAttribute("PacketSize",UintegerValue(1024));
 
 ApplicationContainer clientApp=client.Install(nodes.Get(0));
 clientApp.Start(Seconds(2.0));
 clientApp.Stop(Seconds(10.0));
 
 AsciiTraceHelper ascii;
 pointToPoint.EnableAsciiAll(ascii.CreateFileStream("udp-flow.tr"));
 pointToPoint.EnablePcapAll("udp-flow");
 
 AnimationInterface anim("1nt24is078.xml");
 anim.SetConstantPosition(nodes.Get(0), 0.0, 0.0);
 anim.SetConstantPosition(nodes.Get(1), 50.0, 0.0);

 Simulator::Run();
 Simulator::Destroy();
 return 0;
}
