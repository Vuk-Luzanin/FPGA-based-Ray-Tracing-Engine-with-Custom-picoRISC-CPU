module KMOPR (
    input [31:0] signals,
    output [7:0] address
);

    wire intd = signals[0];
    wire inte = signals[1];
    wire load = signals[2];
    wire store = signals[3];
    wire add = signals[4];
    wire sub = signals[5];
    wire realmul = signals[6];
    wire realclamp = signals[7];
    wire intreal = signals[8];
    wire inc = signals[9];
    wire dec = signals[10];
    wire Aand = signals[11];
    wire Aor = signals[12];
    wire Axor = signals[13];
    wire Aasr = signals[14];
    wire Aasl = signals[15];
    wire call = signals[16];
    wire ret = signals[17];
    // intd, inte, load, store, add, sub, realmul, realclamp, intreal, inc, dec, Aand, Aor, Axor, Aasr, Aasl, call, ret, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0

    assign address = 
        intd ? 8'd20 :
        inte ? 8'd21 :
        load ? 8'd22 :
        store ? 8'd23 :
        add ? 8'd25 :
        sub ? 8'd26 :
        realmul ? 8'd27 :
        realclamp ? 8'd28 :
        intreal ? 8'd29 :
        inc ? 8'd30 :
        dec ? 8'd32 :
        Aand ? 8'd38 :
        Aor ? 8'd39 :
        Axor ? 8'd40 :
        Aasr ? 8'd34 :
        Aasl ? 8'd36 :
        call ? 8'd43 :
        ret ? 8'd45 : 8'b00000000;
endmodule
