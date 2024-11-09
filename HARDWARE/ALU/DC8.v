module DC8 (
    input [7:0] in,       // 8-bit input
    output reg [255:0] out // 256-bit output
);

always @(*) begin
    out = 256'b0;               // Initialize all outputs to 0
    out[in] = 1'b1;             // Set only one bit corresponding to the input
end

endmodule
