//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 
//
// Immediately sends out any message it receives. It can optionally generate
// a message at the beginning of the simulation, to bootstrap the process.
//
simple NodoFuente
{
    parameters:
        @display("i=block/source");
        double l_media;
    gates:
        output out;
}
simple NodoIntermedio
{
    parameters:
        @display("i=block/departure");
        double p;
    gates:
        input in[];
        output out[];
}
simple NodoFinal
{
    parameters:
        @display("i=block/arrival");
    gates:
        input in[];
        output out[];
}

network redPractica
{
    @display("bgb=430,379");
    types:
        channel Channel extends ned.DatarateChannel
        {
            datarate = 3000bps;
            delay = 5ms;
            per = 0.25;
        }
    submodules:
        nodo1: NodoIntermedio {
            @display("p=99.509995,177.62999");
            p = 1/4;
            gates:
                in[3];
                out[2];
        }
        nodo2: NodoIntermedio {
            @display("p=174.84,85.56");
            p = 1/3;
            gates:
                in[5];
                out[4];
        }
        nodo5: NodoIntermedio {
            @display("p=174.84,271.56");
            p = 1/2;
            gates:
                in[4];
                out[3];
        }
        nodo3: NodoFinal {
            @display("p=346.88998,85.56");
            gates:
                in[1];
                out[1];
        }
        nodo4: NodoFinal {
            @display("p=346.88998,271.56");
            gates:
                in[2];
                out[2];
        }
        source1: NodoFuente {
            @display("p=29.759998,177.62999");
            l_media = 9600/3;
        }
        source2: NodoFuente {
            @display("p=174.84,21.39");
            l_media = 9600/3;
        }
        source5: NodoFuente {
            @display("p=174.84,324.56998");
            l_media = 9600/3;
        }
    connections:
        source1.out --> nodo1.in[2];
        source2.out --> nodo2.in[4];
        source5.out --> nodo5.in[3];
        nodo1.out[0] --> Channel --> nodo2.in[2];
        nodo1.in[0] <-- Channel <-- nodo2.out[2];

        nodo1.out[1] --> Channel --> nodo5.in[2];
        nodo1.in[1] <-- Channel <-- nodo5.out[2];

        nodo2.out[0] --> Channel --> nodo3.in[0];
        nodo2.in[0] <-- Channel <-- nodo3.out[0];

        nodo2.out[1] --> Channel --> nodo4.in[0];
        nodo2.in[1] <-- Channel <-- nodo4.out[0];

        nodo5.out[0] --> Channel --> nodo2.in[3];
        nodo5.in[0] <-- Channel <-- nodo2.out[3];

        nodo5.out[1] --> Channel --> nodo4.in[1];
        nodo5.in[1] <-- Channel <-- nodo4.out[1];



}
