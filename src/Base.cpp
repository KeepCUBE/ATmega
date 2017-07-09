/** RF24Mesh_Example_Master.ino by TMRh20


   This example sketch shows how to manually configure a node via RF24Mesh as a master node, which
   will receive all data from sensor nodes.

   The nodes can change physical or logical position in the network, and reconnect through different
   routing nodes as required. The master node manages the address assignments for the individual nodes
   in a manner similar to DHCP.

 */


#include "RF24Network.h"
#include "RF24.h"
#include "RF24Mesh.h"
#include <SPI.h>
// Include eeprom.h for AVR (Uno, Nano) etc. except ATTiny
#include <EEPROM.h>

#include <LiquidCrystal.h>

#include "KeepCube.hpp"


RF24 radio(10, 3);
RF24Network network(radio);
RF24Mesh mesh(radio, network);

// KeepCube cube(radio, network);


uint32_t displayTimer = 0;

void setup() {


    Serial.begin(250000);


    // Set the nodeID to 0 for the master node
    mesh.setNodeID(0);
    // Serial.println(mesh.getNodeID());
    // Connect to the mesh
    mesh.begin();

    radio.setPALevel(RF24_PA_HIGH);

    // printf_begin();
    radio.printDetails();


    //  pinMode(18, OUTPUT);
    //  pinMode(19, OUTPUT);
    //  digitalWrite(18, LOW);


    //  lcd.begin(20, 4);
    //  lcd.cursor();

}


void loop() {


    Serial.println(F("bam..."));


    // mesh.update();
    // mesh.DHCP();


    // if (millis() - displayTimer > 10000) {
    //     displayTimer = millis();
    //
    //     Command msg("#NRFA1D&#SLPL0P2T1000D1000C\\&ff00000000ff\\&;&;");
    //     Serial.println("Sending: " + msg.toString());
    //     String identifier = msg.getIdentifier();
    //
    //     if (identifier == "NRF") {
    //         int ID = msg.getParam('A').toInt(); // jakoze Adress
    //         String data = msg.getParam('D'); // jakoze Data
    //
    //         char a[data.length() + 1];
    //         data.toCharArray(a, data.length() + 1);
    //
    //         int giveupTimer = 0;
    //         while (!mesh.write(&a, 'M', data.length() + 1, ID) && giveupTimer < 10) {
    //             //        if (!mesh.checkConnection()) {
    //             //          //refresh the network address
    //             //          mesh.renewAddress();
    //             //        }
    //             giveupTimer++;
    //             delay(giveupTimer * 100);
    //         }
    //     }
    //
    //
    // }


    // if (Serial.available()) {
    //   Command msg(Serial.readString());
    //   String identifier = msg.getIdentifier();
    //
    //   // Example: #NRFA1D&#SLPL2T1D1P\&ff00000000ff\&;&;
    //   if (identifier == "NRF") {
    //     int ID = msg.getParam('A').toInt(); // jakoze Adress
    //     String data = msg.getParam('D'); // jakoze Data
    //
    //     char a[data.length() + 1];
    //     data.toCharArray(a, data.length() + 1);
    //
    //     int giveupTimer = 0;
    //     while (!mesh.write(&a, 'M', data.length() + 1, ID) && giveupTimer < 10) {
    //       //        if (!mesh.checkConnection()) {
    //       //          //refresh the network address
    //       //          mesh.renewAddress();
    //       //        }
    //       giveupTimer++;
    //       delay(giveupTimer * 100);
    //     }
    //   }


}


//
//  if (millis() - displayTimer > 10000) {
//    displayTimer = millis();
//    Serial.println(" ");
//    Serial.println(F("********Assigned Addresses********"));
//    for (int i = 0; i < mesh.addrListTop; i++) {
//      Serial.print("NodeID: ");
//      Serial.print(mesh.addrList[i].nodeID);
//      Serial.print(" RF24Network Address: 0");
//      Serial.println(mesh.addrList[i].address, OCT);
//    }
//    Serial.println(F("**********************************"));
//
//    // Serial.print("sending: ");
//    // ctr++;
//    // int id = 0;
//    // payload_t payload;
//    // for (int i = 0; i < mesh.addrListTop; i++) {
//    //     if (mesh.addrList[i].nodeID == 1) {  //Searching for node one from address list
//    //         payload = {ctr % 3, ctr};
//    //         id = i;
//    //         break;
//    //     }
//    // }
//
//    // RF24NetworkHeader header(mesh.addrList[id].address, OCT); //Constructing a header
//
//    // // Serial.println(network.write(header, &payload, sizeof(payload) == 1 ? "Send OK" : "Send Fail")); //Sending an message
//
//    // if (network.write(header, &payload, sizeof(payload) == 1)) {
//    //     Serial.println("ok");
//    // }
//    // else {
//    //     Serial.println("failed");
//    // }
//  }


// }
