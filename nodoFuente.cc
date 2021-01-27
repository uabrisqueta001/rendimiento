/*
 * nodoFuente.cc
 *
 *  Created on: Jan 26, 2021
 *      Author: docker
 */

#include <stdio.h>
#include <string.h>
#include <omnetpp.h>
#include <random>
using namespace omnetpp;
#include "paquete_m.h"

class NodoFuente : public cSimpleModule
{
  private:
    long numSent;
    long numReceived;
    double lambda=2;
    double l_media=1000/3;
    int n_paquetes=1000;
    int seq_n=1;//este se cambia al inicializar la instancia

  protected:
    virtual void initialize() override;
    virtual std::vector<double> getArrivals(double lambda, int n_paquetes);
    virtual std::vector<double> getServices(double meanPacketLength, int samples);
    virtual void handleMessage(cMessage *msg) override;
    virtual void refreshDisplay() const override;
    virtual paquete* BuildPkt();
};

Define_Module(NodoFuente);

void NodoFuente::initialize()
{
    numSent=0;
    //numReceived=0;
    WATCH(numSent);

    //WATCH(numReceived);
    std::vector<double> arrivals=getArrivals(lambda,n_paquetes);
    std::vector<double> services=getServices(l_media, n_paquetes);

    for (long unsigned int i=0;i<arrivals.size();i++){
        //aqui se generan los paquetes y se envian en el tiempo que les toque
        paquete* pkt=new paquete();
        pkt -> setBitLength(services[i]); //para no alargar la funcion se ha creado directamente con longitudes como en mi practica de mthematica
        scheduleAt((simtime_t)arrivals[i], pkt);

    }
}
std::vector<double> NodoFuente::getArrivals(double lambda, int n_paquetes) {
    // Crea las llegadas con landa y el numero de paquetes

    std::uniform_real_distribution<double> randomReal(0.0, 1.0);
    std::default_random_engine generator(time(NULL));
    std::vector<double> arrivals(n_paquetes);
    for(int i = 0; i < (int)arrivals.size(); i++) {
        double randomNumber = randomReal(generator);
        double number = (-1/lambda) * log(randomNumber);

        if (i == 0)
            arrivals[i] = number;//aqui se sienta el tiempo en el que llega el primer paquete
        else
            arrivals[i] = arrivals[i - 1] + number;//aqui se sienta eñ tiempo en el que llegan el resto de paquetes sumando un numero aleatorio a la anterior llegada
       // EV << (std::to_string(arrivals[i]));
    }
    return arrivals;
}
std::vector<double> NodoFuente::getServices(double meanPacketLength, int samples) {
    // se considero hacerlo con tiempos en un inicio pero complciaba mucho la algortimia y se ha mantenido con tamaño de paquetes como en mathematica
    std::uniform_real_distribution<double> randomReal(0.0, 1.0);
    std::default_random_engine generator(time(NULL));
    std::vector<double> services(samples);
    for(int i = 0; i < (int)services.size(); i++) {
        double randomNumber = randomReal(generator);
        double number = (-meanPacketLength) * log(randomNumber);
        services[i] = number;//en este caso no se suma porque son tamaños
    }
    return services;
}
paquete* NodoFuente::BuildPkt(){
    std::string packetName = "packet::" + (std::string)(getName()) + "::" + std::to_string(seq_n);//cadena formato --> paquete::idNodoFuente::sequenceNumber
    char *charPacketName = new char[packetName.length() + 1];
    strcpy(charPacketName, packetName.c_str());
    paquete *pkt = new paquete(charPacketName);
    pkt -> setFromSource(true);
    pkt -> setKind(1);
    pkt -> setSequenceNumber(seq_n);
    pkt -> setSource(getName());//se le da el


    seq_n++;
    return pkt;
}

void NodoFuente::handleMessage(cMessage *msg)
{
    paquete *pkt2 = check_and_cast<paquete *>(msg);
    paquete *pkt=BuildPkt();
    pkt -> setBitLength(pkt2->getBitLength());
    EV << std::to_string((pkt ->getCreationTime()).dbl());
    send(pkt,"out");
    numSent++;

}

void NodoFuente::refreshDisplay() const
{
    char buf[40];
    sprintf(buf, "sent: %ld", numSent);
    getDisplayString().setTagArg("t", 0, buf);
}




