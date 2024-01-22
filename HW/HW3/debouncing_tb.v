`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: ShanghaiTech Univ.
// Engineer: Zhaojun Ni
// 
// Create Date: 2023/11/09 21:12:06
// Design Name: 
// Module Name: debouncing_tb
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////
module debouncing_tb();
reg CLK;
reg ButtonIn;
wire ButtonOut;

debouncing #(.N(8)) mydebounce (
    .CLK(CLK),
    .ButtonIn(ButtonIn),
    .ButtonOut(ButtonOut));

always #5 CLK=~CLK;

initial begin
    CLK <= 0;
    ButtonIn <= 0;
    #402;
    ButtonIn <= 1;
    #4;
    ButtonIn <= 0;
    #4;
    ButtonIn <= 1;
    #4;
    ButtonIn <= 0;
    #4;
    ButtonIn <= 1;
    #4;
    ButtonIn <= 0;
    #4;
    ButtonIn <= 1;
    #4;
    ButtonIn <= 0;
    #4;
    ButtonIn <= 1;
    #4;
    ButtonIn <= 0;
    #4;
    ButtonIn <= 1;
    #4;
    ButtonIn <= 0;
    #4;
    ButtonIn <= 1;
    #4;
    ButtonIn <= 0;
    #4;
    ButtonIn <= 1;
    #4;
    ButtonIn <= 0;
    #4;
    ButtonIn <= 1;
    #4;
    ButtonIn <= 0;
    #4;
    ButtonIn <= 1;
    #400;
    ButtonIn <= 0;
    #4;
    ButtonIn <= 1;
    #4;
    ButtonIn <= 0;
    #4;
    ButtonIn <= 1;
    #4;
    ButtonIn <= 0;
    #4;
    ButtonIn <= 1;
    #4;
    ButtonIn <= 0;
    #4;
    ButtonIn <= 1;
    #4;
    ButtonIn <= 0;
    #4;
    ButtonIn <= 1;
    #4;
     ButtonIn <= 0;
    #4;
    ButtonIn <= 1;
    #4;
    ButtonIn <= 0;
    #4;
    ButtonIn <= 1;
    #4;
    ButtonIn <= 0;
    #4;
    ButtonIn <= 1;
    #4;
    ButtonIn <= 0;
    #4;
    ButtonIn <= 1;
    #4;
    ButtonIn <= 0;
    #4;
    ButtonIn <= 1;
    #4;
    ButtonIn <= 0;
    #700;
    
    
    
end
endmodule
