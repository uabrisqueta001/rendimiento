/*
 * nodoIntermedio.cc
 *
 *  Created on: Jan 26, 2021
 *      Author: docker
 */

#include <stdio.h>
#include <string.h>
#include <omnetpp.h>
#include "paquete_m.h"

using namespace omnetpp;
class NodoIntermedio : public cSimpleModule{

   private:
       cChannel *canal[2]; // one canal for each output
       cQueue *ilara[2];  // one ilara for each canal
       double p;  // from 0 to 1
   protected:
       virtual void initialize() override;
       virtual void handleMessage(cMessage *msg) override;
       virtual void sendNew(paquete *pkt);
       virtual void sendNext(int gateIndex);
       virtual void sendPacket(paquete *pkt, int gateIndex);
};

Define_Module(NodoIntermedio);

void NodoIntermedio::initialize() {
    // Consigues links a las dos puertas de salida (ninguno tiene mas de dos enlaces)
    canal[0] = gate("out", 0) -> getTransmissionChannel();
    canal[1] = gate("out", 1) -> getTransmissionChannel();

    // Create one ilara for each canal
    ilara[0] = new cQueue("cola1");
    ilara[1] = new cQueue("cola2");

    // Initialize random number generator
    srand(time(NULL));

    // Get probability parameter
    p = (double) par("p");
}

void NodoIntermedio::handleMessage(cMessage *msg)
{
    paquete *pkt = check_and_cast<paquete *> (msg);
    cGate *arrivalGate = pkt -> getArrivalGate();
    int arrivalGateIndex = arrivalGate -> getIndex();

    EV << ""+(std::string)getName()+":Ha llegado un paquete por la puerta: " + std::to_string(arrivalGateIndex) + " con origen: "+(std::string)(pkt->getSource())+"\n";

    if (pkt -> getFromSource()) {
        // Packet from source
        EV << "Ha llegado un paquete desde la fuente "+(std::string)(pkt->getSource())+"\n";
        pkt -> setFromSource(false);
        sendNew(pkt);
        return;
    }
    if (pkt -> getKind() == 1) { // 1: Packet
        //no hay mecanismos de time out--> solo correcion de errores
        if (pkt -> hasBitError()) {
            EV << "Paquete erroneo, se envia un NAK\n";
            paquete *nak = new paquete("NAK");
            nak -> setKind(3);
            send(nak, "out", arrivalGateIndex);
        }
        else {
            EV << "Paquete correcto, se envia ACK\n";
            paquete *ack = new paquete("ACK");
            ack -> setKind(2);
            send(ack, "out", arrivalGateIndex);
            sendNew(pkt);
        }
    }
    else if (pkt -> getKind() == 2) { // 2: ACK
        EV << "Recepcion de ACK, el paquiete enviado ha sido correcto\n";
        if (ilara[arrivalGateIndex] -> isEmpty())
            EV << "ADVERTENCIA: el paquete se ha confirmado con ACK, pero la cola esta vacia...tic-toc\n";
        else {
            // pop() removes ilara's first packet
            ilara[arrivalGateIndex] -> pop();//ilaratik kentzen da
            sendNext(arrivalGateIndex);
        }
    }
    else { // 3: NAK
        EV << "El paquete que se ha enviado ha llegado de manera erronea, retransmision\n";
        sendNext(arrivalGateIndex);
    }
}

void NodoIntermedio::sendNew(paquete *pkt) {
    int gateIndex;
    double randomNumber = ((double) rand() / (RAND_MAX));
    if (randomNumber < p)
        gateIndex = 0;
    else
        gateIndex = 1;

    if (ilara[gateIndex] -> isEmpty()) {
        EV << "La cola esta vacia, paquete encolado y enviado\n";
        // Insert in ilara (it may have to be sent again)
        ilara[gateIndex] -> insert(pkt);
        sendPacket(pkt, gateIndex);
    } else {
        EV << "la cola no esta vacia, paquete encolado y esperando su turno\n";
        ilara[gateIndex] -> insert(pkt);
    }
}

void NodoIntermedio::sendNext(int gateIndex) {
    if (ilara[gateIndex] -> isEmpty())
        EV << "No more packets in ilara\n";
    else {
        // Se consigue el primer paquete de la cola y se envia a la funcion de envio
        paquete *pkt = check_and_cast<paquete *> (ilara[gateIndex] -> front());
        sendPacket(pkt, gateIndex);
    }
}

void NodoIntermedio::sendPacket(paquete *pkt, int gateIndex) {
    if (canal[gateIndex] -> isBusy()) {
        EV << "ADVERTENCIA: error en la gestion del canal, revisar diseño\n";
    } else {
        // Se envia una copia del paquete y no el original, porque esta en la cola
        paquete *newPkt = check_and_cast<paquete *> (pkt -> dup());
        newPkt -> setHopCount(newPkt->getHopCount()+1);
        send(newPkt, "out", gateIndex);
    }
}


