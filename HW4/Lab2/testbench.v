//Verilog HDL for "PLL", "testbench" "verilog"


module testbench (rst);
output rst;
reg rst;
initial begin
	rst=1'b0;
	#5 rst=1'b1;
end
endmodule