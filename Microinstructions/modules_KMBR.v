module KMBR (
    input [15:0] signals,
    output [7:0] address
);

    wire [7:0] addressTemp;
    assign addressTemp = 8'b00000000;
    wire val0 = signals[0];
    wire val9 = signals[1];
    wire val17 = signals[2];
    wire val19 = signals[3];
    wire val41 = signals[4];
    wire val49 = signals[5];

    assign address = 
        (val0 ? 8'd0 : 8'b00000000) | 
        (val9 ? 8'd9 : 8'b00000000) | 
        (val17 ? 8'd17 : 8'b00000000) | 
        (val19 ? 8'd19 : 8'b00000000) | 
        (val41 ? 8'd41 : 8'b00000000) | 
        (val49 ? 8'd49 : 8'b00000000);

endmodule
