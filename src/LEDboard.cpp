#include <Arduino.h>

#define MESH_DEBUG_MINIMAL 1
#define MESH_DEBUG_SERIAL 1

#include <RF24.h>
#include <RF24Network.h>
#include <RF24Mesh.h>

#include "KeepCube.hpp"


// --- TODO ---
// TODO: Automatické nastavování z Base pomocí cfg commandů (1. parametr je co, další hodnoty)
// TODO: Ukládání nastavení do EEPROM, zase udělat všemožný interfacy přes commandy
// TODO: Radio, network a mesh knihovny vzít, předělat a zakomponovat do KClib, pokud to jejich licence umožňuje

// --- DONE ---
// DONE: Přijímání z NRFka udělat na interrupt
// DONE: Kompletně předělat Command.hpp (možná i integrovat do KeepCube.hpp)
// DONE: Checkování bufferu NRFka počas animování !! UPDATE !! - IRQ pin NRFka je na INTERRUPT!!!


// NRF24L01 pins
const int CEpin = 10;
const int CSpin = 3;
const int IRQpin = 2;

// Configure the nrf24l01 CE(10) and CS(3) pins
RF24 radio(CEpin, CSpin);
RF24Network network(radio);
RF24Mesh mesh(radio, network);
KeepCube cube(radio, network, mesh);


void setup() {
    Serial.begin(250000);
    cube.init();
    attachInterrupt(digitalPinToInterrupt(IRQpin), [] () { cube.interruptHandler(); }, FALLING);
    Serial.println(KeepCube::newNodeID);
    Serial.println(sizeof(uint8_t));
}


void loop() {
    cube.update(); // Update internal processes

    if (cube.messageAvailable()) {
        cube.status.on(); // Turn on status LED

        // NOTE: Zatím se nepřijímají reálné zprávy, protože se ani neodesílají.
        //       Přijímání i odesílání ale normálně funguje.

        // Approx. 1904us
        Command cmd("#slp{bpAN1}1,3,ff0000,100,00ff00,100,00ff00,3000;");

        // Approx. 396us
        if (cmd.header.is("slp")) {
            cube.setLEDpattern(cmd.params.toString());
        }

        Serial.println("[ RX ] " + cube.receiveMessage()); // Výpis přijaté zprávy, proč ne :)

        // Command cmd(cube.receiveMessage()); // Reálné přijetí zprávy do nové instance Commandu

        cube.status.off(); // Turn off status LED
    }

}
