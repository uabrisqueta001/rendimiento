/*
 * nodoFinal.cc
 *
 *  Created on: Jan 26, 2021
 *      Author: docker
 */


#include <stdio.h>
#include <string.h>
#include <omnetpp.h>

#include "paquete_m.h"

using namespace omnetpp;

class NodoFinal : public cSimpleModule
{
    cHistogram hopCountStats;
    cOutVector hopCountVector;
    cOutVector e2edelay;
    cHistogram e2eHistogram;
    long numReceived;
    protected:
        virtual void handleMessage(cMessage *msg) override;
        virtual void finish() override;
        virtual void initialize() override;
        virtual void refreshDisplay() const override;
};

Define_Module(NodoFinal);

void NodoFinal::initialize(){
    hopCountStats.setName("hopCountStats");
    hopCountStats.setRange(0, 5);
    hopCountVector.setName("HopCount");
    e2edelay.setName("e2eDelay");
    e2eHistogram.setName("e2ehistogram");
    numReceived = 0;
    WATCH(numReceived);
}
void NodoFinal::handleMessage(cMessage *msg)
{

    paquete *pkt = (paquete*) msg;
    cGate *arrivalGate = pkt -> getArrivalGate();
    int arrivalGateIndex = arrivalGate -> getIndex();
    EV << ""+(std::string)getName()+":Ha llegado un paquete por la puerta: " + std::to_string(arrivalGateIndex) + " con origen: "+(std::string)(pkt->getSource())+"\n";

    if (pkt -> getKind() == 1) { // 1: Packet
        if (pkt -> hasBitError()) {
            EV << "Paquete erroneo, se envia un NAK\n";
            paquete *nak = new paquete("NAK");
            nak -> setKind(3);
            send(nak, "out", arrivalGateIndex);
        }
        else {
            int hopcount = pkt->getHopCount();
            double delay=(simTime() - pkt ->getCreationTime()).dbl();
            char buf[5000];
            hopCountVector.record(hopcount);
            hopCountStats.collect(hopcount);
            e2edelay.record(delay);
            e2eHistogram.collect(delay);
            sprintf(buf,"El paquete ha llegado con retraso: %lf-%lf=%lf",simTime().dbl(),(pkt -> getCreationTime()).dbl(),delay);
            bubble(buf);
            EV << "Paquete correcto, se envia un ACK\n";
            paquete *ack = new paquete("ACK");
            ack -> setKind(2);
            send(ack, "out", arrivalGateIndex);
            EV << "El paquete ha llegado a destino";
            numReceived++;
        }
    }
}
void NodoFinal::refreshDisplay() const
{
    char buf[40];
    sprintf(buf, "recv: %ld", numReceived);
    getDisplayString().setTagArg("t", 0, buf);
}
void NodoFinal::finish(){
    hopCountStats.recordAs("hop count");
    e2eHistogram.recordAs("end to end delay");
}


