module KMADR (
    input regdir,
    input imm,
    input memdir,
    input regind,
    input regindpom,
    output [7:0] address
);

    assign address = 
                    regdir ? 8'd19 : 
                    imm ? 8'd11 : 
                    memdir ? 8'd12 : 
                    regind ? 8'd14 : 
                    regindpom ? 8'd16 : 
                    8'b00000000;

endmodule
