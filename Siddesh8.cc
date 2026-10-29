#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/netanim-module.h"
#include "ns3/point-to-point-module.h"

using namespace ns3;

void CwndChange(Ptr<OutputStreamWrapper>stream,uint32_t oldCwnd,uint32_t newCwnd){
*stream->GetStream() << Simulator::Now().GetSeconds() << "\t" << newCwnd / 1024
<< std::endl;
}
int main(int argc,char*argv[]){
 CommandLine cmd;
 cmd.Parse(argc, argv);
 NodeContainer nodes;
 nodes.Create(2);
 
 PointToPointHelper pointToPoint;
 pointToPoint.SetDeviceAttribute("DataRate", StringValue("10Mbps"));
 pointToPoint.SetChannelAttribute("Delay", StringValue("2ms"));
 
 NetDeviceContainer devices;
 devices=pointToPoint.Install(nodes);
 
 pointToPoint.EnablePcapAll("tcp_congestion");
 
 InternetStackHelper stack;
 stack.Install(nodes);
 
 Ipv4AddressHelper address;
 address.SetBase("10.1.1.0", "255.255.255.0");
 Ipv4InterfaceContainer interfaces = address.Assign(devices);
 
 uint16_t port=9;
 Address sinkAddress(InetSocketAddress(interfaces.GetAddress(1),port));
 PacketSinkHelper sink("ns3::TcpSocketFactory",sinkAddress);
 ApplicationContainer sinkApp=sink.Install(nodes.Get(1));
 sinkApp.Start(Seconds(1.0));
 sinkApp.Stop(Seconds(20.0));
 
 Ptr<Socket> tcpSocket = Socket::CreateSocket(nodes.Get(0),TcpSocketFactory::GetTypeId());

 AsciiTraceHelper asciiTraceHelper;
 Ptr<OutputStreamWrapper> stream =asciiTraceHelper.CreateFileStream("cwnd-trace.dat");

 tcpSocket->TraceConnectWithoutContext("CongestionWindow",MakeBoundCallback(&CwndChange, stream));
 BulkSendHelper source("ns3::TcpSocketFactory", sinkAddress);
 source.SetAttribute("MaxBytes", UintegerValue(0));
 ApplicationContainer sourceApp = source.Install(nodes.Get(0));
 sourceApp.Start(Seconds(2.0));
 sourceApp.Stop(Seconds(20.0));
 
 AnimationInterface anim("tcp_congestion1nt24is078.xml");
 anim.SetConstantPosition(nodes.Get(0), 0.0, 0.0);
 anim.SetConstantPosition(nodes.Get(1), 50.0, 0.0);
 
 anim.UpdateNodeDescription(nodes.Get(0), "TCP Sender");
 anim.UpdateNodeDescription(nodes.Get(1), "TCP Receiver");
 anim.UpdateNodeColor(nodes.Get(0), 0, 255, 0);
 anim.UpdateNodeColor(nodes.Get(1), 0, 0, 255);
 
 Simulator::Stop(Seconds(20.0));
 Simulator::Run();
 Simulator::Destroy();
 return 0;
 }

