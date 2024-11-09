module KMBR (
    input [15:0] signals,
    output [7:0] address
);

    wire [7:0] addressTemp;
    assign addressTemp = 8'b00000000;
    wire val0 = signals[15];
    wire val8 = signals[14];
    wire val17 = signals[13];
    wire val19 = signals[12];
    wire val41 = signals[11];
    wire val49 = signals[10];

    assign address = 
        (val0 ? 8'd0 : 8'b00000000) | 
        (val8 ? 8'd8 : 8'b00000000) | 
        (val17 ? 8'd17 : 8'b00000000) | 
        (val19 ? 8'd19 : 8'b00000000) | 
        (val41 ? 8'd41 : 8'b00000000) | 
        (val49 ? 8'd49 : 8'b00000000);

endmodule
