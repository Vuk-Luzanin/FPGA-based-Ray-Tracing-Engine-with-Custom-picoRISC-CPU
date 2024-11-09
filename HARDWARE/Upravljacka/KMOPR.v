module KMOPR (
    input [31:0] signals,
    output [7:0] address
);

    wire intd = signals[31];
    wire inte = signals[30];
    wire load = signals[29];
    wire store = signals[28];
    wire add = signals[27];
    wire sub = signals[26];
    wire real_mul = signals[25];
    wire real_clamp = signals[24];
    wire int_real = signals[23];
    wire inc = signals[22];
    wire dec = signals[21];
    wire Aand = signals[20];
    wire Aor = signals[19];
    wire Axor = signals[18];
    wire Aasr = signals[17];
    wire Aasl = signals[16];
    wire call = signals[15];
    wire ret = signals[14];
    // intd, inte, load, store, add, sub, real_mul, real_clamp, int_real, inc, dec, Aand, Aor, Axor, Aasr, Aasl, call, ret, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0

    assign address = 
        intd ? 8'd20 :
        inte ? 8'd21 :
        load ? 8'd22 :
        store ? 8'd23 :
        add ? 8'd25 :
        sub ? 8'd26 :
        real_mul ? 8'd27 :
        real_clamp ? 8'd28 :
        int_real ? 8'd29 :
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
