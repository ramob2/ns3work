
#include <string>
#include "ns3/core-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/internet-module.h"
#include "ns3/network-module.h"
#include "ns3/stats-module.h"
#include "ns3/yans-wifi-channel.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/ipv4-address-helper.h"
#include "ns3/log.h"
#include "ns3/applications-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include <ns3/propagation-module.h>
#include "ns3/wifi-mac-helper.h"
#include <iostream>
#include <ns3/flow-monitor-module.h>
#include <fstream>


/*
- Steps to create a simulation
  1. Decide Physical Layer Framework
  2. Configure the channel: Delay Model, Loss Model
  3. Configure the PHY: Error Rate Model, Codeíng Scheme maybe ?, Output Power, Antenna Gain, Antenna Type (?)
  4. Configure the MAC: MAC Type, MAC Parameters (SSID, ...)
  5. Create WiFi Device: MAC, PHY, Channel, IEEE Standard
  6. Configure Mobility: relative Position
  7. Configure the Network Layer: IP Address, Routing Protocol
  8. Configure the Application Layer: Application Type, Application Parameters
  9. Configure the Simulation: Simulation Time, Tracing, Logging, Statistics, find out where to use the flow monitor
  10. Run the Simulation with various parameters, esp. the distance between the nodes
*/



enum pro_modell {Friis = 0, FixedRss, ThreeLogDistance, TwoRayGround, Nakagami};
using namespace ns3;   

NS_LOG_COMPONENT_DEFINE ("PropagationSimulation");
int32_t total_packet_count_rx = 0;
int32_t total_packet_count_tx = 0;
int32_t total_signal = 0;
int32_t total_noise = 0;
int32_t total_packet_aggregation = 0;
uint32_t packet_size_aggregated = 0;
double calculation_start = 0; 
bool start_flag = false;

void MonitorSniffTx (Ptr<const Packet> packet, uint16_t channelFreqMhz, WifiTxVector txVector, MpduInfo aMpdu, uint16_t staId)
{
  
}
void MonitorSniffRx (Ptr<const Packet> packet, uint16_t channelFreqMhz, WifiTxVector txVector, MpduInfo aMpdu, SignalNoiseDbm signalNoise, uint16_t stadId)
{
  total_noise = signalNoise.noise;
  total_packet_count_tx++;

  if (!start_flag && Simulator::Now().GetSeconds() >= calculation_start)
  {
    start_flag = true;
  }  
  if(start_flag && packet->GetSize() > 1500)
  {
    total_packet_count_rx++;
    total_signal +=  signalNoise.signal;
  }
}

void data_calculation (int distance_square , std::string propagation_model, double app_begin, double app_end , double server_trough)
{

  //propagation_model, distance, data throughput ,calculation_start, runtime (app_end-app_begin), app_begin, app_end (fixed) ,total_packet_count_rx, signal mean, noise mean, signal varianz, 
  std::string file_name = propagation_model + ".csv";
  std::ifstream experiment_file_check(file_name);
  std::ofstream experiment_file(file_name,std::ios_base::app | std::ios_base::out);
  if (!experiment_file_check)
  {
    std::cout << "File "<< file_name<<" not found" << std::endl;
     experiment_file << "distance,troughput,server_troughput,calculation_start_after_app,runtime,total_packet_count_received,signal_mean, noise,signal_varianz" << std::endl;
  }
  double all_trough = (double)total_packet_count_tx*8*1450/((app_end-calculation_start)*1000000);
  double troughput = (double)total_packet_count_rx*8*1450/((app_end-calculation_start)*1000000);
  experiment_file << sqrt(distance_square)<<","<<troughput<<","<<all_trough <<","<<calculation_start-app_begin<<"," <<(double) app_end-calculation_start<<"," <<\
                           total_packet_count_rx<<"," << total_signal/total_packet_count_rx<<"," << total_noise<<"," << "0"<<  std::endl;

  experiment_file.close();  
  
  if (troughput < 0.00000001)
  {
    std::cout<<"Failed"<<std::endl;
  }

}


int main (int argc, char *argv[])
{
  CommandLine cmd (__FILE__);

  double wifi_output_power = 10; //10 dbm  
  double antenna_gain = 1; //dbi
  unsigned int app_packet_size = 1450; //bytes
  int32_t fixed_rss_loss = -40; //dbm
  uint32_t udp_data_rate = 75000000; //bps

  int propagation_type = 0;
  double distance = 1;
  double sim_end= 10.0;
  int server_begin = 0; 
  int server_end = 10;
  double app_begin = 0; 
  double app_end = 10; 


  cmd.AddValue("prop","Propagation Modell",propagation_type);
  cmd.AddValue("dist","Distance between the nodes",distance);
  cmd.AddValue("end","Simulation Endtime",sim_end);
  cmd.AddValue("begin","Simulation Calculation Begin Time",calculation_start);

  cmd.Parse (argc,argv);
  if (distance < 1)
  {
    std::cout << "Distance between the nodes must be greater than 1" << std::endl;
    return 0;
  }
  if (propagation_type < 0 || propagation_type > 4)
  {
    std::cout << "Propagation Modell not found" << std::endl;
    return 0;
  }

  if (sim_end < 0 || sim_end > 200)
  {
    std::cout << "Simulation Endtime must be between 0 and 100" << std::endl;
    return 0;
  }

  server_end = sim_end-1;
  app_begin = server_begin+1;
  app_end = server_end-1;
  calculation_start = calculation_start + app_begin;

  

  std::string propagation_model [] = {"ns3::FriisPropagationLossModel", "ns3::FixedRssLossModel", "ns3::ThreeLogDistancePropagationLossModel", \
                                      "ns3::TwoRayGroundPropagationLossModel","ns3::NakagamiPropagationLossModel"};
  std::string propagation_short [] = {"friis","fixed","threelog","tworay","nakagami"};
  YansWifiChannelHelper wifi_channel;
  YansWifiPhyHelper wifi_phy;

  /*Channel Settings*/
  wifi_channel.SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel");
  if (propagation_type == 1)
  {
    wifi_channel.AddPropagationLoss("ns3::FixedRssLossModel","Rss",DoubleValue(fixed_rss_loss));
  }
  else
  {
    wifi_channel.AddPropagationLoss(propagation_model[propagation_type]);
  }
  /*Physical Layer Settings*/
  wifi_phy.Set("Antennas", UintegerValue(1));
  wifi_phy.Set("MaxSupportedTxSpatialStreams", UintegerValue(1));
  wifi_phy.Set("MaxSupportedRxSpatialStreams", UintegerValue(1));
  wifi_phy.Set("ChannelSettings",StringValue("{0, 40,BAND_5GHZ, 0}"));
  wifi_phy.Set("TxPowerStart", DoubleValue(wifi_output_power));
  wifi_phy.Set("TxPowerEnd", DoubleValue(wifi_output_power));
  wifi_phy.Set("TxGain", DoubleValue(antenna_gain));
  wifi_phy.Set("RxGain", DoubleValue(antenna_gain));
  wifi_phy.SetChannel(wifi_channel.Create());


  /*MAC Settings*/
  NodeContainer wifi_sta_container;
  wifi_sta_container.Create(1);
  NodeContainer wifi_ap_container;
  wifi_ap_container.Create(1);

  WifiMacHelper wifi_mac_sta; 
  WifiMacHelper wifi_mac_ap;
  Ssid ssid = Ssid("ns3-cids-lab");
  wifi_mac_sta.SetType("ns3::AdhocWifiMac","Ssid",SsidValue(ssid));
  wifi_mac_ap.SetType("ns3::AdhocWifiMac","Ssid",SsidValue(ssid));
  WifiHelper wifi_helper;
  wifi_helper.SetStandard(WIFI_STANDARD_80211n);
  wifi_helper.SetRemoteStationManager("ns3::MinstrelHtWifiManager");


  /*Install Netdevices*/
  NetDeviceContainer wifi_sta_device;
  NetDeviceContainer wifi_ap_device;

  wifi_sta_device = wifi_helper.Install (wifi_phy,wifi_mac_sta,wifi_sta_container);
  wifi_ap_device = wifi_helper.Install (wifi_phy,wifi_mac_ap,wifi_ap_container);

  MobilityHelper adac;

  Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();

  positionAlloc->Add (Vector (0.0, 0.0, 1.0));
  positionAlloc->Add (Vector (0.0, distance, 1.0));   //Height needs to be set for two ray ground propagation model to be working

  adac.SetPositionAllocator(positionAlloc);
  adac.SetMobilityModel("ns3::ConstantPositionMobilityModel");
  adac.Install(wifi_sta_container);
  adac.Install(wifi_ap_container);
  InternetStackHelper internet_stack;
  internet_stack.Install(wifi_sta_container);
  internet_stack.Install(wifi_ap_container);
  
  std::cout << "distance betwenn nodes: " << sqrt(adac.GetDistanceSquaredBetween(wifi_sta_container.Get(0),wifi_ap_container.Get(0)))<< std::endl;

  Ipv4AddressHelper ipv4_address;
  ipv4_address.SetBase("192.168.178.0","255.255.255.0");

  Ipv4InterfaceContainer wifi_sta_interface = ipv4_address.Assign(wifi_sta_device);
  Ipv4InterfaceContainer wifi_ap_interface = ipv4_address.Assign(wifi_ap_device);

  ApplicationContainer simulation_server_application;
  ApplicationContainer simulation_client_application;
  UdpServerHelper udp_server(9);    //setup a udp server on port 9
  simulation_server_application = udp_server.Install(wifi_sta_container.Get(0)); //install the server on the first node of the sta container
  simulation_server_application.Start(Seconds(server_begin));
  simulation_server_application.Stop(Seconds(server_end));

  
  

  UdpClientHelper udp_client(wifi_sta_interface.GetAddress(0),9); //setup a udp client on port 9

  udp_client.SetAttribute("MaxPackets",UintegerValue(4294967294U));
  udp_client.SetAttribute("Interval",TimeValue(Seconds((double)app_packet_size*8/udp_data_rate)));
  udp_client.SetAttribute("PacketSize",UintegerValue(app_packet_size));
  simulation_client_application = udp_client.Install(wifi_ap_container.Get(0));
  


  simulation_client_application.Start(Seconds(app_begin));
  simulation_client_application.Stop(Seconds(app_end));


  Ptr<FlowMonitor> flow_monitor;
  FlowMonitorHelper flow_monitor_helper;
  flow_monitor = flow_monitor_helper.InstallAll();

  Config::ConnectWithoutContext("/NodeList/1/DeviceList/*/Phy/MonitorSnifferTx", MakeCallback (&MonitorSniffTx));
  Config::ConnectWithoutContext("/NodeList/0/DeviceList/*/Phy/MonitorSnifferRx", MakeCallback (&MonitorSniffRx));

  Simulator::Stop(Seconds(sim_end));
  Simulator::Run();
  Simulator::Destroy();

       flow_monitor->CheckForLostPackets();

        Ptr<Ipv4FlowClassifier> classifier =
            DynamicCast<Ipv4FlowClassifier>(flow_monitor_helper.GetClassifier());
        FlowMonitor::FlowStatsContainer stats = flow_monitor->GetFlowStats();

        for (auto it = stats.begin(); it != stats.end(); ++it)
        {
            Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow(it->first);
            // std::cout << "Flow (" << t.sourceAddress << " -> " << t.destinationAddress
            //           << ") - Tx Trough: " << (double)it->second.txBytes*8/((app_end-app_begin)*1000000)
            //           << ", Rx Trough: " << (double)it->second.rxBytes*8/((app_end-app_begin)*1000000) <<std::endl;
        }

  uint32_t totalPacketsThrough = DynamicCast<UdpServer>(simulation_server_application.Get(0))->GetReceived();

  double server_trough = (double)totalPacketsThrough*8*1450/((app_end-app_begin)*1000000);
  std::cout << "Total Packets Received: " << totalPacketsThrough << std::endl;
  std::cout << "Total Trougput: " << (double)totalPacketsThrough*8*1450/((app_end-app_begin)*1000000) << std::endl;
  data_calculation (adac.GetDistanceSquaredBetween(wifi_sta_container.Get(0),wifi_ap_container.Get(0)),\
                          propagation_short[propagation_type],app_begin, app_end, server_trough);
  return 0; 




}







