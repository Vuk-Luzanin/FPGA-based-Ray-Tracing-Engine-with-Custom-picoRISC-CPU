module TranslateControl (
    input  [255:0] T,
    input  [15:0] cond,
    output bropr,
    output bradr,
    output bruncnd,
    output brcnd,
    output [15:0] signals
);

    // notSTART, l1, branch, store, notUslov, notPrekid, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0

    wire notSTART;
    wire l1;
    wire branch;
    wire store;
    wire notUslov;
    wire notPrekid;

    wire brNotSTART;
    wire brL1;
    wire brBranch;
    wire brStore;
    wire brNotUslov;
    wire brNotPrekid;

    assign brNotSTART = T[0];
    assign brL1 = T[4];
    assign brBranch = T[9];
    assign brStore = T[12] | T[14] | T[16];
    assign brNotUslov = T[41];
    assign brNotPrekid = T[49];

    assign bruncnd = T[11] | T[13] | T[15] | T[20] | T[21] | T[22] | T[24] | T[25] | T[26] | T[27] | T[28] | T[29] | T[31] | T[33] | T[35] | T[37] | T[38] | T[39] | T[40] | T[42] | T[44] | T[55];

    assign brcnd = (notSTART & brNotSTART) | (l1 & brL1) | (branch & brBranch) | (store & brStore) | (notUslov & brNotUslov) | (notPrekid & brNotPrekid);

    assign bradr = T[10];

    assign bropr = T[19];

    wire val0 = T[0] | T[49] | T[55];
    wire val9 = T[4];
    wire val17 = T[13] | T[15];
    wire val19 = T[11] | T[12] | T[14] | T[16];
    wire val41 = T[9];
    wire val49 = T[20] | T[21] | T[22] | T[24] | T[25] | T[26] | T[27] | T[28] | T[29] | T[31] | T[33] | T[35] | T[37] | T[38] | T[39] | T[40] | T[41] | T[42] | T[44];

    assign signals = { val0, val9, val17, val19, val41, val49, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0 };

    // val0, val9, val17, val19, val41, val49, control_symbols_temp[9..0]

endmodule
